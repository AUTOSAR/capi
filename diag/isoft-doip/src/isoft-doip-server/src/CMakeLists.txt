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
# @date       2024-10-12
# @author     kai.ju
# @version    1.2.0
#
# ================================================================
#
# server library
#
# ================================================================

set(TARGET_NAME server)

set(LIBRARY_NAME isoft-doip-${TARGET_NAME})
set(LIBRARY_FILE_NAME isoft_doip_${TARGET_NAME})
set(EXPORT_NAME ${TARGET_NAME})

find_package(nai REQUIRED)
find_package(isoft-naicpp REQUIRED)
find_package(ara-core-types REQUIRED)
find_package(ara-log REQUIRED)

if (BUILD_WITH_SSL)
    find_package(OpenSSL REQUIRED)
    set(tls_libs OpenSSL::SSL OpenSSL::Crypto)
endif()

aux_source_directory(./ SERVER_SRC_LIST)

if(ARA_WITH_DIAG_DOIP_BUILD_SHARED_LIBS)
    add_library(${TARGET_NAME} SHARED ${SERVER_SRC_LIST})
else()
    add_library(${TARGET_NAME} STATIC ${SERVER_SRC_LIST})
endif()

target_include_directories(${TARGET_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(${TARGET_NAME}
    PUBLIC
        nai
        isoft::naicpp
        ara::core-types
        ara::log
        ${tls_libs}
        isoft::doip::common
)

set_target_properties(${TARGET_NAME}
    PROPERTIES
        SOVERSION ${PROJECT_VERSION_MAJOR}
        VERSION ${PROJECT_VERSION}
        OUTPUT_NAME isoft_doip_${TARGET_NAME}
)

set(INSTALL_CONFIG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../misc)
# General install
include(FindPkgConfig)
if(PKG_CONFIG_FOUND)
    configure_file(${INSTALL_CONFIG_DIR}/${LIBRARY_NAME}.pc.in
        ${LIBRARY_NAME}.pc @ONLY)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${LIBRARY_NAME}.pc DESTINATION
        ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
endif()

# Create *-config.cmake file
include(CMakePackageConfigHelpers)
configure_package_config_file(
    ${INSTALL_CONFIG_DIR}/${LIBRARY_NAME}-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/${LIBRARY_NAME}-config.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${LIBRARY_NAME}
    NO_SET_AND_CHECK_MACRO
)
# Create *-config-version.cmake file
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/${LIBRARY_NAME}-config-version.cmake
    VERSION ${MYLIB_VERSION}
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
    NAMESPACE isoft::doip::
)

################################################################################
# Install header files
install(
    DIRECTORY ${PROJECT_SOURCE_DIR}/src/isoft-doip-server/include/isoft
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)