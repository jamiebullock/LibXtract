# Part of LibXtract
#
# SPDX-FileCopyrightText: 2026 Jamie Bullock
# SPDX-License-Identifier: Zlib

enable_language(CXX)

add_executable(simpletest simpletest.cpp)
target_link_libraries(simpletest PRIVATE xtract::xtract)
set_target_properties(simpletest PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF)

add_test(NAME simpletest COMMAND simpletest WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
