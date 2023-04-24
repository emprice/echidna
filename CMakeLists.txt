cmake_minimum_required(VERSION 3.20)
project(echidna LANGUAGES C CXX)

cmake_policy(SET CMP0048 NEW)
cmake_policy(SET CMP0074 NEW)
cmake_policy(SET CMP0079 NEW)

find_package(MPI 4.0 COMPONENTS C REQUIRED)

add_library(echidna INTERFACE)

target_sources(echidna INTERFACE mutex.cc jobmanager.cc jobworker.cc)
target_include_directories(echidna INTERFACE
    ${MPI_C_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_options(echidna INTERFACE ${MPI_C_COMPILE_OPTIONS})
target_compile_definitions(echidna INTERFACE ${MPI_C_COMPILE_DEFINITIONS})

target_link_libraries(echidna INTERFACE MPI::MPI_C)

add_executable(mutex_demo mutex_demo.cc)
target_link_libraries(mutex_demo echidna)

add_executable(manager_demo manager_demo.cc)
target_link_libraries(manager_demo echidna)

# vim: set ft=cmake:
