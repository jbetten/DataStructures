cmake_minimum_required(VERSION 2.8)
project(DataStructures)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED on)

#if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
#endif()

include_directories(include)

add_subdirectory(test)

