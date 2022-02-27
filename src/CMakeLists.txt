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

#set(CMAKE_POSITION_INDEPENDENT_CODE ON)
add_library(hell    STATIC)
target_sources(hell PRIVATE ${SRCS})
target_include_directories(hell
    PRIVATE "../include/hell"
    INTERFACE "../include")
if (UNIX)
	target_link_libraries(hell INTERFACE xcb dl xcb-keysyms xcb-xinput)
elseif (WIN32)
	target_link_libraries(hell INTERFACE)
	set_target_properties(hell PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
else()
	message(FATAL_ERROR "Os not supported")
endif()
add_library(Hell::Hell    ALIAS hell)
