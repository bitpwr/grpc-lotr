#include "application.hpp"

#include <fmt/format.h>

namespace lotr {

Application::Application(const Options& options)
  : m_signals{ m_context, SIGINT, SIGTERM }
  , m_middleEarth{ m_context, { [this]() { shutdown(); } } }
  , m_sync_service{ SyncService::Callbacks{
      .population = [this]() { return m_middleEarth.mordor_population(); },
      .kill_orcs = [this](std::string_view name,
                          float power) { return m_middleEarth.kill_orcs(name, power); } } }
  , m_grpc_server{ m_sync_service, options.address, options.port }
{
    m_signals.async_wait([this](const boost::system::error_code& error, int signal) {
        if (error) {
            fmt::print("signal handler error: {}\n", error.message());
            return;
        }

        switch (signal) {
            case SIGINT:
            case SIGTERM:
                fmt::print("shutting down on signal ({})\n", signal);
                shutdown();
                exit(EXIT_SUCCESS);
                break;
            default:
                fmt::print("unknown signal ({})\n", signal);
                break;
        }
    });
}

void Application::run()
{
    m_context.run();
}

void Application::shutdown()
{
    m_sync_service.shutdown();
    m_grpc_server.shutdown();
    m_middleEarth.shutdown();
    m_context.stop();
}

} // namespace lotr
