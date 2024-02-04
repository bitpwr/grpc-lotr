set(app_name sync-client)
set(lib_name ${app_name}-static)

find_package(fmt REQUIRED)

add_library(${lib_name} src/sync_client.cpp)
target_include_directories(${lib_name} PUBLIC src)
target_link_libraries(
    ${lib_name}
    PRIVATE
    PUBLIC fmt::fmt utils lotr-proto
)
setup_target(${lib_name})

add_executable(${app_name} src/main.cpp)
target_link_libraries(${app_name} ${lib_name})
setup_target(${app_name})
