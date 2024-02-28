#pragma once

#include <lotr.grpc.pb.h>
#include <utils/grpc_client.hpp>
#include <utils/stream.hpp>

#include <boost/asio/io_context.hpp>
#include <memory>
#include <optional>
#include <string_view>

class AsyncClient
{
public:
    AsyncClient(boost::asio::io_context& context, std::string_view address, std::uint16_t port);

    using PopulationHandler =
      std::function<void(const grpc::Status&, const lotr::proto::MordorPopulation&)>;
    using KillHandler = std::function<void(const grpc::Status&, std::uint64_t)>;
    using StatusHandler = std::function<void(const lotr::proto::GameStatus&)>;
    using DoneHandler = std::function<void(const grpc::Status&)>;

    void shutdown();
    bool connected();

    void population(PopulationHandler handler);
    void kill_orcs(std::string_view weapon_name, float power, KillHandler handler);
    void subscribeToStatus(StatusHandler status_handler, DoneHandler done_handler);
    void unsubscribeToStatus();

private:
    boost::asio::io_context& m_io_context;
    utils::GrpcClient<lotr::proto::LotrService> m_grpc_client;
    std::unique_ptr<utils::StreamReader<lotr::proto::GameStatus>> m_status_reader;
};
