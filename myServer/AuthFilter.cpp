#include "AuthFilter.h"
#include <cstdlib>
#include <drogon/drogon.h>

void AuthFilter::doFilter(
    const drogon::HttpRequestPtr& req,
    drogon::FilterCallback&& fcb,
    drogon::FilterChainCallback&& fccb)
{
    auto auth = req->getHeader("Authorization");

    const char* tokenEnv = std::getenv("API_TOKEN");
    std::string required = tokenEnv ? tokenEnv : "";

    if (required.empty())
    {
        auto resp = drogon::HttpResponse::newHttpJsonResponse("API_TOKEN not configured");
        resp->setStatusCode(drogon::k500InternalServerError);
        fcb(resp);
        return;
    }

    std::string correctValue = "Bearer " + required;

    if (auth != correctValue)
    {
        auto resp = drogon::HttpResponse::newHttpJsonResponse("Unauthorized");
        resp->setStatusCode(drogon::k401Unauthorized);
        fcb(resp);
        return;
    }

    fccb();
}
