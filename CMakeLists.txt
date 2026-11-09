# Part of LibXtract
#
# SPDX-FileCopyrightText: 2026 Jamie Bullock
# SPDX-License-Identifier: Zlib

cmake_minimum_required(VERSION 3.21)

file(STRINGS VERSION XTRACT_VERSION LIMIT_COUNT 1)
project(xtract VERSION ${XTRACT_VERSION} LANGUAGES C)

option(BUILD_SHARED_LIBS "Build a shared library" OFF)
option(XTRACT_BUILD_TESTS "Build the test suite" ${PROJECT_IS_TOP_LEVEL})
option(XTRACT_BUILD_EXAMPLES "Build the examples" ${PROJECT_IS_TOP_LEVEL})
option(XTRACT_BUILD_BENCH "Build the benchmarks" OFF)
option(XTRACT_BUILD_FUZZ "Build the libFuzzer harnesses, which need clang" OFF)

if(PROJECT_IS_TOP_LEVEL AND NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(GNUInstallDirs)

add_subdirectory(src)

if(XTRACT_BUILD_TESTS OR XTRACT_BUILD_EXAMPLES OR XTRACT_BUILD_FUZZ)
    enable_testing()
endif()
if(XTRACT_BUILD_TESTS)
    add_subdirectory(tests)
endif()
if(XTRACT_BUILD_EXAMPLES)
    add_subdirectory(examples/simpletest)
endif()
if(XTRACT_BUILD_BENCH)
    add_subdirectory(bench)
endif()
if(XTRACT_BUILD_FUZZ)
    add_subdirectory(fuzz)
endif()
