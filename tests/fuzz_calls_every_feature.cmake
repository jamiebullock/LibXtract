# Part of LibXtract
#
# SPDX-FileCopyrightText: 2026 Jamie Bullock
# SPDX-License-Identifier: Zlib

# Fails when a feature is in no fuzz harness: none names its enumerator or
# calls its function outside a comment. FEATURES is the header with the
# enumeration and FUZZ the directory of the harnesses.

cmake_minimum_required(VERSION 3.21)

file(READ "${FEATURES}" header)
string(REGEX MATCHALL "XTRACT_[A-Z0-9_]+," enumerators "${header}")
list(LENGTH enumerators count)
if(count LESS 100)
    message(FATAL_ERROR "${FEATURES}: found ${count} features, expected the whole enumeration")
endif()
file(GLOB harnesses "${FUZZ}/fuzz_*.c")
set(source "")
foreach(harness ${harnesses})
    file(READ "${harness}" text)
    string(APPEND source "${text}")
endforeach()
string(REGEX REPLACE "/\\*([^*]|\\*+[^*/])*\\*+/" "" source "${source}")
string(REGEX MATCHALL "XTRACT_[A-Z0-9_]+" named "${source}")
string(REGEX MATCHALL "xtract_[a-z0-9_]+\\(" called "${source}")
set(missing "")
foreach(enumerator ${enumerators})
    string(REPLACE "," "" enumerator "${enumerator}")
    string(TOLOWER "${enumerator}(" function)
    if(NOT enumerator IN_LIST named AND NOT function IN_LIST called)
        list(APPEND missing "${enumerator}")
    endif()
endforeach()
if(missing)
    list(JOIN missing ", " missing)
    message(FATAL_ERROR "No fuzz harness calls ${missing}")
endif()
