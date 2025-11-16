#pragma once
#include <drogon/HttpController.h>
#include <drogon/orm/DbClient.h>

class AuthController : public drogon::HttpController<AuthController, false>
{
public:
    METHOD_LIST_BEGIN
        ADD_METHOD_TO(AuthController::login, "/auth/login", drogon::Post);   // POST /auth/login
    METHOD_LIST_END

    explicit AuthController(const drogon::orm::DbClientPtr& db);

    void login(const drogon::HttpRequestPtr& req,
        std::function<void(const drogon::HttpResponsePtr&)>&& callback);

private:
    drogon::orm::DbClientPtr db_;
};
