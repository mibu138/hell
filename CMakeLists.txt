cmake_minimum_required(VERSION 3.17)

set(CMAKE_C_STANDARD 11)

project(Hell LANGUAGES C VERSION 0.1.0)

option(HELL_SKIP_EXAMPLES "Skip building hell examples" OFF)
option(HELL_NO_WINDOW "Skip building hell window" OFF)

if(WIN32)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
	add_compile_definitions(WIN32)
	set(CMAKE_WIN32_EXECUTABLE TRUE)
elseif(UNIX)
	add_compile_definitions(UNIX)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
endif()

add_subdirectory(src/lib)
add_subdirectory(src/examples)
