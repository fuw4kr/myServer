#pragma once
#include <drogon/HttpController.h>
#include <drogon/orm/DbClient.h>
#include <json/json.h>
#include "../repositories/TableRepository.h"

/**
 * @brief Aggregates latest records from all major tables into one payload.
 *
 * GET /api/all returns persons, cameras, events, alerts, system logs, and embeddings.
 *
 * @example
 * // Invoke via HTTP: GET /api/all
 */
class AllController : public drogon::HttpController<AllController, false>
{
public:
    METHOD_LIST_BEGIN
        ADD_METHOD_TO(AllController::getAll, "/api/all", drogon::Get, "AuthFilter");
    METHOD_LIST_END

    explicit AllController(const drogon::orm::DbClientPtr& db);

    /// @brief Returns a combined JSON document with the latest rows from each repository.
    /// @param req Incoming request (unused).
    /// @param callback Response callback returning aggregated JSON.
    void getAll(const drogon::HttpRequestPtr& req,
                std::function<void(const drogon::HttpResponsePtr&)>&& callback);

private:
    TableRepository personsRepo_;
    TableRepository camerasRepo_;
    TableRepository eventsRepo_;
    TableRepository alertsRepo_;
    TableRepository systemLogsRepo_;
    TableRepository embeddingsRepo_;
};
