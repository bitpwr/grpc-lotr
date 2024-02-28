#include "application.hpp"
#include "lotr.hpp"

#include <chrono>
#include <fmt/format.h>

namespace {
constexpr auto status_interval = std::chrono::milliseconds{ 1500 };
}

namespace lotr {

Application::Application(const Options& options)
  : m_signals{ m_context, SIGINT, SIGTERM }
  , m_status_timer{ m_context }
  , m_middleEarth{ m_context, { [this]() { shutdown(); } } }
  , m_callbacks{ [this]() { return m_middleEarth.mordor_population(); },
                 [this](std::string_view name, float power) {
                     return m_middleEarth.kill_orcs(name, power);
                 } }
  , m_sync_service{ m_callbacks }
  , m_async_service{ m_context, m_callbacks }
  , m_grpc_sync_server{ { &m_sync_service }, options.address, options.sync_port }
  , m_grpc_async_server{ { &m_async_service }, options.address, options.async_port }
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
                break;
            default:
                fmt::print("unknown signal ({})\n", signal);
                break;
        }
    });

    start_timer();
}

void Application::run()
{
    m_context.run();
}

void Application::shutdown()
{
    m_status_timer.cancel();
    m_sync_service.shutdown();
    m_async_service.shutdown();
    m_grpc_sync_server.shutdown();
    m_grpc_async_server.shutdown();
    m_middleEarth.shutdown();
    m_context.stop();
}

void Application::start_timer()
{
    m_status_timer.expires_after(status_interval);
    m_status_timer.async_wait([this](const boost::system::error_code& ec) {
        if (ec) {
            if (ec == boost::asio::error::operation_aborted) {
                return;
            }
            fmt::print("Status timer failed: {}\n", ec.message());
        }
        on_timer();
        start_timer();
    });
}

void Application::on_timer()
{
    const auto status = m_middleEarth.status();
    const auto population = m_middleEarth.mordor_population();
    m_async_service.send_status(status, population);
}

} // namespace lotr
