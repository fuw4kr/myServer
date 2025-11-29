#pragma once
#include <drogon/HttpController.h>
#include "TableControllerBase.h"

/**
 * @brief Manages embedding records associated with persons.
 *
 * GET /api/embeddings lists recent embeddings; POST /api/embeddings inserts one.
 *
 * @example
 * // HTTP: POST /api/embeddings with JSON body to create an embedding.
 */
class EmbeddingController : public drogon::HttpController<EmbeddingController, false>, protected TableControllerBase
{
public:
    METHOD_LIST_BEGIN
        ADD_METHOD_TO(EmbeddingController::getEmbeddings, "/api/embeddings", drogon::Get, "AuthFilter");
        ADD_METHOD_TO(EmbeddingController::createEmbedding, "/api/embeddings", drogon::Post, "AuthFilter");
    METHOD_LIST_END

    explicit EmbeddingController(const drogon::orm::DbClientPtr& db);

    /// @brief Lists recent embeddings.
    /// @param req Incoming request.
    /// @param callback Response callback returning JSON.
    void getEmbeddings(const drogon::HttpRequestPtr& req,
                       std::function<void(const drogon::HttpResponsePtr&)>&& callback);

    /// @brief Creates a new embedding row.
    /// @param req Incoming request with JSON payload.
    /// @param callback Response callback returning created embedding or error.
    void createEmbedding(const drogon::HttpRequestPtr& req,
                         std::function<void(const drogon::HttpResponsePtr&)>&& callback);
};
