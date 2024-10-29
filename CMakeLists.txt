cmake_minimum_required(VERSION 3.22)

project(CodeStatistics LANGUAGES CXX)

#set(CMAKE_BUILD_TYPE Debug)
#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED on)
set(CMAKE_EXPORT_COMPILE_COMMANDS on)

find_package(CLI11 REQUIRED)
find_package(doctest REQUIRED)
find_package(fmt REQUIRED)

add_subdirectory(src)
