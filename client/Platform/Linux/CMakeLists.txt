cmake_minimum_required(VERSION 3.26)

add_library(Client_PlatformLinux STATIC)

target_sources(
        Client_PlatformLinux
        PUBLIC
        PlatformLinux.cpp
)