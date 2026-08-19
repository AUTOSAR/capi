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
# @date       2021-04-27
# @author     jiawei
# @version    1.2.0
#
# ================================================================

cmake_minimum_required(VERSION 3.4)
project(isoft-cmake-common VERSION 1.1.0 LANGUAGES NONE) # project version,not sdk version


include(src/${PROJECT_NAME}.cmake) # test

message(STATUS "Preferred Build Type:     ${CMAKE_BUILD_TYPE}")
message(STATUS "CXX_EXTENSIONS:           ${CMAKE_CXX_EXTENSIONS}")
message(STATUS "CXX_STANDARD:             ${CMAKE_CXX_STANDARD}")

include(GNUInstallDirs)


# Create *-config.cmake file in a relocatable-friendly way.
include(CMakePackageConfigHelpers)

set(INSTALL_DEST "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")

configure_package_config_file(
    ${PROJECT_SOURCE_DIR}/src/${PROJECT_NAME}-config.cmake.in
    ${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config.cmake
    INSTALL_DESTINATION ${INSTALL_DEST}
    NO_CHECK_REQUIRED_COMPONENTS_MACRO
)

install(FILES ${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config.cmake
	DESTINATION ${INSTALL_DEST}
)

install(FILES src/${PROJECT_NAME}.cmake
        DESTINATION ${INSTALL_DEST}
)
