set(app_name async-client)
set(lib_name ${app_name}-static)

find_package(fmt REQUIRED)
find_package(Boost REQUIRED)

add_library(${lib_name} src/async_client.cpp)
target_include_directories(${lib_name} PUBLIC src)
target_link_libraries(
    ${lib_name}
    PRIVATE
    PUBLIC fmt::fmt Boost::system utils lotr-proto
)
setup_target(${lib_name})

add_executable(${app_name} src/main.cpp)
target_link_libraries(${app_name} ${lib_name})
setup_target(${app_name})
