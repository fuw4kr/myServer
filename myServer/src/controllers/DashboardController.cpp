#include "DashboardController.h"
#include <drogon/drogon.h>
#include <json/json.h>
#include <atomic>
#include <cstdlib>

using namespace drogon;
using namespace drogon::orm;

namespace
{
int64_t getInt64(const Result& r, size_t row, const std::string& col)
{
    if (r.size() > row && !r[row][col].isNull())
        return r[row][col].as<int64_t>();
    return 0;
}
} // namespace

DashboardController::DashboardController(const DbClientPtr& db)
    : db_(db)
{
}

void DashboardController::getDashboard(const HttpRequestPtr& req,
                                       std::function<void(const HttpResponsePtr&)>&& callback)
{
    (void)req;
    auto cb = std::move(callback);
    if (!db_)
    {
        auto resp = HttpResponse::newHttpJsonResponse(Json::Value("Database is not configured"));
        resp->setStatusCode(k500InternalServerError);
        cb(resp);
        return;
    }

    struct Shared
    {
        Json::Value body{Json::objectValue};
        std::vector<int> hourly{24, 0};
        Json::Value events{Json::arrayValue};
        std::atomic<int> pending{0};
        std::atomic<bool> failed{false};
    };

    auto shared = std::make_shared<Shared>();
    shared->body["aiActive"] = aiActive();

    auto finish = [cb, shared]()
    {
        if (shared->failed.load())
            return;

        int remaining = shared->pending.fetch_sub(1) - 1;
        if (remaining > 0)
            return;

        Json::Value hourlyArr(Json::arrayValue);
        for (int v : shared->hourly)
            hourlyArr.append(v);
        shared->body["detectionsByHour"] = hourlyArr;
        shared->body["recentEvents"] = shared->events;

        auto resp = HttpResponse::newHttpJsonResponse(shared->body);
        resp->setStatusCode(k200OK);
        cb(resp);
    };

    auto fail = [cb, shared](const std::string& msg)
    {
        if (shared->failed.exchange(true))
            return;
        auto resp = HttpResponse::newHttpJsonResponse(Json::Value(msg));
        resp->setStatusCode(k500InternalServerError);
        cb(resp);
    };

    // We have 5 async queries
    shared->pending.store(5);

    // Cameras
    db_->execSqlAsync(
        "SELECT COUNT(*) AS count FROM cameras WHERE status='active'",
        [shared, finish](const Result& r)
        {
            shared->body["cameras"] = static_cast<Json::Int64>(getInt64(r, 0, "count"));
            finish();
        },
        [fail](const std::exception_ptr& e)
        {
            try
            {
                if (e)
                    std::rethrow_exception(e);
            }
            catch (const std::exception& ex)
            {
                fail(std::string("DB error (cameras): ") + ex.what());
                return;
            }
            fail("DB error (cameras)");
        });

    // Detections today
    db_->execSqlAsync(
        "SELECT COUNT(*) AS count FROM events "
        "WHERE event_type='detect_start' "
        "AND timestamp >= date_trunc('day', now())",
        [shared, finish](const Result& r)
        {
            shared->body["detectionsToday"] = static_cast<Json::Int64>(getInt64(r, 0, "count"));
            finish();
        },
        [fail](const std::exception_ptr& e)
        {
            try
            {
                if (e)
                    std::rethrow_exception(e);
            }
            catch (const std::exception& ex)
            {
                fail(std::string("DB error (detections): ") + ex.what());
                return;
            }
            fail("DB error (detections)");
        });

    // Alerts
    db_->execSqlAsync(
        "SELECT COUNT(*) AS count FROM alerts WHERE resolved = false",
        [shared, finish](const Result& r)
        {
            shared->body["alerts"] = static_cast<Json::Int64>(getInt64(r, 0, "count"));
            finish();
        },
        [fail](const std::exception_ptr& e)
        {
            try
            {
                if (e)
                    std::rethrow_exception(e);
            }
            catch (const std::exception& ex)
            {
                fail(std::string("DB error (alerts): ") + ex.what());
                return;
            }
            fail("DB error (alerts)");
        });

    // Detections by hour
    db_->execSqlAsync(
        "SELECT EXTRACT(HOUR FROM timestamp) AS hour, COUNT(*) AS count "
        "FROM events "
        "WHERE event_type='detect_start' "
        "AND timestamp >= date_trunc('day', now()) "
        "GROUP BY 1",
        [shared, finish](const Result& r)
        {
            for (auto row : r)
            {
                int hour = row["hour"].as<int>();
                int count = row["count"].as<int>();
                if (hour >= 0 && hour < static_cast<int>(shared->hourly.size()))
                    shared->hourly[static_cast<size_t>(hour)] = count;
            }
            finish();
        },
        [fail](const std::exception_ptr& e)
        {
            try
            {
                if (e)
                    std::rethrow_exception(e);
            }
            catch (const std::exception& ex)
            {
                fail(std::string("DB error (hourly): ") + ex.what());
                return;
            }
            fail("DB error (hourly)");
        });

    // Recent events
    db_->execSqlAsync(
        "SELECT to_char(e.timestamp, 'HH24:MI') AS time, e.event_type, COALESCE(c.name, '') AS camera "
        "FROM events e "
        "LEFT JOIN cameras c ON e.camera_id = c.id "
        "ORDER BY e.timestamp DESC "
        "LIMIT 20",
        [shared, finish](const Result& r)
        {
            for (auto row : r)
            {
                Json::Value ev(Json::objectValue);
                ev["time"] = row["time"].as<std::string>();
                ev["event"] = row["event_type"].as<std::string>();
                ev["camera"] = row["camera"].as<std::string>();
                shared->events.append(ev);
            }
            finish();
        },
        [fail](const std::exception_ptr& e)
        {
            try
            {
                if (e)
                    std::rethrow_exception(e);
            }
            catch (const std::exception& ex)
            {
                fail(std::string("DB error (events): ") + ex.what());
                return;
            }
            fail("DB error (events)");
        });
}

bool DashboardController::aiActive() const
{
    const char* env = std::getenv("AI_ENGINE_ENABLED");
    if (!env)
        return true;

    std::string v = env;
    for (auto& c : v)
        c = static_cast<char>(std::tolower(c));

    return !(v == "0" || v == "false" || v == "no");
}
