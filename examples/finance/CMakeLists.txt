# Part of LibXtract
#
# SPDX-FileCopyrightText: 2026 Jamie Bullock
# SPDX-License-Identifier: Zlib

add_executable(volatility volatility.c)
target_link_libraries(volatility PRIVATE xtract::xtract)
set_target_properties(volatility PROPERTIES
    C_STANDARD 99
    C_STANDARD_REQUIRED ON
    C_EXTENSIONS OFF)
if(NOT MSVC)
    target_compile_options(volatility PRIVATE -Wall -pedantic ${XTRACT_STYLE_WARNINGS})
endif()

add_test(NAME finance_volatility COMMAND volatility)
