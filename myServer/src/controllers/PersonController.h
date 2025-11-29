#pragma once
#include <drogon/HttpController.h>
#include "TableControllerBase.h"
#include "../services/StorageService.h"

class PersonController : public drogon::HttpController<PersonController, false>, protected TableControllerBase
{
public:
    METHOD_LIST_BEGIN
        ADD_METHOD_TO(PersonController::getPersons, "/api/persons", drogon::Get, "AuthFilter");
        ADD_METHOD_TO(PersonController::createPerson, "/api/persons", drogon::Post, "AuthFilter");
        ADD_METHOD_TO(PersonController::updatePerson, "/api/persons/{1}", drogon::Put, drogon::Patch, "AuthFilter");
        ADD_METHOD_TO(PersonController::deletePerson, "/api/persons/{1}", drogon::Delete, "AuthFilter");
        ADD_METHOD_TO(PersonController::uploadAvatar, "/api/persons/{1}/avatar", drogon::Post, "AuthFilter");
    METHOD_LIST_END

    explicit PersonController(const drogon::orm::DbClientPtr& db);

    void getPersons(const drogon::HttpRequestPtr& req,
                    std::function<void(const drogon::HttpResponsePtr&)>&& callback);

    void createPerson(const drogon::HttpRequestPtr& req,
                      std::function<void(const drogon::HttpResponsePtr&)>&& callback);

    void updatePerson(const drogon::HttpRequestPtr& req,
                      std::function<void(const drogon::HttpResponsePtr&)>&& callback,
                      std::string id);

    void deletePerson(const drogon::HttpRequestPtr& req,
                      std::function<void(const drogon::HttpResponsePtr&)>&& callback,
                      std::string id);

    void uploadAvatar(const drogon::HttpRequestPtr& req,
                      std::function<void(const drogon::HttpResponsePtr&)>&& callback,
                      std::string id);

private:
    std::shared_ptr<StorageService> storageService_;
};
