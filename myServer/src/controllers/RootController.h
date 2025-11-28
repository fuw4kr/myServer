#pragma once
#include <drogon/HttpController.h>

class RootController : public drogon::HttpController<RootController, false>
{
public:
    METHOD_LIST_BEGIN
        ADD_METHOD_TO(RootController::root, "/", drogon::Get, "AuthFilter");
    METHOD_LIST_END

    void root(const drogon::HttpRequestPtr& req,
              std::function<void(const drogon::HttpResponsePtr&)>&& callback) const;
};
