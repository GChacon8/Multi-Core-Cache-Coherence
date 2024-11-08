cmake_minimum_required(VERSION 3.27)
project(Multi_Core_Cache_Coherence)

set(CMAKE_CXX_STANDARD 14)

add_executable(Multi_Core_Cache_Coherence
        bus.cpp
        cache.cpp
        main.cpp
        MESI.cpp
        PE.cpp
        RAM.cpp
        ROM.cpp)
