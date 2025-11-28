#include "PersonController.h"
#include "../dto/EntityDtos.h"

PersonController::PersonController(const drogon::orm::DbClientPtr& db)
    : TableControllerBase(db,
                          "persons",
                          "id",
                          dto::PersonDto::columns(),
                          dto::PersonDto::fromRow)
{
}

void PersonController::getPersons(
    const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    fetchTable(req, std::move(callback));
}
