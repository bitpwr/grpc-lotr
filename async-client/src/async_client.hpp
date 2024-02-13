#pragma once

#include <lotr.grpc.pb.h>
#include <utils/grpc_client.hpp>

#include <boost/asio/io_context.hpp>
#include <optional>
#include <string_view>

class AsyncClient
{
public:
    AsyncClient(boost::asio::io_context& context, std::string_view address, std::uint16_t port);

    using PopulationHandler =
      std::function<void(const grpc::Status&, const lotr::proto::MordorPopulation&)>;
    using KillHandler = std::function<void(const grpc::Status&, std::uint64_t)>;

    bool connected();

    void population(PopulationHandler handler);
    void kill_orcs(std::string_view weapon_name, float power, KillHandler handler);

private:
    boost::asio::io_context& m_io_context;
    utils::GrpcClient<lotr::proto::LotrService> m_grpc_client;
};
