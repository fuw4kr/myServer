#pragma once

#include <drogon/orm/DbClient.h>
#include <trantor/utils/Date.h>
#include <functional>
#include <string>

class UserRepository
{
public:
    using ResultCallback = std::function<void(const drogon::orm::Result&)>;
    using ErrorCallback = std::function<void(const std::exception_ptr&)>;

    explicit UserRepository(const drogon::orm::DbClientPtr& db);

    void findByEmail(const std::string& email,
                     const ResultCallback& onSuccess,
                     const ErrorCallback& onError) const;

    void updateToken(const std::string& userId,
                     const std::string& token,
                     const trantor::Date& expiresAt,
                     const ResultCallback& onSuccess,
                     const ErrorCallback& onError) const;

    void updateTokenWithPassword(const std::string& userId,
                                 const std::string& token,
                                 const std::string& passwordHash,
                                 const trantor::Date& expiresAt,
                                 const ResultCallback& onSuccess,
                                 const ErrorCallback& onError) const;

    void findByToken(const std::string& token,
                     const ResultCallback& onSuccess,
                     const ErrorCallback& onError) const;

private:
    drogon::orm::DbClientPtr db_;
};
