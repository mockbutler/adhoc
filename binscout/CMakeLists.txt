cmake_minimum_required(VERSION 3.0)
project(binscout)

include(CTest)

set(CMAKE_C_FLAGS "-Wall")
add_executable(binscout binscout.c hex.c hex.h mem.h mem.c bytevec.h bytevec.c)
set_property(TARGET binscout PROPERTY C_STANDARD 11)

#add_executable(test_mem test_mem.c mem.h mem.c)
#add_test(NAME memrev COMMAND test_mem memrev)
#add_test(NAME memeq COMMAND test_mem memeq)
