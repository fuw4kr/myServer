#include "AuthController.h"
#include <drogon/drogon.h>
#include <json/json.h>
#include <stdexcept>
#include "../security/PasswordHasher.h"

using namespace drogon;
using namespace drogon::orm;

AuthController::AuthController(const DbClientPtr& db)
    : db_(db)
{
    if (!db_)
        throw std::invalid_argument("AuthController requires a valid DbClient");
}

void AuthController::login(
    const HttpRequestPtr& req,
    std::function<void(const HttpResponsePtr&)>&& callback)
{
    auto json = req->getJsonObject();
    if (!json || !json->isMember("email") || !json->isMember("password"))
    {
        Json::Value error;
        error["error"] = "Missing fields";

        auto resp = HttpResponse::newHttpJsonResponse(error);
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
                Json::Value err;
                err["error"] = "Invalid email or password";

                auto resp = HttpResponse::newHttpJsonResponse(err);
                resp->setStatusCode(k401Unauthorized);
                callback(resp);
                return;
            }

            std::string dbPass = r[0]["password"].as<std::string>();
            std::string userId = r[0]["id"].as<std::string>();

            bool passwordMatches = false;
            bool needsUpgrade = false;
            std::string upgradedHash;

            const bool storedAsHash = dbPass.rfind("$2", 0) == 0;
            if (storedAsHash)
            {
                passwordMatches = PasswordHasher::verifyPassword(password, dbPass);
            }
            else
            {
                passwordMatches = (dbPass == password);
                if (passwordMatches)
                {
                    try
                    {
                        upgradedHash = PasswordHasher::hashPassword(password);
                        needsUpgrade = true;
                    }
                    catch (const std::exception& ex)
                    {
                        LOG_WARN << "Failed to upgrade password hash for user " << userId
                                 << ": " << ex.what();
                    }
                }
            }

            if (!passwordMatches)
            {
                Json::Value err;
                err["error"] = "Invalid email or password";

                auto resp = HttpResponse::newHttpJsonResponse(err);
                resp->setStatusCode(k401Unauthorized);
                callback(resp);
                return;
            }

            // Generate token
            std::string token = drogon::utils::getUuid();

            auto onSuccess = [callback, token, userId](const Result&)
                {
                    Json::Value body;
                    body["token"] = token;
                    body["userId"] = userId;

                    auto resp = HttpResponse::newHttpJsonResponse(body);
                    resp->setStatusCode(k200OK);
                    callback(resp);
                };

            auto onError = [callback](const std::exception_ptr&)
                {
                    Json::Value err;
                    err["error"] = "Database error";

                    auto resp = HttpResponse::newHttpJsonResponse(err);
                    resp->setStatusCode(k500InternalServerError);
                    callback(resp);
                };

            if (needsUpgrade && !upgradedHash.empty())
            {
                db_->execSqlAsync(
                    "UPDATE users SET api_token=$1, password=$3 WHERE id=$2",
                    onSuccess,
                    onError,
                    token,
                    userId,
                    upgradedHash);
            }
            else
            {
                db_->execSqlAsync(
                    "UPDATE users SET api_token=$1 WHERE id=$2",
                    onSuccess,
                    onError,
                    token,
                    userId);
            }
        },
        [callback](const std::exception_ptr&)
        {
            Json::Value err;
            err["error"] = "Database error";

            auto resp = HttpResponse::newHttpJsonResponse(err);
            resp->setStatusCode(k500InternalServerError);
            callback(resp);
        },
        email);
}
