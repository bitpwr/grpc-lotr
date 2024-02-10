#pragma once

#include "lotr.hpp"

#include <lotr.grpc.pb.h>

namespace lotr {

class SyncService : public proto::LotrService::Service
{
public:
    SyncService(const ServiceCallbacks& callbacks);

    grpc::Status mordor_population(grpc::ServerContext* context,
                                   const google::protobuf::Empty* request,
                                   proto::MordorPopulation* response) override;

    grpc::Status kill_orcs(grpc::ServerContext* context,
                           const proto::Weapon* request,
                           proto::AttackResult* response) override;

    void shutdown();

private:
    const ServiceCallbacks& m_callbacks;
};

} // namespace lotr
