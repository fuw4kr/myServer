#pragma once
#include <drogon/HttpController.h>
#include "TableControllerBase.h"

/**
 * @brief CRUD endpoints for camera records.
 *
 * Provides listing, creation, update, and deletion under /api/cameras.
 *
 * @example
 * // HTTP: POST /api/cameras with JSON payload to create a camera.
 */
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

    /// @brief Lists cameras ordered by id.
    /// @param req Incoming request.
    /// @param callback Response callback returning JSON.
    void getCameras(const drogon::HttpRequestPtr& req,
                    std::function<void(const drogon::HttpResponsePtr&)>&& callback);

    /// @brief Creates a new camera row.
    /// @param req Incoming request with JSON payload.
    /// @param callback Response callback returning created camera or error.
    void createCamera(const drogon::HttpRequestPtr& req,
                      std::function<void(const drogon::HttpResponsePtr&)>&& callback);

    /// @brief Updates an existing camera by id.
    /// @param req Incoming request with JSON payload.
    /// @param callback Response callback.
    /// @param id Camera identifier path parameter.
    void updateCamera(const drogon::HttpRequestPtr& req,
                      std::function<void(const drogon::HttpResponsePtr&)>&& callback,
                      std::string id);

    /// @brief Deletes a camera by id.
    /// @param req Incoming request (unused).
    /// @param callback Response callback.
    /// @param id Camera identifier path parameter.
    void deleteCamera(const drogon::HttpRequestPtr& req,
                      std::function<void(const drogon::HttpResponsePtr&)>&& callback,
                      std::string id);
};
