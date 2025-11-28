#pragma once
#include <drogon/HttpController.h>
#include "TableControllerBase.h"

class EmbeddingController : public drogon::HttpController<EmbeddingController, false>, protected TableControllerBase
{
public:
    METHOD_LIST_BEGIN
        ADD_METHOD_TO(EmbeddingController::getEmbeddings, "/api/embeddings", drogon::Get, "AuthFilter");
    METHOD_LIST_END

    explicit EmbeddingController(const drogon::orm::DbClientPtr& db);

    void getEmbeddings(const drogon::HttpRequestPtr& req,
                       std::function<void(const drogon::HttpResponsePtr&)>&& callback);
};
