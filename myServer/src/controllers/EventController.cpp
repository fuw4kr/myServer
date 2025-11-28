#include "EventController.h"
#include "../dto/EntityDtos.h"

EventController::EventController(const drogon::orm::DbClientPtr& db)
    : TableControllerBase(db,
                          "events",
                          "timestamp",
                          dto::EventDto::columns(),
                          dto::EventDto::fromRow)
{
}

void EventController::getEvents(
    const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    fetchTable(req, std::move(callback));
}
