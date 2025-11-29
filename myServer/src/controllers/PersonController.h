#pragma once
#include <drogon/HttpController.h>
#include "TableControllerBase.h"
#include "../services/StorageService.h"

/**
 * @brief CRUD endpoints for person records plus avatar upload handling.
 *
 * Provides listing, creation, update, deletion under /api/persons and avatar uploads at /api/persons/{id}/avatar.
 *
 * @example
 * // HTTP: POST /api/persons/{id}/avatar with multipart/form-data containing a file.
 */
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

    /// @brief Lists persons ordered by id.
    /// @param req Incoming request.
    /// @param callback Response callback returning JSON.
    void getPersons(const drogon::HttpRequestPtr& req,
                    std::function<void(const drogon::HttpResponsePtr&)>&& callback);

    /// @brief Creates a new person row.
    /// @param req Incoming request with JSON payload.
    /// @param callback Response callback returning created person or error.
    void createPerson(const drogon::HttpRequestPtr& req,
                      std::function<void(const drogon::HttpResponsePtr&)>&& callback);

    /// @brief Updates a person record by id.
    /// @param req Incoming request with JSON payload.
    /// @param callback Response callback.
    /// @param id Person identifier path parameter.
    void updatePerson(const drogon::HttpRequestPtr& req,
                      std::function<void(const drogon::HttpResponsePtr&)>&& callback,
                      std::string id);

    /// @brief Deletes a person record by id.
    /// @param req Incoming request (unused).
    /// @param callback Response callback.
    /// @param id Person identifier path parameter.
    void deletePerson(const drogon::HttpRequestPtr& req,
                      std::function<void(const drogon::HttpResponsePtr&)>&& callback,
                      std::string id);

    /// @brief Uploads an avatar image and persists the resulting URL for the person.
    /// @param req Incoming multipart/form-data request containing the file.
    /// @param callback Response callback returning updated person or error.
    /// @param id Person identifier path parameter.
    void uploadAvatar(const drogon::HttpRequestPtr& req,
                      std::function<void(const drogon::HttpResponsePtr&)>&& callback,
                      std::string id);

private:
    std::shared_ptr<StorageService> storageService_;
};
