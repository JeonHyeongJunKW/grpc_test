cmake_minimum_required(VERSION 3.16)

project(counting_request_client)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Release)

find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)
find_program(GRPC_CPP_PLUGIN_EXECUTABLE grpc_cpp_plugin)

set(PROTO_PATH "${CMAKE_CURRENT_SOURCE_DIR}/proto")
message(STATUS "PROTO_PATH: ${PROTO_PATH}")
set(GEN_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated_proto")
file(MAKE_DIRECTORY "${GEN_DIR}")
message(STATUS "GEN_DIR: ${GEN_DIR}")

set(proto_srcs "${GEN_DIR}/counting_service.pb.cc")
set(proto_hdrs "${GEN_DIR}/counting_service.pb.h")
set(grpc_srcs "${GEN_DIR}/counting_service.grpc.pb.cc")
set(grpc_hdrs "${GEN_DIR}/counting_service.grpc.pb.h")
add_custom_command(
  OUTPUT "${proto_srcs}" "${proto_hdrs}" "${grpc_srcs}" "${grpc_hdrs}"
  COMMAND protoc
  ARGS --grpc_out=${GEN_DIR}
       --cpp_out=${GEN_DIR}
       -I ${PROTO_PATH}
       --plugin=protoc-gen-grpc=${GRPC_CPP_PLUGIN_EXECUTABLE}
       "${PROTO_PATH}/counting_service.proto"
  DEPENDS ${PROTO_PATH}/counting_service.proto
)

add_library(counting_service_proto
  ${proto_srcs}
  ${proto_hdrs}
  ${grpc_srcs}
  ${grpc_hdrs}
)
target_include_directories(
  counting_service_proto
  PUBLIC
    ${GEN_DIR}
)
target_link_libraries(counting_service_proto
  PUBLIC
    gRPC::grpc++
    protobuf::libprotobuf
)

add_executable(counting_request_client
  src/main.cpp
  src/counting_request_client.cpp
)
target_include_directories(
  counting_request_client
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_link_libraries(counting_request_client
  PRIVATE
    counting_service_proto
)
