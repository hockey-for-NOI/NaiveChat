cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++11 -g -pthread")

project(NaiveChat)
include_directories(include)

list(APPEND SRC_LIST src/Debug.cpp)
list(APPEND SRC_LIST src/SocketBase.cpp)

list(APPEND SERVER_SRC_LIST ${SRC_LIST})
list(APPEND CLIENT_SRC_LIST ${SRC_LIST})

list(APPEND SERVER_SRC_LIST src/ServerSocket.cpp)
list(APPEND SERVER_SRC_LIST src/User.cpp)
list(APPEND SERVER_SRC_LIST src/ServerDB.cpp)

list(APPEND CLIENT_SRC_LIST src/ClientSocket.cpp)

add_executable(Server ${SERVER_SRC_LIST} src/server_main.cpp)
add_executable(Client ${CLIENT_SRC_LIST} src/client_main.cpp)
