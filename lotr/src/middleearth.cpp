#include "middleearth.hpp"

#include <fmt/format.h>

using namespace std::chrono_literals;

namespace {
constexpr auto timer_interval{ 1000ms };
constexpr auto mordor_strength_increase{ 0.09f };
} // namespace

namespace lotr {

MiddleEarth::MiddleEarth(boost::asio::io_context& context, Callbacks callbacks)
  : m_context{ context }
  , m_timer{ m_context }
  , m_callbacks{ std::move(callbacks) }
{
    fmt::print("Middle Earth alive\n");
    start_timer();
}

void MiddleEarth::shutdown()
{
    fmt::print("Middle Earth going down\n");
    m_timer.cancel();
}

void MiddleEarth::start_timer()
{
    m_timer.expires_after(timer_interval);
    m_timer.async_wait([this](const boost::system::error_code& ec) {
        if (ec) {
            if (ec == boost::asio::error::operation_aborted) {
                return;
            }
            fmt::print("Timer failed: {}\n", ec.message());
        }
        on_timer();
        start_timer();
    });
}

void MiddleEarth::on_timer()
{
    m_status.mordor_strength += mordor_strength_increase;
    fmt::print("Mordor strenght: {:.1f}%\n", m_status.mordor_strength * 100);

    if (m_status.mordor_strength >= 1) {
        fmt::print("Sauron has taken over Middle Earth. The end is here...\n");
        m_timer.cancel();
        m_callbacks.game_over();
    }
}

} // namespace lotr
