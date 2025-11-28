#include "AllController.h"
#include <drogon/drogon.h>
#include <stdexcept>

using namespace drogon;
using namespace drogon::orm;

AllController::AllController(const DbClientPtr& db)
    : personsRepo_(db, "persons", "id", 20),
      camerasRepo_(db, "cameras", "id", 20),
      eventsRepo_(db, "events", "timestamp", 20),
      alertsRepo_(db, "alerts", "created_at", 20),
      systemLogsRepo_(db, "system_logs", "created_at", 20),
      embeddingsRepo_(db, "embeddings", "created_at", 20)
{
}

void AllController::getAll(
    const HttpRequestPtr& req,
    std::function<void(const HttpResponsePtr&)>&& callback)
{
    (void)req;
    try
    {
        auto persons = personsRepo_.fetchLatestAsync();
        auto cameras = camerasRepo_.fetchLatestAsync();
        auto events = eventsRepo_.fetchLatestAsync();
        auto alerts = alertsRepo_.fetchLatestAsync();
        auto systemLogs = systemLogsRepo_.fetchLatestAsync();
        auto embeddings = embeddingsRepo_.fetchLatestAsync();

        Json::Value all;
        all["persons"] = TableRepository::toJsonArray(persons.get());
        all["cameras"] = TableRepository::toJsonArray(cameras.get());
        all["events"] = TableRepository::toJsonArray(events.get());
        all["alerts"] = TableRepository::toJsonArray(alerts.get());
        all["system_logs"] = TableRepository::toJsonArray(systemLogs.get());
        all["embeddings"] = TableRepository::toJsonArray(embeddings.get());

        callback(HttpResponse::newHttpJsonResponse(all));
    }
    catch (const std::exception& ex)
    {
        auto resp = HttpResponse::newHttpJsonResponse(ex.what());
        resp->setStatusCode(k500InternalServerError);
        callback(resp);
    }
}
