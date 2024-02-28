#include "middleearth.hpp"

#include <fmt/format.h>
#include <random>

using namespace std::chrono_literals;

namespace {
constexpr auto timer_interval{ 1000ms };
constexpr auto mordor_strength_increase{ 0.09f };

float random_effect()
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> dist(0.3f, 0.8f);

    return dist(gen);
}

} // namespace

namespace lotr {

MiddleEarth::MiddleEarth(boost::asio::io_context& context, Callbacks callbacks)
  : m_context{ context }
  , m_timer{ m_context }
  , m_callbacks{ std::move(callbacks) }
  , m_mordor_population{ .orc_count = 1000,
                         .troll_count = 10,
                         .nazgul_count = 9,
                         .sauron_alive = true }
{
    fmt::print("Middle Earth alive\n");
    start_timer();
}

void MiddleEarth::shutdown()
{
    fmt::print("Middle Earth going down\n");
    m_timer.cancel();
}

const MordorPopulation& MiddleEarth::mordor_population()
{
    return m_mordor_population;
}

const GameStatus& MiddleEarth::status()
{
    return m_status;
}

std::optional<std::uint64_t> MiddleEarth::kill_orcs(std::string_view weapon, float power)
{
    fmt::print("kill orcs using '{}' with {}%\n", weapon, power);
    const auto orcs_killed = static_cast<std::uint64_t>(
      static_cast<float>(m_mordor_population.orc_count) * power * random_effect());
    m_mordor_population.orc_count -= orcs_killed;

    return orcs_killed;
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
    m_mordor_population.orc_count =
      static_cast<std::uint64_t>(static_cast<double>(m_mordor_population.orc_count) * 1.3);
    m_mordor_population.troll_count =
      static_cast<std::uint32_t>(m_mordor_population.troll_count * 1.1);

    m_status.mordor_strength = (static_cast<float>(m_mordor_population.orc_count) +
                                static_cast<float>(m_mordor_population.troll_count) * 3.0f +
                                static_cast<float>(m_mordor_population.nazgul_count) * 100.0f) /
                               1.0e6f;

    if (m_status.mordor_strength < 1) {
        fmt::print("Mordor strength: {:.1f}%, Orc count: {}\n",
                   m_status.mordor_strength * 100,
                   m_mordor_population.orc_count);
    } else {
        fmt::print("Sauron has taken over Middle Earth. The end is here...\n");
        m_timer.cancel();
        m_callbacks.game_over();
    }
}

} // namespace lotr
