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

set(EXE_target ${ARA_GEN_EXECUTABLE})
set(EXE_src main.cpp)
add_executable(${EXE_target} ${EXE_src})

set_target_properties(${EXE_target}
PROPERTIES
    OUTPUT_NAME ${ARA_GEN_EXECUTABLE}
)

add_dependencies(${EXE_target}
    ${ARA_GEN_BUILD_HELPER_LIBRARY}
    target-idsm-config
    target-idsm-diag
    target-idsm-filter
    target-idsm-server
    target-idsm-transmit
    target-idsm-log
    target-idsm-event
    ara::idsm::idsm_client
    ara::idsm::ids_proto
)

find_package(ara-idsm-client REQUIRED)
find_package(ara-ids-proto REQUIRED)
find_package(ara-core REQUIRED)
find_package(isoft-ipc)
find_package(isoft-naicpp)
find_package(isoft-ara-fsh REQUIRED)
find_package(ara-core-types REQUIRED)

target_link_libraries(${EXE_target} 
    Threads::Threads
    ${ARA_GEN_BUILD_HELPER_LIBRARY}
    isoft::ipc
    isoft::ipccpp
    isoft::naicpp
    ara::core
    ara::core-types
    $<BUILD_INTERFACE:target-idsm-config>
    $<BUILD_INTERFACE:target-idsm-diag>
    $<BUILD_INTERFACE:target-idsm-server>
    $<BUILD_INTERFACE:target-idsm-filter>
    $<BUILD_INTERFACE:target-idsm-transmit>
    $<BUILD_INTERFACE:target-idsm-log>
    $<BUILD_INTERFACE:target-idsm-event>
    ara::idsm::idsm_client
    ara::idsm::ids_proto
)

# isoft-ap unified handling of target
aragen_helper_register_target(TARGET ${EXE_target} TYPE EXE)

include_directories(${PROJECT_CUR_ROOT_DIR}/src/)
include_directories(${PROJECT_CUR_ROOT_DIR}/../ids_proto/include/)
include_directories(${PROJECT_CUR_ROOT_DIR}/../idsm_client/include/)

add_subdirectory(config)
add_subdirectory(diag)
add_subdirectory(filter)
add_subdirectory(server)
add_subdirectory(transmit)
add_subdirectory(log)
add_subdirectory(event)