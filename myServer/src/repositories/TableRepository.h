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

/**
 * @brief Lightweight repository for CRUD on simple tables.
 *
 * Designed for tables that expose a fixed column list and require basic
 * insert/update/delete plus "latest N" queries ordered by a single column.
 *
 * @example
 * TableRepository repo(db, "persons", "id", dto::PersonDto::columns(), dto::PersonDto::fromRow);
 * repo.insert(payload, dto::PersonDto::columns(), onSuccess, onError);
 */
class TableRepository
{
public:
    using RowMapper = std::function<Json::Value(drogon::orm::Row, const drogon::orm::Result&)>;
    using ResultCallback = std::function<void(const drogon::orm::Result&)>;
    using ErrorCallback = std::function<void(const std::exception_ptr&)>;

    /**
     * @brief Constructs a repository bound to a specific table.
     *
     * @param db Shared database client instance.
     * @param tableName Target table name (e.g. "persons").
     * @param orderByColumn Column used to order results for reads.
     * @param columns Columns returned from queries (empty means "*").
     * @param rowMapper Converts rows to JSON; defaults to @ref defaultRowMapper if empty.
     * @param limit Limit for fetchLatest queries.
     * @throws std::invalid_argument When db is null.
     */
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

    /**
     * @brief Executes "SELECT ... ORDER BY ... LIMIT" and returns the result set.
     *
     * @param onSuccess Called with the query result.
     * @param onError Called when the database returns an error.
     */
    void fetchLatest(const ResultCallback& onSuccess,
                     const ErrorCallback& onError) const;

    /**
     * @brief Asynchronous future-based variant of @ref fetchLatest.
     *
     * @return Future resolving to the query result.
     */
    std::future<drogon::orm::Result> fetchLatestAsync() const;

    /**
     * @brief Inserts a row and returns it via @p onSuccess.
     * @param data JSON object payload.
     * @param allowedColumns Whitelisted column names accepted for insert.
     * @param onSuccess Called with the inserted row.
     * @param onError Called when the database returns an error.
     * @throws std::invalid_argument When payload is not an object or has no allowed fields.
     */
    void insert(const Json::Value& data,
                const std::vector<std::string>& allowedColumns,
                const ResultCallback& onSuccess,
                const ErrorCallback& onError) const;

    /**
     * @brief Updates a row by ID and returns the updated record.
     * @param id String representation of the primary key.
     * @param data JSON payload with candidate fields.
     * @param allowedColumns Whitelisted fields that may be updated.
     * @param onSuccess Called with the updated row (empty if not found).
     * @param onError Called when the database returns an error.
     * @throws std::invalid_argument When payload is not an object or has no allowed fields.
     */
    void updateById(const std::string& id,
                    const Json::Value& data,
                    const std::vector<std::string>& allowedColumns,
                    const ResultCallback& onSuccess,
                    const ErrorCallback& onError) const;

    /**
     * @brief Deletes a row by ID and returns the removed record.
     *
     * @param id String representation of the primary key.
     * @param onSuccess Called with the deleted row (empty if not found).
     * @param onError Called when the database returns an error.
     */
    void deleteById(const std::string& id,
                    const ResultCallback& onSuccess,
                    const ErrorCallback& onError) const;

    /**
     * @brief Maps a SQL result into a JSON array using the configured row mapper.
     *
     * @param result Query result set.
     * @return JSON array where each entry is a mapped row.
     */
    Json::Value toJsonArray(const drogon::orm::Result& result) const;
    /**
     * @brief Maps the first row (if any) of a SQL result into a JSON object.
     *
     * @param result Query result set.
     * @return JSON object for the first row, or empty object when no rows exist.
     */
    Json::Value toJsonObject(const drogon::orm::Result& result) const;

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
    std::string buildColumnList(const std::vector<std::string>& columns) const;
    static std::string buildPlaceholders(size_t count, size_t startIndex = 1);
    static std::vector<std::string> filterWritableColumns(const Json::Value& data,
                                                          const std::vector<std::string>& allowedColumns);
    static std::string buildParamName(size_t index);
};
