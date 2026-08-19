#  Disclaimer
#
#  This work (specification and/or software implementation) and the material
#  contained in it, as released by AUTOSAR, is for the purpose of information
#  only. AUTOSAR and the companies that have contributed to it shall not be
#  liable for any use of the work.
#
#  The material contained in this work is protected by copyright and other
#  types of intellectual property rights. The commercial exploitation of the
#  material contained in this work requires a license to such intellectual
#  property rights.
#
#  This work may be utilized or reproduced without any modification, in any
#  form or by any means, for informational purposes only. For any other
#  purpose, no part of the work may be utilized or reproduced, in any form
#  or by any means, without permission in writing from the publisher.
#
#  The work has been developed for automotive applications only. It has
#  neither been developed, nor tested for non-automotive applications.
#
#  The word AUTOSAR and the AUTOSAR logo are registered trademarks.
#  --------------------------------------------------------------------------

# ================================================================
#
# File description:
# ----------------
# @file       CMakeLists.txt
# @brief
# @details
# @date       2026-01-06
# @author     mazelin
# @version    1.2.0
#
# ================================================================

cmake_minimum_required (VERSION 3.0)


if (NOT DEFINED CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type")
endif()

if (POLICY CMP0025)
    cmake_policy(SET CMP0025 NEW)
endif()
if (POLICY CMP0063)
    cmake_policy(SET CMP0063 NEW) # for CMAKE_C_VISIBILITY_PRESET
endif()
if (POLICY CMP0067)
    cmake_policy(SET CMP0067 NEW) # for try_compile
endif()


project(icc LANGUAGES C CXX VERSION 1.1.0) # project version,not sdk version


option(CMAKE_POSITION_INDEPENDENT_CODE "Build position independent libraries" ON)
option(BUILD_SHARED_LIBS "Build using shared libraries" ON)
option(BUILD_TESTING "Build testing programs" ${ARA_ENABLE_TESTS})
option(BUILD_WITH_DBGINFO "Build with debug information" ON)
option(UNIX_SOCK_PREFIX "Set the prefix of unix socket path")
option(UNIX_SOCK_SUFFIX "Set the suffix of unix socket path")


include(CheckCSourceRuns)
include(CheckSymbolExists)


if (${CMAKE_SYSTEM_NAME} MATCHES "QNX")
    set(QNX 1)
endif()

# os bits
math(EXPR osbits "8*${CMAKE_SIZEOF_VOID_P}")


# configure output dir
if (NOT MSVC)
    set(config_dir ${CMAKE_BUILD_TYPE})
else()
    #set(config_dir $<$<CONFIG:Debug>:Debug>)
    #set(config_dir ${config_dir}$<$<CONFIG:Release>:Release>)
    #set(config_dir ${config_dir}$<$<CONFIG:RelWithDebInfo>:RelWithDebInfo>)
    #set(config_dir ${config_dir}$<$<CONFIG:MinSizeRel>:MinSizeRel>)
endif()
set(output_dir /x${osbits})
set(output_subdir ${output_dir}/${config_dir})

if (UNIX)
  set(NAI_HAVE_LIBM TRUE)
endif()


# force use /MT and disable warnings
if (MSVC)
    foreach(flags
        CMAKE_C_FLAGS
        CMAKE_C_FLAGS_DEBUG
        CMAKE_C_FLAGS_RELEASE
        CMAKE_C_FLAGS_MINSIZEREL
        CMAKE_C_FLAGS_RELWITHDEBINFO
        CMAKE_CXX_FLAGS
        CMAKE_CXX_FLAGS_DEBUG
        CMAKE_CXX_FLAGS_RELEASE
        CMAKE_CXX_FLAGS_MINSIZEREL
        CMAKE_CXX_FLAGS_RELWITHDEBINFO)
        if(${flags} MATCHES "/MD")
            string(REGEX REPLACE "/MD" "/MT" ${flags} "${${flags}}")
        endif(${flags} MATCHES "/MD")
        set(${flags} "${${flags}} /wd4819")
    endforeach(flags)
endif(MSVC)

# set complie options
if (BUILD_WITH_DBGINFO)
    set(CMAKE_C_FLAGS_RELEASE ${CMAKE_C_FLAGS_RELWITHDEBINFO})
    set(CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELWITHDEBINFO})
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE ${CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO})
    set(CMAKE_SHARED_LINKER_FLAGS_RELEASE ${CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO})
    set(CMAKE_STATIC_LINKER_FLAGS_RELEASE ${CMAKE_STATIC_LINKER_FLAGS_RELWITHDEBINFO})
