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
# @date       2022-05-10
# @author     james.feng
# @version    1.2.0
#
# ================================================================
#
# ############################ SMS Client ##############################
#
# ================================================================

set(TARGET_SMS_CLIENT "target-sms-client")

add_library(${TARGET_SMS_CLIENT} OBJECT
    src/client.cpp
)

target_include_directories(${TARGET_SMS_CLIENT}
PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
)

target_link_libraries(${TARGET_SMS_CLIENT}
PRIVATE
    $<BUILD_INTERFACE:target-log>
    $<BUILD_INTERFACE:target-config>
    $<BUILD_INTERFACE:target-public-header>
PUBLIC
    $<BUILD_INTERFACE:target-ipc-client>
)

############################# SMS Server ##############################
set(TARGET_SMS_SERVER "target-sms-server")

add_library(${TARGET_SMS_SERVER} OBJECT
    src/server.cpp
)

target_include_directories(${TARGET_SMS_SERVER}
PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
)

target_link_libraries(${TARGET_SMS_SERVER}
PRIVATE
    $<BUILD_INTERFACE:target-log>
    $<BUILD_INTERFACE:target-config>
    $<BUILD_INTERFACE:target-public-header>
PUBLIC
    $<BUILD_INTERFACE:target-ipc-server>
)
