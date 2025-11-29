/**
 * @brief SystemLogController implementation for listing system logs.
 */
#include "SystemLogController.h"
#include "../dto/EntityDtos.h"

SystemLogController::SystemLogController(const drogon::orm::DbClientPtr& db)
    : TableControllerBase(db,
                          "system_logs",
                          "created_at",
                          dto::SystemLogDto::columns(),
                          dto::SystemLogDto::fromRow)
{
}

/**
 * @brief Returns recent system log entries.
 * @param req Incoming request.
 * @param callback Response callback.
 */
void SystemLogController::getSystemLogs(
    const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    fetchTable(req, std::move(callback));
}
