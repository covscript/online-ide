cmake_minimum_required(VERSION 3.4)

project(cs-online)

set(CMAKE_CXX_STANDARD 14)

include_directories(include)

SET(jsoncpp_sources
        lib_json/json_tool.h
        lib_json/json_reader.cpp
        lib_json/json_valueiterator.inl
        lib_json/json_value.cpp
        lib_json/json_writer.cpp
        lib_json/version.h.in)

add_library(jsoncpp_lib STATIC ${jsoncpp_sources})

add_executable(cs-code-runner cgi-source/cs-code-runner.cpp)
add_executable(cs-online-cgi  cgi-source/cs-online-cgi.cpp)

add_dependencies(cs-online-cgi cs-code-runner)

target_link_libraries(cs-online-cgi jsoncpp_lib)