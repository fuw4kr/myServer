#pragma once
#include <drogon/HttpController.h>

/**
 * @brief Health and discovery endpoint.
 *
 * GET / returns service status and a list of available API routes.
 *
 * @example
 * // HTTP: GET /
 */
class RootController : public drogon::HttpController<RootController, false>
{
public:
    METHOD_LIST_BEGIN
        ADD_METHOD_TO(RootController::root, "/", drogon::Get, "AuthFilter");
    METHOD_LIST_END

    /// @brief Returns basic service status and advertised endpoints.
    /// @param req Incoming request (unused).
    /// @param callback Response callback returning JSON.
    void root(const drogon::HttpRequestPtr& req,
              std::function<void(const drogon::HttpResponsePtr&)>&& callback) const;
};
