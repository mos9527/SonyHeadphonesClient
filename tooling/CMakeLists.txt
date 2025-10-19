cmake_minimum_required(VERSION 3.26)
file(
    GLOB_RECURSE INCLUDES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
)

find_package(Clang)

include(FetchContent)
FetchContent_MakeAvailable(fmt)

add_executable(tooling_EnumCodegen EnumCodegen.cpp)
target_link_libraries(tooling_EnumCodegen PRIVATE fmt::fmt libclang)

add_executable(tooling_SerializationCodegen SerializationCodegen.cpp)
target_link_libraries(tooling_SerializationCodegen PRIVATE fmt::fmt libclang)