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
# @date       2023-01-09
# @author     fengjian
# @version    1.2.0
#
# ================================================================

cmake_minimum_required(VERSION 3.10)
project(none VERSION 0.1.0 LANGUAGES CXX)
include(FindPkgConfig)
include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

if (NOT DEFINED CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

option(ARA_WITH_PER "build with per" ON)
option(ARA_WITH_EXEC "build with exec" ON)
option(ARA_ENABLE_TESTS "Enable ara tests" ON)

find_package(isoft-cmake-common REQUIRED)
find_package(isoft-common REQUIRED)
find_package(nai REQUIRED)
find_package(isoft-naicpp REQUIRED)
find_package(isoft-ipc REQUIRED)
find_package(isoft-ipcbase REQUIRED)
find_package(isoft-ara-fsh REQUIRED)
find_package(isoft-manifestreader REQUIRED)
find_package(ara-core REQUIRED)
find_package(ara-core-types REQUIRED)
find_package(Eigen3 REQUIRED)
if (ARA_WITH_PER)
    find_package(ara-per REQUIRED)
endif()
if (ARA_WITH_EXEC)
    find_package(ara-exec-execution-client REQUIRED)
endif()

if (${CMAKE_BUILD_TYPE} STREQUAL Debug)
    set(CMAKE_VERBOSE_MAKEFILE ON)
    set(BUILD_SHARED_LIBS ON)
    set(CMAKE_EXPORT_COMPILE_COMMANDS YES)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DARA_TSYNC_DEBUG=1")
else()
    set(CMAKE_VERBOSE_MAKEFILE OFF)
    set(CMAKE_EXPORT_COMPILE_COMMANDS NO)
endif()

# Detach from EMD for independent testing
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DARA_TSYNC_DEBUG_WITHOUT_EM=1")
# Persistence trimming macro on daemon side, not needed for internal library

if (ARA_WITH_ASAN)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address  -fno-omit-frame-pointer")
endif()

set(ARA_LOG_USED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DARA_TSYNC_DAEMON_USE_ARA_LOG=1")

if (ARA_WITH_PER)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DARA_TSYNC_WITH_PER=1")
    set(ARA_TSYNC_WITH_PER TRUE)
endif()


if (ARA_WITH_EXEC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DARA_TSYNC_WITH_EXEC=1")
endif()


option(ARA_ENABLE_DOXYGEN "Enables the documentation target" OFF)

add_subdirectory(aralogadaptor)
add_subdirectory(osi)
add_subdirectory(ptp)
add_subdirectory(storage)
add_subdirectory(tsyncd)