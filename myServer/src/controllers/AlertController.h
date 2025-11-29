#pragma once
#include <drogon/HttpController.h>
#include "TableControllerBase.h"

/**
 * @brief CRUD entry points for alerts.
 *
 * GET /api/alerts returns the most recent alerts ordered by created_at.
 * POST /api/alerts inserts a new alert row.
 */
class AlertController : public drogon::HttpController<AlertController, false>, protected TableControllerBase
{
public:
    METHOD_LIST_BEGIN
        ADD_METHOD_TO(AlertController::getAlerts, "/api/alerts", drogon::Get, "AuthFilter");
        ADD_METHOD_TO(AlertController::createAlert, "/api/alerts", drogon::Post, "AuthFilter");
    METHOD_LIST_END

    explicit AlertController(const drogon::orm::DbClientPtr& db);

    /// @brief Lists recent alerts.
    /// @param req Incoming request.
    /// @param callback Response callback returning JSON.
    void getAlerts(const drogon::HttpRequestPtr& req,
                   std::function<void(const drogon::HttpResponsePtr&)>&& callback);

    /// @brief Creates a new alert record.
    /// @param req Incoming request with JSON payload.
    /// @param callback Response callback returning created alert or error JSON.
    void createAlert(const drogon::HttpRequestPtr& req,
                     std::function<void(const drogon::HttpResponsePtr&)>&& callback);
};
