#include "EntityDtos.h"

#include <algorithm>
#include <cctype>
#include <unordered_set>

using namespace drogon::orm;

namespace
{
bool isSensitiveColumn(const std::string& name)
{
    std::string lower = name;
    std::transform(lower.begin(), lower.end(), lower.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    return lower.find("password") != std::string::npos ||
           lower.find("token") != std::string::npos ||
           lower.find("deleted") != std::string::npos;
}

Json::Value mapAllowed(Row row, const Result& res, const std::vector<std::string>& allowed)
{
    const bool useAllowed = !allowed.empty();
    std::unordered_set<std::string> allowSet;
    if (useAllowed)
        allowSet.insert(allowed.begin(), allowed.end());

    Json::Value obj(Json::objectValue);

    for (int i = 0; i < res.columns(); ++i)
    {
        std::string col = res.columnName(i);

        if (useAllowed && allowSet.find(col) == allowSet.end())
            continue;

        if (!useAllowed && isSensitiveColumn(col))
            continue;

        obj[col] = row[i].isNull() ? "" : row[i].as<std::string>();
    }

    return obj;
}
} // namespace

namespace dto
{
const std::vector<std::string>& PersonDto::columns()
{
    static const std::vector<std::string> allowed = {
        "id",
        "registered_at",
        "name",
        "role",
        "image_url",
        "authorized",
        "last_seen"};
    return allowed;
}

Json::Value PersonDto::fromRow(Row row, const Result& res)
{
    return mapAllowed(row, res, columns());
}

const std::vector<std::string>& CameraDto::columns()
{
    static const std::vector<std::string> allowed = {
        "id",
        "name",
        "ip_address",
        "location",
        "status",
        "stream_url",
        "created_at"};
    return allowed;
}

Json::Value CameraDto::fromRow(Row row, const Result& res)
{
    return mapAllowed(row, res, columns());
}

const std::vector<std::string>& EventDto::columns()
{
    static const std::vector<std::string> allowed = {
        "id",
        "camera_id",
        "person_id",
        "event_type",
        "confidence",
        "snapshot_url",
        "timestamp"};
    return allowed;
}

Json::Value EventDto::fromRow(Row row, const Result& res)
{
    return mapAllowed(row, res, columns());
}

const std::vector<std::string>& AlertDto::columns()
{
    static const std::vector<std::string> allowed = {
        "id",
        "event_id",
        "alert_type",
        "message",
        "severity",
        "resolved",
        "created_at"};
    return allowed;
}

Json::Value AlertDto::fromRow(Row row, const Result& res)
{
    return mapAllowed(row, res, columns());
}

const std::vector<std::string>& SystemLogDto::columns()
{
    static const std::vector<std::string> allowed = {
        "id",
        "source",
        "message",
        "level",
        "created_at"};
    return allowed;
}

Json::Value SystemLogDto::fromRow(Row row, const Result& res)
{
    return mapAllowed(row, res, columns());
}

const std::vector<std::string>& EmbeddingDto::columns()
{
    static const std::vector<std::string> allowed = {
        "id",
        "person_id",
        "model_name",
        "vector",
        "created_at"};
    return allowed;
}

Json::Value EmbeddingDto::fromRow(Row row, const Result& res)
{
    return mapAllowed(row, res, columns());
}

const std::vector<std::string>& UserDto::columns()
{
    static const std::vector<std::string> allowed = {
        "id",
        "email",
        "created_at"};
    return allowed;
}

Json::Value UserDto::fromRow(Row row, const Result& res)
{
    return mapAllowed(row, res, columns());
}
} // namespace dto
