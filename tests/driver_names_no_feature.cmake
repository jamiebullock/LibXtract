# Part of LibXtract
#
# SPDX-FileCopyrightText: 2026 Jamie Bullock
# SPDX-License-Identifier: Zlib

# Fails when the driver names a feature. It calls every feature from the
# descriptors alone, so its source has the name of none of them. FEATURES is
# the header with the enumeration and DRIVER the source of the driver.

cmake_minimum_required(VERSION 3.21)

file(READ "${FEATURES}" header)
string(REGEX MATCHALL "XTRACT_[A-Z0-9_]+," enumerators "${header}")
list(LENGTH enumerators count)
if(count LESS 100)
    message(FATAL_ERROR "${FEATURES}: found ${count} features, expected the whole enumeration")
endif()
file(READ "${DRIVER}" source)
string(REGEX MATCHALL "XTRACT_[A-Z0-9_]+" used "${source}")
foreach(enumerator ${enumerators})
    string(REPLACE "," "" enumerator "${enumerator}")
    if(enumerator IN_LIST used)
        message(FATAL_ERROR "${DRIVER} names the feature ${enumerator}")
    endif()
endforeach()
