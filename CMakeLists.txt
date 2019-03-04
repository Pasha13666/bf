cmake_minimum_required(VERSION 3.12)
project(bf)

set(CMAKE_CXX_STANDARD 17)

file(GLOB SOURCES "src/*.cpp")
file(GLOB HEADERS "include/*.hpp")
add_executable(bf ${SOURCES} ${HEADERS})
target_include_directories(bf PRIVATE include)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL GNU)
    target_link_libraries(bf stdc++fs)
else()
    target_link_libraries(bf c++fs)
endif()