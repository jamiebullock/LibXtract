# Part of LibXtract
#
# SPDX-FileCopyrightText: 2026 Jamie Bullock
# SPDX-License-Identifier: Zlib

# The NIST datasets are read from NIST and kept in the build directory: the
# repository has no copy of them. The tests are then built with a table of
# reference values that has the NIST values as well as the committed ones
set(reference_values xttest_reference_values.c)
if(XTRACT_NIST_TESTS)
    find_package(Python3 REQUIRED COMPONENTS Interpreter)
    set(nist ${CMAKE_CURRENT_BINARY_DIR}/nist)
    foreach(dataset Lew Lottery Mavro Michelso NumAcc1 NumAcc2 NumAcc3 NumAcc4 PiDigits)
        set(file ${nist}/published/${dataset}.dat)
        set(attempt 0)
        while(NOT EXISTS ${file})
            file(DOWNLOAD https://www.itl.nist.gov/div898/strd/univ/data/${dataset}.dat ${file}.part
                STATUS status TIMEOUT 60)
            list(GET status 0 code)
            if(code EQUAL 0)
                file(RENAME ${file}.part ${file})
            else()
                file(REMOVE ${file}.part)
                math(EXPR attempt "${attempt} + 1")
                if(attempt EQUAL 5)
                    message(FATAL_ERROR "XTRACT_NIST_TESTS: ${dataset}.dat could not be downloaded from NIST: ${status}")
                endif()
                execute_process(COMMAND ${CMAKE_COMMAND} -E sleep 10)
            endif()
        endwhile()
    endforeach()
    set(reference_values ${nist}/xttest_reference_values.c)
    execute_process(
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/reference/generate/nist.py
            ${nist}/published ${nist}/reference
        COMMAND_ERROR_IS_FATAL ANY)
    execute_process(
        COMMAND ${CMAKE_COMMAND} -DEXTRA=${nist}/reference -DOUTPUT=${reference_values}
            -P ${PROJECT_SOURCE_DIR}/cmake/reference.cmake
        COMMAND_ERROR_IS_FATAL ANY)
    file(GLOB committed reference/values/*.json reference/series/*.json)
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
        ${committed}
        reference/generate/nist.py
        ${PROJECT_SOURCE_DIR}/cmake/reference.cmake
        ${PROJECT_SOURCE_DIR}/cmake/json.cmake
        ${PROJECT_SOURCE_DIR}/src/features.json)
endif()

add_executable(xttest
    xttest_main.c
    xttest_context.c
    xttest_delta.c
    xttest_descriptors.c
    xttest_driver.c
    xttest_grid.c
    xttest_init.c
    xttest_invariance.c
    xttest_multivariate.c
    xttest_properties.c
    xttest_reference.c
    ${reference_values}
    xttest_scalar.c
    xttest_stateful.c
    xttest_util.c
    xttest_vector.c
    xttest_window.c)
target_include_directories(xttest PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(xttest PRIVATE xtract::xtract)
set_target_properties(xttest PROPERTIES
    C_STANDARD 99
    C_STANDARD_REQUIRED ON
    C_EXTENSIONS OFF)

# xttest_context.c tests the scratch and the transform through the library's
# private header, which selects the transform library as the library build does
set_source_files_properties(xttest_context.c PROPERTIES INCLUDE_DIRECTORIES ${PROJECT_SOURCE_DIR}/src)
if(NOT APPLE)
    set_source_files_properties(xttest_context.c PROPERTIES COMPILE_DEFINITIONS USE_OOURA)
endif()

add_test(NAME xttest COMMAND xttest)
if(XTRACT_REALTIME_SANITIZER)
    add_executable(realtime_violation realtime_violation.c)
    set_target_properties(realtime_violation PROPERTIES
        C_STANDARD 99
        C_STANDARD_REQUIRED ON
        C_EXTENSIONS OFF)
    target_link_libraries(realtime_violation PRIVATE xtract::xtract)
    add_test(NAME realtime_sanitizer_reports_an_allocation COMMAND realtime_violation)
    # The program runs on after the report so that it ends normally, since
    # CTest fails a test that aborts whatever it printed
    set_tests_properties(realtime_sanitizer_reports_an_allocation PROPERTIES
        ENVIRONMENT "RTSAN_OPTIONS=halt_on_error=false"
        PASS_REGULAR_EXPRESSION "RealtimeSanitizer: unsafe-library-call")
endif()
add_test(NAME version_check
    COMMAND ${CMAKE_COMMAND}
        -DSCRIPT=${PROJECT_SOURCE_DIR}/cmake/version.cmake
        -DWORK=${CMAKE_CURRENT_BINARY_DIR}/version_check
        -P ${CMAKE_CURRENT_SOURCE_DIR}/version_check.cmake)
add_test(NAME driver_names_no_feature
    COMMAND ${CMAKE_COMMAND}
        -DFEATURES=${PROJECT_SOURCE_DIR}/include/xtract/xtract_features.h
        -DDRIVER=${CMAKE_CURRENT_SOURCE_DIR}/xttest_driver.c
        -P ${CMAKE_CURRENT_SOURCE_DIR}/driver_names_no_feature.cmake)
add_test(NAME fuzz_calls_every_feature
    COMMAND ${CMAKE_COMMAND}
        -DFEATURES=${PROJECT_SOURCE_DIR}/include/xtract/xtract_features.h
        -DFUZZ=${PROJECT_SOURCE_DIR}/fuzz
        -P ${CMAKE_CURRENT_SOURCE_DIR}/fuzz_calls_every_feature.cmake)
add_test(NAME features_are_generated
    COMMAND ${CMAKE_COMMAND}
        -DCHECK=ON
        -DWORK=${CMAKE_CURRENT_BINARY_DIR}/features_check
        -P ${PROJECT_SOURCE_DIR}/cmake/features.cmake)
add_test(NAME reference_values_are_generated
    COMMAND ${CMAKE_COMMAND}
        -DCHECK=ON
        -DWORK=${CMAKE_CURRENT_BINARY_DIR}/reference_check
        -P ${PROJECT_SOURCE_DIR}/cmake/reference.cmake)

if(XTRACT_COVERAGE)
    set(XTRACT_LCOV_IGNORE --ignore-errors unused,inconsistent,format,empty,gcov,unsupported)
    add_custom_target(coverage
        COMMAND lcov --zerocounters --directory src ${XTRACT_LCOV_IGNORE}
        COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
        COMMAND lcov --capture --directory src
            --output-file coverage.info ${XTRACT_LCOV_IGNORE}
        COMMAND lcov --remove coverage.info */ooura/* */c-ringbuf/*
            --output-file coverage.info ${XTRACT_LCOV_IGNORE}
        COMMAND lcov --list coverage.info ${XTRACT_LCOV_IGNORE}
        COMMAND genhtml --quiet coverage.info --output-directory coverage-html ${XTRACT_LCOV_IGNORE}
        COMMAND ${CMAKE_COMMAND} -E echo "Coverage report written to ${PROJECT_BINARY_DIR}/coverage-html/index.html"
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
        DEPENDS xttest
        USES_TERMINAL
        VERBATIM)
endif()

# The mutation preset inherits the sanitizer flags of the asan preset, so a
# mutant that reads past an array fails the tests.
if(XTRACT_MUTATION_PLUGIN)
    set(XTRACT_MULL_RUNNER mull-runner-18 CACHE STRING "Mull runner executable")
    set(XTRACT_MUTATION_THRESHOLD 84 CACHE STRING "Minimum mutation score out of 100")
    set(XTRACT_MUTATION_WORKERS 4 CACHE STRING "Mutants the runner tests in parallel")
    # The runner starts with PATH as its whole environment: the instrumented
    # library reads the environment on every call of a mutated function, so
    # the tests slow down with the number of variables in it
    add_custom_target(mutation
        COMMAND env -i PATH=$ENV{PATH} MULL_CONFIG=${XTRACT_MULL_CONFIG} ${XTRACT_MULL_RUNNER}
            --reporters IDE --reporters Elements
            --report-dir ${PROJECT_BINARY_DIR}/reports --report-name mutation
            --workers ${XTRACT_MUTATION_WORKERS} --timeout 10000
            --mutation-score-threshold ${XTRACT_MUTATION_THRESHOLD}
            $<TARGET_FILE:xttest>
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        DEPENDS xttest
        USES_TERMINAL
        VERBATIM)
endif()
