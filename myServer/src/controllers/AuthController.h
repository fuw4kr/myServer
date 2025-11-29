#pragma once
#include <drogon/HttpController.h>
#include <drogon/orm/DbClient.h>
#include <memory>
#include "../repositories/UserRepository.h"

/**
 * @brief Handles authentication and token issuance.
 *
 * POST /auth/login validates credentials and issues a session token.
 *
 * @example
 * // HTTP: POST /auth/login with JSON { "email": "...", "password": "..." }
 */
class AuthController : public drogon::HttpController<AuthController, false>
{
public:
    METHOD_LIST_BEGIN
        ADD_METHOD_TO(AuthController::login, "/auth/login", drogon::Post);   // POST /auth/login
    METHOD_LIST_END

    explicit AuthController(const drogon::orm::DbClientPtr& db);

    /// @brief Authenticates a user and returns a token if successful.
    /// @param req Incoming request containing credentials.
    /// @param callback Response callback returning token or error JSON.
    void login(const drogon::HttpRequestPtr& req,
        std::function<void(const drogon::HttpResponsePtr&)>&& callback);

private:
    std::shared_ptr<UserRepository> userRepository_;
};
