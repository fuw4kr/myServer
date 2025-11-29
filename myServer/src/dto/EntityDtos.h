#pragma once

#include <drogon/orm/Result.h>
#include <drogon/orm/Row.h>
#include <drogon/orm/Field.h>
#include <json/json.h>
#include <string>
#include <vector>

namespace dto
{
/**
 * @brief Maps database rows from the persons table to JSON.
 * @example dto::PersonDto::fromRow(row, res);
 */
struct PersonDto
{
    static const std::vector<std::string>& columns();
    /// @return Allowed column names for selection.
    static Json::Value fromRow(drogon::orm::Row row, const drogon::orm::Result& res);
};

/**
 * @brief Maps database rows from the cameras table to JSON.
 * @example dto::CameraDto::fromRow(row, res);
 */
struct CameraDto
{
    static const std::vector<std::string>& columns();
    /// @return Allowed column names for selection.
    static Json::Value fromRow(drogon::orm::Row row, const drogon::orm::Result& res);
};

/**
 * @brief Maps database rows from the events table to JSON.
 * @example dto::EventDto::fromRow(row, res);
 */
struct EventDto
{
    static const std::vector<std::string>& columns();
    /// @return Allowed column names for selection.
    static Json::Value fromRow(drogon::orm::Row row, const drogon::orm::Result& res);
};

/**
 * @brief Maps database rows from the alerts table to JSON.
 * @example dto::AlertDto::fromRow(row, res);
 */
struct AlertDto
{
    static const std::vector<std::string>& columns();
    /// @return Allowed column names for selection.
    static Json::Value fromRow(drogon::orm::Row row, const drogon::orm::Result& res);
};

/**
 * @brief Maps database rows from the system_logs table to JSON.
 * @example dto::SystemLogDto::fromRow(row, res);
 */
struct SystemLogDto
{
    static const std::vector<std::string>& columns();
    /// @return Allowed column names for selection.
    static Json::Value fromRow(drogon::orm::Row row, const drogon::orm::Result& res);
};

/**
 * @brief Maps database rows from the embeddings table to JSON.
 * @example dto::EmbeddingDto::fromRow(row, res);
 */
struct EmbeddingDto
{
    static const std::vector<std::string>& columns();
    /// @return Allowed column names for selection.
    static Json::Value fromRow(drogon::orm::Row row, const drogon::orm::Result& res);
};

/**
 * @brief Maps database rows from the users table to JSON.
 * @example dto::UserDto::fromRow(row, res);
 */
struct UserDto
{
    static const std::vector<std::string>& columns();
    /// @return Allowed column names for selection.
    static Json::Value fromRow(drogon::orm::Row row, const drogon::orm::Result& res);
};
} // namespace dto
