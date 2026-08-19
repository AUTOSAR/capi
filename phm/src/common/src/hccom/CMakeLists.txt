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
# client
#
# ================================================================

set(CLIENT_TARGET_NAME "target-hccom-client")

add_library(${CLIENT_TARGET_NAME} OBJECT
    src/hc_client.cpp
    src/hc_message.cpp
)

target_include_directories(${CLIENT_TARGET_NAME}
PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
    $<BUILD_INTERFACE:${ARA_PHM_ROOT_DIR}/src/common/src/base/include>
)

add_dependencies(${CLIENT_TARGET_NAME}
    target-util-log
    target-commoncom-client
)

target_link_libraries(${CLIENT_TARGET_NAME}
PUBLIC
    isoft::ipccpp
    isoft::naicpp
    ara::core-types
    $<BUILD_INTERFACE:target-util-log>
    $<BUILD_INTERFACE:target-commoncom-client>
)

# server
set(SERVER_TARGET_NAME "target-hccom-server")

add_library(${SERVER_TARGET_NAME} OBJECT
    src/hc_server.cpp
    src/hc_message.cpp
)

target_include_directories(${SERVER_TARGET_NAME}
PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
    $<BUILD_INTERFACE:${ARA_PHM_ROOT_DIR}/src/common/src/base/include>
)

add_dependencies(${SERVER_TARGET_NAME}
    target-util-log
    target-commoncom-server
)

target_link_libraries(${SERVER_TARGET_NAME}
PUBLIC
    isoft::ipccpp
    isoft::naicpp
    ara::core-types
    $<BUILD_INTERFACE:target-util-log>
    $<BUILD_INTERFACE:target-commoncom-server>
)

install(
    DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    COMPONENT dev
    FILES_MATCHING PATTERN "*.h"
)
