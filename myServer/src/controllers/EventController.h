#pragma once
#include <drogon/HttpController.h>
#include "TableControllerBase.h"

/**
 * @brief Exposes event listing and creation endpoints.
 *
 * GET /api/events returns latest events; POST /api/events inserts an event.
 *
 * @example
 * // HTTP: GET /api/events
 */
class EventController : public drogon::HttpController<EventController, false>, protected TableControllerBase
{
public:
    METHOD_LIST_BEGIN
        ADD_METHOD_TO(EventController::getEvents, "/api/events", drogon::Get, "AuthFilter");
        ADD_METHOD_TO(EventController::createEvent, "/api/events", drogon::Post, "AuthFilter");
    METHOD_LIST_END

    explicit EventController(const drogon::orm::DbClientPtr& db);

    /// @brief Lists recent events ordered by timestamp.
    /// @param req Incoming request.
    /// @param callback Response callback returning JSON.
    void getEvents(const drogon::HttpRequestPtr& req,
                   std::function<void(const drogon::HttpResponsePtr&)>&& callback);

    /// @brief Creates a new event record.
    /// @param req Incoming request with JSON payload.
    /// @param callback Response callback returning created event or error.
    void createEvent(const drogon::HttpRequestPtr& req,
                     std::function<void(const drogon::HttpResponsePtr&)>&& callback);
};
