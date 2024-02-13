#include "async_client.hpp"

#include <boost/asio/post.hpp>
#include <fmt/format.h>
#include <grpc/status.h>

using namespace std::chrono_literals;

AsyncClient::AsyncClient(boost::asio::io_context& context,
                         std::string_view address,
                         std::uint16_t port)
  : m_io_context{ context }
  , m_grpc_client{ fmt::format("{}:{}", address, port) }
{
}

bool AsyncClient::connected()
{
    return m_grpc_client.connected();
}

void AsyncClient::population(PopulationHandler handler)
{
    auto state = std::make_shared<
      utils::ClientState<google::protobuf::Empty, lotr::proto::MordorPopulation>>();

    m_grpc_client.stub().async()->mordor_population(
      &state->context,
      &state->request,
      &state->response,
      [&io_context = m_io_context, state, handler = std::move(handler)](grpc::Status status) {
          boost::asio::post(io_context,
                            [handler = std::move(handler), status, response = state->response]() {
                                handler(status, response);
                            });
      });
}

void AsyncClient::kill_orcs(std::string_view weapon_name, float power, KillHandler handler)
{
    auto state =
      std::make_shared<utils::ClientState<lotr::proto::Weapon, lotr::proto::AttackResult>>();

    state->request.set_name(std::string(weapon_name));
    state->request.set_power(power);

    m_grpc_client.stub().async()->kill_orcs(
      &state->context,
      &state->request,
      &state->response,
      [&io_context = m_io_context, state, handler = std::move(handler)](grpc::Status status) {
          boost::asio::post(
            io_context,
            [handler = std::move(handler), status, orcs_killed = state->response.orcs_killed()]() {
                handler(status, orcs_killed);
            });
      });
}
