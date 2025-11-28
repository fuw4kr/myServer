#pragma once

#include <drogon/orm/Result.h>
#include <drogon/orm/Row.h>
#include <drogon/orm/Field.h>
#include <json/json.h>
#include <string>
#include <vector>

namespace dto
{
struct PersonDto
{
    static const std::vector<std::string>& columns();
    static Json::Value fromRow(drogon::orm::Row row, const drogon::orm::Result& res);
};

struct CameraDto
{
    static const std::vector<std::string>& columns();
    static Json::Value fromRow(drogon::orm::Row row, const drogon::orm::Result& res);
};

struct EventDto
{
    static const std::vector<std::string>& columns();
    static Json::Value fromRow(drogon::orm::Row row, const drogon::orm::Result& res);
};

struct AlertDto
{
    static const std::vector<std::string>& columns();
    static Json::Value fromRow(drogon::orm::Row row, const drogon::orm::Result& res);
};

struct SystemLogDto
{
    static const std::vector<std::string>& columns();
    static Json::Value fromRow(drogon::orm::Row row, const drogon::orm::Result& res);
};

struct EmbeddingDto
{
    static const std::vector<std::string>& columns();
    static Json::Value fromRow(drogon::orm::Row row, const drogon::orm::Result& res);
};

struct UserDto
{
    static const std::vector<std::string>& columns();
    static Json::Value fromRow(drogon::orm::Row row, const drogon::orm::Result& res);
};
} // namespace dto
