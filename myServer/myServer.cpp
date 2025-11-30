#include <drogon/drogon.h>
#include <drogon/orm/DbClient.h>
#include <cstdlib>
#include <string>
#include "src/controllers/AlertController.h"
#include "src/controllers/AllController.h"
#include "src/controllers/AuthController.h"
#include "src/controllers/CameraController.h"
#include "src/controllers/EmbeddingController.h"
#include "src/controllers/EventController.h"
#include "src/controllers/PersonController.h"
#include "src/controllers/RootController.h"
#include "src/controllers/StatsController.h"
#include "src/controllers/SystemLogController.h"
#include "src/controllers/DashboardController.h"
#include "src/filters/AuthFilter.h"

using namespace drogon;
using namespace drogon::orm;

int main()
{
    LOG_INFO << "Starting Drogon server...";

    const char* dbUrl = std::getenv("SUPABASE_DB_URL");
    if (!dbUrl || std::string(dbUrl).empty())
    {
        LOG_ERROR << "SUPABASE_DB_URL is not set or empty!";
        return 1;
    }
    LOG_INFO << "SUPABASE_DB_URL loaded";

    auto db = drogon::orm::DbClient::newPgClient(dbUrl, 2);

    db->execSqlAsync(
        "SELECT now()",
        [](const Result& r) {
            LOG_INFO << "DB connected, time: " << r[0]["now"].as<std::string>();
        },
        [](const std::exception_ptr& e) {
            try
            {
                if (e)
                    std::rethrow_exception(e);
            }
            catch (const std::exception& ex)
            {
                LOG_ERROR << "DB connection failed: " << ex.what();
            }
        });

    app().registerFilter(std::make_shared<AuthFilter>(db));

    app().registerController(std::make_shared<AuthController>(db));
    app().registerController(std::make_shared<PersonController>(db));
    app().registerController(std::make_shared<CameraController>(db));
    app().registerController(std::make_shared<EventController>(db));
    app().registerController(std::make_shared<AlertController>(db));
    app().registerController(std::make_shared<SystemLogController>(db));
    app().registerController(std::make_shared<EmbeddingController>(db));
    app().registerController(std::make_shared<AllController>(db));
    app().registerController(std::make_shared<DashboardController>(db));
    app().registerController(std::make_shared<StatsController>(db));
    app().registerController(std::make_shared<RootController>());

    uint16_t port = 8080;
    if (const char* p = std::getenv("PORT"))
    {
        try
        {
            port = static_cast<uint16_t>(std::stoi(p));
        }
        catch (...)
        {
            port = 8080;
        }
    }

    LOG_INFO << "Listening on 0.0.0.0:" << port;

    app()
        .addListener("0.0.0.0", port)
        .setThreadNum(2)
        .run();

    return 0;
}
