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
# @date       2021-10-26
# @author     fengjian
# @version    1.2.0
#
# ================================================================

project(ara-tsync-daemon VERSION 0.1.0 LANGUAGES CXX)

include(FindPkgConfig)
include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

if (NOT DEFINED CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
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
# When compiling independently, need to add the pub subdirectory
if(NOT pub_add)
    message(STATUS "Standalone build: Adding pub subdirectory")
    add_subdirectory(../pub ${CMAKE_CURRENT_BINARY_DIR}/pub)
    set(pub_add TRUE)
endif()
add_subdirectory(src)
if(ARA_ENABLE_TESTS)
	enable_testing()
	find_package(GTest REQUIRED CONFIG)
	include(FindPkgConfig)
    find_package(PkgConfig REQUIRED)
    PKG_SEARCH_MODULE(PKG_GMOCK REQUIRED gmock)
    include_directories($ENV{PKG_CONFIG_SYSROOT_DIR}/${PKG_GMOCK_INCLUDEDIR})
    link_directories($ENV{PKG_CONFIG_SYSROOT_DIR}/${PKG_GMOCK_LIBDIR})
    add_subdirectory(test)
endif()