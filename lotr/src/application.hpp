#pragma once

#include "async_service.hpp"
#include "middleearth.hpp"
#include "options.hpp"
#include "sync_service.hpp"

#include <utils/grpc_server.hpp>

#include <boost/asio/io_context.hpp>
#include <boost/asio/signal_set.hpp>

namespace lotr {

class Application
{
public:
    Application(const Options& options);

    void run();
    void shutdown();

private:
    boost::asio::io_context m_context;
    boost::asio::signal_set m_signals;
    MiddleEarth m_middleEarth;
    ServiceCallbacks m_callbacks;
    SyncService m_sync_service;
    AsyncService m_async_service;
    utils::GrpcServer m_grpc_sync_server;
    utils::GrpcServer m_grpc_async_server;
};

} // namespace lotr
