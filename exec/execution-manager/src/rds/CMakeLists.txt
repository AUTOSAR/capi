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
# @date       2023-11-20
# @author     james.feng
# @version    1.2.0
#
# ================================================================

set(TARGET_NAME target-rds)

add_library(${TARGET_NAME} OBJECT
    src/dcp.cpp
    src/debugger.cpp
)

target_include_directories(${TARGET_NAME}
PRIVATE
    $<BUILD_INTERFACE:${RapidJSON_INCLUDE_DIRS}>
PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
)

target_link_libraries(${TARGET_NAME}
PUBLIC
    nai
    ara::core-types
    $<BUILD_INTERFACE:target-osi>
PRIVATE
    $<BUILD_INTERFACE:target-header-util>
)

if (ARA_ENABLE_TESTS)
    target_compile_definitions(${TARGET_NAME} PUBLIC ARA_ENABLE_TESTS)
endif()
