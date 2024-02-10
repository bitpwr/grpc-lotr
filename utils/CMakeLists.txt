project(utils)

find_package(Protobuf REQUIRED)
find_package(gRPC REQUIRED)
find_package(fmt REQUIRED)
find_package(Boost REQUIRED)

add_library(${PROJECT_NAME} src/grpc_server.cpp src/unary_executor.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC include)
target_link_libraries(${PROJECT_NAME} PUBLIC fmt::fmt protobuf::protobuf grpc::grpc Boost::system)
setup_target(${PROJECT_NAME})
