#pragma once
#include <drogon/HttpController.h>
#include "TableControllerBase.h"

class AlertController : public drogon::HttpController<AlertController, false>, protected TableControllerBase
{
public:
    METHOD_LIST_BEGIN
        ADD_METHOD_TO(AlertController::getAlerts, "/api/alerts", drogon::Get, "AuthFilter");
        ADD_METHOD_TO(AlertController::createAlert, "/api/alerts", drogon::Post, "AuthFilter");
    METHOD_LIST_END

    explicit AlertController(const drogon::orm::DbClientPtr& db);

    void getAlerts(const drogon::HttpRequestPtr& req,
                   std::function<void(const drogon::HttpResponsePtr&)>&& callback);

    void createAlert(const drogon::HttpRequestPtr& req,
                     std::function<void(const drogon::HttpResponsePtr&)>&& callback);
};
