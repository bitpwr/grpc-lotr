#pragma once

#include <boost/asio/io_context.hpp>
#include <grpcpp/server_context.h>
#include <grpcpp/support/server_callback.h>

namespace utils {

class UnaryExecutor
{
public:
    explicit UnaryExecutor(boost::asio::io_context& context);

    void shutdown();

    grpc::ServerUnaryReactor* execute(grpc::CallbackServerContext* callback_context,
                                      const std::function<grpc::Status()>& work);

private:
    boost::asio::io_context& m_io_context;
    std::atomic<bool> m_block{ false };
};

} // namespace utils
