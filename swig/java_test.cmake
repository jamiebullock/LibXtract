# Part of LibXtract
#
# SPDX-FileCopyrightText: 2026 Jamie Bullock
# SPDX-License-Identifier: Zlib

# Compiles the generated Java package and test.java, then runs the test
# against the jxtract library in LIBRARY_DIR

file(GLOB package_sources ${JAVA_DIR}/xtract/*.java)
set(classes_dir ${JAVA_DIR}/classes)
file(MAKE_DIRECTORY ${classes_dir})

execute_process(
    COMMAND ${JAVAC} -d ${classes_dir} ${package_sources} ${SOURCE_DIR}/test.java
    RESULT_VARIABLE compile_result)
if(NOT compile_result EQUAL 0)
    message(FATAL_ERROR "javac failed")
endif()

execute_process(
    COMMAND ${JAVA} -Djava.library.path=${LIBRARY_DIR} -cp ${classes_dir} test
    RESULT_VARIABLE run_result)
if(NOT run_result EQUAL 0)
    message(FATAL_ERROR "the Java bindings test failed")
endif()
