cmake_minimum_required(VERSION 3.5)

set(project_name "connection_pool")

project(${project_name})

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


add_source(${project_name} ./src)

include_directories(/usr/include)

add_executable(${project_name} ${all_file})

target_link_libraries(${project_name} libmysqlclient.so)