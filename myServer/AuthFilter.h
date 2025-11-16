#pragma once
#include <drogon/HttpFilter.h>
#include <drogon/orm/DbClient.h>

class AuthFilter : public drogon::HttpFilter<AuthFilter, false>
{
public:
    static constexpr const char* className = "AuthFilter";

    explicit AuthFilter(const drogon::orm::DbClientPtr& db)
        : db_(db)
    {
    }

    void doFilter(
        const drogon::HttpRequestPtr& req,
        drogon::FilterCallback&& fcb,
        drogon::FilterChainCallback&& fccb) override;

private:
    drogon::orm::DbClientPtr db_;
};
