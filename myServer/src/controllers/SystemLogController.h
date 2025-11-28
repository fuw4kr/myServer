#pragma once
#include <drogon/HttpController.h>
#include "TableControllerBase.h"

class SystemLogController : public drogon::HttpController<SystemLogController, false>, protected TableControllerBase
{
public:
    METHOD_LIST_BEGIN
        ADD_METHOD_TO(SystemLogController::getSystemLogs, "/api/system_logs", drogon::Get, "AuthFilter");
    METHOD_LIST_END

    explicit SystemLogController(const drogon::orm::DbClientPtr& db);

    void getSystemLogs(const drogon::HttpRequestPtr& req,
                       std::function<void(const drogon::HttpResponsePtr&)>&& callback);
};
