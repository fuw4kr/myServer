#include "StorageService.h"

#include <drogon/drogon.h>
#include <drogon/utils/Utilities.h>
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <string_view>

namespace
{
std::string getEnv(const char* name)
{
    const char* val = std::getenv(name);
    return val ? std::string(val) : std::string();
}

std::string trimWhitespace(std::string value)
{
    auto isSpace = [](unsigned char c) {
        return std::isspace(c) != 0;
    };
    while (!value.empty() && isSpace(static_cast<unsigned char>(value.front())))
        value.erase(value.begin());
    while (!value.empty() && isSpace(static_cast<unsigned char>(value.back())))
        value.pop_back();
    return value;
}

bool isTruthy(const std::string& value)
{
    if (value.empty())
        return false;
    std::string lower = value;
    std::transform(lower.begin(), lower.end(), lower.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    return lower == "1" || lower == "true" || lower == "yes" || lower == "on";
}

std::string toLower(std::string value)
{
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    return value;
}

std::string trimTrailingSlash(std::string value)
{
    while (!value.empty() && value.back() == '/')
    {
        value.pop_back();
    }
    return value;
}

std::string deriveBaseUrl(const char* urlEnv, const char* dbUrlEnv)
{
    if (urlEnv && *urlEnv)
    {
        return trimTrailingSlash(urlEnv);
    }

    if (!dbUrlEnv || !*dbUrlEnv)
        return {};

    std::string dbUrl = dbUrlEnv;
    auto atPos = dbUrl.find('@');
    if (atPos == std::string::npos || atPos + 1 >= dbUrl.size())
        return {};

    auto hostStart = atPos + 1;
    auto hostEnd = dbUrl.find_first_of(":/", hostStart);
    std::string host = dbUrl.substr(hostStart,
                                    hostEnd == std::string::npos ? std::string::npos : hostEnd - hostStart);

    // Supabase DB hosts look like db.<project>.supabase.co; storage needs https://<project>.supabase.co
    if (host.rfind("db.", 0) == 0 && host.size() > 3)
    {
        host = host.substr(3);
    }

    if (host.empty())
        return {};

    return trimTrailingSlash("https://" + host);
}
} // namespace

StorageService::StorageService()
{
    const std::string urlEnv = trimWhitespace(getEnv("SUPABASE_URL"));
    const std::string storageUrlEnv = trimWhitespace(getEnv("SUPABASE_STORAGE_URL"));
    const std::string dbUrlEnv = trimWhitespace(getEnv("SUPABASE_DB_URL"));
    std::string keyEnv = trimWhitespace(getEnv("SUPABASE_SERVICE_ROLE_KEY"));
    if (keyEnv.empty())
        keyEnv = trimWhitespace(getEnv("SUPABASE_SERVICE_KEY"));
    if (keyEnv.empty())
        keyEnv = trimWhitespace(getEnv("SUPABASE_SERVICE_ROLE"));
    const std::string bucketEnv = trimWhitespace(getEnv("SUPABASE_STORAGE_BUCKET"));
    const std::string folderEnv = trimWhitespace(getEnv("SUPABASE_STORAGE_AVATAR_FOLDER"));
    const bool skipTlsVerify = isTruthy(trimWhitespace(getEnv("SUPABASE_STORAGE_SKIP_TLS_VERIFY"))) ||
                               isTruthy(trimWhitespace(getEnv("SUPABASE_SKIP_TLS_VERIFY")));

    std::string baseUrl = storageUrlEnv.empty() ? deriveBaseUrl(urlEnv.c_str(), dbUrlEnv.c_str())
                                                : trimTrailingSlash(storageUrlEnv);
    if (baseUrl.empty() || keyEnv.empty() || bucketEnv.empty())
    {
        LOG_ERROR << "Supabase storage env vars are missing: SUPABASE_URL (or SUPABASE_DB_URL to derive), "
                  << "SUPABASE_SERVICE_ROLE_KEY/SUPABASE_SERVICE_KEY/SUPABASE_SERVICE_ROLE, SUPABASE_STORAGE_BUCKET";
        LOG_ERROR << "Detected: url=" << (baseUrl.empty() ? "missing" : "present")
                  << ", key=" << (keyEnv.empty() ? "missing" : "present")
                  << " (len=" << keyEnv.size() << ")"
                  << ", bucket=" << (bucketEnv.empty() ? "missing" : "present");
        return;
    }

    bucket_ = stripSlashes(bucketEnv);
    folder_ = stripSlashes(folderEnv.empty() ? "avatars" : folderEnv);
    serviceKey_ = keyEnv;

    if (bucket_.empty())
    {
        LOG_ERROR << "SUPABASE_STORAGE_BUCKET is empty after normalization";
        return;
    }

    client_ = drogon::HttpClient::newHttpClient(baseUrl, nullptr, false, !skipTlsVerify);
    if (skipTlsVerify)
    {
        LOG_WARN << "Supabase storage TLS verification disabled via SUPABASE_STORAGE_SKIP_TLS_VERIFY/SUPABASE_SKIP_TLS_VERIFY";
    }
    publicBaseUrl_ = baseUrl + "/storage/v1/object/public/" + bucket_;
    isReady_ = static_cast<bool>(client_);

    if (!isReady_)
    {
        LOG_ERROR << "Failed to create Supabase HttpClient";
    }
    else
    {
        LOG_INFO << "Supabase storage configured for bucket '" << bucket_ << "' with base " << baseUrl;
    }
}

void StorageService::uploadAvatar(
    const drogon::HttpFile& file,
    const SuccessCallback& onSuccess,
    const ErrorCallback& onError) const
{
    if (!isReady_ || !client_)
    {
        onError("Storage service is not configured");
        return;
    }

    auto dataView = file.fileContent();
    if (dataView.empty())
    {
        onError("Uploaded file is empty");
        return;
    }

    const std::string extension = normalizeExtension(std::string(file.getFileExtension()));
    const std::string objectPath = buildObjectPath(extension);

    auto req = drogon::HttpRequest::newHttpRequest();
    req->setMethod(drogon::Post);
    req->setPath("/storage/v1/object/" + bucket_ + "/" + objectPath);
    req->addHeader("Authorization", "Bearer " + serviceKey_);
    req->addHeader("apikey", serviceKey_);
    req->addHeader("Content-Type", mimeFromFile(file, extension));
    req->setBody(std::string{dataView.data(), dataView.size()});

    client_->sendRequest(
        req,
        [onSuccess, onError, objectPath, this](drogon::ReqResult result, const drogon::HttpResponsePtr& resp)
        {
            if (result != drogon::ReqResult::Ok || !resp)
            {
                std::string message = "Supabase upload failed";
                if (result != drogon::ReqResult::Ok)
                {
                    message += ": ";
                    message += std::string(drogon::to_string_view(result));
                }
                onError(message);
                return;
            }

            const auto status = resp->getStatusCode();
            if (status < 200 || status >= 300)
            {
                const auto body = resp->getBody();
                std::string message = "Supabase upload rejected (status " + std::to_string(status) + ")";
                if (!body.empty())
                {
                    message += ": ";
                    message.append(body.data(), body.size());
                }
                onError(message);
                return;
            }

            onSuccess(publicBaseUrl_ + "/" + objectPath);
        },
        30.0);
}

std::string StorageService::buildObjectPath(const std::string& extension) const
{
    std::string name = drogon::utils::getUuid();
    if (!extension.empty())
    {
        name += ".";
        name += extension;
    }

    if (folder_.empty())
        return name;

    return folder_ + "/" + name;
}

std::string StorageService::normalizeExtension(std::string ext)
{
    while (!ext.empty() && ext.front() == '.')
    {
        ext.erase(ext.begin());
    }
    return toLower(ext);
}

std::string StorageService::mimeFromFile(
    const drogon::HttpFile& file,
    const std::string& extension)
{
    switch (file.getContentType())
    {
        case drogon::CT_IMAGE_PNG:
            return "image/png";
        case drogon::CT_IMAGE_JPG:
            return "image/jpeg";
        case drogon::CT_IMAGE_WEBP:
            return "image/webp";
        case drogon::CT_IMAGE_GIF:
            return "image/gif";
        default:
            break;
    }

    const auto ext = toLower(extension);
    if (ext == "png")
        return "image/png";
    if (ext == "jpg" || ext == "jpeg")
        return "image/jpeg";
    if (ext == "webp")
        return "image/webp";
    if (ext == "gif")
        return "image/gif";
    if (ext == "bmp")
        return "image/bmp";

    return "application/octet-stream";
}

std::string StorageService::stripSlashes(const std::string& value)
{
    if (value.empty())
        return value;

    size_t start = 0;
    size_t end = value.size();
    while (start < end && value[start] == '/')
        ++start;
    while (end > start && value[end - 1] == '/')
        --end;

    return value.substr(start, end - start);
}