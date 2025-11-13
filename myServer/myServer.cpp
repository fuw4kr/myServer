#include <drogon/drogon.h>
#include <drogon/orm/DbClient.h>
#include <json/json.h>

using namespace drogon;
using namespace drogon::orm;

int main()
{
    const char* dbUrl = std::getenv("SUPABASE_DB_URL");
    if (!dbUrl || std::string(dbUrl).empty())
    {
        LOG_ERROR << "SUPABASE_DB_URL is not set";
        return 1;
    }

    auto db = drogon::orm::DbClient::newPgClient(dbUrl, 5);

    db->execSqlAsync(
        "SELECT now()",
        [](const drogon::orm::Result& r) {
            LOG_INFO << "Connected OK " << r[0]["now"].as<std::string>();
        },
        [](const std::exception_ptr& e) {
            try {
                if (e) std::rethrow_exception(e);
            }
            catch (const std::exception& ex) {
                LOG_ERROR << "DB failed: " << ex.what();
            }
        }
    );

    // === Root endpoint ===
    app().registerHandler(
        "/",
        [](const HttpRequestPtr&, std::function<void(const HttpResponsePtr&)>&& cb) {
            Json::Value info;
            info["status"] = "ok";
            info["endpoints"] = Json::arrayValue;
            info["endpoints"].append("/api/persons");
            info["endpoints"].append("/api/cameras");
            info["endpoints"].append("/api/events");
            info["endpoints"].append("/api/alerts");
            info["endpoints"].append("/api/system_logs");
            info["endpoints"].append("/api/embeddings");
            info["endpoints"].append("/api/all");
            cb(HttpResponse::newHttpJsonResponse(info));
        },
        { Get }
    );

    auto makeHandler = [db](const std::string& table, const std::string& orderBy, int limit = 20) {
        return [db, table, orderBy, limit](const HttpRequestPtr&, std::function<void(const HttpResponsePtr&)>&& cb) {
            std::string query = "SELECT * FROM " + table + " ORDER BY " + orderBy + " DESC LIMIT " + std::to_string(limit);
            db->execSqlAsync(
                query,
                [cb](const Result& r) {
                    Json::Value arr(Json::arrayValue);
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
                        arr.append(item);
                    }
                    cb(HttpResponse::newHttpJsonResponse(arr));
                },
                [cb](const std::exception_ptr& e) {
                    std::string err = "unknown error";
                    try {
                        if (e) std::rethrow_exception(e);
                    }
                    catch (const std::exception& ex) {
                        err = ex.what();
                    }
                    auto resp = HttpResponse::newHttpJsonResponse(Json::Value(err));
                    resp->setStatusCode(k500InternalServerError);
                    cb(resp);
                }
            );
            };
        };

    app().registerHandler("/api/persons", makeHandler("persons", "id"), { Get });
    app().registerHandler("/api/cameras", makeHandler("cameras", "id"), { Get });
    app().registerHandler("/api/events", makeHandler("events", "timestamp"), { Get });
    app().registerHandler("/api/alerts", makeHandler("alerts", "created_at"), { Get });
    app().registerHandler("/api/system_logs", makeHandler("system_logs", "created_at"), { Get });
    app().registerHandler("/api/embeddings", makeHandler("embeddings", "created_at"), { Get });

    // === /api/all ===
    app().registerHandler(
        "/api/all",
        [db](const HttpRequestPtr&, std::function<void(const HttpResponsePtr&)>&& cb) {
            auto f1 = db->execSqlAsyncFuture("SELECT * FROM persons ORDER BY id DESC LIMIT 20");
            auto f2 = db->execSqlAsyncFuture("SELECT * FROM cameras ORDER BY id DESC LIMIT 20");
            auto f3 = db->execSqlAsyncFuture("SELECT * FROM events ORDER BY timestamp DESC LIMIT 20");
            auto f4 = db->execSqlAsyncFuture("SELECT * FROM alerts ORDER BY created_at DESC LIMIT 20");
            auto f5 = db->execSqlAsyncFuture("SELECT * FROM system_logs ORDER BY created_at DESC LIMIT 20");
            auto f6 = db->execSqlAsyncFuture("SELECT * FROM embeddings ORDER BY created_at DESC LIMIT 20");

            try {
                Result r1 = f1.get();
                Result r2 = f2.get();
                Result r3 = f3.get();
                Result r4 = f4.get();
                Result r5 = f5.get();
                Result r6 = f6.get();

                auto toArray = [](const Result& r) {
                    Json::Value arr(Json::arrayValue);
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
                        arr.append(item);
                    }
                    return arr;
                    };

                Json::Value all;
                all["persons"] = toArray(r1);
                all["cameras"] = toArray(r2);
                all["events"] = toArray(r3);
                all["alerts"] = toArray(r4);
                all["system_logs"] = toArray(r5);
                all["embeddings"] = toArray(r6);

                cb(HttpResponse::newHttpJsonResponse(all));
            }
            catch (const std::exception& ex) {
                auto resp = HttpResponse::newHttpJsonResponse(Json::Value(ex.what()));
                resp->setStatusCode(k500InternalServerError);
                cb(resp);
            }
        },
        { Get }
    );
   
    drogon::app()
        .addListener("0.0.0.0", 8080)
        .setThreadNum(2)
        .run();

    return 0;
}