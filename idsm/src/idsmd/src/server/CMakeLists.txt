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
# @date       2026-01-26
# @author     niuliming
# @version    1.2.0
#
# ================================================================

set(TARGET_NAME "target-idsm-server")
message(STATUS "${PROJECT_NAME}.${TARGET_NAME}: CMAKE DIR ${CMAKE_CURRENT_SOURCE_DIR}")
file(GLOB_RECURSE SRC_LIST CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
add_library(${TARGET_NAME} OBJECT
    ${SRC_LIST}
)

# IDSM decouples IAM
if(ARA_WITH_IAM)
    add_definitions(-DARA_WITH_IAM)
    find_package(ara-iam REQUIRED)
    target_link_libraries(${TARGET_NAME} 
    PRIVATE 
        ara::iam
    )
    message(STATUS "ISDM Daemon Compile with macro -DARA_WITH_IAM")
endif(ARA_WITH_IAM)

# IDSM decouples TSYNC
if (ARA_WITH_TSYNC) 
    add_definitions(-DARA_WITH_TSYNC)
    find_package(ara-tsync REQUIRED)
    target_link_libraries(${TARGET_NAME} 
    PRIVATE 
        ara::tsync
    )
    message(STATUS "ISDM Daemon Compile with macro -DARA_WITH_TSYNC")
endif(ARA_WITH_TSYNC)

# IDSM decouples SM
if (ARA_WITH_SM) 
    add_definitions(-DARA_WITH_SM)
    find_package(ara-sm-fg-state-notify-client REQUIRED)
    find_package(ara-sm-internal-utils REQUIRED)
    target_link_libraries(${TARGET_NAME}
    PRIVATE
        ara::sm::fg_state_notify_client
    )
    message(STATUS "ISDM Daemon Compile with macro -DARA_WITH_SM")
endif(ARA_WITH_SM)

add_dependencies(${TARGET_NAME}
    target-idsm-config
    target-idsm-event
    target-idsm-log
)

target_link_libraries(${TARGET_NAME}
PUBLIC
    ara::core-types
    $<BUILD_INTERFACE:target-idsm-config>
)

target_include_directories(${TARGET_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)
