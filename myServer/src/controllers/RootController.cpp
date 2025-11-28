#include "RootController.h"
#include <drogon/drogon.h>
#include <json/json.h>

using namespace drogon;

void RootController::root(
    const HttpRequestPtr& req,
    std::function<void(const HttpResponsePtr&)>&& callback) const
{
    (void)req;
    Json::Value info;
    info["status"] = "ok";

    info["endpoints"] = Json::arrayValue;
    info["endpoints"].append("/auth/login");
    info["endpoints"].append("/api/persons");
    info["endpoints"].append("/api/cameras");
    info["endpoints"].append("/api/events");
    info["endpoints"].append("/api/alerts");
    info["endpoints"].append("/api/system_logs");
    info["endpoints"].append("/api/embeddings");
    info["endpoints"].append("/api/all");

    callback(HttpResponse::newHttpJsonResponse(info));
}
