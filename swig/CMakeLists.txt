# Part of LibXtract
#
# SPDX-FileCopyrightText: 2026 Jamie Bullock
# SPDX-License-Identifier: Zlib

# Each of XTRACT_BUILD_PYTHON, XTRACT_BUILD_JAVA and XTRACT_BUILD_NODE is
# ON, OFF or AUTO; AUTO builds the binding when its toolchain is found.
# Each module and its support files land in one flat directory, which the
# generator expression in the output directory keeps free of per-config
# subdirectories, and its test runs from there.

find_package(SWIG 4.2)
if(NOT SWIG_FOUND)
    if(XTRACT_BUILD_PYTHON STREQUAL "ON" OR XTRACT_BUILD_JAVA STREQUAL "ON" OR XTRACT_BUILD_NODE STREQUAL "ON")
        message(FATAL_ERROR "A binding is ON but SWIG 4.2 or later was not found")
    endif()
    message(STATUS "SWIG not found: no bindings")
    return()
endif()
include(UseSWIG)

set_property(SOURCE xtract.i PROPERTY SWIG_MODULE_NAME xtract)
set_property(SOURCE xtract.i PROPERTY INCLUDE_DIRECTORIES ${PROJECT_SOURCE_DIR}/include)

function(xtract_binding_requirement binding found)
    if(XTRACT_BUILD_${binding} STREQUAL "ON" AND NOT found)
        message(FATAL_ERROR "XTRACT_BUILD_${binding} is ON but its toolchain was not found")
    endif()
endfunction()

if(NOT XTRACT_BUILD_PYTHON STREQUAL "OFF")
    find_package(Python COMPONENTS Interpreter Development.Module)
    if(Python_Interpreter_FOUND AND Python_Development.Module_FOUND)
        set(python_found ON)
    else()
        set(python_found OFF)
    endif()
    xtract_binding_requirement(PYTHON ${python_found})
endif()
if(python_found)
    set(python_dir ${CMAKE_CURRENT_BINARY_DIR}/python)
    swig_add_library(xtract_python
        LANGUAGE python
        TYPE MODULE
        SOURCES xtract.i
        OUTPUT_DIR ${python_dir}
        OUTFILE_DIR ${python_dir})
    set_target_properties(xtract_python PROPERTIES
        OUTPUT_NAME xtract
        LIBRARY_OUTPUT_DIRECTORY $<1:${python_dir}>)
    target_link_libraries(xtract_python PRIVATE xtract::xtract Python::Module)
    add_custom_command(TARGET xtract_python POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/test.py ${python_dir}/test.py)
    add_test(NAME python_bindings
        COMMAND Python::Interpreter ${python_dir}/test.py
        WORKING_DIRECTORY ${python_dir})
    message(STATUS "Python bindings: ${Python_EXECUTABLE}")
endif()

if(NOT XTRACT_BUILD_JAVA STREQUAL "OFF")
    find_package(Java COMPONENTS Development)
    find_package(JNI)
    if(Java_Development_FOUND AND JNI_FOUND)
        set(java_found ON)
    else()
        set(java_found OFF)
    endif()
    xtract_binding_requirement(JAVA ${java_found})
endif()
if(java_found)
    set(java_dir ${CMAKE_CURRENT_BINARY_DIR}/java)
    set_property(SOURCE xtract.i APPEND PROPERTY COMPILE_OPTIONS -package xtract)
    swig_add_library(xtract_java
        LANGUAGE java
        TYPE SHARED
        SOURCES xtract.i
        OUTPUT_DIR ${java_dir}/xtract
        OUTFILE_DIR ${java_dir})
    set_property(SOURCE xtract.i PROPERTY COMPILE_OPTIONS)
    set_target_properties(xtract_java PROPERTIES
        OUTPUT_NAME jxtract
        LIBRARY_OUTPUT_DIRECTORY $<1:${java_dir}>
        RUNTIME_OUTPUT_DIRECTORY $<1:${java_dir}>)
    if(WIN32)
        set_target_properties(xtract_java PROPERTIES PREFIX "")
    endif()
    target_include_directories(xtract_java PRIVATE ${JNI_INCLUDE_DIRS})
    target_link_libraries(xtract_java PRIVATE xtract::xtract)
    add_test(NAME java_bindings
        COMMAND ${CMAKE_COMMAND}
            -DJAVAC=${Java_JAVAC_EXECUTABLE}
            -DJAVA=${Java_JAVA_EXECUTABLE}
            -DSOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}
            -DJAVA_DIR=${java_dir}
            -DLIBRARY_DIR=${java_dir}
            -P ${CMAKE_CURRENT_SOURCE_DIR}/java_test.cmake)
    message(STATUS "Java bindings: ${Java_JAVAC_EXECUTABLE}")
