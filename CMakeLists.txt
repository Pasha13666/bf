cmake_minimum_required(VERSION 3.13)
project(bf)

set(CMAKE_CXX_STANDARD 17)

file(GLOB SOURCES "src/*.cpp")
file(GLOB HEADERS "include/*.hpp")
message("--${SOURCES}")
add_executable(bf ${SOURCES} ${HEADERS})
target_include_directories(bf PRIVATE include)