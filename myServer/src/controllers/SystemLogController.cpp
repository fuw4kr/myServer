#include "SystemLogController.h"

SystemLogController::SystemLogController(const drogon::orm::DbClientPtr& db)
    : TableControllerBase(db, "system_logs", "created_at")
{
}

void SystemLogController::getSystemLogs(
    const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    fetchTable(req, std::move(callback));
}
