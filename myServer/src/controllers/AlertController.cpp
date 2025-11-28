#include "AlertController.h"
#include "../dto/EntityDtos.h"

AlertController::AlertController(const drogon::orm::DbClientPtr& db)
    : TableControllerBase(db,
                          "alerts",
                          "created_at",
                          dto::AlertDto::columns(),
                          dto::AlertDto::fromRow)
{
}

void AlertController::getAlerts(
    const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    fetchTable(req, std::move(callback));
}
