cmake_minimum_required(VERSION 3.4)

project(cs-online-ide)

include(CheckCXXSymbolExists)
include(CheckIncludeFiles)
include(CheckCXXCompilerFlag)
include(CheckCCompilerFlag)
include(CheckCSourceCompiles)

#### Check C++11
check_cxx_compiler_flag("-std=c++11" COMPILER_SUPPORTS_CXX11)
check_cxx_compiler_flag("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if (COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif (COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else ()
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif ()
set(CMAKE_CXX_STANDARD 11)

#### Check C11
check_c_compiler_flag("-std=c11" COMPILER_SUPPORTS_C11)
if (COMPILER_SUPPORTS_CXX11)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11")
else ()
    message(STATUS "The compiler ${CMAKE_C_COMPILER} has no C11 support. Please use a different C compiler.")
endif ()
set(CMAKE_C_STANDARD 11)

#### Check functions
check_cxx_symbol_exists(sigtimedwait csignal HAVE_SIGTIMEDWAIT)

add_definitions("-DHAVE_CONFIG_H")
configure_file(${CMAKE_SOURCE_DIR}/cmake/cs-online-ide-config.h.in
        ${CMAKE_BINARY_DIR}/generated-headers/cs-online-ide-config.h)

include_directories(${CMAKE_BINARY_DIR}/generated-headers)

add_subdirectory(third-party/jsoncpp)
add_subdirectory(third-party/cppcodec)
include_directories(third-party/cppcodec)

add_executable(cs-code-runner cgi-source/cs-code-runner.cpp)
add_executable(cs-online-cgi cgi-source/cs-online-cgi.cpp)

add_dependencies(cs-online-cgi cs-code-runner)

target_link_libraries(cs-online-cgi jsoncpp_lib)
