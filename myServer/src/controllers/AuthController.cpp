#include "AuthController.h"
#include <drogon/drogon.h>
#include <json/json.h>
#include <stdexcept>

using namespace drogon;
using namespace drogon::orm;

AuthController::AuthController(const drogon::orm::DbClientPtr& db)
    : db_(db)
{
    if (!db_)
    {
        throw std::invalid_argument("AuthController requires a valid DbClient");
    }
}

void AuthController::login(
    const HttpRequestPtr& req,
    std::function<void(const HttpResponsePtr&)>&& callback)
{
    auto json = req->getJsonObject();
    if (!json || !json->isMember("email") || !json->isMember("password"))
    {
        auto resp = HttpResponse::newHttpJsonResponse("Missing fields");
        resp->setStatusCode(k400BadRequest);
        callback(resp);
        return;
    }

    std::string email = (*json)["email"].asString();
    std::string password = (*json)["password"].asString();

    db_->execSqlAsync(
        "SELECT id, password FROM users WHERE email=$1",
        [this, callback, password](const Result& r)
        {
            if (r.empty())
            {
                auto resp = HttpResponse::newHttpJsonResponse("Invalid email or password");
                resp->setStatusCode(k401Unauthorized);
                callback(resp);
                return;
            }

            std::string dbPass = r[0]["password"].as<std::string>();
            std::string userId = r[0]["id"].as<std::string>();

            if (dbPass != password)
            {
                auto resp = HttpResponse::newHttpJsonResponse("Invalid email or password");
                resp->setStatusCode(k401Unauthorized);
                callback(resp);
                return;
            }

            // --- Generate token ---
            std::string token = drogon::utils::getUuid();

            // --- Store token ---
            db_->execSqlAsync(
                "UPDATE users SET api_token=$1 WHERE id=$2",
                [callback, token, userId](const Result&)
                {
                    Json::Value body;
                    body["token"] = token;
                    body["userId"] = userId;

                    auto resp = HttpResponse::newHttpJsonResponse(body);
                    resp->setStatusCode(k200OK);
                    callback(resp);
                },
                [callback](const std::exception_ptr& e)
                {
                    auto resp = HttpResponse::newHttpJsonResponse("DB error");
                    resp->setStatusCode(k500InternalServerError);
                    callback(resp);
                },
                token, userId);
        },
        [callback](const std::exception_ptr& e)
        {
            auto resp = HttpResponse::newHttpJsonResponse("DB error");
            resp->setStatusCode(k500InternalServerError);
            callback(resp);
        },
        email);
}
