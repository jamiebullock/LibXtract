# Part of LibXtract
#
# SPDX-FileCopyrightText: 2026 Jamie Bullock
# SPDX-License-Identifier: Zlib

# Checks that the newest release in CHANGELOG.md is the version in VERSION.

cmake_minimum_required(VERSION 3.21)

get_filename_component(root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
file(STRINGS "${root}/VERSION" version LIMIT_COUNT 1)
file(STRINGS "${root}/CHANGELOG.md" heading REGEX "^## " LIMIT_COUNT 1)

if(NOT heading MATCHES "^## \\[([0-9]+\\.[0-9]+\\.[0-9]+)\\] - ")
    message(FATAL_ERROR "CHANGELOG.md has no release heading of the form '## [X.Y.Z] - YYYY-MM-DD'")
endif()
if(NOT CMAKE_MATCH_1 STREQUAL version)
    message(FATAL_ERROR "The newest release in CHANGELOG.md is ${CMAKE_MATCH_1} and VERSION holds ${version}")
endif()
