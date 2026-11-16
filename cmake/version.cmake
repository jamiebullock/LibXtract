# Part of LibXtract
#
# SPDX-FileCopyrightText: 2026 Jamie Bullock
# SPDX-License-Identifier: Zlib

# Checks VERSION against the newest release in CHANGELOG.md: it must be that
# release or a later version, the one in development. With -DRELEASE=ON it
# must be the newest release, which the release tag workflow requires of a
# commit pushed to main.

cmake_minimum_required(VERSION 3.21)

get_filename_component(root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
file(STRINGS "${root}/VERSION" version LIMIT_COUNT 1)
file(STRINGS "${root}/CHANGELOG.md" heading REGEX "^## " LIMIT_COUNT 1)

if(NOT version MATCHES "^[0-9]+\\.[0-9]+\\.[0-9]+$")
    message(FATAL_ERROR "VERSION holds '${version}', which is not of the form X.Y.Z")
endif()
if(NOT heading MATCHES "^## \\[([0-9]+\\.[0-9]+\\.[0-9]+)\\] - ")
    message(FATAL_ERROR "CHANGELOG.md has no release heading of the form '## [X.Y.Z] - YYYY-MM-DD'")
endif()
set(newest "${CMAKE_MATCH_1}")

if(version VERSION_LESS newest)
    message(FATAL_ERROR "VERSION holds ${version}, which is behind ${newest}, the newest release in CHANGELOG.md")
endif()
if(RELEASE AND NOT version STREQUAL newest)
    message(FATAL_ERROR "VERSION holds ${version} and the newest release in CHANGELOG.md is ${newest}: a release needs its entry in the changelog")
endif()
