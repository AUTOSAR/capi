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
# @brief      Utils common module - shared by log_lib and logd
# @details
# @date       2026-03-23
# @author     yang
# @version    1.2.0
#
# ================================================================
#
# 1. Set a name for TARGET_NAME
#
# ================================================================

set(TARGET_NAME "ara-log-utils")

message(STATUS "======== Begin : ${PROJECT_NAME}.${TARGET_NAME} ========")
message(STATUS "  **  CMAKE_CURRENT_SOURCE_DIR  = ${CMAKE_CURRENT_SOURCE_DIR}")

# 2. Add the list of files to be compiled
file(GLOB_RECURSE SRC_PH_LIST CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
message(STATUS "<<<<< SRC_DIR = ${CMAKE_CURRENT_SOURCE_DIR}, SRC_FILE_LIST: ")

foreach(src_file IN LISTS SRC_PH_LIST)
    message(STATUS "  **  srcFile = ${src_file}")
endforeach()

message(STATUS ">>>>>")

# 3. Add src to TARGET_NAME
add_library(${TARGET_NAME} OBJECT ${SRC_PH_LIST})


# 4. Add include to TARGET_NAME
target_include_directories(${TARGET_NAME}
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../..>
)

# 5. Set C++ standard (inherit project settings or specify explicitly)
set_target_properties(${TARGET_NAME} PROPERTIES

    # CXX_STANDARD 14             # Configuration in doubt, CXX configuration should be unified with ap-all
    # CXX_STANDARD_REQUIRED ON    # Configuration in doubt, CXX configuration should be unified with ap-all
    POSITION_INDEPENDENT_CODE ON # Support compilation into shared library
)

# Create alias for ease of use
add_library(ara::log-utils ALIAS ${TARGET_NAME})
message(STATUS "======== End   : ${PROJECT_NAME}.${TARGET_NAME} ========")