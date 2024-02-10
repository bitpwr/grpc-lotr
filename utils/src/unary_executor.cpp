#include "utils/unary_executor.hpp"

#include <boost/asio/post.hpp>

namespace utils {

UnaryExecutor::UnaryExecutor(boost::asio::io_context& context)
  : m_io_context{ context }
{
}

void UnaryExecutor::shutdown()
{
    m_block = true;
}

grpc::ServerUnaryReactor* UnaryExecutor::execute(grpc::CallbackServerContext* callback_context,
                                                 const std::function<grpc::Status()>& work)
{
    auto reactor = callback_context->DefaultReactor();

    if (m_block) {
        reactor->Finish(grpc::Status::CANCELLED);
        return reactor;
    }

    boost::asio::post(m_io_context, [reactor, work]() {
        const auto status = work();
        reactor->Finish(std::move(status));
    });

    return reactor;
}

} // namespace utils
