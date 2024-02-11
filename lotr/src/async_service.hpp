#pragma once

#include "lotr.hpp"

#include <lotr.grpc.pb.h>

#include <boost/asio/io_context.hpp>
#include <utils/unary_executor.hpp>

namespace lotr {

class AsyncService : public proto::LotrService::CallbackService
{
public:
    AsyncService(boost::asio::io_context& context, const ServiceCallbacks& callbacks);

    void shutdown();

    grpc::ServerUnaryReactor* mordor_population(grpc::CallbackServerContext* context,
                                                const google::protobuf::Empty* request,
                                                proto::MordorPopulation* response) override;

    grpc::ServerUnaryReactor* kill_orcs(grpc::CallbackServerContext* context,
                                        const proto::Weapon* request,
                                        proto::AttackResult* response) override;

private:
    boost::asio::io_context& m_io_context;
    const ServiceCallbacks& m_callbacks;
    utils::UnaryExecutor m_executor;
};

} // namespace lotr
