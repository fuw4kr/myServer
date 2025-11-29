#pragma once
#include <drogon/HttpFilter.h>
#include <drogon/orm/DbClient.h>
#include <memory>
#include "../repositories/UserRepository.h"

/**
 * @brief Simple bearer-token authorization filter.
 *
 * Validates `Authorization: Bearer <token>` against stored session tokens.
 * @example
 * app().registerFilter(std::make_shared<AuthFilter>(db));
 */
class AuthFilter : public drogon::HttpFilter<AuthFilter, false>
{
public:
    static constexpr const char* className = "AuthFilter";

    explicit AuthFilter(const drogon::orm::DbClientPtr& db)
        : userRepository_(std::make_shared<UserRepository>(db))
    {
    }

    /**
     * @brief Validates bearer token and either fails the request or continues the chain.
     * @param req Incoming HTTP request with Authorization header.
     * @param fcb Callback invoked when the request should be blocked.
     * @param fccb Callback invoked when the request is authorized and should proceed.
     * @return void
     */
    void doFilter(
        const drogon::HttpRequestPtr& req,
        drogon::FilterCallback&& fcb,
        drogon::FilterChainCallback&& fccb) override;

private:
    std::shared_ptr<UserRepository> userRepository_;
};
