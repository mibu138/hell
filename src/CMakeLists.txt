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

add_library(hell SHARED ${SRCS})
add_executable(demo main.c)
target_link_libraries(hell xcb dl xcb-keysyms)
target_link_libraries(demo hell)
set_target_properties(hell PROPERTIES PUBLIC_HEADER "${PUBLIC_HEADERS}")
install(TARGETS hell
    LIBRARY DESTINATION lib 
    PUBLIC_HEADER DESTINATION include/hell)

install(TARGETS hell 
    LIBRARY DESTINATION ${HOUDINI_TOOL_INS_DIR}/lib 
    PUBLIC_HEADER DESTINATION ${HOUDINI_TOOL_INS_DIR}/include/hell)
