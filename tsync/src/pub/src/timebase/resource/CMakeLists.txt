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
# @date       2023-01-09
# @author     fengjian
# @version    1.2.0
#
# ================================================================

set(TARGET_NAME "target-timebase-resource")

add_library(${TARGET_NAME} OBJECT
    tbcontext.cpp
    tbresmanager.cpp
    timecorrection.cpp
)

target_include_directories(${TARGET_NAME}
PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../../../../tsync_client/src/include/public>
)

get_target_property(common_libs target-common LINK_LIBRARIES)
get_target_property(osi_ipc_libs target-osi-ipc LINK_LIBRARIES)
get_target_property(config_libs target-config LINK_LIBRARIES)
get_target_property(timedomain_libs target-timedomain LINK_LIBRARIES)
set(all_libs ${osi_ipc_libs} ${common_libs} ${config_libs} ${timedomain_libs})
list(REMOVE_DUPLICATES all_libs)

target_link_libraries(${TARGET_NAME}
PUBLIC
    pthread
    isoft::ara_fsh
    ara::core-types
    ${all_libs}
    $<BUILD_INTERFACE:target-common>
    $<BUILD_INTERFACE:target-config>
    $<BUILD_INTERFACE:target-timedomain>
    
    $<BUILD_INTERFACE:target-osi-ipc>
)

