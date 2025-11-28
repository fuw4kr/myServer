#pragma once
#include <drogon/HttpController.h>
#include "TableControllerBase.h"

class EventController : public drogon::HttpController<EventController, false>, protected TableControllerBase
{
public:
    METHOD_LIST_BEGIN
        ADD_METHOD_TO(EventController::getEvents, "/api/events", drogon::Get, "AuthFilter");
        ADD_METHOD_TO(EventController::createEvent, "/api/events", drogon::Post, "AuthFilter");
    METHOD_LIST_END

    explicit EventController(const drogon::orm::DbClientPtr& db);

    void getEvents(const drogon::HttpRequestPtr& req,
                   std::function<void(const drogon::HttpResponsePtr&)>&& callback);

    void createEvent(const drogon::HttpRequestPtr& req,
                     std::function<void(const drogon::HttpResponsePtr&)>&& callback);
};
