# Part of LibXtract
#
# SPDX-FileCopyrightText: 2026 Jamie Bullock
# SPDX-License-Identifier: Zlib

add_executable(xttest
    xttest_main.c
    xttest_delta.c
    xttest_descriptors.c
    xttest_init.c
    xttest_scalar.c
    xttest_stateful.c
    xttest_util.c
    xttest_vector.c
    xttest_window.c)
target_link_libraries(xttest PRIVATE xtract::xtract)
set_target_properties(xttest PROPERTIES
    C_STANDARD 99
    C_STANDARD_REQUIRED ON
    C_EXTENSIONS OFF)

add_test(NAME xttest COMMAND xttest)
