# Part of LibXtract
#
# SPDX-FileCopyrightText: 2026 Jamie Bullock
# SPDX-License-Identifier: Zlib

# Runs Vale over the files that git tracks, with the rules that .vale.ini
# names. vale sync downloads them into .vale/styles first. VALE is the vale
# command, vale by default. Its version must be the one CI installs.

cmake_minimum_required(VERSION 3.21)

if(NOT VALE)
    set(VALE vale)
endif()
get_filename_component(root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)

execute_process(
    COMMAND ${VALE} sync
    WORKING_DIRECTORY "${root}"
    RESULT_VARIABLE result)
if(NOT result EQUAL 0)
    message(FATAL_ERROR "${VALE} sync: ${result}")
endif()

execute_process(
    COMMAND git -c core.quotePath=false ls-files
    WORKING_DIRECTORY "${root}"
    RESULT_VARIABLE result
    OUTPUT_VARIABLE files
    OUTPUT_STRIP_TRAILING_WHITESPACE)
if(NOT result EQUAL 0)
    message(FATAL_ERROR "git ls-files: ${result}")
endif()
string(REPLACE ";" "\\;" files "${files}")
string(REPLACE "\n" ";" files "${files}")

execute_process(
    COMMAND ${VALE} --no-wrap ${files}
    WORKING_DIRECTORY "${root}"
    RESULT_VARIABLE result)
if(NOT result EQUAL 0)
    message(FATAL_ERROR "${VALE}: ${result}")
endif()
