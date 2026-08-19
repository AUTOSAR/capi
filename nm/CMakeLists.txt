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
# @date       2022-06-02
# @author     junwei.he
# @version    1.2.0
#
# ================================================================

cmake_minimum_required(VERSION 3.10)
project(None) # placeholder only, will be updated to ARA_GEN_EXECUTABLE and its version in aragen-helper.cmake below


if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type")
endif()

if (${CMAKE_BUILD_TYPE} STREQUAL Debug)
    set(CMAKE_VERBOSE_MAKEFILE ON)
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DARA_NM_DEBUG=1")
else()
    set(CMAKE_VERBOSE_MAKEFILE OFF)
    set(CMAKE_EXPORT_COMPILE_COMMANDS OFF)
endif()

add_subdirectory(src)
