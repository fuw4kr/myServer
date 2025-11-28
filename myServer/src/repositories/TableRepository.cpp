#include "TableRepository.h"

#include <algorithm>
#include <cctype>
#include <stdexcept>
#include <utility>

using namespace drogon::orm;

TableRepository::TableRepository(const DbClientPtr& db,
                                 std::string tableName,
                                 std::string orderByColumn,
                                 std::vector<std::string> columns,
                                 RowMapper rowMapper,
                                 int limit)
    : db_(db),
      table_(std::move(tableName)),
      orderBy_(std::move(orderByColumn)),
      columns_(std::move(columns)),
      rowMapper_(std::move(rowMapper)),
      limit_(limit)
{
    if (!db_)
    {
        throw std::invalid_argument("TableRepository requires a valid DbClient");
    }

    if (!rowMapper_)
    {
        rowMapper_ = [](Row row, const Result& res) {
            return defaultRowMapper(row, res);
        };
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

Json::Value TableRepository::toJsonArray(const Result& result) const
{
    Json::Value array(Json::arrayValue);

    for (auto row : result)
    {
        array.append(rowMapper_(row, result));
    }

    return array;
}

std::string TableRepository::buildQuery() const
{
    return "SELECT " + buildColumnList() + " FROM " + table_ + " ORDER BY " + orderBy_ + " DESC LIMIT " +
           std::to_string(limit_);
}

Json::Value TableRepository::defaultRowMapper(Row row, const Result& res)
{
    Json::Value item(Json::objectValue);

    for (int i = 0; i < res.columns(); i++)
    {
        const std::string columnName = res.columnName(i);
        std::string lower = columnName;
        std::transform(lower.begin(), lower.end(), lower.begin(), [](unsigned char c) {
            return static_cast<char>(std::tolower(c));
        });

        if (lower.find("password") != std::string::npos ||
            lower.find("token") != std::string::npos ||
            lower.find("deleted") != std::string::npos)
        {
            continue;
        }

        item[columnName] = row[i].isNull() ? "" : row[i].as<std::string>();
    }

    return item;
}

std::string TableRepository::buildColumnList() const
{
    if (columns_.empty())
        return "*";

    std::string list;
    for (size_t i = 0; i < columns_.size(); ++i)
    {
        if (i > 0)
            list += ", ";
        list += columns_[i];
    }
    return list;
}
