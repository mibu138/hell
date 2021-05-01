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

target_link_libraries(${LIBRARY_NAME} xcb dl xcb-keysyms)
set_target_properties(${LIBRARY_NAME} PROPERTIES PUBLIC_HEADER "${PUBLIC_HEADERS}")

if (NOT DEFINED LIB_INSTALL_PREFIX)
    set(LIB_INSTALL_PREFIX lib)
endif()

install(TARGETS ${LIBRARY_NAME} 
    LIBRARY DESTINATION ${LIB_INSTALL_PREFIX})

if (NOT DEFINED LIB_ONLY)
    add_executable(demo main.c)
    target_link_libraries(demo ${LIBRARY_NAME})
endif()

if (NOT DEFINED NO_HEADER_INSTALL)
    install(TARGETS ${LIBRARY_NAME} 
        PUBLIC_HEADER DESTINATION ${HEADER_INSTALL_PREFIX}${LIBRARY_NAME})
endif()
