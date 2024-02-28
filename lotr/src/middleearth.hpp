#pragma once

#include "lotr.hpp"

#include <boost/asio/io_context.hpp>
#include <boost/asio/steady_timer.hpp>
#include <functional>
#include <optional>

namespace lotr {

class MiddleEarth
{
public:
    struct Callbacks
    {
        std::function<void()> game_over;
    };

    MiddleEarth(boost::asio::io_context& context, Callbacks callbacks);

    void shutdown();

    const MordorPopulation& mordor_population();
    const GameStatus& status();
    std::optional<std::uint64_t> kill_orcs(std::string_view weapon, float power);

private:
    void start_timer();
    void on_timer();

    boost::asio::io_context& m_context;
    boost::asio::steady_timer m_timer;
    Callbacks m_callbacks;
    GameStatus m_status{};
    MordorPopulation m_mordor_population{};
};

} // namespace lotr
