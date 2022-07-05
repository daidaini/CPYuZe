cmake_minimum_required(VERSION 3.5)

#工程名
SET(PROJECT_NAME "test_epoll")
project(${PROJECT_NAME})

set(CMAKE_BUILD_TYPE "Debug")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -pthread")

set(SOURCES
    include/base.h
    include/base.cpp
    main.cpp)

#link_directories(${PROJECT_SOURCE_DIR}/../../fmt)  

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} 
                        PRIVATE
                            libfmt.a)
