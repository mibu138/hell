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
