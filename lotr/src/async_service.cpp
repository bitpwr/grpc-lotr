#include "async_service.hpp"

#include <boost/asio/post.hpp>
#include <fmt/format.h>

namespace lotr {

AsyncService::AsyncService(boost::asio::io_context& context, const ServiceCallbacks& callbacks)
  : m_io_context{ context }
  , m_callbacks{ callbacks }
  , m_executor{ context }
  , m_status_writer{ context }
{
}

void AsyncService::shutdown()
{
    m_executor.shutdown();
    m_status_writer.shutdown();
}

void AsyncService::send_status(const GameStatus& game_status, const MordorPopulation& population)
{
    proto::GameStatus status;

    status.set_mordor_strenght(game_status.mordor_strength);
    status.set_gondor_strenght(game_status.gondor_strength);
    status.set_orc_count(population.orc_count);

    m_status_writer.send_message(status);
}

grpc::ServerUnaryReactor* AsyncService::mordor_population(grpc::CallbackServerContext* context,
                                                          const google::protobuf::Empty*,
                                                          proto::MordorPopulation* response)
{
    fmt::print("got async population request\n");
    // standard way of handling unary calls. See function below more generic handling
    auto reactor = context->DefaultReactor();
    boost::asio::post(m_io_context, [reactor, response, this] {
        const auto pop = m_callbacks.population();

        response->set_orc_count(pop.orc_count);
        response->set_troll_count(pop.troll_count);
        response->set_nazgul_count(pop.nazgul_count);
        response->set_sauron_alive(pop.sauron_alive);

        reactor->Finish(grpc::Status::OK);
    });

    return reactor;
}

grpc::ServerUnaryReactor* AsyncService::kill_orcs(grpc::CallbackServerContext* context,
                                                  const proto::Weapon* request,
                                                  proto::AttackResult* response)
{
    if (request->power() < 0 || request->power() > 1) {
        auto reactor = context->DefaultReactor();
        reactor->Finish({ grpc::StatusCode::INVALID_ARGUMENT, "Power must be between 0 and 1" });
        return reactor;
    }

    return m_executor.execute(context, [this, request, response]() {
        const auto result = m_callbacks.kill_orcs(request->name(), request->power());
        if (!result) {
            return grpc::Status{ grpc::StatusCode::INTERNAL, "Too late, Sauron has taken over" };
        }

        response->set_orcs_killed(result.value());
        return grpc::Status::OK;
    });
}

grpc::ServerWriteReactor<proto::GameStatus>* AsyncService::subscribeToStatus(
  grpc::CallbackServerContext*,
  const google::protobuf::Empty*)
{
    return m_status_writer.create_writer();
}

} // namespace lotr
