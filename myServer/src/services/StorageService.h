#pragma once
#include <drogon/HttpClient.h>
#include <drogon/MultiPart.h>
#include <functional>
#include <string>

// Service that uploads files to Supabase Storage and returns a public URL.
class StorageService
{
public:
    using SuccessCallback = std::function<void(const std::string& url)>;
    using ErrorCallback = std::function<void(const std::string& message)>;

    StorageService();

    void uploadAvatar(const drogon::HttpFile& file,
                      const SuccessCallback& onSuccess,
                      const ErrorCallback& onError) const;

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

    std::string buildObjectPath(const std::string& extension) const;
    static std::string normalizeExtension(std::string ext);
    static std::string mimeFromFile(const drogon::HttpFile& file,
                                    const std::string& extension);
    static std::string stripSlashes(const std::string& value);
};
