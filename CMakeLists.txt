cmake_minimum_required(VERSION 3.20)
project(echidna LANGUAGES C CXX)

cmake_policy(SET CMP0048 NEW)
cmake_policy(SET CMP0074 NEW)
cmake_policy(SET CMP0079 NEW)

find_package(MPI 4.0 COMPONENTS C REQUIRED)

add_library(echidna echidna.cc)
target_include_directories(echidna PRIVATE ${MPI_C_INCLUDE_DIRS})
target_include_directories(echidna INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_options(echidna PRIVATE ${MPI_C_COMPILE_OPTIONS})
target_compile_definitions(echidna PRIVATE ${MPI_C_COMPILE_DEFINITIONS})
target_link_libraries(echidna INTERFACE MPI::MPI_C echidna)
set_target_properties(echidna PROPERTIES OUTPUT_NAME echidna)

# vim: set ft=cmake:
