cmake_minimum_required(VERSION 3.5)

#工程名
SET(PROJECT_NAME "thread_pool")
project(${PROJECT_NAME})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -pthread")

macro(add_package name dir)
	include_directories(${dir})
	set(hfind ${ARGV1})
	string(APPEND hfind "/*.h")
	set(hppfind ${ARGV1})
	string(APPEND hppfind "/*.hpp")
	set(cppfind ${ARGV1})
	string(APPEND cppfind "/*.cpp")
	set(cfind ${ARGV1})
	string(APPEND cfind "/*.c")
	file(GLOB_RECURSE name_header ${hfind})
	file(GLOB_RECURSE name_hpp ${hppfind})
	file(GLOB_RECURSE name_cpp ${cppfind})
	file(GLOB_RECURSE name_c ${cfind})
	source_group(${ARGV0} FILES  ${name_header} ${name_hpp} ${name_cpp} ${name_c})
	set(all_file ${all_file}  ${name_header}  ${name_hpp} ${name_cpp} ${name_c})
endmacro(add_package)

add_package(lib ./lib)

add_executable(${PROJECT_NAME} main.cpp ${all_file})
