#include "sync_service.hpp"

#include <chrono>
#include <fmt/format.h>
#include <thread>

using namespace std::chrono_literals;

namespace lotr {

SyncService::SyncService(Callbacks callbacks)
  : m_callbacks{ std::move(callbacks) }
{
}

void SyncService::shutdown() {}

grpc::Status SyncService::mordor_population(grpc::ServerContext*,
                                            const google::protobuf::Empty*,
                                            proto::MordorPopulation* response)
{
    fmt::print("got population request\n");
    // this call is not thread safe, but this synchronous service is just for show
    const auto pop = m_callbacks.population();
    std::this_thread::sleep_for(200ms);
    response->set_orc_count(pop.orc_count);
    response->set_troll_count(pop.troll_count);
    response->set_nazgul_count(pop.nazgul_count);

    return grpc::Status::OK;
}

grpc::Status SyncService::kill_orcs(grpc::ServerContext*,
                                    const proto::Weapon* request,
                                    proto::AttackResult* response)
{
    fmt::print("kill orcs with {} power {}\n", request->name(), request->power());

    const auto result = m_callbacks.kill_orcs(request->name(), request->power());
    response->set_orcs_killed(result);
    response->set_trolls_killed(0);
    response->set_nazguls_killed(0);
    return grpc::Status::OK;
}

} // namespace lotr
