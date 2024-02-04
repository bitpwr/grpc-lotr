project(utils)

find_package(Protobuf REQUIRED)
find_package(gRPC REQUIRED)
find_package(fmt REQUIRED)

add_library(${PROJECT_NAME} src/grpc_server.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC include)
target_link_libraries(${PROJECT_NAME} PUBLIC fmt::fmt protobuf::protobuf grpc::grpc)
setup_target(${PROJECT_NAME})
