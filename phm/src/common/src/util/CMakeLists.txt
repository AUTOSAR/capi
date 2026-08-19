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
# @date       2024-10-11
# @author     fengjian
# @version    1.2.0
#
# ================================================================
#
# log
#
# ================================================================

set(LOG_TARGET_NAME "target-util-log")
add_library(${LOG_TARGET_NAME} OBJECT
    src/phm_log.cpp
)

target_include_directories(${LOG_TARGET_NAME}
PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
)

target_link_libraries(${LOG_TARGET_NAME}
PUBLIC
    ara::log
)

#timer
set(TIMER_TARGET_NAME "target-util-timer")
add_library(${TIMER_TARGET_NAME} OBJECT
    src/timer.cpp
)

target_include_directories(${TIMER_TARGET_NAME}
PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
)

add_dependencies(${TIMER_TARGET_NAME}
  ${LOG_TARGET_NAME}
)

target_link_libraries(${TIMER_TARGET_NAME}
PUBLIC
    ara::core-types
    isoft::naicpp
)

install(
    DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    COMPONENT dev
    FILES_MATCHING PATTERN "*.h"
)
