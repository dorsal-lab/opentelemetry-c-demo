cmake_minimum_required(VERSION 3.15)

project(opentelemetry-c-demo)

find_library(ZeroMQ czmq REQUIRED)
find_library(opentelemetry-c opentelemetry-c REQUIRED)

# client
add_executable(client)
target_link_libraries(client PRIVATE czmq zmq opentelemetry-c ${OPENTELEMETRY_C_LIBRARIES})
target_include_directories(client PRIVATE include ${OPENTELEMETRY_C_INCLUDE_DIRS})
target_sources(client PRIVATE
    src/client.c)

# proxy
add_executable(proxy)
target_link_libraries(proxy PRIVATE czmq zmq opentelemetry-c ${OPENTELEMETRY_C_LIBRARIES})
target_include_directories(proxy PRIVATE include ${OPENTELEMETRY_C_INCLUDE_DIRS})
target_sources(proxy PRIVATE
    src/proxy.c)

# server
add_executable(server)
target_link_libraries(server PRIVATE czmq zmq opentelemetry-c ${OPENTELEMETRY_C_LIBRARIES})
target_include_directories(server PRIVATE include ${OPENTELEMETRY_C_INCLUDE_DIRS})
target_sources(server PRIVATE
    src/server.c
    src/prime.c)
