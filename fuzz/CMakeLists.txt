# Part of LibXtract
#
# SPDX-FileCopyrightText: 2026 Jamie Bullock
# SPDX-License-Identifier: Zlib

# One libFuzzer harness per feature header, each run by ctest for
# XTRACT_FUZZ_TIME seconds. The compiler must be a clang with the libFuzzer
# runtime, which Apple clang lacks: on macOS install LLVM from Homebrew and
# configure with -DCMAKE_C_COMPILER=/opt/homebrew/opt/llvm/bin/clang.
# AddressSanitizer's runtime deadlocks during initialisation on some macOS
# toolchains; -DXTRACT_FUZZ_SANITIZERS=undefined fuzzes with UBSan alone
# there.
set(XTRACT_FUZZ_TIME 60 CACHE STRING "Seconds each fuzz harness runs for under ctest")
set(XTRACT_FUZZ_SANITIZERS "address,undefined" CACHE STRING "Sanitizers the fuzz build enables")

target_compile_options(xtract PRIVATE
    -fsanitize=fuzzer-no-link,${XTRACT_FUZZ_SANITIZERS} -fno-sanitize-recover=all)

foreach(harness scalar delta vector)
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
