#pragma once
#include <drogon/HttpController.h>
#include "TableControllerBase.h"

class CameraController : public drogon::HttpController<CameraController, false>, protected TableControllerBase
{
public:
    METHOD_LIST_BEGIN
        ADD_METHOD_TO(CameraController::getCameras, "/api/cameras", drogon::Get, "AuthFilter");
        ADD_METHOD_TO(CameraController::createCamera, "/api/cameras", drogon::Post, "AuthFilter");
        ADD_METHOD_TO(CameraController::updateCamera, "/api/cameras/{1}", drogon::Put, drogon::Patch, "AuthFilter");
        ADD_METHOD_TO(CameraController::deleteCamera, "/api/cameras/{1}", drogon::Delete, "AuthFilter");
    METHOD_LIST_END

    explicit CameraController(const drogon::orm::DbClientPtr& db);

    void getCameras(const drogon::HttpRequestPtr& req,
                    std::function<void(const drogon::HttpResponsePtr&)>&& callback);

    void createCamera(const drogon::HttpRequestPtr& req,
                      std::function<void(const drogon::HttpResponsePtr&)>&& callback);

    void updateCamera(const drogon::HttpRequestPtr& req,
                      std::function<void(const drogon::HttpResponsePtr&)>&& callback,
                      std::string id);

    void deleteCamera(const drogon::HttpRequestPtr& req,
                      std::function<void(const drogon::HttpResponsePtr&)>&& callback,
                      std::string id);
};
