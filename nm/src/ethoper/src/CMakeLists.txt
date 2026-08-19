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
# @date       2022-07-04
# @author     hejunwei
# @version    1.2.0
#
# ================================================================

cmake_minimum_required(VERSION 3.10)
project(ara-nmethoper-lib VERSION 0.1.0 LANGUAGES CXX)

include(FindPkgConfig)
include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

option(ARA_WITH_NM_ETHOPER_BUILD_SHARED_LIBS "Enables the ARA_WITH_NM_ETHOPER_BUILD_SHARED_LIBS target" ${ARA_WITH_NM_BUILD_SHARED_LIBS})

find_package(isoft-cmake-common REQUIRED)
find_package(isoft-common REQUIRED)
find_package(ara-core-types REQUIRED)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

if (${CMAKE_BUILD_TYPE} STREQUAL Debug)
    set(CMAKE_VERBOSE_MAKEFILE ON)
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
else()
    set(CMAKE_VERBOSE_MAKEFILE OFF)
    set(CMAKE_EXPORT_COMPILE_COMMANDS OFF)
endif()

set(LIBRARY_NAME ara-nmethoper)
set(LIBRARY_FILE_NAME ara_nmethoper)
set(TARGET_NAME target-lib-${LIBRARY_NAME})
set(EXPORT_NAME nmethoper)

file(GLOB_RECURSE LIB_SRCS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

if(ARA_WITH_NM_ETHOPER_BUILD_SHARED_LIBS)
    add_library(${TARGET_NAME} SHARED ${LIB_SRCS})
else()
    add_library(${TARGET_NAME} STATIC ${LIB_SRCS})
endif()
add_library(ara::nmethoper ALIAS ${TARGET_NAME})

set_target_properties(${TARGET_NAME} PROPERTIES
    OUTPUT_NAME ${LIBRARY_FILE_NAME}
    EXPORT_NAME ${EXPORT_NAME}
)

target_include_directories(${TARGET_NAME}
PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src/include/>
PRIVATE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src/include/>
    ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(${TARGET_NAME}
PUBLIC 
    ara::core-types
)

# Create *.pc file
if(PKG_CONFIG_FOUND)
    set(LIBRARIES "-l${LIBRARY_FILE_NAME} lpthread lrt lcore-types")
    configure_file(${PROJECT_SOURCE_DIR}/../misc/ara-nmethoper.pc.in ${LIBRARY_NAME}.pc @ONLY)
    install(
        FILES ${CMAKE_CURRENT_BINARY_DIR}/${LIBRARY_NAME}.pc
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
    )
endif()

# Create *-config.cmake file
configure_package_config_file(
    ${PROJECT_SOURCE_DIR}/../misc/ara-nmethoper-config.cmake.in
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

# Install lib 
install(
    TARGETS ${TARGET_NAME}
    EXPORT ${TARGET_NAME}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/ara/nmethoper
)
# Install ara-nmethoper-targets.cmake
install(
    EXPORT ${TARGET_NAME}
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${LIBRARY_NAME}
    NAMESPACE ara::
    FILE ara-nmethoper-targets.cmake
)
if(ARA_ENABLE_TESTS)
	enable_testing()
	find_package(GTest REQUIRED CONFIG)
	include(FindPkgConfig)
        find_package(PkgConfig REQUIRED)
        PKG_SEARCH_MODULE(PKG_GMOCK REQUIRED gmock)
        include_directories($ENV{PKG_CONFIG_SYSROOT_DIR}/${PKG_GMOCK_INCLUDEDIR})
        link_directories($ENV{PKG_CONFIG_SYSROOT_DIR}/${PKG_GMOCK_LIBDIR})

    add_subdirectory(test)
endif(ARA_ENABLE_TESTS)