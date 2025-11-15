#pragma once
#include <drogon/HttpFilter.h>

class AuthFilter : public drogon::HttpFilter<AuthFilter>
{
public:
    AuthFilter() = default;

    void doFilter(
        const drogon::HttpRequestPtr& req,
        drogon::FilterCallback&& fcb,
        drogon::FilterChainCallback&& fccb) override;
};

