cmake_minimum_required(VERSION 3.21)

project(
    grpc-demo
    VERSION 0.1.0
    LANGUAGES CXX
)

include(cmake/project-settings.cmake)
include(cmake/compiler-warnings.cmake)

set_compiler_warnings()

add_subdirectory(lotr-proto)
add_subdirectory(utils)
add_subdirectory(lotr)
add_subdirectory(sync-client)
add_subdirectory(async-client)
