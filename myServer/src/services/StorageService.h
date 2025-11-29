#pragma once
#include <drogon/HttpClient.h>
#include <drogon/MultiPart.h>
#include <functional>
#include <string>

/**
 * @brief Client for uploading files to Supabase Storage.
 *
 * The service is configured from the following environment variables:
 * - SUPABASE_URL or SUPABASE_DB_URL (derive host) or SUPABASE_STORAGE_URL
 * - SUPABASE_SERVICE_ROLE_KEY / SUPABASE_SERVICE_KEY / SUPABASE_SERVICE_ROLE
 * - SUPABASE_STORAGE_BUCKET (required) and optional SUPABASE_STORAGE_AVATAR_FOLDER
 * - SUPABASE_STORAGE_SKIP_TLS_VERIFY / SUPABASE_SKIP_TLS_VERIFY (optional)
 *
 * When misconfigured, @ref isConfigured returns false and uploads will fail fast.
 *
 * @example
 * StorageService storage;
 * if (storage.isConfigured()) {
 *     storage.uploadAvatar(file, onSuccess, onError);
 * }
 */
class StorageService
{
public:
    using SuccessCallback = std::function<void(const std::string& url)>;
    using ErrorCallback = std::function<void(const std::string& message)>;

    StorageService();

    /**
     * @brief Uploads a single avatar file to Supabase Storage.
     *
     * @param file Parsed multipart file from the request.
     * @param onSuccess Called with a public URL when the upload and response succeed.
     * @param onError Called with a human-friendly message on validation or HTTP failure.
     * @return void
     */
    void uploadAvatar(const drogon::HttpFile& file,
                      const SuccessCallback& onSuccess,
                      const ErrorCallback& onError) const;

    /**
     * @brief Indicates whether the service initialized correctly.
     * @return true if ready to upload, false otherwise.
     */
    bool isConfigured() const
    {
        return isReady_;
    }

private:
    drogon::HttpClientPtr client_;
    std::string bucket_;
    std::string publicBaseUrl_;
    std::string folder_;
    std::string serviceKey_;
    bool isReady_{false};

    /**
     * @brief Builds a normalized storage object path using the configured folder and file extension.
     *
     * @param extension File extension without a leading dot.
     * @return Full object path relative to the bucket.
     */
    std::string buildObjectPath(const std::string& extension) const;
    static std::string normalizeExtension(std::string ext);
    static std::string mimeFromFile(const drogon::HttpFile& file,
                                    const std::string& extension);
    static std::string stripSlashes(const std::string& value);
};
