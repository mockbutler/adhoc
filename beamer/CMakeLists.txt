cmake_minimum_required(VERSION 3.22)
project(beamer C)

add_definitions(-D_XOPEN_SOURCE=500 -D_GNU_SOURCE)
add_compile_options(-Wall -Wextra)
add_executable(beamer beamer.c)
set_property(TARGET beamer PROPERTY C_STANDARD 11)