#include "CameraController.h"

CameraController::CameraController(const drogon::orm::DbClientPtr& db)
    : TableControllerBase(db, "cameras", "id")
{
}

void CameraController::getCameras(
    const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    fetchTable(req, std::move(callback));
}
