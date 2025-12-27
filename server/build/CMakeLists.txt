# cmake_minimum_required(VERSION 3.10)
# project(QtGoband_EchoServer LANGUAGES CXX)

# set(CMAKE_CXX_STANDARD 14)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)

# # Determine project root (one level up from this build folder)
# get_filename_component(PROJECT_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/.." ABSOLUTE)

# set(MUDUO_SRC_DIR "${PROJECT_ROOT}/my_muduo/src")
# # set(ECHO_DIR "${PROJECT_ROOT}/EchoServer")
# set(ECHO_DIR "${PROJECT_ROOT}/GobandServer")

# # Collect all .cc sources from my_muduo/src
# file(GLOB MUDUO_SOURCES
#     "${MUDUO_SRC_DIR}/*.cc"
# )

# # Add the test server source together with the library sources
# set(ALL_SOURCES ${MUDUO_SOURCES} "${ECHO_DIR}/goband.cc")

# add_executable(goband ${ALL_SOURCES})

# target_include_directories(goband PRIVATE ${MUDUO_SRC_DIR})
# target_compile_options(goband PRIVATE -Wall -Wextra)

# find_package(Threads REQUIRED)
# target_link_libraries(goband PRIVATE Threads::Threads)

# message(STATUS "PROJECT_ROOT = ${PROJECT_ROOT}")
# message(STATUS "MUDUO_SRC_DIR = ${MUDUO_SRC_DIR}")
# message(STATUS "EchoServer = ${ECHO_DIR}")


cmake_minimum_required(VERSION 3.10)
project(QtGoband LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 项目根目录
get_filename_component(PROJECT_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/.." ABSOLUTE)

# muduo 源码目录
set(MUDUO_SRC_DIR "${PROJECT_ROOT}/my_muduo")
# GobandServer 源码目录
set(GOBAND_SRC_DIR "${PROJECT_ROOT}/GobandServer")

# 收集所有源文件
file(GLOB MUDUO_SOURCES "${MUDUO_SRC_DIR}/*.cc")
file(GLOB GOBAND_SOURCES "${GOBAND_SRC_DIR}/*.cc")

# 合并所有源文件
set(ALL_SOURCES
    ${MUDUO_SOURCES}
    ${GOBAND_SOURCES}
)

# 添加可执行文件
add_executable(goband ${ALL_SOURCES})

# include 目录
target_include_directories(goband PRIVATE
    ${MUDUO_SRC_DIR}
    ${GOBAND_SRC_DIR}
    /usr/include/jsoncpp
    /usr/include/mysql
)

target_compile_options(goband PRIVATE -Wall -Wextra)

# 线程库
find_package(Threads REQUIRED)

# 链接库
target_link_libraries(goband PRIVATE
    Threads::Threads
    jsoncpp
    mysqlclient
)

# Debug 输出
message(STATUS "PROJECT_ROOT = ${PROJECT_ROOT}")
message(STATUS "MUDUO_SRC_DIR = ${MUDUO_SRC_DIR}")
message(STATUS "GOBAND_SRC_DIR = ${GOBAND_SRC_DIR}")
message(STATUS "ALL_SOURCES = ${ALL_SOURCES}")


