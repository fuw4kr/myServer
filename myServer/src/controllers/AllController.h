#pragma once
#include <drogon/HttpController.h>
#include <drogon/orm/DbClient.h>
#include <json/json.h>

class AllController : public drogon::HttpController<AllController, false>
{
public:
    METHOD_LIST_BEGIN
        ADD_METHOD_TO(AllController::getAll, "/api/all", drogon::Get, "AuthFilter");
    METHOD_LIST_END

    explicit AllController(const drogon::orm::DbClientPtr& db);

    void getAll(const drogon::HttpRequestPtr& req,
                std::function<void(const drogon::HttpResponsePtr&)>&& callback);

private:
    drogon::orm::DbClientPtr db_;

    static Json::Value toJsonArray(const drogon::orm::Result& result);
};
