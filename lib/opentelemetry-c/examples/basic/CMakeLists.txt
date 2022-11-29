project(basic-example C)
set(CMAKE_CXX_STANDARD 11)
add_executable(basic-example)
target_sources(basic-example PRIVATE main.cpp)
