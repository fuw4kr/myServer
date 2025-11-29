#pragma once
#include <drogon/HttpController.h>
#include <drogon/orm/DbClient.h>
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include "../repositories/TableRepository.h"

/**
 * @brief Shared helper for controllers that expose read-only table listings.
 *
 * Wraps a @ref TableRepository with common error handling and JSON conversion.
 *
 * @example
 * class ExampleController : public drogon::HttpController<ExampleController, false>, protected TableControllerBase {
 * public:
 *     ExampleController(const drogon::orm::DbClientPtr& db)
 *         : TableControllerBase(db, "example", "created_at", dto::ExampleDto::columns(), dto::ExampleDto::fromRow) {}
 * };
 */
class TableControllerBase
{
public:
    TableControllerBase(const drogon::orm::DbClientPtr& db,
                        std::string tableName,
                        std::string orderByColumn,
                        std::vector<std::string> columns,
                        TableRepository::RowMapper rowMapper,
                        int defaultLimit = 20);
    virtual ~TableControllerBase() = default;

protected:
    using Callback = std::function<void(const drogon::HttpResponsePtr&)>;

    /**
     * @brief Executes a paged "latest N" query and returns JSON.
     * @param req Incoming request (currently unused but available for overrides).
     * @param callback Response callback.
     * @return void
     */
    void fetchTable(const drogon::HttpRequestPtr& req,
                    Callback callback) const;

    std::shared_ptr<TableRepository> tableRepository_;
};
