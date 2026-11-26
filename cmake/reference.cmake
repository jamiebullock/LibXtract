# Part of LibXtract
#
# SPDX-FileCopyrightText: 2026 Jamie Bullock
# SPDX-License-Identifier: Zlib

# Writes the table of reference values that the tests check
# (tests/xttest_reference_values.c) from the files of tests/reference/values,
# one for each source of values, and from the series in
# tests/reference/series that their records name, after checking every
# record against src/features.json. The written file is committed. With
# CHECK set it is written under WORK instead and compared with the committed
# one, and a difference is an error.
#
# With EXTRA set, a directory that has values and series of its own, the
# table has those sources as well and is written to OUTPUT. The committed
# table never has them.

cmake_minimum_required(VERSION 3.21)

get_filename_component(root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
set(table tests/xttest_reference_values.c)
set(directories "${root}/tests/reference")
if(EXTRA)
    if(CHECK OR NOT OUTPUT)
        message(FATAL_ERROR "EXTRA needs OUTPUT, a file to write to, and cannot be given with CHECK")
    endif()
    list(APPEND directories "${EXTRA}")
endif()
set(NUMBER "^-?[0-9]+(\\.[0-9]+)?(e[-+]?[0-9]+)?$")
set(VALUES_PER_LINE 8)

function(fail where message)
    message(FATAL_ERROR "${data_file}: ${where}: ${message}")
endfunction()

include("${CMAKE_CURRENT_LIST_DIR}/json.cmake")

# Sets ${out} to the number at the index of the JSON array, as C reads it
function(number_at out where key json index)
    string(JSON type TYPE "${json}" ${index})
    if(NOT type STREQUAL "NUMBER")
        fail("${where}" "'${key}' has a member that is ${type}, expected NUMBER")
    endif()
    string(JSON value GET "${json}" ${index})
    set(${out} "${value}" PARENT_SCOPE)
endfunction()

# Sets series_text to the C array of the series in the directory of series,
# and series_length_${name} to the number of its values, the first time a
# record names it
function(use_series where directory name)
    if(DEFINED series_length_${name})
        return()
    endif()
    check_name("${where}" data "${name}")
    set(file "${directory}/${name}.txt")
    if(NOT EXISTS "${file}")
        fail("${where}" "'data' is '${name}', and ${directory} has no ${name}.txt")
    endif()
    file(STRINGS "${file}" values)
    list(LENGTH values count)
    if(count EQUAL 0)
        fail("${where}" "the series '${name}' has no values")
    endif()
    set(rows "")
    set(row "")
    set(in_row 0)
    foreach(value ${values})
        if(NOT value MATCHES "${NUMBER}")
            fail("${where}" "the series '${name}' has the line '${value}', which is not a number")
        endif()
        if(in_row EQUAL VALUES_PER_LINE)
            string(APPEND rows "    ${row},\n")
            set(row "")
            set(in_row 0)
        endif()
        if(in_row GREATER 0)
            string(APPEND row ", ")
        endif()
        string(APPEND row "${value}")
        math(EXPR in_row "${in_row} + 1")
    endforeach()
    string(APPEND rows "    ${row}")
    string(APPEND series_text "static const double series_${name}[${count}] = {\n${rows}};\n\n")
    list(APPEND used_series "${name}")
    set(series_text "${series_text}" PARENT_SCOPE)
    set(used_series "${used_series}" PARENT_SCOPE)
    set(series_length_${name} "${count}" PARENT_SCOPE)
endfunction()

set(data_file "${root}/src/features.json")
file(READ "${data_file}" features_doc)
string(JSON feature_count LENGTH "${features_doc}" features)
math(EXPR last_feature "${feature_count} - 1")
foreach(i RANGE ${last_feature})
    string(JSON name GET "${features_doc}" features ${i} name)
    set(feature_index_${name} ${i})
endforeach()

set(value_files "")
foreach(directory ${directories})
    file(GLOB found "${directory}/values/*.json")
    if(NOT found)
        message(FATAL_ERROR "${directory}/values has no .json file")
    endif()
    list(SORT found)
    list(APPEND value_files ${found})
endforeach()

set(series_text "")
set(used_series "")
set(source_text "")
set(source_pointers "")
set(test_text "")
set(ids "")
foreach(data_file ${value_files})
    get_filename_component(id "${data_file}" NAME_WE)
    check_name("the file name" name "${id}")
    if(id IN_LIST ids)
        fail("the file name" "another file of values is named '${id}'")
    endif()
    list(APPEND ids "${id}")
    get_filename_component(series_directory "${data_file}/../../series" ABSOLUTE)
    file(READ "${data_file}" doc)
    only_keys("the file" "${doc}" source records)

    required(source "the file" "${doc}" OBJECT source)
    only_keys("'source'" "${source}" name version url licence how level)
    foreach(key name version licence how)
        required(source_${key} "'source'" "${source}" STRING ${key})
        c_string(source_${key}_c "'source'" ${key} "${source_${key}}")
    endforeach()
    optional(source_url "'source'" "${source}" STRING url "")
    required(level "'source'" "${source}" NUMBER level)
    if(NOT level MATCHES "^[1-3]$")
        fail("'source'" "'level' is ${level}, expected 1, 2 or 3")
    endif()

    required(records "the file" "${doc}" ARRAY records)
    string(JSON record_count LENGTH "${records}")
    if(record_count EQUAL 0)
        fail("the file" "no records")
    endif()
    set(array_text "")
    set(record_rows "")
    math(EXPR last_record "${record_count} - 1")
    foreach(r RANGE ${last_record})
        string(JSON record GET "${records}" ${r})
        set(where "record ${r}")
        only_keys("${where}" "${record}" feature data arguments expected tolerance call)
        required(name "${where}" "${record}" STRING feature)
        if(NOT DEFINED feature_index_${name})
            fail("${where}" "'feature' is '${name}', which is not a feature")
        endif()
        set(where "${where}, of '${name}'")
        string(TOUPPER "XTRACT_${name}" feature_c)
        string(JSON feature GET "${features_doc}" features ${feature_index_${name}})

        string(JSON input_format GET "${feature}" input format)
        optional(series "${where}" "${record}" STRING data "")
        if(input_format STREQUAL "no_data")
            if(NOT series STREQUAL "")
                fail("${where}" "it has 'data', and the feature takes none")
            endif()
            set(series_c "NULL")
            set(data_c "NULL")
            set(n 0)
        else()
            if(series STREQUAL "")
                fail("${where}" "'data' is missing")
            endif()
            use_series("${where}" "${series_directory}" "${series}")
            set(series_c "\"${series}\"")
            set(data_c "series_${series}")
            set(n ${series_length_${series}})
        endif()

        # argv points to the values of the arguments in the order of the
        # feature's record
        optional(given "${where}" "${record}" OBJECT arguments "{}")
        string(JSON arguments GET "${feature}" arguments)
        string(JSON argument_count LENGTH "${arguments}")
        string(JSON given_count LENGTH "${given}")
        set(argument_values "")
        if(argument_count GREATER 0)
            math(EXPR last_argument "${argument_count} - 1")
            foreach(a RANGE ${last_argument})
                string(JSON argument_name GET "${arguments}" ${a} name)
                string(JSON argument_kind GET "${arguments}" ${a} kind)
                if(argument_kind STREQUAL "object")
                    fail("${where}" "the argument '${argument_name}' is an object, which a record cannot give")
                endif()
                required(value "${where}, 'arguments'" "${given}" NUMBER "${argument_name}")
                list(APPEND argument_values "${value}")
            endforeach()
        endif()
        if(NOT given_count EQUAL argument_count)
            fail("${where}" "'arguments' has ${given_count} members, and the feature has ${argument_count} arguments")
        endif()
        if(argument_count GREATER 0)
            list(JOIN argument_values ", " argument_values)
            string(APPEND array_text "static const double arguments_${id}_${r}[] = {${argument_values}};\n")
            set(arguments_c "arguments_${id}_${r}")
        else()
            set(arguments_c "NULL")
        endif()

        required(expected "${where}" "${record}" ARRAY expected)
        string(JSON expected_count LENGTH "${expected}")
        if(expected_count EQUAL 0)
            fail("${where}" "'expected' has no values")
        endif()
        set(expected_values "")
        math(EXPR last_expected "${expected_count} - 1")
        foreach(e RANGE ${last_expected})
            number_at(value "${where}" expected "${expected}" ${e})
            list(APPEND expected_values "${value}")
        endforeach()
        list(JOIN expected_values ", " expected_values)
        string(APPEND array_text "static const double expected_${id}_${r}[] = {${expected_values}};\n")

        required(tolerance "${where}" "${record}" OBJECT tolerance)
        only_keys("${where}, 'tolerance'" "${tolerance}" relative reason)
        required(relative "${where}, 'tolerance'" "${tolerance}" NUMBER relative)
        if(relative LESS 0)
            fail("${where}, 'tolerance'" "'relative' is ${relative}, which is below 0")
        endif()
        required(reason "${where}, 'tolerance'" "${tolerance}" STRING reason)
        if(reason STREQUAL "")
            fail("${where}, 'tolerance'" "'reason' is empty")
        endif()
        required(call "${where}" "${record}" STRING call)
        if(call STREQUAL "")
            fail("${where}" "'call' is empty")
        endif()

        string(APPEND record_rows
            "    {.feature = ${feature_c}, .series = ${series_c}, .data = ${data_c}, .n = ${n}, .arguments = ${arguments_c}, "
            ".expected = expected_${id}_${r}, .expected_count = ${expected_count}, .tolerance = ${relative}},\n")
    endforeach()

    string(APPEND source_text
        "${array_text}\n"
        "static const xttest_reference_record records_${id}[] = {\n${record_rows}};\n\n"
        "static const xttest_reference_source source_${id} = {.id = \"${id}\", .name = ${source_name_c}, .level = ${level}, "
        ".record_count = ${record_count}, .records = records_${id}};\n\n")
    list(APPEND source_pointers "&source_${id}")
    string(APPEND test_text
        "\nUTEST(reference, results_agree_with_${id})\n{\n"
        "    xttest_reference_check(&source_${id}, utest_result);\n}\n")
endforeach()
list(LENGTH ids source_count)

set(series_names "")
foreach(directory ${directories})
    file(GLOB series_files "${directory}/series/*")
    foreach(file ${series_files})
        get_filename_component(name "${file}" NAME)
        string(REGEX REPLACE "\\.txt$" "" stem "${name}")
        if(NOT stem IN_LIST used_series)
            message(FATAL_ERROR "${file}: no record names it")
        endif()
        if(stem IN_LIST series_names)
            message(FATAL_ERROR "${file}: another series is named '${stem}'")
        endif()
        list(APPEND series_names "${stem}")
    endforeach()
endforeach()

list(JOIN source_pointers ", " source_pointers)
string(CONCAT table_text
    "/* Part of LibXtract\n *\n * SPDX-FileCopyrightText: 2026 Jamie Bullock\n * SPDX-License-Identifier: Zlib\n */\n\n"
    "/* The reference values that the tests check and a test for each of their sources, "
    "written from tests/reference by the build target \"reference\" */\n\n"
    "#include <stddef.h>\n\n"
    "#include \"utest.h\"\n\n"
    "#include \"xtract/libxtract.h\"\n\n"
    "#include \"xttest_reference.h\"\n\n"
    "${series_text}"
    "${source_text}"
    "const xttest_reference_source *const xttest_reference_sources[] = {${source_pointers}};\n\n"
    "const int xttest_reference_source_count = ${source_count};\n"
    "${test_text}")

if(CHECK)
    if(NOT WORK)
        message(FATAL_ERROR "CHECK needs WORK, a directory to write to")
    endif()
    file(WRITE "${WORK}/${table}" "${table_text}")
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E compare_files --ignore-eol "${root}/${table}" "${WORK}/${table}"
        RESULT_VARIABLE result
        OUTPUT_QUIET ERROR_QUIET)
    if(NOT result EQUAL 0)
        message(FATAL_ERROR "${table} is not what tests/reference gives: build the target \"reference\" and commit the result")
    endif()
elseif(EXTRA)
    file(WRITE "${OUTPUT}" "${table_text}")
else()
    file(WRITE "${root}/${table}" "${table_text}")
endif()
