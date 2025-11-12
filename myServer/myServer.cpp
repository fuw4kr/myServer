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
        LOG_ERROR << " SUPABASE_DB_URL is not set";
        return 1;
    }

    const char* serviceRole = std::getenv("SUPABASE_SERVICE_ROLE");
    if (!serviceRole)
        serviceRole = "";

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


    app()
        .addListener("0.0.0.0", 8080) 
        .setThreadNum(2);

    app().registerHandler(
        "/",
        [](const HttpRequestPtr&, std::function<void(const HttpResponsePtr&)>&& cb) {
            cb(HttpResponse::newHttpJsonResponse(Json::Value("ok")));
        },
        { Get });

    app().registerHandler(
        "/api/messages",
        [db](const HttpRequestPtr&, std::function<void(const HttpResponsePtr&)>&& cb) {
            db->execSqlAsync(
                "SELECT id, content, created_at FROM public.messages ORDER BY id DESC LIMIT 10",
                [cb](const Result& r) {
                    Json::Value arr(Json::arrayValue);
                    for (auto const& row : r)
                    {
                        Json::Value item;
                        item["id"] = row["id"].as<long long>();
                        item["content"] = row["content"].as<std::string>();
                        if (!row["created_at"].isNull())
                            item["created_at"] = row["created_at"].as<std::string>();
                        arr.append(item);
                    }
                    cb(HttpResponse::newHttpJsonResponse(arr));
                },
                [cb](const std::exception_ptr& e) {   
                    std::string errMsg = "unknown error";
                    try {
                        if (e) std::rethrow_exception(e);
                    }
                    catch (const std::exception& ex) {
                        errMsg = ex.what();
                    }
                    auto resp = HttpResponse::newHttpJsonResponse(Json::Value(errMsg));
                    resp->setStatusCode(k500InternalServerError);
                    cb(resp);
                }
            );
        }
    ),

    app().run();
    return 0;
}
