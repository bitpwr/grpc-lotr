#include "utils/grpc_server.hpp"

#include <fmt/format.h>
#include <grpcpp/server_builder.h>

namespace utils {

GrpcServer::GrpcServer(std::vector<grpc::Service*> services,
                       std::string_view address,
                       uint16_t port)
  : m_server_thread{ [this, services = std::move(services), address, port]() {
      run(services, fmt::format("{}:{}", address, port));
  } }
{
}

void GrpcServer::run(const std::vector<grpc::Service*>& services, const std::string& listening_uri)
{
    grpc::ServerBuilder builder;
    builder.AddListeningPort(listening_uri, grpc::InsecureServerCredentials());
    for (const auto service : services) {

        builder.RegisterService(service);
    }

    m_server = builder.BuildAndStart();

    if (!m_server) {
        fmt::print("Failed to create grpc::Server\n");
        return;
    }

    fmt::print("Server listening on {}\n", listening_uri);
    m_server->Wait();
    fmt::print("Server exited\n");
}

void GrpcServer::shutdown()
{
    if (m_server) {
        m_server->Shutdown();
        m_server_thread.join();
    }
}

} // namespace utils
