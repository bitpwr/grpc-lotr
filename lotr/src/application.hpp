#pragma once

#include "middleearth.hpp"

#include <boost/asio/io_context.hpp>
#include <boost/asio/signal_set.hpp>

namespace lotr {

class Application
{
public:
    Application();

    void run();
    void shutdown();

    boost::asio::io_context& context();

private:
    boost::asio::io_context m_context;
    boost::asio::signal_set m_signals;
    MiddleEarth m_middleEarth;
};

} // namespace lotr
