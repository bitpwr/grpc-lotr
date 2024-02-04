#pragma once

#include <grpcpp/grpcpp.h>
#include <memory>
#include <string>

namespace utils {

template<typename Service>
class GrpcClient
{
public:
    GrpcClient(const std::string& server_address)
      : m_channel{ grpc::CreateChannel(server_address, grpc::InsecureChannelCredentials()) }
      , m_stub{ Service::NewStub(m_channel) }
    {
    }

    Service::Stub& stub() { return *m_stub.get(); }

    bool connected() { return m_channel->GetState(true) == GRPC_CHANNEL_READY; }

private:
    std::shared_ptr<grpc::Channel> m_channel;
    std::unique_ptr<typename Service::Stub> m_stub;
};

} // namespace utils
