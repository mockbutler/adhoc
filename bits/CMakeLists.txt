cmake_minimum_required(VERSION 3.1)
project(bits)

add_executable(bits bits.cc)
target_compile_features(bits PRIVATE cxx_range_for)

enable_testing()
add_test(NAME core COMMAND bits)
