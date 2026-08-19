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
# @date       2026-02-12
# @author     longxiao.liang
# @version    1.2.0
#
# ================================================================

set(TARGET_NAME ${ARA_GEN_EXECUTABLE})
add_executable(${TARGET_NAME} main.cpp)

if(ARA_WITH_DIAG)
    add_subdirectory(${DM_COMM})
endif()

if(ARA_WITH_NM)
    add_subdirectory(${NM_COMM})
endif()

if(ARA_WITH_UCM)
    add_subdirectory(${UCM_COMM})
endif()

if(ARA_WITH_SM_COMPILE_PHM)
    add_subdirectory(${PHM_COMM})
endif()

add_subdirectory(${CONFIG})
add_subdirectory(${FG_STATE_COMM})
add_subdirectory(${SM_STATE_COMM})
add_subdirectory(${SHELL_COMM})
add_subdirectory(${EM_COMM})
add_subdirectory(${EVENT_MANAGEMENT})
add_subdirectory(${FG_STATE_NOTIFY_COMM})
add_subdirectory(${STATE_MACHINE_MANAGEMENT})

set_target_properties(${TARGET_NAME} PROPERTIES OUTPUT_NAME ${ARA_GEN_EXECUTABLE})

target_link_libraries(${TARGET_NAME}
PUBLIC
    $<BUILD_INTERFACE:${ARA_GEN_BUILD_HELPER_LIBRARY}>
    $<BUILD_INTERFACE:${EVENT_MANAGEMENT}>
    $<BUILD_INTERFACE:${CONFIG}>
    $<BUILD_INTERFACE:${EM_COMM}>
    $<BUILD_INTERFACE:${FG_STATE_COMM}>
    $<BUILD_INTERFACE:${SHELL_COMM}>
    $<BUILD_INTERFACE:${SM_STATE_COMM}>
    $<BUILD_INTERFACE:${FG_STATE_NOTIFY_COMM}>
    $<BUILD_INTERFACE:${STATE_MACHINE_MANAGEMENT}>
)

if(ARA_WITH_DIAG)
    target_link_libraries(${TARGET_NAME} PUBLIC $<BUILD_INTERFACE:${DM_COMM}>)
    add_dependencies(${TARGET_NAME} ${DM_COMM})
endif()

if(ARA_WITH_SM_COMPILE_PHM)
    target_link_libraries(${TARGET_NAME} PUBLIC $<BUILD_INTERFACE:${PHM_COMM}>)
    add_dependencies(${TARGET_NAME} ${PHM_COMM})
endif()

if(ARA_WITH_UCM)
    target_link_libraries(${TARGET_NAME} PUBLIC $<BUILD_INTERFACE:${UCM_COMM}>)
    add_dependencies(${TARGET_NAME} ${UCM_COMM})
endif()

if(ARA_WITH_NM)
    target_link_libraries(${TARGET_NAME} PUBLIC $<BUILD_INTERFACE:${NM_COMM}>)
    add_dependencies(${TARGET_NAME} ${NM_COMM})
endif()

if(ARA_WITH_PER)
    add_subdirectory(${STORAGE})
    target_link_libraries(${TARGET_NAME} PUBLIC $<BUILD_INTERFACE:${STORAGE}>)
endif()

if(ARA_ENABLE_SM_ECU_STATE)
    add_subdirectory(${ECU_STATE_COMM})
    target_link_libraries(${TARGET_NAME} PUBLIC $<BUILD_INTERFACE:${ECU_STATE_COMM}>)
endif()

add_dependencies(${TARGET_NAME} ${ARA_GEN_BUILD_HELPER_LIBRARY})
add_dependencies(${TARGET_NAME} ${EVENT_MANAGEMENT})

# isoft-ap unified handling of target
aragen_helper_register_target(TARGET ${TARGET_NAME} TYPE EXE)