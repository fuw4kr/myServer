#pragma once
#include <drogon/HttpFilter.h>

class AuthFilter : public drogon::HttpFilter<AuthFilter, false>
{
public:
    AuthFilter() = default;

    static constexpr const char* className = "AuthFilter";

    void doFilter(
        const drogon::HttpRequestPtr& req,
        drogon::FilterCallback&& fcb,
        drogon::FilterChainCallback&& fccb) override;
};

