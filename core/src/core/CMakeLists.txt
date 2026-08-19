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
# @date       2021-04-14
# @author     jiawei
# @version    1.2.0
#
# ================================================================

cmake_minimum_required(VERSION 3.8)

project(ara-core VERSION 1.1.0 LANGUAGES CXX) # project version,not sdk version


include(FindPkgConfig)
include(CMakePackageConfigHelpers)

find_package(isoft-cmake-common REQUIRED)
include(GNUInstallDirs)

find_package(ara-core-types REQUIRED)
find_package(isoft-naicpp REQUIRED)
find_package(isoft-ara-fsh REQUIRED)
find_package(isoft-ipcbase REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DARA_LOG_WITHOUT_MODEL_MESSAGE=1")


if(ARA_ENABLE_AP_ENV)
    add_definitions(-DCOMPILE_WITH_AP_ENV)
endif()

# if (${CMAKE_BUILD_TYPE} STREQUAL Debug)
#     set(CMAKE_VERBOSE_MAKEFILE ON)
#     set(CMAKE_EXPORT_COMPILE_COMMANDS YES)
#     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DARA_CORE_DEBUG=1")
#     set(ARA_ENABLE_TESTS TRUE)
#     enable_testing()
#     find_package(GTest REQUIRED CONFIG)
# else()
#     set(CMAKE_VERBOSE_MAKEFILE OFF)
#     set(CMAKE_EXPORT_COMPILE_COMMANDS NO)
#     set(ARA_ENABLE_TESTS FALSE)
# endif()

add_subdirectory(src)

if(ARA_ENABLE_TESTS)
    enable_testing()
    find_package(GTest REQUIRED CONFIG)
    add_subdirectory(test)
endif()
