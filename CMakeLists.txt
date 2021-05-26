cmake_minimum_required(VERSION 3.17)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/cmake-common)

project(Hell VERSION 0.1.0)

include(header)

add_subdirectory(src/lib)
add_subdirectory(src/examples)
add_subdirectory(cmake)

