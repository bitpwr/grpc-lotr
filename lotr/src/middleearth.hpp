#pragma once

#include <boost/asio/io_context.hpp>
#include <boost/asio/steady_timer.hpp>

namespace lotr {

struct Status
{
    float mordor_strength{};
};

class MiddleEarth
{
public:
    struct Callbacks
    {
        std::function<void()> game_over;
    };

    MiddleEarth(boost::asio::io_context& context, Callbacks callbacks);

    void shutdown();

private:
    void start_timer();
    void on_timer();

    boost::asio::io_context& m_context;
    boost::asio::steady_timer m_timer;
    Status m_status;
    Callbacks m_callbacks;
};

} // namespace lotr
