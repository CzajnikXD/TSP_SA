cmake_minimum_required(VERSION 3.24)
project(TSP_SA)

set(CMAKE_CXX_STANDARD 23)

# Add optimization flags for different build types
set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -flto -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "-g")

add_executable(TSP_SA main.cpp)

# Set Release as the default build type if not specified
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()