endif()
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin${output_subdir})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin${output_subdir})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib${output_subdir})
set(CMAKE_PDB_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/pdb${output_subdir})


# show complie options
message(STATUS "osbits: ${osbits}")
message(STATUS "c compiler: ${CMAKE_C_COMPILER_ID} ${CMAKE_C_COMPILER_VERSION}")
message(STATUS "cxx compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "debug c flags: ${CMAKE_C_FLAGS_DEBUG}")
message(STATUS "debug cxx flags: ${CMAKE_CXX_FLAGS_DEBUG}")
message(STATUS "debug link flags: ${CMAKE_SHARED_LINKER_FLAGS_DEBUG}")
message(STATUS "release c flags: ${CMAKE_C_FLAGS_RELEASE}")
message(STATUS "release cxx flags: ${CMAKE_CXX_FLAGS_RELEASE}")
message(STATUS "release link flags: ${CMAKE_SHARED_LINKER_FLAGS_RELEASE}")
message(STATUS "output directories:")
message(STATUS "  bin: " ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
message(STATUS "  lib: " ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})
message(STATUS "  pdb: " ${CMAKE_PDB_OUTPUT_DIRECTORY})


execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})
execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_PDB_OUTPUT_DIRECTORY})


# exports compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


##############################################################################
# depends


include(ExternalProject)


# thridparty path
set(thirdparty_dir ${PROJECT_SOURCE_DIR}/thirdparty)
set(thirdparty_install ${thirdparty_dir}/install)
execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${thirdparty_install})
execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${thirdparty_install}/include)


# add cmake scripts
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/misc/cmake)


# shell suffix
set(exe_suffix "")
set(cmd_suffix "")
if (WIN32)
    set(exe_suffix ".exe")
    set(cmd_suffix ".cmd")
endif(WIN32)


if (QNX)
    list(APPEND sys "socket")
endif()


find_package(NAI QUIET)
set(nai "nai")
if (NOT NAI_FOUND)
    if (MSVC)
        set(nai_libname libnai.lib)
        set(nai_libdir lib/${output_dir}/${CMAKE_BUILD_TYPE})
    elseif(BUILD_SHARED_LIBS)
        set(nai_libname libnai${CMAKE_SHARED_LIBRARY_SUFFIX})
        set(nai_libdir bin/${output_dir}/${config_dir})
    else()
        set(nai_libname libnai${CMAKE_STATIC_LIBRARY_SUFFIX})
        set(nai_libdir lib/${output_dir}/${config_dir})
    endif()
    if (DEFINED ENV{ARA_SYSROOT})
        set(NAI_INCLUDE_DIRS ${CMAKE_PREFIX_PATH}/include)
        set(NAI_LIBRARIES ${CMAKE_PREFIX_PATH}/lib/${nai_libname})
    else()
        get_filename_component(nai_path ../../nai ABSOLUTE)
        set(NAI_INCLUDE_DIRS ${nai_path}/include)
        set(NAI_LIBRARIES ${nai_path}/${nai_libdir}/${nai_libname})
    endif()

    message(STATUS "nai include: ${NAI_INCLUDE_DIRS}")
    message(STATUS "nai libs: ${NAI_LIBRARIES}")

    add_library(${nai} UNKNOWN IMPORTED)
    set_target_properties(${nai} PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES ${NAI_INCLUDE_DIRS})
    set_target_properties(${nai} PROPERTIES
        IMPORTED_LOCATION ${NAI_LIBRARIES})

endif()


# target for thirdparty
add_custom_target(thirdparty DEPENDS ${nai})


##############################################################################
# porijects


# open warning
if (NOT MSVC)
    add_definitions(-Wall)
    add_definitions(-Wextra)
    add_definitions(-Wno-missing-field-initializers)
    add_definitions(-Wdouble-promotion)
    add_definitions(-Winit-self)
    add_definitions(-Wswitch-default)
    add_definitions(-Wuninitialized)
    add_definitions(-Wfloat-equal)
    add_definitions(-Wshadow)
    add_definitions(-Wunreachable-code)
	add_compile_options("-Wno-pedantic")
	add_compile_options("-Wno-sign-conversion")
	add_compile_options("-Wno-cast-qual")
	add_compile_options("-Wno-conversion")
