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
# @date       2024-12-30
# @author     jiangzhiyuan
# @version    1.2.0
#
#
# ================================================================

set(ARA_GEN_EXECUTABLE fwd)
set(EXE_src main.cpp)
set(EXE_target ${ARA_GEN_EXECUTABLE})

message(STATUS "======== Begin : ${PROJECT_NAME}.${EXE_target} ========")
add_executable(${EXE_target} ${EXE_src})
set_target_properties(${EXE_target}
    PROPERTIES
        OUTPUT_NAME ${ARA_GEN_EXECUTABLE}
    )

find_package(isoft-ipc REQUIRED)
find_package(isoft-naicpp REQUIRED)
find_package(ara-exec-execution-client REQUIRED)
find_package(ara-fw-engine REQUIRED)
find_package(ara-fw-client REQUIRED)
find_package(ara-idsm-client REQUIRED)

include_directories(${PROJECT_SOURCE_DIR}/../libfwengine/include)
include_directories(${PROJECT_SOURCE_DIR}/../libfwclient/include)
include_directories(${PROJECT_SOURCE_DIR}/../fwd/src/)

target_link_libraries(${EXE_target} 
    isoft::ipccpp
    isoft::naicpp
    ara::exec::execution_client
    ara::fw::fw_engine
    ara::fw::fw_client
    ara::idsm::idsm_client
    $<BUILD_INTERFACE:target-fw-config>
    $<BUILD_INTERFACE:target-fw-filter>
    $<BUILD_INTERFACE:target-fw-filterchain>
    $<BUILD_INTERFACE:target-fw-ipc>
    $<BUILD_INTERFACE:target-fw-manager>
    $<BUILD_INTERFACE:target-fw-logmonitor>
   ${ARA_GEN_BUILD_HELPER_LIBRARY}
)


add_dependencies(${EXE_target}
    ${ARA_GEN_BUILD_HELPER_LIBRARY}
    target-fw-config
    target-fw-filter
    target-fw-ipc
    target-fw-filterchain
    target-fw-manager
    target-fw-logmonitor
    ara::fw::fw_engine
    ara::fw::fw_client
)

# Unified processing of target by isoft-ap
aragen_helper_register_target(TARGET ${EXE_target} TYPE EXE)

# Add user target (exe) and tests
add_subdirectory(config)
add_subdirectory(filter)
add_subdirectory(filterchain)
add_subdirectory(ipc)
add_subdirectory(fwmanager)
add_subdirectory(logsync)

message(STATUS "======== End   : ${PROJECT_NAME}.${EXE_target} ========")
