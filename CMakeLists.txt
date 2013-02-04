cmake_minimum_required (VERSION 2.6)
project (c_hashtable)

find_library(PTHREAD_LIBRARY NAMES pthread)

set (VERSION_MAJOR 0)
set (VERSION_MINOR 1)
set (VERSION_PATCH 0)
set (CMAKE_C_FLAGS "-Wall -g -std=c89 -pedantic")

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
    src/MurmurHash3.c
    src/hashtable.c
)

add_library(htable ${HTABLE_SOURCES})

add_executable(tests/bin/test-01-new tests/test-01-new.c)
target_link_libraries(tests/bin/test-01-new htable)

add_executable(tests/bin/test-02-add tests/test-02-add.c)
target_link_libraries(tests/bin/test-02-add htable)

add_executable(tests/bin/test-03-clone tests/test-03-clone.c)
target_link_libraries(tests/bin/test-03-clone htable)

add_executable(tests/bin/test-04-resize tests/test-04-resize.c)
target_link_libraries(tests/bin/test-04-resize htable)

add_executable(tests/bin/test-05-update tests/test-05-update.c)
target_link_libraries(tests/bin/test-05-update htable)

add_executable(tests/bin/test-06-remove tests/test-06-remove.c)
target_link_libraries(tests/bin/test-06-remove htable)

add_executable(tests/bin/test-07-get tests/test-07-get.c)
target_link_libraries(tests/bin/test-07-get htable)

add_executable(tests/bin/test-08-intersect tests/test-08-intersect.c)
target_link_libraries(tests/bin/test-08-intersect htable)

add_executable(tests/bin/test-09-difference tests/test-09-difference.c)
target_link_libraries(tests/bin/test-09-difference htable)

add_executable(tests/bin/test-10-integers tests/test-10-integers.c)
target_link_libraries(tests/bin/test-10-integers htable)
