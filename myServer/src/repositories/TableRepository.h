#pragma once

#include <drogon/orm/DbClient.h>
#include <drogon/orm/Result.h>
#include <drogon/orm/Row.h>
#include <drogon/orm/Field.h>
#include <json/json.h>
#include <functional>
#include <future>
#include <string>
#include <vector>

// Repository for simple table reads (SELECT * ... ORDER BY ... LIMIT ...).
class TableRepository
{
public:
    using RowMapper = std::function<Json::Value(drogon::orm::Row, const drogon::orm::Result&)>;
    using ResultCallback = std::function<void(const drogon::orm::Result&)>;
    using ErrorCallback = std::function<void(const std::exception_ptr&)>;

    TableRepository(const drogon::orm::DbClientPtr& db,
                    std::string tableName,
                    std::string orderByColumn,
                    std::vector<std::string> columns,
                    RowMapper rowMapper,
                    int limit = 20);

    template <typename Dto>
    static TableRepository forDto(const drogon::orm::DbClientPtr& db,
                                  std::string tableName,
                                  std::string orderByColumn,
                                  int limit = 20)
    {
        return TableRepository(
            db,
            std::move(tableName),
            std::move(orderByColumn),
            Dto::columns(),
            [](drogon::orm::Row row, const drogon::orm::Result& res) {
                return Dto::fromRow(row, res);
            },
            limit);
    }

    void fetchLatest(const ResultCallback& onSuccess,
                     const ErrorCallback& onError) const;

    std::future<drogon::orm::Result> fetchLatestAsync() const;

    Json::Value toJsonArray(const drogon::orm::Result& result) const;

private:
    drogon::orm::DbClientPtr db_;
    std::string table_;
    std::string orderBy_;
    std::vector<std::string> columns_;
    RowMapper rowMapper_;
    int limit_;

    std::string buildQuery() const;
    static Json::Value defaultRowMapper(drogon::orm::Row row, const drogon::orm::Result& res);
    std::string buildColumnList() const;
};
