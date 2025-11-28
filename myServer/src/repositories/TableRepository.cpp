#include "TableRepository.h"

#include <algorithm>
#include <cctype>
#include <exception>
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

void TableRepository::insert(const Json::Value& data,
                             const std::vector<std::string>& allowedColumns,
                             const ResultCallback& onSuccess,
                             const ErrorCallback& onError) const
{
    if (!data.isObject())
    {
        throw std::invalid_argument("Insert payload must be a JSON object");
    }

    const auto columns = filterWritableColumns(data, allowedColumns);
    if (columns.empty())
    {
        throw std::invalid_argument("No valid fields provided for insert");
    }

    const std::string sql = "INSERT INTO " + table_ + " (" + buildColumnList(columns) + ") VALUES (" +
                            buildPlaceholders(columns.size(), 1) + ") RETURNING " + buildColumnList();

    auto binder = (*db_) << sql;
    for (const auto& col : columns)
    {
        binder << data[col];
    }
    binder >> onSuccess;
    binder >> onError;
    try
    {
        binder.exec();
    }
    catch (...)
    {
        if (onError)
            onError(std::current_exception());
        else
            throw;
    }
}

void TableRepository::updateById(const std::string& id,
                                 const Json::Value& data,
                                 const std::vector<std::string>& allowedColumns,
                                 const ResultCallback& onSuccess,
                                 const ErrorCallback& onError) const
{
    if (!data.isObject())
    {
        throw std::invalid_argument("Update payload must be a JSON object");
    }

    const auto columns = filterWritableColumns(data, allowedColumns);
    if (columns.empty())
    {
        throw std::invalid_argument("No valid fields provided for update");
    }

    std::string assignments;
    for (size_t i = 0; i < columns.size(); ++i)
    {
        if (i > 0)
            assignments += ", ";
        assignments += columns[i] + "=" + buildParamName(i + 1);
    }

    const std::string sql = "UPDATE " + table_ + " SET " + assignments + " WHERE id=" +
                            buildParamName(columns.size() + 1) + " RETURNING " + buildColumnList();

    auto binder = (*db_) << sql;
    for (const auto& col : columns)
    {
        binder << data[col];
    }
    binder << id;
    binder >> onSuccess;
    binder >> onError;
    try
    {
        binder.exec();
    }
    catch (...)
    {
        if (onError)
            onError(std::current_exception());
        else
            throw;
    }
}

void TableRepository::deleteById(const std::string& id,
                                 const ResultCallback& onSuccess,
                                 const ErrorCallback& onError) const
{
    const std::string sql = "DELETE FROM " + table_ + " WHERE id=" + buildParamName(1) +
                            " RETURNING " + buildColumnList();
    auto binder = (*db_) << sql;
    binder << id;
    binder >> onSuccess;
    binder >> onError;
    try
    {
        binder.exec();
    }
    catch (...)
    {
        if (onError)
            onError(std::current_exception());
        else
            throw;
    }
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

Json::Value TableRepository::toJsonObject(const Result& result) const
{
    if (result.empty())
    {
        return Json::Value(Json::objectValue);
    }

    return rowMapper_(result[0], result);
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

    return buildColumnList(columns_);
}

std::string TableRepository::buildPlaceholders(size_t count, size_t startIndex)
{
    std::string list;
    for (size_t i = 0; i < count; ++i)
    {
        if (i > 0)
            list += ", ";
        list += buildParamName(startIndex + i);
    }
    return list;
}

std::vector<std::string> TableRepository::filterWritableColumns(
    const Json::Value& data,
    const std::vector<std::string>& allowedColumns)
{
    std::vector<std::string> filtered;
    filtered.reserve(allowedColumns.size());

    for (const auto& col : allowedColumns)
    {
        if (data.isMember(col))
        {
            filtered.push_back(col);
        }
    }

    return filtered;
}

std::string TableRepository::buildColumnList(const std::vector<std::string>& columns) const
{
    if (columns.empty())
        return "*";

    std::string list;
    for (size_t i = 0; i < columns.size(); ++i)
    {
        if (i > 0)
            list += ", ";
        list += columns[i];
    }
    return list;
}

std::string TableRepository::buildParamName(size_t index)
{
    return "$" + std::to_string(index);
}
