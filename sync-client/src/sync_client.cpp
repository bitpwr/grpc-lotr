#include "sync_client.hpp"

#include <fmt/format.h>
#include <grpc/status.h>

using namespace std::chrono_literals;

SyncClient::SyncClient(std::string_view address, std::uint16_t port)
  : m_grpc_client{ fmt::format("{}:{}", address, port) }
{
}

bool SyncClient::connected() const
{
    return m_grpc_client.connected();
}

std::optional<lotr::proto::MordorPopulation> SyncClient::population()
{
    google::protobuf::Empty request;
    lotr::proto::MordorPopulation response;
    grpc::ClientContext context;

    context.set_deadline(std::chrono::system_clock::now() + 1s);

    const auto status = m_grpc_client.stub().mordor_population(&context, request, &response);

    if (!status.ok()) {
        fmt::print(
          "Error: {}: {}\n", static_cast<int>(status.error_code()), status.error_message());
        return std::nullopt;
    }

    return response;
}

std::optional<std::uint64_t> SyncClient::kill_orcs(std::string_view weapon_name, float power)
{
    lotr::proto::Weapon weapon;
    lotr::proto::AttackResult response;
    grpc::ClientContext context;

    weapon.set_name(std::string{ weapon_name });
    weapon.set_power(power);
    const auto status = m_grpc_client.stub().kill_orcs(&context, weapon, &response);

    if (!status.ok()) {
        fmt::print(
          "Error: {}: {}\n", static_cast<int>(status.error_code()), status.error_message());
        return std::nullopt;
    }

    return response.orcs_killed();
}
