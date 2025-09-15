cmake_minimum_required(VERSION 3.27)
project(ece463)

set(CMAKE_CXX_STANDARD 17)

add_executable(ece463 main.cpp
        Cache.cpp
        Cache.h
        types.h
        Set.cpp
        Set.h
        Manager.cpp
        Manager.h
)
