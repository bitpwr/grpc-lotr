
#include "lotr.hpp"

#include <lotr.grpc.pb.h>

namespace lotr {

class SyncService : public proto::LotrService::Service
{
public:
    struct Callbacks
    {
        std::function<MordorPopulation()> population;
        std::function<std::uint64_t(std::string_view, float)> kill_orcs;
    };

    SyncService(Callbacks callbacks);

    grpc::Status mordor_population(grpc::ServerContext* context,
                                   const google::protobuf::Empty* request,
                                   proto::MordorPopulation* response) override;

    grpc::Status kill_orcs(grpc::ServerContext* context,
                           const proto::Weapon* request,
                           proto::AttackResult* response) override;

    void shutdown();

private:
    Callbacks m_callbacks;
};

} // namespace lotr
