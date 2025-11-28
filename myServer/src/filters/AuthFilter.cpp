#include "AuthFilter.h"
#include <drogon/drogon.h>
#include <json/json.h>

using namespace drogon;

void AuthFilter::doFilter(
    const HttpRequestPtr& req,
    FilterCallback&& fcb,
    FilterChainCallback&& fccb)
{
    LOG_INFO << "AuthFilter hit";
    auto auth = req->getHeader("Authorization");
    LOG_INFO << "Authorization header: " << auth;

    if (auth.rfind("Bearer ", 0) != 0)
    {
        LOG_INFO << "No Bearer prefix";
        auto resp = HttpResponse::newHttpJsonResponse(
            Json::Value("Unauthorized")
        );
        resp->setStatusCode(k401Unauthorized);
        fcb(resp);
        return;
    }

    std::string token = auth.substr(7);

    userRepository_->findByToken(
        token,
        [fcb, fccb](const orm::Result& res) mutable
        {
            if (res.empty())
            {
                auto resp = HttpResponse::newHttpJsonResponse(
                    Json::Value("Invalid token")
                );
                resp->setStatusCode(k401Unauthorized);
                fcb(resp);
                return;
            }

            bool isExpired = res[0]["expired"].as<bool>();
            if (isExpired)
            {
                auto resp = HttpResponse::newHttpJsonResponse(
                    Json::Value("Token expired")
                );
                resp->setStatusCode(k401Unauthorized);
                fcb(resp);
                return;
            }

            fccb();
        },
        [fcb](const std::exception_ptr& e) mutable
        {
            auto resp = HttpResponse::newHttpJsonResponse(
                Json::Value("DB error")
            );
            resp->setStatusCode(k500InternalServerError);
            fcb(resp);
        });
}
