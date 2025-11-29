#include "PersonController.h"
#include <drogon/MultiPart.h>
#include <drogon/drogon.h>
#include <json/json.h>
#include "../dto/EntityDtos.h"

namespace
{
const std::vector<std::string>& personCreateColumns()
{
    static const std::vector<std::string> cols = {
        "id",
        "registered_at",
        "name",
        "role",
        "image_url",
        "authorized",
        "last_seen"};
    return cols;
}

const std::vector<std::string>& personUpdateColumns()
{
    static const std::vector<std::string> cols = {
        "registered_at",
        "name",
        "role",
        "image_url",
        "authorized",
        "last_seen"};
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

PersonController::PersonController(const drogon::orm::DbClientPtr& db)
    : TableControllerBase(db,
                          "persons",
                          "id",
                          dto::PersonDto::columns(),
                          dto::PersonDto::fromRow),
      storageService_(std::make_shared<StorageService>())
{
}

void PersonController::getPersons(
    const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    fetchTable(req, std::move(callback));
}

void PersonController::createPerson(
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
        tableRepository_->insert(*json, personCreateColumns(), onSuccess, onError);
    }
    catch (const std::exception& ex)
    {
        cb(makeErrorResponse(drogon::k400BadRequest, ex.what()));
    }
}

void PersonController::updatePerson(
    const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback,
    std::string id)
{
    if (id.empty())
    {
        callback(makeErrorResponse(drogon::k400BadRequest, "Person id is required"));
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
            cb(makeErrorResponse(drogon::k404NotFound, "Person not found"));
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
        tableRepository_->updateById(id, *json, personUpdateColumns(), onSuccess, onError);
    }
    catch (const std::exception& ex)
    {
        cb(makeErrorResponse(drogon::k400BadRequest, ex.what()));
    }
}

void PersonController::deletePerson(
    const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback,
    std::string id)
{
    (void)req;
    if (id.empty())
    {
        callback(makeErrorResponse(drogon::k400BadRequest, "Person id is required"));
        return;
    }

    auto cb = std::move(callback);
    auto onSuccess = [cb, repo = tableRepository_](const drogon::orm::Result& res) mutable
    {
        if (res.empty())
        {
            cb(makeErrorResponse(drogon::k404NotFound, "Person not found"));
            return;
        }

        Json::Value body;
        body["deleted"] = true;
        body["person"] = repo->toJsonObject(res);

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

void PersonController::uploadAvatar(
    const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback,
    std::string id)
{
    if (id.empty())
    {
        callback(makeErrorResponse(drogon::k400BadRequest, "Person id is required"));
        return;
    }

    if (!storageService_ || !storageService_->isConfigured())
    {
        callback(makeErrorResponse(drogon::k500InternalServerError, "Storage service is not configured"));
        return;
    }

    drogon::MultiPartParser parser;
    if (parser.parse(req) != 0)
    {
        callback(makeErrorResponse(drogon::k400BadRequest, "Invalid multipart/form-data payload"));
        return;
    }

    const auto& files = parser.getFiles();
    if (files.empty())
    {
        callback(makeErrorResponse(drogon::k400BadRequest, "Avatar file is missing"));
        return;
    }

    drogon::HttpFile file = files.front();
    auto cb = std::move(callback);
    auto personId = std::move(id);

    storageService_->uploadAvatar(
        file,
        [this, cb, personId](const std::string& url) mutable
        {
            Json::Value payload;
            payload["image_url"] = url;

            auto onSuccess = [cb, repo = tableRepository_](const drogon::orm::Result& res) mutable
            {
                if (res.empty())
                {
                    cb(makeErrorResponse(drogon::k404NotFound, "Person not found"));
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

            tableRepository_->updateById(personId, payload, personUpdateColumns(), onSuccess, onError);
        },
        [cb](const std::string& message) mutable
        {
            cb(makeErrorResponse(drogon::k502BadGateway, message));
        });
}
