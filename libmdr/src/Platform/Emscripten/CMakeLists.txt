cmake_minimum_required(VERSION 3.31)

add_library(mdr_PlatformEmscripten PlatformEmscripten.cpp)
target_link_libraries(mdr_PlatformEmscripten PRIVATE mdr-c mdr)