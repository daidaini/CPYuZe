cmake_minimum_required(VERSION 3.5)

#工程名
SET(PROJECT_NAME "hd_mid_conn")
project(${PROJECT_NAME})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -pthread")

macro(add_source soure_name soure_dir)
    include_directories(${soure_dir})
    set(hfind ${ARGV1})
    string(APPEND hfind "/*.h")
    set(cppfind ${ARGV1})
    string(APPEND cppfind "/*.cpp")
    file(GLOB name_header ${hfind})
    file(GLOB name_cpp ${cppfind})
    set(all_file ${all_file} ${name_header} ${name_cpp})
    source_group(${ARGV0} FILES ${name_header} ${name_cpp})
endmacro(add_source)

add_definitions("-D_CRT_SECURE_NO_WARNINGS")
add_definitions("-DUNICODE -D_UNICODE")

add_source(src ./src)
add_source(Utility ./Utility)
add_source(interface ./interface)
add_source(base ./base)
add_source(json ./jsoncpp)

include_directories(./Log)

#add_executable(${PROJECT_NAME} ${all_file} main.cpp)
add_library(${PROJECT_NAME} SHARED ${all_file})