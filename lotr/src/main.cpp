#include "application.hpp"
#include "options.hpp"

#include <fmt/format.h>
#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/health_check_service_interface.h>

int main(int argc, char** argv)
{
    const auto options = lotr::parse(argc, argv);

    if (!options) {
        return EXIT_FAILURE;
    }

    grpc::EnableDefaultHealthCheckService(true);
    grpc::reflection::InitProtoReflectionServerBuilderPlugin();

    lotr::Application app{ options.value() };

    try {
        app.run();
    } catch (const std::exception& e) {
        fmt::print("unexpected failure: {}\n", e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
