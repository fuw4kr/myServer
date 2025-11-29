#pragma once
#include <drogon/HttpController.h>
#include <drogon/orm/DbClient.h>

class DashboardController : public drogon::HttpController<DashboardController, false>
{
public:
    METHOD_LIST_BEGIN
        ADD_METHOD_TO(DashboardController::getDashboard, "/api/dashboard", drogon::Get, "AuthFilter");
    METHOD_LIST_END

    explicit DashboardController(const drogon::orm::DbClientPtr& db);

    void getDashboard(const drogon::HttpRequestPtr& req,
                      std::function<void(const drogon::HttpResponsePtr&)>&& callback);

private:
    drogon::orm::DbClientPtr db_;
    bool aiActive() const;
};
