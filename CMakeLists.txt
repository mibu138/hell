cmake_minimum_required(VERSION 3.17)

project(Hell VERSION 1.0.0)

include(GNUInstallDirs)
include(cmake/macros.cmake)

add_subdirectory(src/lib)
add_subdirectory(src/examples)
add_subdirectory(cmake)

