# Part of LibXtract
#
# SPDX-FileCopyrightText: 2026 Jamie Bullock
# SPDX-License-Identifier: Zlib

add_executable(xttest
    xttest_main.c
    xttest_delta.c
    xttest_descriptors.c
    xttest_init.c
    xttest_scalar.c
    xttest_stateful.c
    xttest_util.c
    xttest_vector.c
    xttest_window.c)
target_link_libraries(xttest PRIVATE xtract::xtract)
set_target_properties(xttest PROPERTIES
    C_STANDARD 99
    C_STANDARD_REQUIRED ON
    C_EXTENSIONS OFF)

add_test(NAME xttest COMMAND xttest)

if(XTRACT_COVERAGE)
    set(XTRACT_LCOV_IGNORE --ignore-errors unused,inconsistent,format,empty,gcov,unsupported)
    add_custom_target(coverage
        COMMAND lcov --zerocounters --directory ${PROJECT_BINARY_DIR}/src ${XTRACT_LCOV_IGNORE}
        COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
        COMMAND lcov --capture --directory ${PROJECT_BINARY_DIR}/src
            --output-file coverage.info ${XTRACT_LCOV_IGNORE}
        COMMAND lcov --remove coverage.info */ooura/* */c-ringbuf/* */dywapitchtrack/*
            --output-file coverage.info ${XTRACT_LCOV_IGNORE}
        COMMAND lcov --list coverage.info ${XTRACT_LCOV_IGNORE}
        COMMAND genhtml --quiet coverage.info --output-directory coverage-html ${XTRACT_LCOV_IGNORE}
        COMMAND ${CMAKE_COMMAND} -E echo "Coverage report written to ${PROJECT_BINARY_DIR}/coverage-html/index.html"
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
        DEPENDS xttest
        USES_TERMINAL
        VERBATIM)
endif()

if(XTRACT_MUTATION_PLUGIN)
    set(XTRACT_MULL_RUNNER mull-runner-18 CACHE STRING "Mull runner executable")
    set(XTRACT_MUTATION_THRESHOLD 84 CACHE STRING "Minimum mutation score out of 100")
    set(XTRACT_MUTATION_WORKERS 4 CACHE STRING "Mutants the runner tests in parallel")
    # The runner starts with PATH as its whole environment: the instrumented
    # library reads the environment on every call of a mutated function, so
    # the tests slow down with the number of variables in it
    add_custom_target(mutation
        COMMAND env -i PATH=$ENV{PATH} ${XTRACT_MULL_RUNNER}
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
