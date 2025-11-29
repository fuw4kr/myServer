/**
 * @brief Implements user lookup and token update queries for authentication.
 */
#include "UserRepository.h"

#include <stdexcept>

using namespace drogon::orm;

UserRepository::UserRepository(const DbClientPtr& db)
    : db_(db)
{
    if (!db_)
    {
        throw std::invalid_argument("UserRepository requires a valid DbClient");
    }
}

/**
 * @brief Finds a user by email.
 * @param email Target email address.
 * @param onSuccess Invoked with query result (empty when not found).
 * @param onError Invoked on DB error.
 */
void UserRepository::findByEmail(
    const std::string& email,
    const ResultCallback& onSuccess,
    const ErrorCallback& onError) const
{
    db_->execSqlAsync(
        "SELECT id, password FROM users WHERE email=$1",
        onSuccess,
        onError,
        email);
}

/**
 * @brief Updates token and expiry for a user.
 * @param userId User identifier.
 * @param token Session token.
 * @param expiresAt Expiration timestamp.
 * @param onSuccess Invoked with updated row.
 * @param onError Invoked on DB error.
 */
void UserRepository::updateToken(
    const std::string& userId,
    const std::string& token,
    const trantor::Date& expiresAt,
    const ResultCallback& onSuccess,
    const ErrorCallback& onError) const
{
    db_->execSqlAsync(
        "UPDATE users SET api_token=$1, api_token_expires_at=$3 WHERE id=$2",
        onSuccess,
        onError,
        token,
        userId,
        expiresAt);
}

/**
 * @brief Updates token, password hash, and expiry for a user.
 * @param userId User identifier.
 * @param token New session token.
 * @param passwordHash Hashed password.
 * @param expiresAt Expiration timestamp.
 * @param onSuccess Invoked with updated row.
 * @param onError Invoked on DB error.
 */
void UserRepository::updateTokenWithPassword(
    const std::string& userId,
    const std::string& token,
    const std::string& passwordHash,
    const trantor::Date& expiresAt,
    const ResultCallback& onSuccess,
    const ErrorCallback& onError) const
{
    db_->execSqlAsync(
        "UPDATE users SET api_token=$1, password=$3, api_token_expires_at=$4 WHERE id=$2",
        onSuccess,
        onError,
        token,
        userId,
        passwordHash,
        expiresAt);
}

/**
 * @brief Looks up a user by active session token.
 * @param token Session token string.
 * @param onSuccess Invoked with query result (empty when not found).
 * @param onError Invoked on DB error.
 */
void UserRepository::findByToken(
    const std::string& token,
    const ResultCallback& onSuccess,
    const ErrorCallback& onError) const
{
    db_->execSqlAsync(
        "SELECT id, (api_token_expires_at IS NOT NULL AND api_token_expires_at <= NOW()) AS expired "
        "FROM users WHERE api_token=$1",
        onSuccess,
        onError,
        token);
}