endif()

if(NOT XTRACT_BUILD_NODE STREQUAL "OFF")
    find_program(XTRACT_NODE_EXECUTABLE node)
    if(XTRACT_NODE_EXECUTABLE)
        get_filename_component(node_prefix ${XTRACT_NODE_EXECUTABLE} DIRECTORY)
        get_filename_component(node_prefix ${node_prefix} DIRECTORY)
        execute_process(
            COMMAND ${XTRACT_NODE_EXECUTABLE} -p "require('node-addon-api').include_dir"
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            OUTPUT_VARIABLE node_addon_api_dir
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_QUIET)
    endif()
    find_path(XTRACT_NODE_INCLUDE_DIR node_api.h
        HINTS ${node_prefix}/include/node
        DOC "Directory holding the Node.js headers")
    find_path(XTRACT_NODE_ADDON_API_DIR napi.h
        HINTS ${node_addon_api_dir} ${CMAKE_CURRENT_SOURCE_DIR}/node_modules/node-addon-api
        DOC "Directory holding the node-addon-api headers")
    if(WIN32)
        find_library(XTRACT_NODE_LIBRARY node HINTS ${node_prefix} DOC "node.lib import library")
        set(node_found "${XTRACT_NODE_EXECUTABLE};${XTRACT_NODE_INCLUDE_DIR};${XTRACT_NODE_ADDON_API_DIR};${XTRACT_NODE_LIBRARY}")
    else()
        set(node_found "${XTRACT_NODE_EXECUTABLE};${XTRACT_NODE_INCLUDE_DIR};${XTRACT_NODE_ADDON_API_DIR}")
    endif()
    string(FIND "${node_found}" "NOTFOUND" node_missing)
    if(node_missing EQUAL -1)
        set(node_found ON)
    else()
        set(node_found OFF)
    endif()
    xtract_binding_requirement(NODE ${node_found})
endif()
if(node_found)
    enable_language(CXX)
    set(node_dir ${CMAKE_CURRENT_BINARY_DIR}/node)
    set_property(SOURCE xtract.i PROPERTY CPLUSPLUS ON)
    set_property(SOURCE xtract.i PROPERTY COMPILE_OPTIONS -napi)
    swig_add_library(xtract_node
        LANGUAGE javascript
        TYPE MODULE
        SOURCES xtract.i
        OUTPUT_DIR ${node_dir}
        OUTFILE_DIR ${node_dir})
    set_property(SOURCE xtract.i PROPERTY CPLUSPLUS OFF)
    set_property(SOURCE xtract.i PROPERTY COMPILE_OPTIONS)
    set_target_properties(xtract_node PROPERTIES
        OUTPUT_NAME xtract
        PREFIX ""
        SUFFIX .node
        LIBRARY_OUTPUT_DIRECTORY $<1:${node_dir}>
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON)
    target_include_directories(xtract_node PRIVATE ${XTRACT_NODE_INCLUDE_DIR} ${XTRACT_NODE_ADDON_API_DIR})
    target_link_libraries(xtract_node PRIVATE xtract::xtract)
    if(APPLE)
        target_link_options(xtract_node PRIVATE -undefined dynamic_lookup)
    elseif(WIN32)
        target_link_libraries(xtract_node PRIVATE ${XTRACT_NODE_LIBRARY})
    endif()
    add_custom_command(TARGET xtract_node POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/test.js ${node_dir}/test.js)
    add_test(NAME node_bindings
        COMMAND ${XTRACT_NODE_EXECUTABLE} ${node_dir}/test.js
        WORKING_DIRECTORY ${node_dir})
    message(STATUS "Node.js bindings: ${XTRACT_NODE_EXECUTABLE}")
endif()
