# Part of LibXtract
#
# SPDX-FileCopyrightText: 2026 Jamie Bullock
# SPDX-License-Identifier: Zlib

# Runs clang-format over the first-party C sources that git tracks; the
# third-party code, the SWIG bindings and the C++ example are excluded. With
# FIX set the files are rewritten in place, otherwise any file that is not
# formatted is an error. CLANG_FORMAT is the clang-format command line,
# clang-format by default. Its version must be the one CI installs, because
# the output differs between releases.

cmake_minimum_required(VERSION 3.21)

if(NOT CLANG_FORMAT)
    set(CLANG_FORMAT clang-format)
endif()
get_filename_component(root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)

execute_process(
    COMMAND git ls-files -- *.c *.h
        :!:src/ooura/** :!:src/c-ringbuf/** :!:src/dywapitchtrack/**
        :!:swig/** :!:examples/simpletest/**
        :!:tests/utest.h :!:bench/ubench.h
    WORKING_DIRECTORY "${root}"
    RESULT_VARIABLE result
    OUTPUT_VARIABLE files
    OUTPUT_STRIP_TRAILING_WHITESPACE)
if(NOT result EQUAL 0)
    message(FATAL_ERROR "git ls-files: ${result}")
endif()
string(REPLACE "\n" ";" files "${files}")

if(FIX)
    set(mode -i)
else()
    set(mode --dry-run --Werror)
endif()
execute_process(
    COMMAND ${CLANG_FORMAT} ${mode} ${files}
    WORKING_DIRECTORY "${root}"
    RESULT_VARIABLE result)
if(NOT result EQUAL 0)
    list(JOIN CLANG_FORMAT " " command)
    message(FATAL_ERROR "${command}: ${result}")
endif()
