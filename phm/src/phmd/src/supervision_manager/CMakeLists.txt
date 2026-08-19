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

set(TARGET_NAME "target-supervision-manager")

add_library(${TARGET_NAME} OBJECT
    src/supervision_checkpoint.cpp
    src/base_supervision.cpp
    src/alive_supervision.cpp
    src/deadline_supervision.cpp
    src/logical_supervision.cpp
    src/local_supervision.cpp
    src/global_supervision.cpp
    src/supervision_mode.cpp
    src/fg_supervision_mode.cpp
    src/supervision_factory.cpp
    src/supervision_controller.cpp
)

target_include_directories(${TARGET_NAME}
PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
    $<BUILD_INTERFACE:${ARA_PHM_ROOT_DIR}/src/common/src/util/include>
    $<BUILD_INTERFACE:${ARA_PHM_ROOT_DIR}/src/common/src/base/include>
    $<BUILD_INTERFACE:${ARA_PHM_ROOT_DIR}/src/common/src/svcom/include>
    $<BUILD_INTERFACE:${ARA_PHM_ROOT_DIR}/src/common/src/racom/include>
    $<BUILD_INTERFACE:${ARA_PHM_ROOT_DIR}/src/common/src/raextcom/include>
    $<BUILD_INTERFACE:${ARA_PHM_ROOT_DIR}/src/phm_client/include>
    $<BUILD_INTERFACE:${ARA_PHM_ROOT_DIR}/src/phmd/src/config_manager/include>
)

add_dependencies(${TARGET_NAME}
    target-util-log
    target-svcom-server
    target-racom-client
    target-raextcom-server
)

target_link_libraries(${TARGET_NAME}
PRIVATE
    ara::core-types
    isoft::ara_fsh
    $<BUILD_INTERFACE:target-util-log>
    $<BUILD_INTERFACE:target-svcom-server>
    $<BUILD_INTERFACE:target-racom-client>
    $<BUILD_INTERFACE:target-raextcom-server>
)
