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
# @date       2026-01-30
# @author     xuhua
# @version    1.2.0
#
# ================================================================

cmake_minimum_required(VERSION 3.16)

set(TARGET_NAME diag-common)

set(LIBRARY_NAME ${TARGET_NAME})
set(LIBRARY_FILE_NAME diag_common)

find_package(ara-log REQUIRED)
find_package(nai REQUIRED)
find_package(nsomeip REQUIRED)
find_package(ara-core-types REQUIRED)

# common library
file(GLOB cancellation_src ./cancellation/*.cpp)
file(GLOB log_src ./log/*.cpp)
file(GLOB netProxy_src ./netProxy/*.cpp)
file(GLOB thread_pool_src ./thread_pool/*.cpp)

set(common_src
    ${cancellation_src}
    ${log_src}
    ${netProxy_src}
    ${thread_pool_src}
)

add_library(${TARGET_NAME} STATIC
    ${common_src})
add_library(ara::${TARGET_NAME} ALIAS ${TARGET_NAME})

target_include_directories(${TARGET_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/../dm/src/adaptive-diagnostic-library/src>
)

target_link_libraries(${TARGET_NAME}
    PUBLIC
        nsomeip
        isoft::manifestreader
        ara::core-types
)

set_target_properties(${TARGET_NAME}
    PROPERTIES
        OUTPUT_NAME ${LIBRARY_FILE_NAME}
)

# General install
include(FindPkgConfig)
if(PKG_CONFIG_FOUND)
    configure_file(${PROJECT_SOURCE_DIR}/misc/${LIBRARY_NAME}.pc.in ${LIBRARY_NAME}.pc @ONLY)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${LIBRARY_NAME}.pc DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
endif()

# Create *-config.cmake file
include(CMakePackageConfigHelpers)
configure_package_config_file(
    ${PROJECT_SOURCE_DIR}/misc/${LIBRARY_NAME}-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/${LIBRARY_NAME}-config.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${LIBRARY_NAME}
    NO_SET_AND_CHECK_MACRO
)

# Create *-config-version.cmake file
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/${LIBRARY_NAME}-config-version.cmake
    VERSION 1.1.0
    COMPATIBILITY ExactVersion
)

# Install *-config.cmake and *-version.cmake files
install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/${LIBRARY_NAME}-config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/${LIBRARY_NAME}-config-version.cmake
    DESTINATION
        ${CMAKE_INSTALL_LIBDIR}/cmake/${LIBRARY_NAME}
)

# Install exported targets, i.e. *-targets.cmake
install(
    TARGETS ${TARGET_NAME}
    EXPORT ${LIBRARY_NAME}-targets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(
    EXPORT ${LIBRARY_NAME}-targets
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${LIBRARY_NAME}
    NAMESPACE ara::
)

# Internal use, do not export header files
# Install header files
# install(
#     DIRECTORY ${PROJECT_SOURCE_DIR}/include/
#     DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
# )
