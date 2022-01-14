cmake_minimum_required(VERSION 3.5)

set(project_name "for_cpp_basis")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -pthread")

#SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -00 -Wall -g -ggdb")
#SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -032 -Wall")

project(${project_name})

set(CMAKE_BUILD_TYPE "Debug")

set(SOURCES
    ../common/TimerLog.h
    ../common/comm.h
    include/t_mutex.h
	include/t_weak_ptr.h
    main.cpp)

include_directories(include)

add_executable(${project_name} ${SOURCES})


target_include_directories(${project_name} 
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/../common
)

install(TARGETS ${project_name} DESTINATION bin)