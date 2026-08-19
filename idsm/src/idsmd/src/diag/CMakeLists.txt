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
# @date       2022-12-27
# @author     zhaoyunfei
# @version    1.2.0
#
# ================================================================

set(TARGET_NAME "target-idsm-diag")
message(STATUS "${PROJECT_NAME}.${TARGET_NAME}: CMAKE DIR ${CMAKE_CURRENT_SOURCE_DIR}")
include(${PROJECT_CUR_ROOT_DIR}/idsm-decouple-diag-cmake.in)
file(GLOB_RECURSE SRC_LIST CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
add_library(${TARGET_NAME} OBJECT
    ${SRC_LIST}
)

add_dependencies(${TARGET_NAME}
    target-idsm-log
    target-idsm-event
)

target_link_libraries(${TARGET_NAME}
PUBLIC
    ara::core-types
    $<BUILD_INTERFACE:target-idsm-log>
    $<BUILD_INTERFACE:target-idsm-event>
)

if (ARA_WITH_DIAG) 
target_link_libraries(${TARGET_NAME}
    PRIVATE
    ara::diag
)
endif(ARA_WITH_DIAG)

target_include_directories(${TARGET_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)
