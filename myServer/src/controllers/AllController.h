#pragma once
#include <drogon/HttpController.h>
#include <drogon/orm/DbClient.h>
#include <json/json.h>
#include "../repositories/TableRepository.h"

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
    TableRepository personsRepo_;
    TableRepository camerasRepo_;
    TableRepository eventsRepo_;
    TableRepository alertsRepo_;
    TableRepository systemLogsRepo_;
    TableRepository embeddingsRepo_;
};
