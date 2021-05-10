cmake_minimum_required(VERSION 3.17)

project(hell)

if (NOT DEFINED GLOBAL_CMAKE_PATH)
    include(/home/michaelb/dev/cmake/global.cmake)
else()
    include({GLOBAL_CMAKE_PATH})
endif()

set(LIBRARY_NAME ${CMAKE_PROJECT_NAME})

add_subdirectory(src)


