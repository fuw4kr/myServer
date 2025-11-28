#include "TableRepository.h"

#include <stdexcept>
#include <utility>

using namespace drogon::orm;

TableRepository::TableRepository(const DbClientPtr& db,
                                 std::string tableName,
                                 std::string orderByColumn,
                                 int limit)
    : db_(db),
      table_(std::move(tableName)),
      orderBy_(std::move(orderByColumn)),
      limit_(limit)
{
    if (!db_)
    {
        throw std::invalid_argument("TableRepository requires a valid DbClient");
    }
}

void TableRepository::fetchLatest(
    const ResultCallback& onSuccess,
    const ErrorCallback& onError) const
{
    db_->execSqlAsync(buildQuery(), onSuccess, onError);
}

std::future<Result> TableRepository::fetchLatestAsync() const
{
    return db_->execSqlAsyncFuture(buildQuery());
}

Json::Value TableRepository::toJsonArray(const Result& result)
{
    Json::Value array(Json::arrayValue);

    for (const auto& row : result)
    {
        Json::Value item;
        for (int i = 0; i < result.columns(); i++)
        {
            const std::string columnName = result.columnName(i);
            if (row[i].isNull())
                item[columnName] = "";
            else
                item[columnName] = row[i].as<std::string>();
        }
        array.append(item);
    }

    return array;
}

std::string TableRepository::buildQuery() const
{
    return "SELECT * FROM " + table_ + " ORDER BY " + orderBy_ + " DESC LIMIT " +
           std::to_string(limit_);
}
