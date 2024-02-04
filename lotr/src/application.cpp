#include "application.hpp"

#include <fmt/format.h>

namespace lotr {

Application::Application()
  : m_signals{ m_context, SIGINT, SIGTERM }
  , m_middleEarth{ m_context, { [this]() { shutdown(); } } }
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
    m_middleEarth.shutdown();
    m_context.stop();
}

boost::asio::io_context& Application::context()
{
    return m_context;
}

} // namespace lotr
