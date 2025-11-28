#pragma once
#include <drogon/HttpFilter.h>
#include <drogon/orm/DbClient.h>
#include <memory>
#include "../repositories/UserRepository.h"

class AuthFilter : public drogon::HttpFilter<AuthFilter, false>
{
public:
    static constexpr const char* className = "AuthFilter";

    explicit AuthFilter(const drogon::orm::DbClientPtr& db)
        : userRepository_(std::make_shared<UserRepository>(db))
    {
    }

    void doFilter(
        const drogon::HttpRequestPtr& req,
        drogon::FilterCallback&& fcb,
        drogon::FilterChainCallback&& fccb) override;

private:
    std::shared_ptr<UserRepository> userRepository_;
};
