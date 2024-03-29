#pragma once

#include <grpcpp/impl/service_type.h>
#include <grpcpp/server.h>
#include <thread>

namespace utils {

class GrpcServer
{
public:
    GrpcServer(std::vector<grpc::Service*> services, std::string_view address, uint16_t port);

    void shutdown();

private:
    void run(const std::vector<grpc::Service*>& services, const std::string& listening_uri);

    std::thread m_server_thread;
    std::unique_ptr<grpc::Server> m_server;
};

} // namespace utils
