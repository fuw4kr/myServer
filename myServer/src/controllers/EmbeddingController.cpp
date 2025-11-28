#include "EmbeddingController.h"
#include "../dto/EntityDtos.h"

EmbeddingController::EmbeddingController(const drogon::orm::DbClientPtr& db)
    : TableControllerBase(db,
                          "embeddings",
                          "created_at",
                          dto::EmbeddingDto::columns(),
                          dto::EmbeddingDto::fromRow)
{
}

void EmbeddingController::getEmbeddings(
    const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    fetchTable(req, std::move(callback));
}
