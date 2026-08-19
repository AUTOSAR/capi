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
# @date       2021-12-31
# @author     mazelin
# @version    1.2.0
#
# ================================================================
cmake_minimum_required(VERSION 3.12)
message(STATUS "======== Begin : ${PROJECT_NAME}.${LIBRARY_NAME} ========")

message(STATUS " ***  ARA_SYSROOT               = ${ARA_SYSROOT} ")
message(STATUS " ***  ARA_LOG_ROOT_DIR          = ${ARA_LOG_ROOT_DIR} ")
message(STATUS "  **  CMAKE_SOURCE_DIR          = ${CMAKE_SOURCE_DIR}")
message(STATUS "  **  PROJECT_SOURCE_DIR        = ${PROJECT_SOURCE_DIR} ")
message(STATUS "  **  CMAKE_CURRENT_SOURCE_DIR  = ${CMAKE_CURRENT_SOURCE_DIR} ")

include(../misc/Config.cmake)

find_package(isoft-cmake-common REQUIRED)

add_subdirectory(src)

if(ARA_ENABLE_EXAMPLES)
add_subdirectory(example)
endif()

