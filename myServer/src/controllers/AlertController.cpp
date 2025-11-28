#include "AlertController.h"

AlertController::AlertController(const drogon::orm::DbClientPtr& db)
    : TableControllerBase(db, "alerts", "created_at")
{
}

void AlertController::getAlerts(
    const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    fetchTable(req, std::move(callback));
}
