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
# @date       2024-10-11
# @author     longxiao.liang
# @version    1.2.0
#
# ================================================================

cmake_minimum_required(VERSION 3.4)
project(fg-state-notify-client LANGUAGES CXX VERSION 0.1.0)
set(CMAKE_EXPORT_COMPILE_COMMANDS True)
# If ARA_SM_ROOT_DIR is not defined, set it to the parent directory of this file
if(NOT DEFINED ARA_SM_ROOT_DIR)
    get_filename_component(ARA_SM_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/.." ABSOLUTE)
    message(STATUS "ARA_SM_ROOT_DIR not defined, setting to: ${ARA_SM_ROOT_DIR}")
endif()
set(LIBRARY_NAME ara-sm-fg-state-notify-client)
set(LIBRARY_FILE_NAME ara_sm_fg_state_notify_client)
set(TARGET_NAME target-sm-fg-state-notify-client)
set(EXPORT_NAME fg_state_notify_client)

include(CMakePackageConfigHelpers)
find_package(isoft-cmake-common REQUIRED)


find_package(nai REQUIRED)
find_package(ara-log REQUIRED)
find_package(isoft-ipc)
find_package(isoft-naicpp)
find_package(isoft-manifestreader REQUIRED)
find_package(ara-sm-internal-utils)

file(GLOB_RECURSE LIB_SRCS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

if(BUILD_SHARED_LIBS)
    add_library(${TARGET_NAME} SHARED ${LIB_SRCS})
else()
    add_library(${TARGET_NAME} STATIC ${LIB_SRCS})
endif()

add_library(ara::sm::${EXPORT_NAME} ALIAS ${TARGET_NAME})

target_include_directories(${TARGET_NAME}
PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(${TARGET_NAME}
PUBLIC
    nai
    ara::core-types
    ara::log
    isoft::ipc
    isoft::naicpp
    ara::sm::internal_utils
)

# Create *.pc file
if(PKG_CONFIG_FOUND)
	set(LIBRARIES "-l${LIBRARY_FILE_NAME}")
	configure_file(${PROJECT_SOURCE_DIR}/misc/pkgconfig/ara-sm.pc.in ${LIBRARY_NAME}.pc @ONLY)
    install(
		FILES ${CMAKE_CURRENT_BINARY_DIR}/${LIBRARY_NAME}.pc
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
    )
endif()

# Create *-config.cmake file
configure_package_config_file(
    ${PROJECT_SOURCE_DIR}/misc/cmake/ara-sm-config.cmake.in
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
    DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    COMPONENT dev
    FILES_MATCHING PATTERN "*.h"
)

set_target_properties(${TARGET_NAME} PROPERTIES
	OUTPUT_NAME ${LIBRARY_FILE_NAME}
	#PUBLIC_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/include/ara/exec/execution_client.h
	EXPORT_NAME ${EXPORT_NAME}
)

# Install lib 
install(
	TARGETS ${TARGET_NAME}
	EXPORT ${TARGET_NAME}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/ara/sm
)

# Install ara-exec-targets.cmake
install(
	EXPORT ${TARGET_NAME}
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${LIBRARY_NAME}
    NAMESPACE ara::sm::
    FILE ara-sm-targets.cmake
)
