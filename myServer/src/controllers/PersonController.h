#pragma once
#include <drogon/HttpController.h>
#include "TableControllerBase.h"

class PersonController : public drogon::HttpController<PersonController, false>, protected TableControllerBase
{
public:
    METHOD_LIST_BEGIN
        ADD_METHOD_TO(PersonController::getPersons, "/api/persons", drogon::Get, "AuthFilter");
    METHOD_LIST_END

    explicit PersonController(const drogon::orm::DbClientPtr& db);

    void getPersons(const drogon::HttpRequestPtr& req,
                    std::function<void(const drogon::HttpResponsePtr&)>&& callback);
};
