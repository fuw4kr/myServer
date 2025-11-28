#include "TableControllerBase.h"
#include <drogon/drogon.h>
#include <stdexcept>
#include <utility>

using namespace drogon;
using namespace drogon::orm;

TableControllerBase::TableControllerBase(const DbClientPtr& db,
                                         std::string tableName,
                                         std::string orderByColumn,
                                         int defaultLimit)
    : db_(db),
      table_(std::move(tableName)),
      orderBy_(std::move(orderByColumn)),
      limit_(defaultLimit)
{
    if (!db_)
    {
        throw std::invalid_argument("TableControllerBase requires a valid DbClient");
    }
}

void TableControllerBase::fetchTable(
    const HttpRequestPtr& req,
    Callback callback) const
{
    (void)req;
    const std::string query =
        "SELECT * FROM " + table_ + " ORDER BY " + orderBy_ + " DESC LIMIT " + std::to_string(limit_);

    db_->execSqlAsync(
        query,
        [cb = callback](const Result& result) mutable
        {
            cb(HttpResponse::newHttpJsonResponse(toJsonArray(result)));
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

Json::Value TableControllerBase::toJsonArray(const Result& result)
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
