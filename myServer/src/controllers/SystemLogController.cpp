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

void SystemLogController::getSystemLogs(
    const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    fetchTable(req, std::move(callback));
}
