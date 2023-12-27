cmake_minimum_required(VERSION 3.2...3.28)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(Radish VERSION 1.0.0 LANGUAGES CXX)

option(RADISH_BUILD_TESTS "Create test targets" ON)

enable_testing()

add_subdirectory(src)

if(RADISH_BUILD_TESTS) 
    add_subdirectory(test)
endif()
