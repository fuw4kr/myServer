/**
 * @brief Shared implementation helpers for read-only table controllers.
 */
#include "TableControllerBase.h"
#include <drogon/drogon.h>
#include <stdexcept>
#include <utility>

using namespace drogon;
using namespace drogon::orm;

/**
 * @brief Constructs the base with a configured TableRepository.
 * @throws std::invalid_argument when repository initialization fails.
 */
TableControllerBase::TableControllerBase(const DbClientPtr& db,
                                         std::string tableName,
                                         std::string orderByColumn,
                                         std::vector<std::string> columns,
                                         TableRepository::RowMapper rowMapper,
                                         int defaultLimit)
    : tableRepository_(std::make_shared<TableRepository>(
          db,
          std::move(tableName),
          std::move(orderByColumn),
          std::move(columns),
          std::move(rowMapper),
          defaultLimit))
{
    if (!tableRepository_)
    {
        throw std::invalid_argument("TableControllerBase failed to initialize repository");
    }
}

/**
 * @brief Executes a latest-N query and returns JSON to the caller.
 * @param req Incoming request (unused).
 * @param callback Response callback.
 */
void TableControllerBase::fetchTable(
    const HttpRequestPtr& req,
    Callback callback) const
{
    (void)req;
    tableRepository_->fetchLatest(
        [cb = callback, repo = tableRepository_](const Result& result) mutable
        {
            cb(HttpResponse::newHttpJsonResponse(repo->toJsonArray(result)));
        },
        [cb = callback](const std::exception_ptr& ex) mutable
        {
            std::string errMsg = "unknown error";
            try
            {
                if (ex)
                    std::rethrow_exception(ex);
            }
            catch (const std::exception& e)
            {
                errMsg = e.what();
            }

            auto resp = HttpResponse::newHttpJsonResponse(errMsg);
            resp->setStatusCode(k500InternalServerError);
            cb(resp);
        });
}
