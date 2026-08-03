cmake_minimum_required(VERSION 3.31)

add_executable(mdr_tests Main.cpp)
target_link_libraries(mdr_tests PRIVATE mdr)
