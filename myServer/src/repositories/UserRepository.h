#pragma once

#include <drogon/orm/DbClient.h>
#include <trantor/utils/Date.h>
#include <functional>
#include <string>

/**
 * @brief User-specific queries for authentication flows.
 *
 * Provides token lookup and updates for session management.
 *
 * @example
 * UserRepository repo(db);
 * repo.findByEmail(email, onSuccess, onError);
 */
class UserRepository
{
public:
    using ResultCallback = std::function<void(const drogon::orm::Result&)>;
    using ErrorCallback = std::function<void(const std::exception_ptr&)>;

    explicit UserRepository(const drogon::orm::DbClientPtr& db);

    /// @brief Finds a user by email; returns an empty result when not found.
    /// @param email Email address to search.
    /// @param onSuccess Called with the query result.
    /// @param onError Called when the database returns an error.
    void findByEmail(const std::string& email,
                     const ResultCallback& onSuccess,
                     const ErrorCallback& onError) const;

    /// @brief Updates token and expiry for a user.
    /// @param userId Target user identifier.
    /// @param token Session token.
    /// @param expiresAt Expiration timestamp.
    /// @param onSuccess Called with the updated row.
    /// @param onError Called when the database returns an error.
    void updateToken(const std::string& userId,
                     const std::string& token,
                     const trantor::Date& expiresAt,
                     const ResultCallback& onSuccess,
                     const ErrorCallback& onError) const;

    /// @brief Updates token, password hash, and expiry for a user.
    /// @param userId Target user identifier.
    /// @param token Session token.
    /// @param passwordHash New hashed password.
    /// @param expiresAt Expiration timestamp.
    /// @param onSuccess Called with the updated row.
    /// @param onError Called when the database returns an error.
    void updateTokenWithPassword(const std::string& userId,
                                 const std::string& token,
                                 const std::string& passwordHash,
                                 const trantor::Date& expiresAt,
                                 const ResultCallback& onSuccess,
                                 const ErrorCallback& onError) const;

    /// @brief Looks up a user by active session token.
    /// @param token Session token string.
    /// @param onSuccess Called with the query result.
    /// @param onError Called when the database returns an error.
    void findByToken(const std::string& token,
                     const ResultCallback& onSuccess,
                     const ErrorCallback& onError) const;

private:
    drogon::orm::DbClientPtr db_;
};
