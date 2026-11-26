# Part of LibXtract
#
# SPDX-FileCopyrightText: 2026 Jamie Bullock
# SPDX-License-Identifier: Zlib

# Functions that read and check the members of a JSON object, for the
# scripts that write C from a data file. Each reports an error by calling
# fail(where message), which the script that includes this file defines.

# Fails when the object has a member that is not one of the keys in ARGN
function(only_keys where json)
    string(JSON count LENGTH "${json}")
    if(count EQUAL 0)
        return()
    endif()
    math(EXPR last "${count} - 1")
    foreach(i RANGE ${last})
        string(JSON key MEMBER "${json}" ${i})
        if(NOT key IN_LIST ARGN)
            fail("${where}" "the unknown key '${key}'")
        endif()
    endforeach()
endfunction()

# Sets ${out} to the member at the key, which must have the JSON type
function(required out where json type key)
    string(JSON actual ERROR_VARIABLE error TYPE "${json}" "${key}")
    if(error)
        fail("${where}" "'${key}' is missing")
    endif()
    if(NOT actual STREQUAL type)
        fail("${where}" "'${key}' is ${actual}, expected ${type}")
    endif()
    string(JSON value GET "${json}" "${key}")
    set(${out} "${value}" PARENT_SCOPE)
endfunction()

# Sets ${out} to the member at the key, which must have the JSON type, or to
# the default when the member is absent
function(optional out where json type key default)
    string(JSON actual ERROR_VARIABLE error TYPE "${json}" "${key}")
    if(error)
        set(${out} "${default}" PARENT_SCOPE)
        return()
    endif()
    if(NOT actual STREQUAL type)
        fail("${where}" "'${key}' is ${actual}, expected ${type}")
    endif()
    string(JSON value GET "${json}" "${key}")
    set(${out} "${value}" PARENT_SCOPE)
endfunction()

# Fails when the text is not a name: lower case letters, digits and
# underscores, the first a letter
function(check_name where key text)
    if(NOT text MATCHES "^[a-z][a-z0-9_]*$")
        fail("${where}" "'${key}' is '${text}', which is not lower case letters, digits and underscores")
    endif()
endfunction()

# Sets ${out} to a C string literal of the text
function(c_string out where key text)
    if(text STREQUAL "")
        fail("${where}" "'${key}' is empty")
    endif()
    if(NOT text MATCHES "^[ -~]*$")
        fail("${where}" "'${key}' has a character that is not printable ASCII")
    endif()
    string(REPLACE "\\" "\\\\" text "${text}")
    string(REPLACE "\"" "\\\"" text "${text}")
    # Two question marks and a third character can form a trigraph, which a
    # C99 compiler replaces inside a string literal
    string(REPLACE "?" "\\?" text "${text}")
    set(${out} "\"${text}\"" PARENT_SCOPE)
endfunction()
