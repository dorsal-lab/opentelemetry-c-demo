cmake_minimum_required(VERSION 3.15)

project(otel-experiment)

find_library(ZeroMQ czmq REQUIRED)

set(BUILD_EXAMPLES OFF)
add_subdirectory(third-party/opentelemetry-c)

# client
add_executable(client)
target_link_libraries(client PRIVATE czmq zmq opentelemetry-c)
target_include_directories(client PRIVATE include)
target_sources(client PRIVATE
    src/client.c)

# proxy
add_executable(proxy)
target_link_libraries(proxy PRIVATE czmq zmq opentelemetry-c)
target_include_directories(proxy PRIVATE include)
target_sources(proxy PRIVATE
    src/proxy.c)

# server
add_executable(server)
target_link_libraries(server PRIVATE czmq zmq opentelemetry-c)
target_include_directories(server PRIVATE include)
target_sources(server PRIVATE
    src/server.c
    src/prime.c)
