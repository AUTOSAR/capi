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
# @date       2022-01-01
# @author     james.feng
# @version    1.2.0
#
# ================================================================

set(TARGET_NAME "target-config")

add_library(${TARGET_NAME} OBJECT
    src/machine_manifest.cpp
    src/execution_manifest.cpp
    src/function_group_set.cpp
    src/software_cluster_manifest.cpp
)

target_include_directories(${TARGET_NAME}
PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
)

target_link_libraries(${TARGET_NAME}
PUBLIC
    isoft::ara_fsh
    ara::core-types
    isoft::manifestreader
    $<BUILD_INTERFACE:target-osi>
    $<BUILD_INTERFACE:target-file-util>
    $<BUILD_INTERFACE:target-string-util>
    $<BUILD_INTERFACE:target-security-util>
PRIVATE
    $<BUILD_INTERFACE:target-log>
)
