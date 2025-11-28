#include "EventController.h"

EventController::EventController(const drogon::orm::DbClientPtr& db)
    : TableControllerBase(db, "events", "timestamp")
{
}

void EventController::getEvents(
    const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    fetchTable(req, std::move(callback));
}
