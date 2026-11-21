# Part of LibXtract
#
# SPDX-FileCopyrightText: 2026 Jamie Bullock
# SPDX-License-Identifier: Zlib

# Writes the feature enumeration (include/xtract/xtract_features.h) and the
# dispatch and descriptor tables (src/features.c) from src/features.json,
# after checking every record. The written files are committed. With CHECK
# set they are written under WORK instead and compared with the committed
# ones, and a difference is an error.

cmake_minimum_required(VERSION 3.21)

get_filename_component(root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
set(data_file "${root}/src/features.json")
set(header include/xtract/xtract_features.h)
set(source src/features.c)

set(UNITS unknown none any any_squared any_cubed hertz hertz_squared per_hertz any_amplitude_hertz
          seconds per_second decibels percent bins midi_cent sone acum)
set(RELATIVE_UNITS unknown none any any_squared any_cubed)
set(FORMATS spectral spectral_magnitudes spectral_peaks spectral_peaks_magnitudes spectral_peaks_frequencies
            spectral_harmonics spectral_harmonics_magnitudes spectral_harmonics_frequencies autocorrelation_coeffs
            arbitrary_series audio_samples mel_coeffs lpc_coeffs lpcc_coeffs bark_coeffs subframes no_data
            spectral_magnitude_phase chroma_coeffs channels square_matrix grid central_moment_coeffs
            normalised_moment_coeffs)
set(ARGUMENT_TYPES float floatarray int filterbank cqt_kernel last_n_state wavelet_f0_state)
set(BUILDERS init_mfcc init_bark init_windowed init_gfcc init_cqt)
set(MAXARGS 5)
set(MAX_NAME_LENGTH 64)
set(MAX_AUTHOR_LENGTH 128)
set(MAX_DESC_LENGTH 256)

function(fail feature message)
    message(FATAL_ERROR "${data_file}: feature '${feature}': ${message}")
endfunction()

# Fails when the object has a member that is not one of the keys in ARGN
function(only_keys feature json where)
    string(JSON count LENGTH "${json}")
    if(count EQUAL 0)
        return()
    endif()
    math(EXPR last "${count} - 1")
    foreach(i RANGE ${last})
        string(JSON key MEMBER "${json}" ${i})
        if(NOT key IN_LIST ARGN)
            fail("${feature}" "${where} has the unknown key '${key}'")
        endif()
    endforeach()
endfunction()

# Sets ${out} to the member at the path in ARGN, which must have the JSON type
function(required out feature json type)
    string(REPLACE ";" "." path "${ARGN}")
    string(JSON actual ERROR_VARIABLE error TYPE "${json}" ${ARGN})
    if(error)
        fail("${feature}" "'${path}' is missing")
    endif()
    if(NOT actual STREQUAL type)
        fail("${feature}" "'${path}' is ${actual}, expected ${type}")
    endif()
    string(JSON value GET "${json}" ${ARGN})
    set(${out} "${value}" PARENT_SCOPE)
endfunction()

# Sets ${out} to the member at the key, which must have the JSON type, or to
# the default when the member is absent
function(optional out feature json type key default)
    string(JSON actual ERROR_VARIABLE error TYPE "${json}" "${key}")
    if(error)
        set(${out} "${default}" PARENT_SCOPE)
        return()
    endif()
    if(NOT actual STREQUAL type)
        fail("${feature}" "'${key}' is ${actual}, expected ${type}")
    endif()
    string(JSON value GET "${json}" "${key}")
    set(${out} "${value}" PARENT_SCOPE)
endfunction()

# Sets ${out} to the number at the key, which must be a whole number from 0
# to the largest int, or to the default when the member is absent
function(optional_count out feature json key default)
    optional(value "${feature}" "${json}" NUMBER "${key}" "${default}")
    if(NOT value MATCHES "^[0-9]+$" OR value GREATER 2147483647)
        fail("${feature}" "'${key}' is ${value}, expected a whole number from 0 to 2147483647")
    endif()
    set(${out} "${value}" PARENT_SCOPE)
endfunction()

# Sets ${out} to a C string literal of the text, which must fit an array of
# ${size} characters with its terminator
function(c_string out feature key size text)
    string(LENGTH "${text}" length)
    if(NOT length LESS size)
        fail("${feature}" "'${key}' has ${length} characters, and the most is ${size} less 1")
    endif()
    if(NOT text MATCHES "^[\t -~]*$")
        fail("${feature}" "'${key}' has a character that is not printable ASCII")
    endif()
    string(REPLACE "\\" "\\\\" text "${text}")
    string(REPLACE "\"" "\\\"" text "${text}")
    string(REPLACE "\t" "\\t" text "${text}")
    # Two question marks and a third character can form a trigraph, which a
    # C99 compiler replaces inside a string literal
    string(REPLACE "?" "\\?" text "${text}")
    set(${out} "\"${text}\"" PARENT_SCOPE)
endfunction()

function(unit_enumerator out feature key name)
    if(NOT name IN_LIST UNITS)
        fail("${feature}" "'${key}' is '${name}', which is not a unit")
    endif()
    string(TOUPPER "${name}" upper)
    if(name IN_LIST RELATIVE_UNITS)
        set(${out} "XTRACT_UNIT_${upper}" PARENT_SCOPE)
    else()
        set(${out} "XTRACT_${upper}" PARENT_SCOPE)
    endif()
endfunction()

function(format_enumerator out feature key name)
    if(NOT name IN_LIST FORMATS)
        fail("${feature}" "'${key}' is '${name}', which is not a format")
    endif()
    string(TOUPPER "XTRACT_${name}" upper)
    set(${out} "${upper}" PARENT_SCOPE)
endfunction()

# Appends to the list the number the argument has at the key, or ${absent}
# when it has no such member
macro(append_number list feature argument key absent)
    optional(value "${feature}" "${argument}" NUMBER "${key}" "${absent}")
    list(APPEND ${list} "${value}")
endmacro()

file(READ "${data_file}" doc)
only_keys("(none)" "${doc}" "the file" features)
string(JSON count LENGTH "${doc}" features)
if(count EQUAL 0)
    message(FATAL_ERROR "${data_file}: no features")
endif()
math(EXPR last "${count} - 1")

set(names "")
foreach(i RANGE ${last})
    string(JSON feature GET "${doc}" features ${i})
    required(name "number ${i}" "${feature}" STRING name)
    if(NOT name MATCHES "^[a-z][a-z0-9_]*$")
        fail("${name}" "the name is not lower case letters, digits and underscores")
    endif()
    if(name IN_LIST names)
        fail("${name}" "the name is used twice")
    endif()
    list(APPEND names "${name}")
endforeach()

set(enumeration "")
set(dispatch "")
set(table "")
foreach(i RANGE ${last})
    string(JSON feature GET "${doc}" features ${i})
    string(JSON name GET "${feature}" name)
    only_keys("${name}" "${feature}" "the feature"
        name title description pretty_description reference input argument_type argument_count arguments delta result)
    string(TOUPPER "XTRACT_${name}" id)
    string(APPEND enumeration "        ${id},\n")
    string(APPEND dispatch "        xtract_${name},\n")

    required(title "${name}" "${feature}" STRING title)
    required(description "${name}" "${feature}" STRING description)
    required(pretty_description "${name}" "${feature}" STRING pretty_description)
    c_string(name_c "${name}" name ${MAX_NAME_LENGTH} "${name}")
    c_string(title_c "${name}" title ${MAX_NAME_LENGTH} "${title}")
    c_string(description_c "${name}" description ${MAX_DESC_LENGTH} "${description}")
    c_string(pretty_description_c "${name}" pretty_description ${MAX_DESC_LENGTH} "${pretty_description}")

    optional(reference "${name}" "${feature}" OBJECT reference "{}")
    only_keys("${name}" "${reference}" "'reference'" author year)
    optional(author "${name}" "${reference}" STRING author "")
    optional_count(year "${name}" "${reference}" year 0)
    c_string(author_c "${name}" reference.author ${MAX_AUTHOR_LENGTH} "${author}")

    required(input "${name}" "${feature}" OBJECT input)
    only_keys("${name}" "${input}" "'input'" format unit)
    required(input_format "${name}" "${input}" STRING format)
    required(input_unit "${name}" "${input}" STRING unit)
    format_enumerator(input_format "${name}" input.format "${input_format}")
    unit_enumerator(input_unit "${name}" input.unit "${input_unit}")

    required(arguments "${name}" "${feature}" ARRAY arguments)
    string(JSON listed LENGTH "${arguments}")
    if(listed GREATER MAXARGS)
        fail("${name}" "${listed} arguments, and the most is ${MAXARGS}")
    endif()
    optional_count(argc "${name}" "${feature}" argument_count ${listed})
    if(argc LESS listed)
        fail("${name}" "'argument_count' is ${argc}, fewer than the ${listed} arguments listed")
    endif()
    if(argc EQUAL 0)
        # An argument that does not exist has the type of the first enumerator,
        # no bounds, no default and no donor
        set(argument_type float)
        set(minima "XTRACT_UNBOUNDED_MIN")
        set(maxima "XTRACT_UNBOUNDED_MAX")
        set(defaults "XTRACT_NO_DEFAULT")
        set(units "XTRACT_UNIT_NONE")
        set(donors "XTRACT_ANY")
    else()
        required(argument_type "${name}" "${feature}" STRING argument_type)
        if(NOT argument_type IN_LIST ARGUMENT_TYPES)
            fail("${name}" "'argument_type' is '${argument_type}', which is not a type")
        endif()
        if(listed EQUAL 0)
            fail("${name}" "'argument_count' is ${argc} and no argument is listed")
        endif()
        set(minima "")
        set(maxima "")
        set(defaults "")
        set(units "")
        set(donors "")
        math(EXPR last_argument "${listed} - 1")
        foreach(a RANGE ${last_argument})
            string(JSON argument GET "${arguments}" ${a})
            only_keys("${name}" "${argument}" "argument ${a}" unit min max default donor)
            append_number(minima "${name}" "${argument}" min XTRACT_UNBOUNDED_MIN)
            append_number(maxima "${name}" "${argument}" max XTRACT_UNBOUNDED_MAX)
            append_number(defaults "${name}" "${argument}" default XTRACT_NO_DEFAULT)
            required(argument_unit "${name}" "${argument}" STRING unit)
            unit_enumerator(argument_unit "${name}" "argument ${a} unit" "${argument_unit}")
            list(APPEND units "${argument_unit}")
            optional(donor "${name}" "${argument}" STRING donor "")
            if(donor STREQUAL "")
                list(APPEND donors XTRACT_ANY)
            elseif(donor IN_LIST names OR donor IN_LIST BUILDERS)
                string(TOUPPER "XTRACT_${donor}" donor)
                list(APPEND donors "${donor}")
            else()
                fail("${name}" "the donor of argument ${a} is '${donor}', which is not a feature")
            endif()
        endforeach()
    endif()
    string(TOUPPER "XTRACT_${argument_type}" argument_type)
    foreach(column minima maxima defaults units donors)
        list(JOIN ${column} ", " ${column})
    endforeach()

    optional(delta "${name}" "${feature}" BOOLEAN delta OFF)
    if(delta)
        set(is_delta XTRACT_TRUE)
    else()
        set(is_delta XTRACT_FALSE)
    endif()

    required(result "${name}" "${feature}" OBJECT result)
    required(kind "${name}" "${result}" STRING kind)
    required(result_unit "${name}" "${result}" STRING unit)
    unit_enumerator(result_unit "${name}" result.unit "${result_unit}")
    if(kind STREQUAL "scalar")
        only_keys("${name}" "${result}" "'result'" kind unit min max)
        optional(result_minimum "${name}" "${result}" NUMBER min XTRACT_UNBOUNDED_MIN)
        optional(result_maximum "${name}" "${result}" NUMBER max XTRACT_UNBOUNDED_MAX)
        set(is_scalar XTRACT_TRUE)
        set(result_c ".scalar = {${result_minimum}, ${result_maximum}, ${result_unit}}")
    elseif(kind STREQUAL "vector")
        only_keys("${name}" "${result}" "'result'" kind unit format)
        required(result_format "${name}" "${result}" STRING format)
        format_enumerator(result_format "${name}" result.format "${result_format}")
        set(is_scalar XTRACT_FALSE)
        set(result_c ".vector = {${result_format}, ${result_unit}}")
    else()
        fail("${name}" "'result.kind' is '${kind}', which is not scalar or vector")
    endif()

    string(APPEND table
        "    [${id}] = {\n"
        "        .id = ${id},\n"
        "        .algo = {${name_c}, ${title_c}, ${description_c}, ${pretty_description_c}, ${author_c}, ${year}},\n"
        "        .data = {${input_format}, ${input_unit}},\n"
        "        .argc = ${argc},\n"
        "        .argv = {${argument_type}, {${minima}}, {${maxima}}, {${defaults}}, {${units}}, {${donors}}},\n"
        "        .is_scalar = ${is_scalar},\n"
        "        .is_delta = ${is_delta},\n"
        "        .result = {${result_c}},\n"
        "    },\n")
endforeach()

set(licence "/* Part of LibXtract\n *\n * SPDX-FileCopyrightText: 2026 Jamie Bullock\n * SPDX-License-Identifier: Zlib\n */\n")
set(notice "written from src/features.json by the build target \"features\"")

string(CONCAT header_text
    "${licence}\n"
    "/** \\file xtract_features.h: the enumeration of the features, ${notice} */\n\n"
    "#ifndef XTRACT_FEATURES_H\n#define XTRACT_FEATURES_H\n\n"
    "#ifdef __cplusplus\nextern \"C\"\n{\n#endif\n\n"
    "    /** \\brief Enumeration of features, elements are used as indices to an array of pointers to feature extraction functions */\n"
    "    enum xtract_features_\n    {\n"
    "${enumeration}"
    "        XTRACT_FEATURE_COUNT\n    };\n\n"
    "#ifdef __cplusplus\n}\n#endif\n\n#endif\n")
string(CONCAT source_text
    "${licence}\n"
    "/* The dispatch table and the descriptor table, ${notice} */\n\n"
    "#include \"xtract/libxtract.h\"\n"
    "#include \"xtract_descriptors_private.h\"\n\n"
    "int (*xtract[])(xtract_context *, const double *, const int, const void *, double *) =\n    {\n"
    "${dispatch}"
    "};\n\n"
    "const xtract_function_descriptor_t xtract_descriptor_table[XTRACT_FEATURES] = {\n"
    "${table}"
    "};\n")

if(CHECK)
    if(NOT WORK)
        message(FATAL_ERROR "CHECK needs WORK, a directory to write to")
    endif()
    set(destination "${WORK}")
else()
    set(destination "${root}")
endif()
file(WRITE "${destination}/${header}" "${header_text}")
file(WRITE "${destination}/${source}" "${source_text}")

if(CHECK)
    foreach(file ${header} ${source})
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E compare_files --ignore-eol "${root}/${file}" "${WORK}/${file}"
            RESULT_VARIABLE result
            OUTPUT_QUIET ERROR_QUIET)
        if(NOT result EQUAL 0)
            message(FATAL_ERROR "${file} is not what src/features.json gives: build the target \"features\" and commit the result")
        endif()
    endforeach()
endif()
