# Part of LibXtract
#
# SPDX-FileCopyrightText: 2026 Jamie Bullock
# SPDX-License-Identifier: Zlib

set(XTRACT_SOURCES
    delta.c
    descriptors.c
    helper.c
    init.c
    libxtract.c
    scalar.c
    stateful.c
    vector.c
    window.c)

set(XTRACT_THIRD_PARTY_SOURCES
    c-ringbuf/ringbuf.c
    dywapitchtrack/dywapitchtrack.c
    ooura/fftsg.c)

add_library(xtract ${XTRACT_SOURCES} ${XTRACT_THIRD_PARTY_SOURCES})
add_library(xtract::xtract ALIAS xtract)

target_include_directories(xtract
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

set_target_properties(xtract PROPERTIES
    C_STANDARD 99
    C_STANDARD_REQUIRED ON
    C_EXTENSIONS OFF
    POSITION_INDEPENDENT_CODE ON
    WINDOWS_EXPORT_ALL_SYMBOLS ON
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR})

# The style warnings apply to the first-party sources only; the third-party
# directories keep their upstream style
if(CMAKE_C_COMPILER_ID STREQUAL "MSVC")
    set(XTRACT_STYLE_WARNINGS /W4 /wd4068)
else()
    target_compile_options(xtract PRIVATE -Wall -pedantic)
    set(XTRACT_STYLE_WARNINGS
        -Wextra
        -Wno-unused-parameter
        -Wshadow
        -Wdeclaration-after-statement
        -Wstrict-prototypes)
    if(CMAKE_C_COMPILER_ID MATCHES "Clang")
        list(APPEND XTRACT_STYLE_WARNINGS -Wconditional-uninitialized)
    endif()
endif()
set_source_files_properties(${XTRACT_SOURCES} PROPERTIES COMPILE_OPTIONS "${XTRACT_STYLE_WARNINGS}")

if(APPLE)
    target_link_libraries(xtract PUBLIC "-framework Accelerate")
    set(XTRACT_PC_LIBS_PRIVATE "-framework Accelerate")
else()
    target_compile_definitions(xtract PRIVATE USE_OOURA)
    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(xtract PRIVATE -fopenmp-simd)
    endif()
    set(XTRACT_PC_LIBS_PRIVATE "")
endif()
if(NOT APPLE AND NOT MSVC)
    target_link_libraries(xtract PUBLIC m)
    set(XTRACT_PC_LIBS_PRIVATE "-lm")
endif()

set(XTRACT_MUTATION_PLUGIN "" CACHE FILEPATH "Mull IR frontend plugin to compile the library with")
if(XTRACT_MUTATION_PLUGIN)
    if(NOT CMAKE_C_COMPILER_ID MATCHES "Clang")
        message(FATAL_ERROR "XTRACT_MUTATION_PLUGIN needs a Clang compiler, the C compiler is ${CMAKE_C_COMPILER_ID}")
    endif()
    target_compile_options(xtract PRIVATE -fpass-plugin=${XTRACT_MUTATION_PLUGIN} -grecord-command-line)
endif()

include(CMakePackageConfigHelpers)

install(TARGETS xtract EXPORT xtractTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/xtract DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(EXPORT xtractTargets
    NAMESPACE xtract::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/xtract)

configure_package_config_file(${PROJECT_SOURCE_DIR}/cmake/xtractConfig.cmake.in
    ${PROJECT_BINARY_DIR}/xtractConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/xtract)
write_basic_package_version_file(${PROJECT_BINARY_DIR}/xtractConfigVersion.cmake
    COMPATIBILITY SameMajorVersion)
install(FILES
    ${PROJECT_BINARY_DIR}/xtractConfig.cmake
    ${PROJECT_BINARY_DIR}/xtractConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/xtract)

# The pkg-config paths are written relative to the installed .pc file, so
# they follow an install prefix given at install time and do not depend on
# the prefix pkg-config infers
set(XTRACT_PC_DIR ${CMAKE_INSTALL_FULL_LIBDIR}/pkgconfig)
file(RELATIVE_PATH XTRACT_PC_PREFIX ${XTRACT_PC_DIR} ${CMAKE_INSTALL_PREFIX})
string(REGEX REPLACE "/$" "" XTRACT_PC_PREFIX "${XTRACT_PC_PREFIX}")
if(IS_ABSOLUTE "${CMAKE_INSTALL_INCLUDEDIR}")
    set(XTRACT_PC_INCLUDEDIR "${CMAKE_INSTALL_INCLUDEDIR}")
else()
    file(RELATIVE_PATH XTRACT_PC_INCLUDEDIR ${XTRACT_PC_DIR} ${CMAKE_INSTALL_FULL_INCLUDEDIR})
    string(REGEX REPLACE "/$" "" XTRACT_PC_INCLUDEDIR "${XTRACT_PC_INCLUDEDIR}")
    set(XTRACT_PC_INCLUDEDIR "\${pcfiledir}/${XTRACT_PC_INCLUDEDIR}")
endif()
configure_file(${PROJECT_SOURCE_DIR}/cmake/xtract.pc.in ${PROJECT_BINARY_DIR}/xtract.pc @ONLY)
install(FILES ${PROJECT_BINARY_DIR}/xtract.pc DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
