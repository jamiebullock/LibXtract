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
option(XTRACT_COVERAGE "Instrument the build for gcov and add a coverage report target" OFF)
set(XTRACT_BUILD_PYTHON AUTO CACHE STRING "Build the Python bindings: ON, OFF or AUTO")
set(XTRACT_BUILD_JAVA AUTO CACHE STRING "Build the Java bindings: ON, OFF or AUTO")
set(XTRACT_BUILD_NODE AUTO CACHE STRING "Build the Node.js bindings: ON, OFF or AUTO")
set_property(CACHE XTRACT_BUILD_PYTHON XTRACT_BUILD_JAVA XTRACT_BUILD_NODE PROPERTY STRINGS AUTO ON OFF)

if(PROJECT_IS_TOP_LEVEL AND NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(GNUInstallDirs)

if(XTRACT_COVERAGE)
    add_compile_options(--coverage)
    add_link_options(--coverage)
endif()

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
if(NOT (XTRACT_BUILD_PYTHON STREQUAL "OFF" AND XTRACT_BUILD_JAVA STREQUAL "OFF" AND XTRACT_BUILD_NODE STREQUAL "OFF"))
    enable_testing()
    add_subdirectory(swig)
endif()
