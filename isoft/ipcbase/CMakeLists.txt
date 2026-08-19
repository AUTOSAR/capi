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
# @date       2021-12-01
# @author     fengjian
# @version    1.2.0
#
# ================================================================

cmake_minimum_required(VERSION 3.10)
project(ara-exec VERSION 0.1.0 LANGUAGES CXX C)
# Initialize compilation options, and define public CMake functions and variables (for AP platform)
find_package(isoft-cmake-common REQUIRED) 

include(FindPkgConfig)
include(CMakePackageConfigHelpers)

find_package(nai REQUIRED)
find_package(isoft-ara-fsh REQUIRED)

set(ISOFT_ARA_IPCBASE_DEBUG TRUE)
if (ISOFT_ARA_IPCBASE_DEBUG)
    SET(CMAKE_VERBOSE_MAKEFILE ON)
    SET(CMAKE_EXPORT_COMPILE_COMMANDS YES)
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DISOFT_ARA_IPCBASE_DEBUG=1")
endif()

add_subdirectory(src)
