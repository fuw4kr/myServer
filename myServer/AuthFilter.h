#pragma once
#include <drogon/HttpFilter.h>
#include <drogon/orm/DbClient.h>

class AuthFilter : public drogon::HttpFilter<AuthFilter>
{
public:
    static constexpr const char* className = "AuthFilter";

    AuthFilter();

    void doFilter(
        const drogon::HttpRequestPtr& req,
        drogon::FilterCallback&& fcb,
        drogon::FilterChainCallback&& fccb) override;

private:
    drogon::orm::DbClientPtr db_;
};
