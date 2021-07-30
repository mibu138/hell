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
    server.c
    window.c)
set(HEADERS 
    ../include/hell/attributes.h
    ../include/hell/buffer.h
    ../include/hell/client.h
    ../include/hell/CMakeLists.txt
    ../include/hell/cmd.h
    ../include/hell/common.h
    ../include/hell/debug.h
    ../include/hell/ds.h
    ../include/hell/evcodes.h
    ../include/hell/hell.h
    ../include/hell/input.h
    ../include/hell/io.h
    ../include/hell/len.h
    ../include/hell/locations.h
    ../include/hell/minmax.h
    ../include/hell/platform.h
    ../include/hell/server.h
    ../include/hell/types.h
    ../include/hell/unix_window.h
    ../include/hell/win32_window.h
    ../include/hell/win32_window_type.h
    ../include/hell/window.h
    ../include/hell/win_local.h
    ../include/hell/xcb_window_type.h)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
add_library(hellObj OBJECT)
add_library(hell    SHARED)
target_sources(hellObj PRIVATE ${SRCS} PUBLIC ${HEADERS})
target_include_directories(hellObj 
    PRIVATE "../include/hell"
    INTERFACE "../include")
target_link_libraries(hellObj INTERFACE xcb dl xcb-keysyms)
target_link_libraries(hell PUBLIC hellObj)
add_library(Hell::HellObj ALIAS hellObj)
add_library(Hell::Hell    ALIAS hell)
