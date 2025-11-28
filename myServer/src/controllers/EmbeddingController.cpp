#include "EmbeddingController.h"

EmbeddingController::EmbeddingController(const drogon::orm::DbClientPtr& db)
    : TableControllerBase(db, "embeddings", "created_at")
{
}

void EmbeddingController::getEmbeddings(
    const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    fetchTable(req, std::move(callback));
}
