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
# @brief      cmake configuration file
# @details
# @date       2023-10-12
# @author     junwei.he
# @version    1.2.0
#
# ================================================================
#
# cmake_minimum_required(VERSION 3.16)
# project(ara-log-lib VERSION 0.1.0 LANGUAGES CXX)
# Function: print_source_list(<source_list> <log_label>)
# Parameter description:
# source_list : source file list (pass value directly, not variable name)
# log_label   : label used in logs, e.g., "srcFileName"
#
# ================================================================

function(print_source_list source_list log_label)
    message(STATUS "<<<<< ${PROJECT_NAME}.${LIBRARY_NAME}, SRC_FILE_LIST: ")
    foreach(src_file IN LISTS source_list)
        message(STATUS "  **  ${log_label} = ${src_file}")
    endforeach()
    message(STATUS ">>>>>")
endfunction()

# 1. Name and call find_package
set(LIBRARY_NAME ara-log)
set(LIBRARY_FILE_NAME ara_log)
set(EXPORT_NAME log)

# If the parent does not define ARA_LOG_ROOT_DIR, set it by itself
if(NOT DEFINED ARA_LOG_ROOT_DIR)
    get_filename_component(ARA_LOG_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../" ABSOLUTE)
endif()
message(STATUS "======== Begin : ${PROJECT_NAME}.${LIBRARY_NAME} ========")

message(STATUS " ***  ARA_SYSROOT               = ${ARA_SYSROOT} ")
message(STATUS " ***  ARA_LOG_ROOT_DIR          = ${ARA_LOG_ROOT_DIR} ")
message(STATUS "  **  CMAKE_SOURCE_DIR          = ${CMAKE_SOURCE_DIR}")
message(STATUS "  **  PROJECT_SOURCE_DIR        = ${PROJECT_SOURCE_DIR} ")
message(STATUS "  **  CMAKE_CURRENT_SOURCE_DIR  = ${CMAKE_CURRENT_SOURCE_DIR} ")

# 2. Define the source list
file(GLOB_RECURSE SRC_PH_LIST CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/DltLog/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/DltLogDerm/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/DltLogInterface/*.cpp"
)

set(ALL_SRC_LIST ${SRC_PH_LIST})

# Handle DltSinkers
set(LIB_SINK_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/DltSinkers/file_sinker_no_locker.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/DltSinkers/ipc_shared_state.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/DltSinkers/remote_sinker.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/DltSinkers/rollover_helper.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/DltSinkers/semaphore_lock.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/DltSinkers/zip_compressor.cpp
)

if(IS_LINUX)
    list(APPEND LIB_SINK_SRCS
        ${CMAKE_CURRENT_SOURCE_DIR}/DltSinkers/file_sinker_withLocker.cpp
    )
endif()
list(APPEND ALL_SRC_LIST ${LIB_SINK_SRCS})

# Handle DltProtocol
set(LIB_PROTOCOL_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/DltProtocol/dlt_plain_encoder.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/DltProtocol/dlt_protocol_encoder.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/DltProtocol/dlt_protocol_model_encoder.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/DltProtocol/dlt_protocol_nomodel_encoder.cpp
)

if(ARA_LOG_HAS_REMOTE)
    list(APPEND LIB_PROTOCOL_SRCS
        ${CMAKE_CURRENT_SOURCE_DIR}/DltProtocol/log_command_excutor.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/DltProtocol/log_command_ipc_client.cpp
    )
endif()
list(APPEND ALL_SRC_LIST ${LIB_PROTOCOL_SRCS})

# Output the list of source files involved in compilation
print_source_list( "${ALL_SRC_LIST}" "srcFile")

# 3. Build the shared library uniformly (including all conditionally appended object files)
message("ARA_WITH_LOG_BUILD_SHARED_LIBS:" ${ARA_WITH_LOG_BUILD_SHARED_LIBS})

if(ARA_WITH_LOG_BUILD_SHARED_LIBS)
    add_library(${LIBRARY_NAME} SHARED ${ALL_SRC_LIST})
else()
    add_library(${LIBRARY_NAME} STATIC ${ALL_SRC_LIST})
endif()

add_library(ara::log ALIAS ${LIBRARY_NAME})

# Link ara-log-utils to inherit its include directory
target_link_libraries(${LIBRARY_NAME} PRIVATE ara::log-utils)

# Link object libraries (via target_sources)
target_sources(${LIBRARY_NAME} PRIVATE
    $<TARGET_OBJECTS:ara::log-utils>
)
# 4. Include file paths
# Utils header include path - supports #include "Utils/xxx.h" form
target_include_directories(${LIBRARY_NAME}
    PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    PRIVATE
    ${ARA_LOG_ROOT_DIR}
    ${ARA_LOG_ROOT_DIR}/Utils/src
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/include/ara/log
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# 5. Add precompilation macros to pass to C++
# Should this be removed?
# if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
# target_compile_options(${LIBRARY_NAME} PRIVATE
# -Wall -Wno-long-long -Wpedantic -Wunused-variable)
# endif()
if(ARA_ENABLE_AP_ENV)
    target_compile_definitions(${LIBRARY_NAME} PRIVATE ARA_ENABLE_AP_ENV)
endif()

# if(IS_LINUX)
# if(COMPILER_HAS_LINUX_MACRO)
# message(STATUS "Compiler already has __linux__ macro, no need to manually define")
# else()
# message(STATUS "Compiler does not provide __linux__ macro, manually adding...")
# target_compile_definitions(${LIBRARY_NAME} PRIVATE __linux__)
# endif()
# target_link_libraries(${LIBRARY_NAME} PUBLIC pthread rt)
# endif()

if(IS_LINUX)
    find_package(Threads REQUIRED)
    target_link_libraries(${LIBRARY_NAME} PUBLIC Threads::Threads)
    set(LIBRT rt)
    message("link pthread")
endif()

if(ARA_WITH_TSYNC)
    message("lib ARA_WITH_TSYNC on -----------")
    find_package(ara-tsync REQUIRED)
    target_compile_definitions(${LIBRARY_NAME} PRIVATE ARA_WITH_TSYNC)
    target_link_libraries(${LIBRARY_NAME}
        PUBLIC
        ara::tsync
    )
endif()

if(ARA_LOG_INTERNAL_IPC_DEBUG)
    target_compile_definitions(${LIBRARY_NAME} PRIVATE ARA_LOG_INTERNAL_IPC_DEBUG)
endif()

target_compile_definitions(${LIBRARY_NAME}
    PUBLIC
    HAS_ARA_LOG
    PRIVATE ARA_MAJOR_VERSION=${ARA_MAJOR_VERSION}
)

# 6. Other properties
set_target_properties(${LIBRARY_NAME} PROPERTIES
    OUTPUT_NAME ${LIBRARY_FILE_NAME}
    EXPORT_NAME ${EXPORT_NAME}
)

if(ARA_LOG_HAS_REMOTE)
    target_compile_definitions(${LIBRARY_NAME} PUBLIC ARA_LOG_HAS_REMOTE)
    target_link_libraries(${LIBRARY_NAME}
        PUBLIC
        isoft::ipc
        isoft::ipccpp
        isoft::ipcbase
    )
endif()

if(ARA_LOG_HAS_FILE_CHECKER)
    target_compile_definitions(${LIBRARY_NAME} PRIVATE ARA_LOG_HAS_FILE_CHECKER)
endif()

# 7. Dependent libraries
target_link_libraries(${LIBRARY_NAME}
    PUBLIC
    nai
    isoft::ara_fsh
    isoft::naicpp
    ara::core-types
    atomic
    ${LIBTHREADD}
    ${LIBRT}
    RapidJSON
    ZLIB::ZLIB
)

# #================== Installation configuration related
# Perform installation only after the target is successfully built
if(TARGET ${LIBRARY_NAME})
    # Create *.pc file
    if(PKG_CONFIG_FOUND)
        set(LIBRARIES "-l${LIBRARY_FILE_NAME} lpthread lrt lcore-types lnai lnaicpp lisoft_ipccpp lisoft_ipc  lisoft_ipc_em  lisoft_ipcbase lara_tsync  -lisoft_ara_fsh")
        configure_file(${PROJECT_SOURCE_DIR}/misc/pkgconfig/ara-log.pc.in ${LIBRARY_NAME}.pc @ONLY)
        install(
            FILES ${CMAKE_CURRENT_BINARY_DIR}/${LIBRARY_NAME}.pc
            DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
        )
    endif()

    if(ARA_WITH_TSYNC)
        set(optional_find_dependency_tsync "find_dependency(ara-tsync)")
    else()
        set(optional_find_dependency_tsync "")
    endif(ARA_WITH_TSYNC)

    # Create *-config.cmake file
    configure_package_config_file(
        ${PROJECT_SOURCE_DIR}/misc/cmake/ara-log-config.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/${LIBRARY_NAME}-config.cmake
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${LIBRARY_NAME}
        NO_CHECK_REQUIRED_COMPONENTS_MACRO
    )

    # Create *-config-version.cmake file
    write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/${LIBRARY_NAME}-config-version.cmake
        COMPATIBILITY ExactVersion
    )

    # Install *-config.cmake and *-version.cmake files
    install(
        FILES
        ${CMAKE_CURRENT_BINARY_DIR}/${LIBRARY_NAME}-config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/${LIBRARY_NAME}-config-version.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${LIBRARY_NAME}
    )

    # Install header files
    install(
        DIRECTORY ${PROJECT_SOURCE_DIR}/include/ara/log/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/ara/log
        COMPONENT dev
        FILES_MATCHING PATTERN "*.h"
    )

    # Install lib
    install(
        TARGETS ${LIBRARY_NAME}
        EXPORT ${LIBRARY_NAME}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/ara/log
    )

    # Install ara-log-targets.cmake
    install(
        EXPORT ${LIBRARY_NAME}
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${LIBRARY_NAME}
        NAMESPACE ara::
        FILE ara-log-targets.cmake
    )

    message(STATUS "Installation rules configured for ${LIBRARY_NAME}")
else()
    message(WARNING "Target ${LIBRARY_NAME} not found, skipping installation configuration")
endif()

message(STATUS "======== End   : ${PROJECT_NAME}.${LIBRARY_NAME} ========")