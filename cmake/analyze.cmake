# Part of LibXtract
#
# SPDX-FileCopyrightText: 2026 Jamie Bullock
# SPDX-License-Identifier: Zlib

# Runs the clang static analyzer over SOURCES with the flags recorded for
# each of them in COMPILE_COMMANDS and fails on any finding. NDEBUG is
# dropped so that the asserts state their invariants to the analyzer.

file(READ "${COMPILE_COMMANDS}" database)
string(JSON count LENGTH "${database}")
math(EXPR last "${count} - 1")

set(findings "")
foreach(index RANGE ${last})
    string(JSON file GET "${database}" ${index} file)
    if(NOT file IN_LIST SOURCES)
        continue()
    endif()
    string(JSON directory GET "${database}" ${index} directory)
    string(JSON command GET "${database}" ${index} command)
    separate_arguments(arguments NATIVE_COMMAND "${command}")
    list(POP_FRONT arguments)
    list(FIND arguments -o output_position)
    if(output_position GREATER -1)
        list(REMOVE_AT arguments ${output_position})
        list(REMOVE_AT arguments ${output_position})
    endif()
    list(REMOVE_ITEM arguments -c -DNDEBUG -Wconditional-uninitialized "${file}")
    execute_process(
        COMMAND ${CLANG} --analyze
            -Xanalyzer -analyzer-output=text
            -Xanalyzer -analyzer-checker=security
            -Xanalyzer -analyzer-checker=optin.core.EnumCastOutOfRange
            ${arguments} "${file}" -o /dev/null
        WORKING_DIRECTORY "${directory}"
        OUTPUT_VARIABLE output
        ERROR_VARIABLE errors)
    string(APPEND findings "${output}${errors}")
endforeach()

if(findings)
    message(FATAL_ERROR "${findings}")
endif()
message(STATUS "static analysis clean")
