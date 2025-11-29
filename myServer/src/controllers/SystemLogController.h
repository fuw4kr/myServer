#pragma once
#include <drogon/HttpController.h>
#include "TableControllerBase.h"

/**
 * @brief Read-only access to recent system logs.
 *
 * GET /api/system_logs returns the latest log entries.
 *
 * @example
 * // HTTP: GET /api/system_logs
 */
class SystemLogController : public drogon::HttpController<SystemLogController, false>, protected TableControllerBase
{
public:
    METHOD_LIST_BEGIN
        ADD_METHOD_TO(SystemLogController::getSystemLogs, "/api/system_logs", drogon::Get, "AuthFilter");
    METHOD_LIST_END

    explicit SystemLogController(const drogon::orm::DbClientPtr& db);

    /// @brief Lists recent system logs.
    /// @param req Incoming request.
    /// @param callback Response callback returning JSON.
    void getSystemLogs(const drogon::HttpRequestPtr& req,
                       std::function<void(const drogon::HttpResponsePtr&)>&& callback);
};
