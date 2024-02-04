#pragma once

#include <lotr.grpc.pb.h>
#include <utils/grpc_client.hpp>

#include <optional>

class SyncClient
{
public:
    SyncClient(std::string_view address, std::uint16_t port);

    std::optional<lotr::proto::MordorPopulation> population();
    std::optional<std::uint64_t> kill_orcs(std::string_view weapon_name, float power);

private:
    utils::GrpcClient<lotr::proto::LotrService> m_grpc_client;
};
