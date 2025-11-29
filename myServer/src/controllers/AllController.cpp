/**
 * @brief Aggregates latest records from multiple repositories into one payload.
 */
#include "AllController.h"
#include <drogon/drogon.h>
#include <stdexcept>
#include "../dto/EntityDtos.h"

using namespace drogon;
using namespace drogon::orm;

AllController::AllController(const DbClientPtr& db)
    : personsRepo_(TableRepository::forDto<dto::PersonDto>(db, "persons", "id", 20)),
      camerasRepo_(TableRepository::forDto<dto::CameraDto>(db, "cameras", "id", 20)),
      eventsRepo_(TableRepository::forDto<dto::EventDto>(db, "events", "timestamp", 20)),
      alertsRepo_(TableRepository::forDto<dto::AlertDto>(db, "alerts", "created_at", 20)),
      systemLogsRepo_(TableRepository::forDto<dto::SystemLogDto>(db, "system_logs", "created_at", 20)),
      embeddingsRepo_(TableRepository::forDto<dto::EmbeddingDto>(db, "embeddings", "created_at", 20))
{
}

/**
 * @brief Returns combined latest rows for persons, cameras, events, alerts, system logs, and embeddings.
 * @param req Incoming request (unused).
 * @param callback Response callback with aggregated JSON.
 */
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
        all["persons"] = personsRepo_.toJsonArray(persons.get());
        all["cameras"] = camerasRepo_.toJsonArray(cameras.get());
        all["events"] = eventsRepo_.toJsonArray(events.get());
        all["alerts"] = alertsRepo_.toJsonArray(alerts.get());
        all["system_logs"] = systemLogsRepo_.toJsonArray(systemLogs.get());
        all["embeddings"] = embeddingsRepo_.toJsonArray(embeddings.get());

        callback(HttpResponse::newHttpJsonResponse(all));
    }
    catch (const std::exception& ex)
    {
        auto resp = HttpResponse::newHttpJsonResponse(ex.what());
        resp->setStatusCode(k500InternalServerError);
        callback(resp);
    }
}
