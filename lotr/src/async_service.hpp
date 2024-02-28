#pragma once

#include "lotr.hpp"

#include <boost/asio/io_context.hpp>
#include <lotr.grpc.pb.h>
#include <utils/stream.hpp>
#include <utils/unary_executor.hpp>

namespace lotr {

class AsyncService : public proto::LotrService::CallbackService
{
public:
    AsyncService(boost::asio::io_context& context, const ServiceCallbacks& callbacks);

    void shutdown();

    void send_status(const GameStatus& game_status, const MordorPopulation& population);

    grpc::ServerUnaryReactor* mordor_population(grpc::CallbackServerContext* context,
                                                const google::protobuf::Empty* request,
                                                proto::MordorPopulation* response) override;

    grpc::ServerUnaryReactor* kill_orcs(grpc::CallbackServerContext* context,
                                        const proto::Weapon* request,
                                        proto::AttackResult* response) override;

    grpc::ServerWriteReactor<proto::GameStatus>* subscribeToStatus(
      grpc::CallbackServerContext* context,
      const google::protobuf::Empty* request) override;

private:
    boost::asio::io_context& m_io_context;
    const ServiceCallbacks& m_callbacks;
    utils::UnaryExecutor m_executor;
    utils::StreamWriter<proto::GameStatus> m_status_writer;
};

} // namespace lotr
