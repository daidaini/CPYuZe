cmake_minimum_required(VERSION 3.5)

#工程名
SET(PROJECT_NAME "thread_pool")
project(${PROJECT_NAME})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -pthread")

add_executable(${PROJECT_NAME} main.cpp thread_pool.h)