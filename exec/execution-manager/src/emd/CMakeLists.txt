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
# @date       2021-04-27
# @author     james.feng
# @version    1.2.0
#
# ================================================================

set(TARGET_NAME emd)

add_executable(${TARGET_NAME}
    src/ipc.cpp
    src/main.cpp
    src/process.cpp
    src/config_manager.cpp
    src/function_group.cpp
    src/execution_manager.cpp
    src/exec_trust_platform.cpp
)

target_compile_options(${TARGET_NAME} PUBLIC "-Wno-old-style-cast")

target_include_directories(${TARGET_NAME}
PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/include
)

target_link_libraries(${TARGET_NAME}
PRIVATE
    $<BUILD_INTERFACE:target-log>
    $<BUILD_INTERFACE:target-osi>
    $<BUILD_INTERFACE:target-rds>
    $<BUILD_INTERFACE:target-fps>
    $<BUILD_INTERFACE:target-config>
    $<BUILD_INTERFACE:target-ipc-server>
    $<BUILD_INTERFACE:target-ems-server>
    $<BUILD_INTERFACE:target-ums-server>
    $<BUILD_INTERFACE:target-sms-server>
    $<BUILD_INTERFACE:target-file-util>
    $<BUILD_INTERFACE:target-string-util>
    $<BUILD_INTERFACE:target-header-util>
    $<BUILD_INTERFACE:target-process-util>
    $<BUILD_INTERFACE:target-security-util>
    $<BUILD_INTERFACE:target-public-header>
)

install(
    TARGETS ${TARGET_NAME}
    RUNTIME DESTINATION ${CMAKE_INSTALL_SBINDIR}
)
