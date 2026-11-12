# Part of LibXtract
#
# SPDX-FileCopyrightText: 2026 Jamie Bullock
# SPDX-License-Identifier: Zlib

# Apple clang has no libFuzzer runtime: on macOS configure with
# -DCMAKE_C_COMPILER=$(brew --prefix llvm)/bin/clang. AddressSanitizer
# deadlocks during initialisation on some macOS toolchains;
# -DXTRACT_FUZZ_SANITIZERS=undefined fuzzes with UBSan alone there.
set(XTRACT_FUZZ_TIME 60 CACHE STRING "Seconds each fuzz harness runs for under ctest")
set(XTRACT_FUZZ_SANITIZERS "address,undefined" CACHE STRING "Sanitizers the fuzz build enables")

target_compile_options(xtract PRIVATE
    -fsanitize=fuzzer-no-link,${XTRACT_FUZZ_SANITIZERS} -fno-sanitize-recover=all)

foreach(harness scalar delta vector multivariate)
    add_executable(xtfuzz_${harness} fuzz_${harness}.c)
    target_link_libraries(xtfuzz_${harness} PRIVATE xtract::xtract)
    target_compile_options(xtfuzz_${harness} PRIVATE
        -fsanitize=fuzzer,${XTRACT_FUZZ_SANITIZERS} -fno-sanitize-recover=all)
    target_link_options(xtfuzz_${harness} PRIVATE -fsanitize=fuzzer,${XTRACT_FUZZ_SANITIZERS})
    add_test(NAME fuzz_${harness}
        COMMAND xtfuzz_${harness}
            -max_total_time=${XTRACT_FUZZ_TIME}
            -rss_limit_mb=4096
            -artifact_prefix=${CMAKE_CURRENT_BINARY_DIR}/)
endforeach()
