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
# @author     yanlong.wang
# @version    1.2.0
#
# ================================================================

cmake_minimum_required(VERSION 3.10)
project(ara-phm-lib-recovery-action VERSION 0.1.0 LANGUAGES CXX)

# If ARA_PHM_ROOT_DIR is not defined, set it to the parent directory of this file
if(NOT DEFINED ARA_PHM_ROOT_DIR)
    get_filename_component(ARA_PHM_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../.." ABSOLUTE)
    message(STATUS "ARA_PHM_ROOT_DIR not defined, setting to: ${ARA_PHM_ROOT_DIR}")
endif()

include(${ARA_PHM_ROOT_DIR}/misc/cmake/phm_lib.cmake)

if(NOT TARGET target-commoncom-client)
    add_subdirectory(${ARA_PHM_ROOT_DIR}/src/common ${CMAKE_BINARY_DIR}/src/common)
endif()

set(LIBRARY_NAME ara-phm-recovery-action)
set(LIBRARY_FILE_NAME ara_phm_recovery_action)
set(TARGET_NAME phm-recovery-action)
set(EXPORT_NAME phm_recovery_action)

add_library(${TARGET_NAME} SHARED
    src/recovery_action.cpp
)

add_library(ara::phm::${EXPORT_NAME} ALIAS ${TARGET_NAME})
target_compile_definitions(${TARGET_NAME}
PUBLIC
	HAS_ARA_PHM_CLIENT # for check library , see ara::core::Initialize
)

target_include_directories(${TARGET_NAME}
PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
    $<BUILD_INTERFACE:${ARA_PHM_ROOT_DIR}/src/common/src/util/include>
    $<BUILD_INTERFACE:${ARA_PHM_ROOT_DIR}/src/common/src/racom/include>
    $<BUILD_INTERFACE:${ARA_PHM_ROOT_DIR}/src/common/src/raextcom/include>
    $<BUILD_INTERFACE:${ARA_PHM_ROOT_DIR}/src/common/src/hcacom/include>
    $<BUILD_INTERFACE:${ARA_PHM_ROOT_DIR}/src/common/src/hcaextcom/include>
)

add_dependencies(${TARGET_NAME}
    target-util-log
    target-commoncom-server
    target-commoncom-client
    target-racom-server
    target-hcacom-server
    target-raextcom-client
    target-hcaextcom-client
)

target_link_libraries(${TARGET_NAME}
PUBLIC
    isoft::ipccpp
    isoft::naicpp
    ara::core-types
    $<BUILD_INTERFACE:target-util-log>
    $<BUILD_INTERFACE:target-commoncom-server>
    $<BUILD_INTERFACE:target-commoncom-client>
    $<BUILD_INTERFACE:target-racom-server>
    $<BUILD_INTERFACE:target-hcacom-server>
    $<BUILD_INTERFACE:target-raextcom-client>
    $<BUILD_INTERFACE:target-hcaextcom-client>
)

if(PKG_CONFIG_FOUND)
	set(LIBRARIES "-l${LIBRARY_FILE_NAME}")
	configure_file(${ARA_PHM_ROOT_DIR}/misc/pkgconfig/ara-phm.pc.in ${LIBRARY_NAME}.pc @ONLY)
    install(
		FILES ${CMAKE_CURRENT_BINARY_DIR}/${LIBRARY_NAME}.pc
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
    )
endif()

include(CMakePackageConfigHelpers)

configure_package_config_file(
    ${ARA_PHM_ROOT_DIR}/misc/cmake/ara-phm-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/${LIBRARY_NAME}-config.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${LIBRARY_NAME}
    NO_CHECK_REQUIRED_COMPONENTS_MACRO
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/${LIBRARY_NAME}-config-version.cmake
    COMPATIBILITY ExactVersion
)

install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/${LIBRARY_NAME}-config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/${LIBRARY_NAME}-config-version.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${LIBRARY_NAME}
)

set_target_properties(${TARGET_NAME} PROPERTIES
	OUTPUT_NAME ${LIBRARY_FILE_NAME}
	#PUBLIC_HEADER ${CMAKE_CURRENT_LIST_DIR}/include/ara/exec/execution_client.h
	EXPORT_NAME ${EXPORT_NAME}
)

install(
    DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    COMPONENT dev
    FILES_MATCHING PATTERN "*.h"
)

install(
	TARGETS ${TARGET_NAME}
	EXPORT ${TARGET_NAME}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/ara/phm
)

install(
	EXPORT ${TARGET_NAME}
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${LIBRARY_NAME}
    NAMESPACE ara::phm::
    FILE ara-phm-targets.cmake
)

if(ARA_ENABLE_TESTS)
	enable_testing()
	find_package(GTest REQUIRED CONFIG)
	# add_subdirectory(test)
endif(ARA_ENABLE_TESTS)
