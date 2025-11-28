#pragma once
#include <drogon/HttpController.h>
#include <drogon/orm/DbClient.h>
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include "../repositories/TableRepository.h"

// Shared helper for controllers that expose simple table reads.
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

    void fetchTable(const drogon::HttpRequestPtr& req,
                    Callback callback) const;

    std::shared_ptr<TableRepository> tableRepository_;
};
