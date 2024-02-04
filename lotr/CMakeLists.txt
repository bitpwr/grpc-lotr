set(app_name lotr)
set(lib_name ${app_name}-static)

find_package(
    Boost
    COMPONENTS program_options
    REQUIRED
)
find_package(fmt REQUIRED)

add_library(${lib_name} src/middleearth.cpp src/options.cpp)
target_include_directories(${lib_name} PUBLIC src)
target_link_libraries(
    ${lib_name}
    PRIVATE Boost::program_options
    PUBLIC fmt::fmt Boost::system
)
setup_target(${lib_name})

add_executable(${app_name} src/main.cpp)
target_link_libraries(${app_name} ${lib_name})
setup_target(${app_name})

# add_subdirectory(tests)
