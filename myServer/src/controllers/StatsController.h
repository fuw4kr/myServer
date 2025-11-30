#pragma once

#include <drogon/HttpController.h>
#include <drogon/orm/DbClient.h>

class StatsController : public drogon::HttpController<StatsController, false>
{
public:
    METHOD_LIST_BEGIN
        ADD_METHOD_TO(StatsController::getSummary, "/api/stats/summary", drogon::Get, "AuthFilter");
        ADD_METHOD_TO(StatsController::getDetectionsByHour, "/api/stats/detections-by-hour", drogon::Get, "AuthFilter");
        ADD_METHOD_TO(StatsController::getEvents, "/api/stats/events", drogon::Get, "AuthFilter");
    METHOD_LIST_END

    explicit StatsController(const drogon::orm::DbClientPtr& db);

    void getSummary(const drogon::HttpRequestPtr& req,
                    std::function<void(const drogon::HttpResponsePtr&)>&& callback);

    void getDetectionsByHour(const drogon::HttpRequestPtr& req,
                             std::function<void(const drogon::HttpResponsePtr&)>&& callback);

    void getEvents(const drogon::HttpRequestPtr& req,
                   std::function<void(const drogon::HttpResponsePtr&)>&& callback);

private:
    drogon::orm::DbClientPtr db_;
    bool aiActive() const;
};
