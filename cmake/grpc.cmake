include_guard()

include(${CMAKE_CURRENT_LIST_DIR}/common.cmake)

find_package(Protobuf REQUIRED)
find_package(gRPC REQUIRED)

set(out_folder ${CMAKE_BINARY_DIR}/proto)
file(MAKE_DIRECTORY ${out_folder})

# ~~~
# generates proto cpp sources and builds them into a static library
#
# generate_proto_cpp(LIB_NAME proto1 proto2 ...)
#   LIB_NAME    name of static lib to create
#   protoX      relative path to protofiles
# ~~~
function(generate_proto_cpp LIB_NAME)
    if(NOT ARGN)
        message(SEND_ERROR "Error: generate_proto_cpp() called without any proto files")
        return()
    endif()

    foreach(file ${ARGN})
        message(STATUS "Process ${file}")

        get_filename_component(proto_file ${file} ABSOLUTE)
        get_filename_component(proto_name ${file} NAME_WE)
        get_filename_component(proto_path ${proto_file} PATH)

        # if cross-compiling, find host plugin
        if(CMAKE_CROSSCOMPILING)
            find_program(_gRPC_CPP_PLUGIN grpc_cpp_plugin)
        else()
            set(_gRPC_CPP_PLUGIN ${GRPC_CPP_PLUGIN_PROGRAM})
        endif()

        set(_GRPC_HEADERS
            "${out_folder}/${proto_name}.grpc.pb.h" "${out_folder}/${proto_name}_mock.grpc.pb.h"
            "${out_folder}/${proto_name}.pb.h"
        )
        set(_GRPC_SRCS "${out_folder}/${proto_name}.grpc.pb.cc" "${out_folder}/${proto_name}.pb.cc")
        list(APPEND GRPC_CPP ${_GRPC_SRCS})
        list(APPEND GRPC_HPP ${_GRPC_HEADERS})

        add_custom_command(
            OUTPUT ${_GRPC_SRCS} ${_GRPC_HEADERS}
            COMMAND
                ${Protobuf_PROTOC_EXECUTABLE} ARGS --grpc_out=generate_mock_code=true:${out_folder}
                --cpp_out=${out_folder} --plugin=protoc-gen-grpc=${_gRPC_CPP_PLUGIN} -I
                ${protobuf_INCLUDE_DIR} -I ${proto_path} ${proto_file}
            DEPENDS ${proto_file} ${Protobuf_PROTOC_EXECUTABLE} ${_gRPC_CPP_PLUGIN}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            COMMENT "Running gRPC C++ protocol buffer compiler on ${file}"
            VERBATIM
        )

    endforeach()

    add_library(${LIB_NAME} STATIC ${GRPC_CPP})
    target_include_directories(${LIB_NAME} SYSTEM PUBLIC ${out_folder})
    target_link_libraries(${LIB_NAME} PUBLIC protobuf::protobuf grpc::grpc)
    setup_target(${LIB_NAME})

endfunction()
