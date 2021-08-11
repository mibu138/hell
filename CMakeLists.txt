cmake_minimum_required(VERSION 3.17)

set(CMAKE_C_STANDARD 11)

project(Hell LANGUAGES C VERSION 0.1.0)

if(WIN32)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
	add_compile_definitions(WIN32)
	set(CMAKE_WIN32_EXECUTABLE TRUE)
endif()

add_subdirectory(src/lib)
add_subdirectory(src/examples)
