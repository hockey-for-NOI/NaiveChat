cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g")

project(NaiveChat)
include_directories(include)

list(APPEND SRC_LIST src/SocketBase.cpp)

add_executable(NaiveChatServer ${SRC_LIST} src/server_main.cpp)
add_executable(NaiveChatClient ${SRC_LIST} src/client_main.cpp)
