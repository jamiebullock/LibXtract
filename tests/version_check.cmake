# Part of LibXtract
#
# SPDX-FileCopyrightText: 2026 Jamie Bullock
# SPDX-License-Identifier: Zlib

# Runs the version check, SCRIPT, on trees made under WORK of a VERSION file
# and a changelog, and fails when a run does not end as expected.

cmake_minimum_required(VERSION 3.21)

set(runs 0)

function(expect_check outcome version heading)
    math(EXPR runs "${runs} + 1")
    set(runs ${runs} PARENT_SCOPE)
    set(tree "${WORK}/${runs}")
    file(REMOVE_RECURSE "${tree}")
    file(COPY "${SCRIPT}" DESTINATION "${tree}/cmake")
    file(WRITE "${tree}/VERSION" "${version}\n")
    file(WRITE "${tree}/CHANGELOG.md" "# Changelog\n\n${heading}\n")
    execute_process(
        COMMAND "${CMAKE_COMMAND}" ${ARGN} -P "${tree}/cmake/version.cmake"
        RESULT_VARIABLE result
        OUTPUT_QUIET
        ERROR_QUIET)
    if(result EQUAL 0)
        set(ended passes)
    else()
        set(ended fails)
    endif()
    if(NOT ended STREQUAL outcome)
        message(SEND_ERROR "The version check ${ended} for VERSION ${version} and the heading '${heading}' ${ARGN}")
    endif()
endfunction()

expect_check(passes 1.6.0 "## [1.6.0] - 2026-11-16")
expect_check(passes 2.0.0 "## [1.6.0] - 2026-11-16")
expect_check(passes 1.6.1 "## [1.6.0] - 2026-11-16")
expect_check(passes 1.10.0 "## [1.9.0] - 2026-11-16")
expect_check(fails 1.5.9 "## [1.6.0] - 2026-11-16")
expect_check(fails 1.9.0 "## [1.10.0] - 2026-11-16")
expect_check(fails 2.0 "## [1.6.0] - 2026-11-16")
expect_check(fails 2.0.0-dev "## [1.6.0] - 2026-11-16")
expect_check(fails 2.0.0 "## Unreleased")

expect_check(passes 1.6.0 "## [1.6.0] - 2026-11-16" -DRELEASE=ON)
expect_check(fails 2.0.0 "## [1.6.0] - 2026-11-16" -DRELEASE=ON)
expect_check(fails 1.5.9 "## [1.6.0] - 2026-11-16" -DRELEASE=ON)
