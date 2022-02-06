cmake_minimum_required(VERSION 3.17)

set(CMAKE_C_STANDARD 11)

project(Hell LANGUAGES C VERSION 0.1.0)

option(HELL_SKIP_EXAMPLES "Skip building hell examples" OFF)
option(HELL_SKIP_TESTS "Skip building hell tests" OFF)
option(HELL_NO_WINDOW "Skip building hell window" OFF)
option(HELL_XCB_DEBUG_MESSAGES "Turn on debug messages for xcb" OFF)
option(HELL_VAR_VALUE_64_BIT "Make Hell_Var values doubles (floats by default)" OFF)

if (${HELL_XCB_DEBUG_MESSAGES})
    add_compile_definitions(HELL_XCB_DEBUG_MESSAGES)
endif()

if (${HELL_VAR_VALUE_64_BIT})
    add_compile_definitions(HELL_VAR_VALUE_64_BIT)
endif()

if (MSVC)
    # warning level 4 and all warnings as errors
    add_compile_options(/W4 /WX)
else()
    # lots of warnings and all warnings as errors
    add_compile_options(-Wall -Wno-missing-braces)
endif()

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
if (NOT HELL_SKIP_TESTS)
    enable_testing()
    add_subdirectory(src/tests)
endif()
