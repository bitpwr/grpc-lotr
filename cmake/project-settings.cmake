include_guard()

include(${CMAKE_CURRENT_LIST_DIR}/common.cmake)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# use third parties in release for debug builds
set(CMAKE_MAP_IMPORTED_CONFIG_DEBUG RELEASE)
set(Boost_USE_STATIC_LIBS ON)

# find_package(Catch2 REQUIRED)
# enable_testing()

# function(add_unittest)
#     set(options)
#     set(oneValueArgs NAME)
#     set(multiValueArgs FILES LIBS)

#     cmake_parse_arguments(
#         UT
#         "${options}"
#         "${oneValueArgs}"
#         "${multiValueArgs}"
#         ${ARGN}
#     )

#     add_executable(${UT_NAME} ${UT_FILES})
#     target_link_libraries(${UT_NAME} PRIVATE Catch2::Catch2WithMain ${UT_LIBS})
#     add_test(NAME ${UT_NAME} COMMAND ${UT_NAME})
#     set_tests_properties(${UT_NAME} PROPERTIES TIMEOUT 10)
#     set_target_properties(
#         ${UT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/tests"
#     )
# endfunction()
