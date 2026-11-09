# Part of LibXtract
#
# SPDX-FileCopyrightText: 2026 Jamie Bullock
# SPDX-License-Identifier: Zlib

set(XTRACT_FUZZ_TIME 60 CACHE STRING "Seconds each fuzz harness runs for under ctest")

target_compile_options(xtract PRIVATE -fsanitize=fuzzer-no-link)

foreach(harness scalar delta vector)
    add_executable(xtfuzz_${harness} fuzz_${harness}.c)
    target_link_libraries(xtfuzz_${harness} PRIVATE xtract::xtract)
    target_compile_options(xtfuzz_${harness} PRIVATE -fsanitize=fuzzer)
    target_link_options(xtfuzz_${harness} PRIVATE -fsanitize=fuzzer)
    add_test(NAME fuzz_${harness}
        COMMAND xtfuzz_${harness}
            -max_total_time=${XTRACT_FUZZ_TIME}
            -rss_limit_mb=4096
            -artifact_prefix=${CMAKE_CURRENT_BINARY_DIR}/)
endforeach()
