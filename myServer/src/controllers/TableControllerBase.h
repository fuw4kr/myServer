#pragma once
#include <drogon/HttpController.h>
#include <drogon/orm/DbClient.h>
#include <json/json.h>
#include <functional>
#include <string>

// Shared helper for controllers that expose simple table reads.
class TableControllerBase
{
public:
    TableControllerBase(const drogon::orm::DbClientPtr& db,
                        std::string tableName,
                        std::string orderByColumn,
                        int defaultLimit = 20);
    virtual ~TableControllerBase() = default;

protected:
    using Callback = std::function<void(const drogon::HttpResponsePtr&)>;

    void fetchTable(const drogon::HttpRequestPtr& req,
                    Callback callback) const;

    drogon::orm::DbClientPtr db_;
    const std::string table_;
    const std::string orderBy_;
    const int limit_;

private:
    static Json::Value toJsonArray(const drogon::orm::Result& result);
};
