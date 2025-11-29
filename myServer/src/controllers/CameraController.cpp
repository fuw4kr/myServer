/**
 * @brief CameraController implementation for CRUD operations on cameras.
 */
#include "CameraController.h"
#include <drogon/drogon.h>
#include <json/json.h>
#include "../dto/EntityDtos.h"

namespace
{
const std::vector<std::string>& cameraCreateColumns()
{
    static const std::vector<std::string> cols = {
        "id",
        "name",
        "ip_address",
        "location",
        "status",
        "stream_url",
        "created_at"};
    return cols;
}

const std::vector<std::string>& cameraUpdateColumns()
{
    static const std::vector<std::string> cols = {
        "name",
        "ip_address",
        "location",
        "status",
        "stream_url",
        "created_at"};
    return cols;
}

drogon::HttpResponsePtr makeErrorResponse(drogon::HttpStatusCode code, const std::string& message)
{
    Json::Value body;
    body["error"] = message;
    auto resp = drogon::HttpResponse::newHttpJsonResponse(body);
    resp->setStatusCode(code);
    return resp;
}
} // namespace

CameraController::CameraController(const drogon::orm::DbClientPtr& db)
    : TableControllerBase(db,
                          "cameras",
                          "id",
                          dto::CameraDto::columns(),
                          dto::CameraDto::fromRow)
{
}

/**
 * @brief Returns the list of cameras.
 * @param req Incoming request.
 * @param callback Response callback.
 */
void CameraController::getCameras(
    const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    fetchTable(req, std::move(callback));
}

/**
 * @brief Creates a new camera record.
 * @param req Incoming request with JSON payload.
 * @param callback Response callback.
 */
void CameraController::createCamera(
    const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    auto json = req->getJsonObject();
    if (!json || !json->isObject())
    {
        callback(makeErrorResponse(drogon::k400BadRequest, "Body must be a JSON object"));
        return;
    }

    auto cb = std::move(callback);
    auto onSuccess = [cb, repo = tableRepository_](const drogon::orm::Result& res) mutable
    {
        if (res.empty())
        {
            cb(makeErrorResponse(drogon::k500InternalServerError, "Insert returned no data"));
            return;
        }

        auto resp = drogon::HttpResponse::newHttpJsonResponse(repo->toJsonObject(res));
        resp->setStatusCode(drogon::k201Created);
        cb(resp);
    };

    auto onError = [cb](const std::exception_ptr& ex) mutable
    {
        std::string msg = "Database error";
        try
        {
            if (ex)
                std::rethrow_exception(ex);
        }
        catch (const std::exception& e)
        {
            msg = e.what();
        }
        cb(makeErrorResponse(drogon::k500InternalServerError, msg));
    };

    try
    {
        tableRepository_->insert(*json, cameraCreateColumns(), onSuccess, onError);
    }
    catch (const std::exception& ex)
    {
        cb(makeErrorResponse(drogon::k400BadRequest, ex.what()));
    }
}

/**
 * @brief Updates an existing camera by id.
 * @param req Incoming request with JSON payload.
 * @param callback Response callback.
 * @param id Camera identifier.
 */
void CameraController::updateCamera(
    const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback,
    std::string id)
{
    if (id.empty())
    {
        callback(makeErrorResponse(drogon::k400BadRequest, "Camera id is required"));
        return;
    }

    auto json = req->getJsonObject();
    if (!json || !json->isObject())
    {
        callback(makeErrorResponse(drogon::k400BadRequest, "Body must be a JSON object"));
        return;
    }

    auto cb = std::move(callback);
    auto onSuccess = [cb, repo = tableRepository_](const drogon::orm::Result& res) mutable
    {
        if (res.empty())
        {
            cb(makeErrorResponse(drogon::k404NotFound, "Camera not found"));
            return;
        }

        auto resp = drogon::HttpResponse::newHttpJsonResponse(repo->toJsonObject(res));
        resp->setStatusCode(drogon::k200OK);
        cb(resp);
    };

    auto onError = [cb](const std::exception_ptr& ex) mutable
    {
        std::string msg = "Database error";
        try
        {
            if (ex)
                std::rethrow_exception(ex);
        }
        catch (const std::exception& e)
        {
            msg = e.what();
        }
        cb(makeErrorResponse(drogon::k500InternalServerError, msg));
    };

    try
    {
        tableRepository_->updateById(id, *json, cameraUpdateColumns(), onSuccess, onError);
    }
    catch (const std::exception& ex)
    {
        cb(makeErrorResponse(drogon::k400BadRequest, ex.what()));
    }
}

/**
 * @brief Deletes a camera by id.
 * @param req Incoming request (unused).
 * @param callback Response callback.
 * @param id Camera identifier.
 */
void CameraController::deleteCamera(
    const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback,
    std::string id)
{
    (void)req;
    if (id.empty())
    {
        callback(makeErrorResponse(drogon::k400BadRequest, "Camera id is required"));
        return;
    }

    auto cb = std::move(callback);
    auto onSuccess = [cb, repo = tableRepository_](const drogon::orm::Result& res) mutable
    {
        if (res.empty())
        {
            cb(makeErrorResponse(drogon::k404NotFound, "Camera not found"));
            return;
        }

        Json::Value body;
        body["deleted"] = true;
        body["camera"] = repo->toJsonObject(res);

        auto resp = drogon::HttpResponse::newHttpJsonResponse(body);
        resp->setStatusCode(drogon::k200OK);
        cb(resp);
    };

    auto onError = [cb](const std::exception_ptr& ex) mutable
    {
        std::string msg = "Database error";
        try
        {
            if (ex)
                std::rethrow_exception(ex);
        }
        catch (const std::exception& e)
        {
            msg = e.what();
        }
        cb(makeErrorResponse(drogon::k500InternalServerError, msg));
    };

    tableRepository_->deleteById(id, onSuccess, onError);
}
