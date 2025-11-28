#include "AllController.h"
#include <drogon/drogon.h>
#include <stdexcept>

using namespace drogon;
using namespace drogon::orm;

AllController::AllController(const DbClientPtr& db)
    : db_(db)
{
    if (!db_)
    {
        throw std::invalid_argument("AllController requires a valid DbClient");
    }
}

void AllController::getAll(
    const HttpRequestPtr& req,
    std::function<void(const HttpResponsePtr&)>&& callback)
{
    (void)req;
    try
    {
        auto persons = db_->execSqlAsyncFuture("SELECT * FROM persons ORDER BY id DESC LIMIT 20");
        auto cameras = db_->execSqlAsyncFuture("SELECT * FROM cameras ORDER BY id DESC LIMIT 20");
        auto events = db_->execSqlAsyncFuture("SELECT * FROM events ORDER BY timestamp DESC LIMIT 20");
        auto alerts = db_->execSqlAsyncFuture("SELECT * FROM alerts ORDER BY created_at DESC LIMIT 20");
        auto systemLogs = db_->execSqlAsyncFuture("SELECT * FROM system_logs ORDER BY created_at DESC LIMIT 20");
        auto embeddings = db_->execSqlAsyncFuture("SELECT * FROM embeddings ORDER BY created_at DESC LIMIT 20");

        Json::Value all;
        all["persons"] = toJsonArray(persons.get());
        all["cameras"] = toJsonArray(cameras.get());
        all["events"] = toJsonArray(events.get());
        all["alerts"] = toJsonArray(alerts.get());
        all["system_logs"] = toJsonArray(systemLogs.get());
        all["embeddings"] = toJsonArray(embeddings.get());

        callback(HttpResponse::newHttpJsonResponse(all));
    }
    catch (const std::exception& ex)
    {
        auto resp = HttpResponse::newHttpJsonResponse(ex.what());
        resp->setStatusCode(k500InternalServerError);
        callback(resp);
    }
}

Json::Value AllController::toJsonArray(const Result& result)
{
    Json::Value array(Json::arrayValue);

    for (const auto& row : result)
    {
        Json::Value item;
        for (int i = 0; i < result.columns(); i++)
        {
            const std::string columnName = result.columnName(i);
            if (row[i].isNull())
                item[columnName] = "";
            else
                item[columnName] = row[i].as<std::string>();
        }
        array.append(item);
    }

    return array;
}
