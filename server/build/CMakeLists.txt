cmake_minimum_required(VERSION 3.10)
project(QtGoband_EchoServer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Determine project root (one level up from this build folder)
get_filename_component(PROJECT_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/.." ABSOLUTE)

set(MUDUO_SRC_DIR "${PROJECT_ROOT}/my_muduo/src")
set(ECHO_DIR "${PROJECT_ROOT}/EchoServer")

# Collect all .cc sources from my_muduo/src
file(GLOB MUDUO_SOURCES
    "${MUDUO_SRC_DIR}/*.cc"
)

# Add the test server source together with the library sources
set(ALL_SOURCES ${MUDUO_SOURCES} "${ECHO_DIR}/testServer.cc")

add_executable(testServer ${ALL_SOURCES})

target_include_directories(testServer PRIVATE ${MUDUO_SRC_DIR})
target_compile_options(testServer PRIVATE -Wall -Wextra)

find_package(Threads REQUIRED)
target_link_libraries(testServer PRIVATE Threads::Threads)

message(STATUS "PROJECT_ROOT = ${PROJECT_ROOT}")
message(STATUS "MUDUO_SRC_DIR = ${MUDUO_SRC_DIR}")
message(STATUS "EchoServer = ${ECHO_DIR}")
