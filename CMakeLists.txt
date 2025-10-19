cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
project(SonyHeadphonesClient C CXX)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(contrib)
add_subdirectory(libmdr)
add_subdirectory(client)
add_subdirectory(tooling)
