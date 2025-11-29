/**
 * @brief EmbeddingController implementation for listing and creating embeddings.
 */
#include "EmbeddingController.h"
#include <drogon/drogon.h>
#include <json/json.h>
#include "../dto/EntityDtos.h"

namespace
{
const std::vector<std::string>& embeddingCreateColumns()
{
    static const std::vector<std::string> cols = {
        "id",
        "person_id",
        "model_name",
        "vector",
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

EmbeddingController::EmbeddingController(const drogon::orm::DbClientPtr& db)
    : TableControllerBase(db,
                          "embeddings",
                          "created_at",
                          dto::EmbeddingDto::columns(),
                          dto::EmbeddingDto::fromRow)
{
}

/**
 * @brief Returns recent embeddings.
 * @param req Incoming request.
 * @param callback Response callback.
 */
void EmbeddingController::getEmbeddings(
    const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    fetchTable(req, std::move(callback));
}

/**
 * @brief Inserts a new embedding row.
 * @param req Incoming request with JSON payload.
 * @param callback Response callback.
 */
void EmbeddingController::createEmbedding(
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
        tableRepository_->insert(*json, embeddingCreateColumns(), onSuccess, onError);
    }
    catch (const std::exception& ex)
    {
        cb(makeErrorResponse(drogon::k400BadRequest, ex.what()));
    }
}
