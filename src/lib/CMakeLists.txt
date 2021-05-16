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

target_link_libraries(hell PRIVATE xcb dl xcb-keysyms) #use a find for this

target_include_directories(hell
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

set_target_properties(hell PROPERTIES PUBLIC_HEADER "${PUBLIC_HEADERS}")
set_target_properties(hell PROPERTIES EXPORT_NAME Hell)

install(TARGETS hell EXPORT ${PROJECT_NAME}Targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/hell)

add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS hell)
