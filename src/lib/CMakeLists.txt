set(SRCS
    client.c
    cmd.c
    common.c
    debug.c
    buffer.c
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
    buffer.h
    hell.h
    types.h
    input.h
    io.h
    len.h
    locations.h
    minmax.h
    server.h
    types.h
    evcodes.h
    platform.h
    window.h)

include(author_library)
author_library(hell
    EXPORT_NAME Hell
    SOURCES ${SRCS}
    PUBLIC_HEADERS ${PUBLIC_HEADERS}
    DEPS xcb dl xcb-keysyms)
