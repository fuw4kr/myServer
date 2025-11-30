#include "StatsController.h"

#include <drogon/drogon.h>
#include <json/json.h>
#include <array>
#include <atomic>
#include <cctype>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <optional>
#include <sstream>
#include <string>

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

std::string todayAsDateString()
{
    std::time_t now = std::time(nullptr);
    std::tm tm {};
#ifdef _WIN32
    localtime_s(&tm, &now);
#else
    localtime_r(&now, &tm);
#endif
    char buf[11] = {};
    if (std::strftime(buf, sizeof(buf), "%Y-%m-%d", &tm) == 0)
        return {};
    return std::string(buf);
}

int parseQueryInt(const HttpRequestPtr& req,
                  const std::string& name,
                  int defaultValue,
                  int minValue,
                  int maxValue)
{
    try
    {
        const auto valueStr = req->getParameter(name);
        if (valueStr.empty())
            return defaultValue;

        int value = std::stoi(valueStr);
        if (value < minValue)
            return minValue;
        if (value > maxValue)
            return maxValue;
        return value;
    }
    catch (...)
    {
        return defaultValue;
    }
}

std::optional<std::string> parseDateYmd(const std::string& value)
{
    std::tm tm{};
    std::istringstream iss(value);
    iss >> std::get_time(&tm, "%Y-%m-%d");

    // Reject if parsing failed or extra characters remain
    if (iss.fail() || (iss.peek() != std::char_traits<char>::eof()))
        return std::nullopt;

    char buf[11] = {};
    if (std::strftime(buf, sizeof(buf), "%Y-%m-%d", &tm) == 0)
        return std::nullopt;

    return std::string(buf);
}
} // namespace

StatsController::StatsController(const drogon::orm::DbClientPtr& db)
    : db_(db)
{
}

