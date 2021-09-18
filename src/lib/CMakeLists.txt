set(SRCS
    client.c
    cmd.c
    ds.c
    common.c
    debug.c
    buffer.c
    frame.c
    input.c
    io.c
    mem.c
    server.c)

if (NOT ${HELL_NO_WINDOW})
    list(APPEND SRCS window.c)
endif()

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
add_library(hellObj OBJECT)
add_library(hell    SHARED)
target_sources(hellObj PRIVATE ${SRCS})
target_include_directories(hellObj 
    PRIVATE "../include/hell"
    INTERFACE "../include")
if (UNIX)
	target_link_libraries(hellObj INTERFACE xcb dl xcb-keysyms)
elseif (WIN32)
	target_link_libraries(hellObj INTERFACE)
	set_target_properties(hell PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
else()
	message(FATAL_ERROR "Os not supported")
endif()
target_link_libraries(hell PUBLIC hellObj)
add_library(Hell::HellObj ALIAS hellObj)
add_library(Hell::Hell    ALIAS hell)
