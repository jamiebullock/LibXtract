# Part of LibXtract
#
# SPDX-FileCopyrightText: 2026 Jamie Bullock
# SPDX-License-Identifier: Zlib

add_executable(xtbench bench_scalar.c)
add_executable(xtbench_moving bench_moving.c)
target_link_libraries(xtbench PRIVATE xtract::xtract)
target_link_libraries(xtbench_moving PRIVATE xtract::xtract)

add_custom_target(bench
    COMMAND xtbench
    COMMAND xtbench_moving
    USES_TERMINAL)
