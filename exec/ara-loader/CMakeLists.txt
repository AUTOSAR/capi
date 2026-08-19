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
# @author     james.feng
# @version    1.2.0
#
# ================================================================

cmake_minimum_required(VERSION 3.10)
project(ara-loader VERSION 0.1.0 LANGUAGES CXX)

include(FindPkgConfig)
find_package(RapidJSON REQUIRED)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

set(TARGET_NAME ara_loader)

add_executable(${TARGET_NAME}
    src/main.cpp
    src/file.cpp
    src/config.cpp
    src/manager.cpp
)

target_include_directories(${TARGET_NAME}
PRIVATE
    $<BUILD_INTERFACE:${RapidJSON_INCLUDE_DIRS}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
)

install(
    TARGETS ${TARGET_NAME}
    RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/../
)

IF (NOT DEFINED ARA_SDK_FULL_NAME)
    set(ARA_SDK_FULL_NAME generic_x84_x64)
ENDIF()

FILE(WRITE ${CMAKE_CURRENT_BINARY_DIR}/VERSION "${ARA_SDK_FULL_NAME}")

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/VERSION
    DESTINATION ${CMAKE_INSTALL_PREFIX}/../
    RENAME VERSION
)
