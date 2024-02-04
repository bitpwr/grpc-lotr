include(../cmake/grpc.cmake)

set(lib_name lotr-proto)

generate_proto_cpp(${lib_name} protos/lotr.proto)
