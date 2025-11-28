#include "CameraController.h"
#include "../dto/EntityDtos.h"

CameraController::CameraController(const drogon::orm::DbClientPtr& db)
    : TableControllerBase(db,
                          "cameras",
                          "id",
                          dto::CameraDto::columns(),
                          dto::CameraDto::fromRow)
{
}

void CameraController::getCameras(
    const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    fetchTable(req, std::move(callback));
}
