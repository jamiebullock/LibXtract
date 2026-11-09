# Part of LibXtract
#
# SPDX-FileCopyrightText: 2026 Jamie Bullock
# SPDX-License-Identifier: Zlib

cmake_minimum_required(VERSION 3.21)
project(xtract_package_test LANGUAGES C)

find_package(xtract 1.2 REQUIRED)

add_executable(package_test package_test.c)
target_link_libraries(package_test PRIVATE xtract::xtract)

enable_testing()
add_test(NAME package_test COMMAND package_test)
