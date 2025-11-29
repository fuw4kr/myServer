#pragma once
#include <drogon/HttpController.h>
#include <drogon/orm/DbClient.h>

/**
 * @brief Returns dashboard metrics and feature flags.
 *
 * GET /api/dashboard aggregates counts and AI status for the UI.
 *
 * @example
 * // HTTP: GET /api/dashboard
 */
class DashboardController : public drogon::HttpController<DashboardController, false>
{
public:
    METHOD_LIST_BEGIN
        ADD_METHOD_TO(DashboardController::getDashboard, "/api/dashboard", drogon::Get, "AuthFilter");
    METHOD_LIST_END

    explicit DashboardController(const drogon::orm::DbClientPtr& db);

    /// @brief Returns dashboard statistics payload.
    /// @param req Incoming request.
    /// @param callback Response callback returning JSON counts/status.
    void getDashboard(const drogon::HttpRequestPtr& req,
                      std::function<void(const drogon::HttpResponsePtr&)>&& callback);

private:
    drogon::orm::DbClientPtr db_;
    bool aiActive() const;
};