endif()
if (${CMAKE_C_COMPILER_ID} STREQUAL "GNU")
    add_definitions(-Wabi=99)
elseif (${CMAKE_C_COMPILER_ID} STREQUAL "Clang")
    add_definitions(-Wabi)
endif()


# config.h
if (UNIX_SOCK_PREFIX)
    set(NSI_UNIX_SOCK_PREFIX ${UNIX_SOCK_PREFIX})
endif()
if (UNIX_SOCK_SUFFIX)
    set(NSI_UNIX_SOCK_SUFFIX ${UNIX_SOCK_SUFFIX})
endif()
configure_file(cmake.config.h.in
    ${CMAKE_CURRENT_SOURCE_DIR}/include/icc/config/icc_config.h
    @ONLY)

# list icc sources
file(GLOB    core_FILES     src/icc/core/*.c    src/icc/core/*.h)
file(GLOB    net_FILES      src/icc/net/*.c     src/icc/net/*.h)
file(GLOB    include_FILES  include/icc/*.h
                            include/icc/net/*.h)


# add icc library
set(icc "icc")
set(icc_depends ${nai} ${sys})
set(icc_include include src)
set(icc_source
    ${core_FILES}
    ${net_FILES})


add_library(${icc} ${icc_source})
set_target_properties(${icc} PROPERTIES C_STANDARD 99)
set_target_properties(${icc} PROPERTIES C_VISIBILITY_PRESET hidden)
set_target_properties(${icc} PROPERTIES VISIBILITY_INLINES_HIDDEN ON)

if (MSVC)
    set_target_properties(${nai} PROPERTIES OUTPUT_NAME "libicc")
endif()

target_link_libraries(${icc} ${icc_depends})
target_include_directories(${icc} PRIVATE src)
target_include_directories(${icc} PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)

if (BUILD_SHARED_LIBS)
    target_compile_definitions(${icc} PRIVATE ICC_DLL)
endif()

##############################################################################
# install


include(CMakePackageConfigHelpers)


if (NOT DEFINED CMAKE_INSTALL_LIBDIR)
    set(CMAKE_INSTALL_LIBDIR lib)
endif()
if (NOT DEFINED CMAKE_INSTALL_BINDIR)
    set(CMAKE_INSTALL_BINDIR bin)
endif()
if (NOT DEFINED CMAKE_INSTALL_INCLUDEDIR)
    set(CMAKE_INSTALL_INCLUDEDIR include)
endif()


install(TARGETS ${icc}
    EXPORT "${PROJECT_NAME}-targets"
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})

install(DIRECTORY
    ${CMAKE_CURRENT_SOURCE_DIR}/include/icc
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    PATTERN *.h)

install(
    EXPORT "${PROJECT_NAME}-targets"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})


configure_package_config_file(
    ${PROJECT_SOURCE_DIR}/misc/cmake/${PROJECT_NAME}-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
    NO_CHECK_REQUIRED_COMPONENTS_MACRO)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake
    COMPATIBILITY ExactVersion)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})


##############################################################################
# testing

if(BUILD_TESTING)
# set(CMAKE_CXX_STANDARD_COMPUTED_DEFAULT "14")

file(GLOB    test_server_FILES  src/test/icc/common/*.cpp  src/test/icc/server/*.cpp)
file(GLOB    test_client_FILES  src/test/icc/common/*.cpp  src/test/icc/client/*.cpp)

find_package(NAI REQUIRED)
find_package(isoft-manifestreader REQUIRED)
find_package(Threads REQUIRED)

set(test_server "test_server")
set(test_client "test_client")

add_executable(${test_server} ${test_server_FILES})
add_executable(${test_client} ${test_client_FILES})

target_compile_definitions(${test_server} PUBLIC ICC_STATIC)
target_include_directories(${test_server} PUBLIC include)
target_include_directories(${test_server} PUBLIC src)
target_link_libraries(${test_server} PRIVATE ${icc_depends} ${icc} isoft::manifestreader Threads::Threads)

target_compile_definitions(${test_client} PUBLIC ICC_STATIC)
target_include_directories(${test_client} PUBLIC include)
target_include_directories(${test_client} PUBLIC src)
target_link_libraries(${test_client} PRIVATE ${icc_depends} ${icc} isoft::manifestreader Threads::Threads)
endif()
