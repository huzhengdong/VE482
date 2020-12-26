cmake_minimum_required(VERSION 3.11)
project(Linklist C)

set(CMAKE_C_STANDARD 99)
add_library(kernel_dynamic SHARED kernel.c)
add_library(api_dynamic SHARED api.c)
target_link_libraries(api_dynamic kernel_dynamic)

add_executable(Cmdlist Cmd.c interface.c interface.h)
add_executable(Menulist Menu.c interface.c interface.h)

target_link_libraries(Cmdlist api_dynamic kernel_dynamic)
target_link_libraries(Menulist api_dynamic kernel_dynamic)