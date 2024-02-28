#include "async_client.hpp"

#include <boost/asio/post.hpp>
#include <fmt/format.h>
#include <grpc/status.h>

using namespace std::chrono_literals;

namespace {
google::protobuf::Empty empty_proto_msg;
}

AsyncClient::AsyncClient(boost::asio::io_context& context,
                         std::string_view address,
                         std::uint16_t port)
  : m_io_context{ context }
  , m_grpc_client{ fmt::format("{}:{}", address, port) }
{
}

void AsyncClient::shutdown()
{
    unsubscribeToStatus();
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

void AsyncClient::subscribeToStatus(StatusHandler status_handler, DoneHandler done_handler)
{
    if (m_status_reader) {
        fmt::print("Already subscribing\n");
        return;
    }

    m_status_reader = std::make_unique<utils::StreamReader<lotr::proto::GameStatus>>(
      m_io_context,
      [handler = std::move(status_handler)](const lotr::proto::GameStatus& status) {
          handler(status);
      },
      [this, handler = std::move(done_handler)](grpc::Status status) {
          m_status_reader.reset();
          handler(status);
      });

    m_grpc_client.stub().async()->subscribeToStatus(
      m_status_reader->client_context(), &empty_proto_msg, m_status_reader.get());

    m_status_reader->start();
}

void AsyncClient::unsubscribeToStatus()
{
    if (m_status_reader) {
        m_status_reader->shutdown();
    }
}
