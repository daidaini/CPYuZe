cmake_minimum_required(VERSION 3.5)

set(project_name "for_cpp_basis")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -pthread")

project(${project_name})

set(CMAKE_BUILD_TYPE "Debug")

set(SOURCES
    include/t_mutex.h
	include/t_weak_ptr.h
    main.cpp)

add_executable(${project_name} ${SOURCES})

target_include_directories(${project_name} 
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include)
