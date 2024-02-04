#include "middleearth.hpp"

#include <fmt/format.h>

namespace {
constexpr int exit_code = 128;
}

namespace lotr {

MiddleEarth::MiddleEarth()
  : m_signals(m_context, SIGINT, SIGTERM)
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
                exit(exit_code + signal);
                break;
            default:
                fmt::print("unknown signal ({})\n", signal);
                break;
        }
    });
}

void MiddleEarth::run()
{
    m_context.run();
}

void MiddleEarth::shutdown()
{
    m_context.stop();
}

boost::asio::io_context& MiddleEarth::context()
{
    return m_context;
}

} // namespace lotr
