#include "PersonController.h"

PersonController::PersonController(const drogon::orm::DbClientPtr& db)
    : TableControllerBase(db, "persons", "id")
{
}

void PersonController::getPersons(
    const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    fetchTable(req, std::move(callback));
}
