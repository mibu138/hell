add_executable(simple simple.c)
add_executable(mouse mouse-input.c)

target_link_libraries(simple Hell::Hell)
target_link_libraries(mouse Hell::Hell)
