#pragma once
#include <drogon/HttpController.h>
#include "TableControllerBase.h"

class CameraController : public drogon::HttpController<CameraController, false>, protected TableControllerBase
{
public:
    METHOD_LIST_BEGIN
        ADD_METHOD_TO(CameraController::getCameras, "/api/cameras", drogon::Get, "AuthFilter");
    METHOD_LIST_END

    explicit CameraController(const drogon::orm::DbClientPtr& db);

    void getCameras(const drogon::HttpRequestPtr& req,
                    std::function<void(const drogon::HttpResponsePtr&)>&& callback);
};
