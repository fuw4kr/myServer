#pragma once

#include <drogon/orm/DbClient.h>
#include <json/json.h>
#include <functional>
#include <future>
#include <string>

// Repository for simple table reads (SELECT * ... ORDER BY ... LIMIT ...).
class TableRepository
{
public:
    using ResultCallback = std::function<void(const drogon::orm::Result&)>;
    using ErrorCallback = std::function<void(const std::exception_ptr&)>;

    TableRepository(const drogon::orm::DbClientPtr& db,
                    std::string tableName,
                    std::string orderByColumn,
                    int limit = 20);

    void fetchLatest(const ResultCallback& onSuccess,
                     const ErrorCallback& onError) const;

    std::future<drogon::orm::Result> fetchLatestAsync() const;

    static Json::Value toJsonArray(const drogon::orm::Result& result);

private:
    drogon::orm::DbClientPtr db_;
    std::string table_;
    std::string orderBy_;
    int limit_;

    std::string buildQuery() const;
};
