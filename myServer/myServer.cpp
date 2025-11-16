#include <drogon/drogon.h>
#include <drogon/orm/DbClient.h>
#include <json/json.h>
#include "AuthFilter.h"
#include "src/controllers/AuthController.h"

using namespace drogon;
using namespace drogon::orm;

int main()
{
    LOG_INFO << "Starting Drogon server...";

    // === Load DB URL ===
    const char* dbUrl = std::getenv("SUPABASE_DB_URL");
    if (!dbUrl || std::string(dbUrl).empty())
    {
        LOG_ERROR << "SUPABASE_DB_URL is not set or empty!";
        return 1;
    }
    LOG_INFO << "SUPABASE_DB_URL loaded";

    // === Create DB client ===
    auto db = drogon::orm::DbClient::newPgClient(dbUrl, 2);

    // === Test DB connection ===
    db->execSqlAsync(
        "SELECT now()",
        [](const Result& r) {
            LOG_INFO << "DB connected, time: " << r[0]["now"].as<std::string>();
        },
        [](const std::exception_ptr& e) {
            try { if (e) std::rethrow_exception(e); }
            catch (const std::exception& ex) {
                LOG_ERROR << "DB connection failed: " << ex.what();
            }
        }
    );

    // === Register AuthFilter globally ===
    app().registerFilter(std::make_shared<AuthFilter>(db));

    // === Register AuthController for /auth/login ===
    app().registerController(std::make_shared<AuthController>(db));

    // ================================================================
    // ========================= PUBLIC ROOT ===========================
    // ================================================================
    app().registerHandler(
        "/",
        [](const HttpRequestPtr&, std::function<void(const HttpResponsePtr&)>&& cb)
        {
            Json::Value info;
            info["status"] = "ok";

            info["endpoints"] = Json::arrayValue;
            info["endpoints"].append("/auth/login");
            info["endpoints"].append("/api/persons");
            info["endpoints"].append("/api/cameras");
            info["endpoints"].append("/api/events");
            info["endpoints"].append("/api/alerts");
            info["endpoints"].append("/api/system_logs");
            info["endpoints"].append("/api/embeddings");
            info["endpoints"].append("/api/all");

            cb(HttpResponse::newHttpJsonResponse(info));
        },
        { Get },
        { "AuthFilter" }
    );

    // ================================================================
    // ================== GENERIC TABLE HANDLER ========================
    // ================================================================
    auto makeHandler = [db](const std::string& table, const std::string& orderBy, int limit = 20)
        {
            return [db, table, orderBy, limit](const HttpRequestPtr&, std::function<void(const HttpResponsePtr&)>&& cb)
                {
                    std::string query =
                        "SELECT * FROM " + table + " ORDER BY " + orderBy + " DESC LIMIT " + std::to_string(limit);

                    db->execSqlAsync(
                        query,
                        [cb](const Result& r)
                        {
                            Json::Value array(Json::arrayValue);

                            for (const auto& row : r)
                            {
                                Json::Value item;
                                for (int i = 0; i < r.columns(); i++)
                                {
                                    std::string colName = r.columnName(i);
                                    if (row[i].isNull())
                                        item[colName] = "";
                                    else
                                        item[colName] = row[i].as<std::string>();
                                }
                                array.append(item);
                            }

                            cb(HttpResponse::newHttpJsonResponse(array));
                        },
                        [cb](const std::exception_ptr& e)
                        {
                            std::string errMsg = "unknown error";
                            try { if (e) std::rethrow_exception(e); }
                            catch (const std::exception& ex) { errMsg = ex.what(); }

                            auto resp = HttpResponse::newHttpJsonResponse(errMsg);
                            resp->setStatusCode(k500InternalServerError);
                            cb(resp);
                        }
                    );
                };
        };

    // ================================================================
    // =================== PROTECTED API ROUTES ========================
    // ================================================================
    app().registerHandler("/api/persons", makeHandler("persons", "id"), { Get }, { "AuthFilter" });
    app().registerHandler("/api/cameras", makeHandler("cameras", "id"), { Get }, { "AuthFilter" });
    app().registerHandler("/api/events", makeHandler("events", "timestamp"), { Get }, { "AuthFilter" });
    app().registerHandler("/api/alerts", makeHandler("alerts", "created_at"), { Get }, { "AuthFilter" });
    app().registerHandler("/api/system_logs", makeHandler("system_logs", "created_at"), { Get }, { "AuthFilter" });
    app().registerHandler("/api/embeddings", makeHandler("embeddings", "created_at"), { Get }, { "AuthFilter" });

    // ================================================================
    // ========================= /api/all ==============================
    // ================================================================
    app().registerHandler(
        "/api/all",
        [db](const HttpRequestPtr&, std::function<void(const HttpResponsePtr&)>&& cb)
        {
            try {
                auto f1 = db->execSqlAsyncFuture("SELECT * FROM persons ORDER BY id DESC LIMIT 20");
                auto f2 = db->execSqlAsyncFuture("SELECT * FROM cameras ORDER BY id DESC LIMIT 20");
                auto f3 = db->execSqlAsyncFuture("SELECT * FROM events ORDER BY timestamp DESC LIMIT 20");
                auto f4 = db->execSqlAsyncFuture("SELECT * FROM alerts ORDER BY created_at DESC LIMIT 20");
                auto f5 = db->execSqlAsyncFuture("SELECT * FROM system_logs ORDER BY created_at DESC LIMIT 20");
                auto f6 = db->execSqlAsyncFuture("SELECT * FROM embeddings ORDER BY created_at DESC LIMIT 20");

                auto toArray = [](const Result& r) {
                    Json::Value array(Json::arrayValue);

                    for (const auto& row : r)
                    {
                        Json::Value item;
                        for (int i = 0; i < r.columns(); i++)
                        {
                            std::string colName = r.columnName(i);
                            if (row[i].isNull())
                                item[colName] = "";
                            else
                                item[colName] = row[i].as<std::string>();
                        }
                        array.append(item);
                    }
                    return array;
                    };

                Json::Value all;
                all["persons"] = toArray(f1.get());
                all["cameras"] = toArray(f2.get());
                all["events"] = toArray(f3.get());
                all["alerts"] = toArray(f4.get());
                all["system_logs"] = toArray(f5.get());
                all["embeddings"] = toArray(f6.get());

                cb(HttpResponse::newHttpJsonResponse(all));
            }
            catch (const std::exception& ex)
            {
                auto resp = HttpResponse::newHttpJsonResponse(ex.what());
                resp->setStatusCode(k500InternalServerError);
                cb(resp);
            }
        },
        { Get },
        { "AuthFilter" }
    );

    // ================================================================
    // ========================= START SERVER ==========================
    // ================================================================
    uint16_t port = 8080;
    if (const char* p = std::getenv("PORT"))
    {
        try { port = std::stoi(p); }
        catch (...) { port = 8080; }
    }

    LOG_INFO << "Listening on 0.0.0.0:" << port;

    app()
        .addListener("0.0.0.0", port)
        .setThreadNum(2)
        .run();

    return 0;
}
