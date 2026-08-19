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
# @date       2021-10-26
# @author     fengjian
# @version    1.2.0
#
# ================================================================

cmake_minimum_required(VERSION 3.10)
project(none VERSION 0.1.0 LANGUAGES CXX)
set(LIBRARY_NAME ara-tsync)
set(LIBRARY_FILE_NAME ara_tsync)
set(TARGET_NAME target-lib-${LIBRARY_NAME})
set(EXPORT_NAME tsync)

include(FindPkgConfig)
include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

find_package(isoft-cmake-common REQUIRED)
find_package(isoft-common REQUIRED)
find_package(nai REQUIRED)
find_package(isoft-naicpp REQUIRED)
find_package(isoft-ipc REQUIRED)
find_package(isoft-ipcbase REQUIRED)
find_package(isoft-ara-fsh REQUIRED)
find_package(isoft-manifestreader REQUIRED)
find_package(ara-core-types REQUIRED)

if (NOT DEFINED CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    set(CMAKE_VERBOSE_MAKEFILE ON)
    set(BUILD_SHARED_LIBS ON)
    set(CMAKE_EXPORT_COMPILE_COMMANDS YES)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DARA_TSYNC_DEBUG=1")
else()
    set(CMAKE_VERBOSE_MAKEFILE OFF)
    set(CMAKE_EXPORT_COMPILE_COMMANDS NO)
endif()


set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(ARA_CMAKE_INSTALL_BIN "bin")

option(ARA_ENABLE_DOXYGEN "Enables the documentation target" OFF)

add_library(${TARGET_NAME}  SHARED
    ./synchronized_time_base_consumer.cpp
    ./synchronized_time_base_consumer_private_impl.cpp
    ./synchronized_time_base_provider.cpp
    ./synchronized_time_base_provider_private_impl.cpp
    ./synchronized_time_base_status.cpp
    ./offset_time_base_provider.cpp
    ./offset_time_base_provider_private_impl.cpp
    ./time_base_provider_common.cpp
)


add_library(ara::tsync ALIAS ${TARGET_NAME})



set_target_properties(${TARGET_NAME} PROPERTIES
    OUTPUT_NAME ${LIBRARY_FILE_NAME}
    EXPORT_NAME ${EXPORT_NAME}
)

target_include_directories(${TARGET_NAME}
PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include/public>
    $<INSTALL_INTERFACE:include>
PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include/private>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include/internal/private>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include/internal/public>
)


target_link_libraries(${TARGET_NAME}
PUBLIC
    ${all_libs}
    ara::core-types
    isoft::ara_fsh
PRIVATE
    ara::tsync-pub
)

# Create *.pc file
if(PKG_CONFIG_FOUND)
    set(LIBRARIES "-l${LIBRARY_FILE_NAME} -lnaicpp -lisoft_ipc -lisoft_ara_fsh")
    configure_file(${PROJECT_SOURCE_DIR}/../misc/ara-tsync.pc.in ${LIBRARY_NAME}.pc @ONLY)
    install(
        FILES ${CMAKE_CURRENT_BINARY_DIR}/${LIBRARY_NAME}.pc
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
    )
endif()

# Create *-config.cmake file
configure_package_config_file(
    ${PROJECT_SOURCE_DIR}/../misc/ara-tsync-config.cmake.in
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
    DIRECTORY include/public/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    COMPONENT dev
    FILES_MATCHING PATTERN "*.h"
)

# Install internal header files
install(
    DIRECTORY include/internal/public/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    COMPONENT dev
    FILES_MATCHING PATTERN "*.h"
)

# Install lib 
install(
    TARGETS ${TARGET_NAME}
    EXPORT ${TARGET_NAME}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/ara/tsync
)

# Install ara-tsync-targets.cmake
install(
    EXPORT ${TARGET_NAME}
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${LIBRARY_NAME}
    NAMESPACE ara::
    FILE ara-tsync-targets.cmake
)
