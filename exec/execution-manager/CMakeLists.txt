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
project(execution-manager VERSION 0.1.0 LANGUAGES CXX)

include(FindPkgConfig)
include(CMakePackageConfigHelpers)

find_package(nai REQUIRED)
find_package(ara-log REQUIRED)
find_package(isoft-ipc REQUIRED)
find_package(RapidJSON REQUIRED)
find_package(isoft-naicpp REQUIRED)
find_package(isoft-ipcbase REQUIRED)
find_package(isoft-ara-fsh REQUIRED)
find_package(ara-core-types REQUIRED)
find_package(isoft-cmake-common REQUIRED)
find_package(isoft-manifestreader REQUIRED)

if (${CMAKE_BUILD_TYPE} STREQUAL Debug)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DARA_EXEC_DEBUG=1")
endif()

add_subdirectory(src)

option(ARA_ENABLE_SAMPLES "enable ara samples" OFF)
if (ARA_ENABLE_SAMPLES)
#    add_subdirectory(example)
endif()

option(ARA_ENABLE_TESTS "enable ara tests" OFF)
if (ARA_ENABLE_TESTS)
#    add_subdirectory(test) 
endif()
