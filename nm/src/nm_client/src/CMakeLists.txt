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
# @author     hejunwei
# @version    1.2.0
#
# ================================================================

cmake_minimum_required(VERSION 3.10)
project(ara-nm-lib VERSION 0.1.0 LANGUAGES CXX)
set(rootDir ${CMAKE_CURRENT_LIST_DIR}/../../../)

message("-------------------" ${CMAKE_SOURCE_DIR})
message("-------------------" ${CMAKE_CURRENT_LIST_DIR})
include(FindPkgConfig)
include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

option(ARA_NM_NO_CHECKSM "" OFF)

find_package(isoft-cmake-common REQUIRED)
find_package(isoft-common REQUIRED)
find_package(isoft-naicpp REQUIRED)
find_package(isoft-ipc REQUIRED)
find_package(isoft-ara-fsh REQUIRED)
find_package(isoft-manifestreader REQUIRED)
find_package(ara-core-types REQUIRED)
find_package(ara-log REQUIRED)
find_package(RapidJSON REQUIRED)
 

message(STATUS "ARA_NM_NO_CHECKSM=${ARA_NM_NO_CHECKSM}")
if (${ARA_NM_NO_CHECKSM} STREQUAL ON)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DARA_NM_NOCHECK_SM_PROCESS=1")
endif()

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -pedantic -Wunused-variable")
endif()



set(LIBRARY_NAME ara-nm)
set(LIBRARY_FILE_NAME ara_nm)
set(TARGET_NAME target-lib-${LIBRARY_NAME})
set(EXPORT_NAME nm)

if(ARA_ENABLE_AP_ENV)
    message("using ap environment variables")
    add_definitions(-DARA_ENABLE_AP_ENV)
endif()

file(GLOB_RECURSE LIB_SRCS ${CMAKE_CURRENT_LIST_DIR}/*.cpp)

if(ARA_WITH_NM_BUILD_SHARED_LIBS)
    add_library(${TARGET_NAME} SHARED ${LIB_SRCS})
else()
    add_library(${TARGET_NAME} STATIC ${LIB_SRCS})
endif()

add_library(ara::nm ALIAS ${TARGET_NAME})

target_compile_definitions(${TARGET_NAME}
	PUBLIC
	HAS_ARA_NM
)

set_target_properties(${TARGET_NAME} PROPERTIES
    OUTPUT_NAME ${LIBRARY_FILE_NAME}
    EXPORT_NAME ${EXPORT_NAME}
)
## -------------Include paths and dependent libraries related-----------------------------------
target_include_directories(${TARGET_NAME}
PUBLIC
    $<BUILD_INTERFACE:${rootDir}/src/include/>
PRIVATE
    $<BUILD_INTERFACE:
    ${CMAKE_CURRENT_LIST_DIR}/include/
)

target_link_libraries(${TARGET_NAME}
PUBLIC 
    ara::core-types
    ara::log
    isoft::ara_fsh
    pthread
    rt
    isoft::ipc
    isoft::naicpp
    isoft::ipcbase
    isoft::ipccpp
    RapidJSON
)

##==================Installation configuration related

# Create *.pc file
if(PKG_CONFIG_FOUND)
    set(LIBRARIES "-l${LIBRARY_FILE_NAME} lrt lcore-types lnaicpp lisoft_ipccpp lisoft_ipc  lisoft_ipcbase lara_log  -lisoft_ara_fsh")
    configure_file(${CMAKE_CURRENT_LIST_DIR}/../misc/ara-nm.pc.in ${LIBRARY_NAME}.pc @ONLY)
    install(
        FILES ${CMAKE_CURRENT_BINARY_DIR}/${LIBRARY_NAME}.pc
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
    )
endif()

# Create *-config.cmake file
configure_package_config_file(
    ${CMAKE_CURRENT_LIST_DIR}/../misc/ara-nm-config.cmake.in
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
    DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/include/public/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/ara/nm
    COMPONENT dev
    FILES_MATCHING PATTERN "*.h"
)
install(
    DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/include/internal/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/ara/nm/internal/
    COMPONENT dev
    FILES_MATCHING PATTERN "*.h"
)
# Install lib 
install(
    TARGETS ${TARGET_NAME}
    EXPORT ${TARGET_NAME}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/ara/nm
)

# Install ara-nm-targets.cmake
install(
    EXPORT ${TARGET_NAME}
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${LIBRARY_NAME}
    NAMESPACE ara::
    FILE ara-nm-targets.cmake
)
