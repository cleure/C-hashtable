cmake_minimum_required (VERSION 2.6)
project (c_hashtable)

find_library(PTHREAD_LIBRARY NAMES pthread)

set (VERSION_MAJOR 0)
set (VERSION_MINOR 1)
set (VERSION_PATCH 0)
set (CMAKE_C_FLAGS "-Wall -g")

configure_file (
    "${PROJECT_SOURCE_DIR}/config.h.in"
    "${PROJECT_BINARY_DIR}/config.h"
)

include_directories(
    "${PROJECT_SOURCE_DIR}"
    "${PROJECT_BINARY_DIR}"
    "src"
)

set(HTABLE_SOURCES
    src/MurmurHash3.cpp
    src/hashtable.c
)

add_library(htable ${HTABLE_SOURCES})

add_executable(test-01 tests/test-01.c)
target_link_libraries(test-01 htable pthread)