void StatsController::getSummary(
    const HttpRequestPtr& req,
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
        std::atomic<int> pending{0};
        std::atomic<bool> failed{false};
    };

    auto shared = std::make_shared<Shared>();
    shared->body["aiActive"] = aiActive();
    shared->pending.store(3);

    auto finish = [shared, cb]()
    {
        if (shared->failed.load())
            return;

        int remaining = shared->pending.fetch_sub(1) - 1;
        if (remaining > 0)
            return;

        auto resp = HttpResponse::newHttpJsonResponse(shared->body);
        resp->setStatusCode(k200OK);
        cb(resp);
    };

    auto fail = [shared, cb](const std::string& msg)
    {
        if (shared->failed.exchange(true))
            return;
        auto resp = HttpResponse::newHttpJsonResponse(Json::Value(msg));
        resp->setStatusCode(k500InternalServerError);
        cb(resp);
    };

    db_->execSqlAsync(
        "SELECT COUNT(*) AS count FROM cameras WHERE status='active'",
        [shared, finish](const Result& r)
        {
            shared->body["activeCameras"] = static_cast<Json::Int64>(getInt64(r, 0, "count"));
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
}

void StatsController::getDetectionsByHour(
    const HttpRequestPtr& req,
    std::function<void(const HttpResponsePtr&)>&& callback)
{
    auto cb = std::move(callback);
    if (!db_)
    {
        auto resp = HttpResponse::newHttpJsonResponse(Json::Value("Database is not configured"));
        resp->setStatusCode(k500InternalServerError);
        cb(resp);
        return;
    }

    const std::string dateParam = req->getParameter("date");
    const bool hasDate = !dateParam.empty();

    std::optional<std::string> parsedDate;
    if (hasDate)
    {
        parsedDate = parseDateYmd(dateParam);
        if (!parsedDate)
        {
            auto resp = HttpResponse::newHttpJsonResponse(Json::Value("Invalid date, expected YYYY-MM-DD"));
            resp->setStatusCode(k400BadRequest);
            cb(resp);
            return;
        }
    }
    else
    {
        parsedDate = todayAsDateString();
    }

    const std::string effectiveDate = *parsedDate;

    auto onSuccess = [cb, effectiveDate](const Result& r)
    {
        std::array<int, 24> hourly{};
        for (auto row : r)
        {
            const int hour = row["hour"].as<int>();
            const int count = row["count"].as<int>();
            if (hour >= 0 && hour < static_cast<int>(hourly.size()))
                hourly[static_cast<size_t>(hour)] = count;
        }

        Json::Value counts(Json::arrayValue);
        for (int v : hourly)
            counts.append(v);

        Json::Value body(Json::objectValue);
        body["date"] = effectiveDate;
        body["detectionsByHour"] = counts;

        auto resp = HttpResponse::newHttpJsonResponse(body);
        resp->setStatusCode(k200OK);
        cb(resp);
    };

    auto onError = [cb](const std::exception_ptr& e)
    {
        std::string message = "DB error (detections-by-hour)";
        try
        {
            if (e)
                std::rethrow_exception(e);
        }
        catch (const std::exception& ex)
        {
            message += ": ";
            message += ex.what();
        }

        auto resp = HttpResponse::newHttpJsonResponse(Json::Value(message));
        resp->setStatusCode(k500InternalServerError);
        cb(resp);
    };

    if (hasDate)
    {
        db_->execSqlAsync(
            "SELECT EXTRACT(HOUR FROM timestamp) AS hour, COUNT(*) AS count "
            "FROM events "
            "WHERE event_type='detect_start' "
            "AND timestamp >= $1::date "
            "AND timestamp < ($1::date + INTERVAL '1 day') "
            "GROUP BY 1",
            onSuccess,
            onError,
            effectiveDate);
    }
    else
    {
        db_->execSqlAsync(
            "SELECT EXTRACT(HOUR FROM timestamp) AS hour, COUNT(*) AS count "
            "FROM events "
            "WHERE event_type='detect_start' "
            "AND timestamp >= date_trunc('day', now()) "
            "GROUP BY 1",
            onSuccess,
            onError);
    }
}

void StatsController::getEvents(
    const HttpRequestPtr& req,
    std::function<void(const HttpResponsePtr&)>&& callback)
{
    auto cb = std::move(callback);
    if (!db_)
    {
        auto resp = HttpResponse::newHttpJsonResponse(Json::Value("Database is not configured"));
        resp->setStatusCode(k500InternalServerError);
        cb(resp);
        return;
    }

    const int limit = parseQueryInt(req, "limit", 100, 1, 500);
    const int offset = parseQueryInt(req, "offset", 0, 0, 1000000);

    // Build query string with validated numeric limit/offset to avoid driver serialization issues.
    const std::string sql =
        "SELECT e.timestamp, e.event_type, e.camera_id, COALESCE(c.name, '') AS camera_name "
        "FROM events e "
        "LEFT JOIN cameras c ON e.camera_id = c.id "
        "ORDER BY e.timestamp DESC "
        "LIMIT " +
        std::to_string(limit) +
        " OFFSET " +
        std::to_string(offset);

    db_->execSqlAsync(
        sql,
        [cb, limit, offset](const Result& r)
        {
            Json::Value events(Json::arrayValue);
            for (auto row : r)
            {
                Json::Value ev(Json::objectValue);
                ev["timestamp"] = row["timestamp"].isNull() ? "" : row["timestamp"].as<std::string>();
                ev["eventType"] = row["event_type"].isNull() ? "" : row["event_type"].as<std::string>();
                ev["cameraName"] = row["camera_name"].isNull() ? "" : row["camera_name"].as<std::string>();
                if (!row["camera_id"].isNull())
                    ev["cameraId"] = row["camera_id"].as<std::string>();
                events.append(ev);
            }

            Json::Value body(Json::objectValue);
            body["limit"] = limit;
            body["offset"] = offset;
            body["events"] = events;

            auto resp = HttpResponse::newHttpJsonResponse(body);
            resp->setStatusCode(k200OK);
            cb(resp);
        },
        [cb](const std::exception_ptr& e)
        {
            std::string message = "DB error (events)";
            try
            {
                if (e)
                    std::rethrow_exception(e);
            }
            catch (const std::exception& ex)
            {
                message += ": ";
                message += ex.what();
            }

            auto resp = HttpResponse::newHttpJsonResponse(Json::Value(message));
            resp->setStatusCode(k500InternalServerError);
            cb(resp);
        });
}

bool StatsController::aiActive() const
{
    const char* env = std::getenv("AI_ENGINE_ENABLED");
    if (!env)
        return true;

    std::string v = env;
    for (auto& c : v)
        c = static_cast<char>(std::tolower(c));

    return !(v == "0" || v == "false" || v == "no");
}
