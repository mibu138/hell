set(SRCS
    client.c
    cmd.c
    common.c
    debug.c
    ds.c
    frame.c
    input.c
    io.c
    mem.c
    server.c
    window.c)

set(PUBLIC_HEADERS 
    attributes.h
    client.h
    cmd.h
    common.h
    debug.h
    ds.h
    hell.h
    input.h
    io.h
    len.h
    locations.h
    minmax.h
    server.h
    types.h
    evcodes.h
    platform.h
    xcb_window_type.h
    win32_window_type.h
    window.h)

add_library(${LIBRARY_NAME} SHARED ${SRCS})
if (DEFINED UNIX)
    target_link_libraries(${LIBRARY_NAME} xcb dl xcb-keysyms)
else()
    message(FATAL_ERROR "Need to specify windows specific libraries needed.")
endif()

install_library()
