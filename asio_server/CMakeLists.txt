cmake_minimum_required(VERSION 3.5)

#工程名
SET(PROJECT_NAME "asio_server")
project(${PROJECT_NAME})

set(CMAKE_BUILD_TYPE "Debug")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -pthread")

set(SOURCES
    server.h
    main.cpp)

set(EXECUTABLE_OUTPUT_PATH ../)

link_directories(/usr/boost_lib/stage/lib)  

add_executable(${PROJECT_NAME} ${SOURCES})


#target_link_libraries(${PROJECT_NAME} libboost_filesystem.so)
