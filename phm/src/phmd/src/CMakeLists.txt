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
# @date       2021-07-19
# @author     jiawei
# @version    1.2.0
#
# ================================================================

cmake_minimum_required(VERSION 3.10)
project(None)

include(${ARA_PHM_ROOT_DIR}/src/phmd/gen/aragen-helper.cmake)
set(ARA_GEN_ARXMLS ${ARA_GEN_ARXMLS} 
        ${ARA_ARXMLS_DIR}/common
        ${ARA_ARXMLS_DIR}/development_machines/system
        ${ARA_ARXMLS_DIR}/development_machines/machine1/common
        ${ARA_ARXMLS_DIR}/development_machines/machine2/common

        ${ARA_ARXMLS_DIR}/platform_components/phm
        ${ARA_ARXMLS_DIR}/development_machines/machine1/processes/phmd 
        ${ARA_ARXMLS_DIR}/development_machines/machine2/processes/phmd

        #only for depends process staus and debug(to gen manifest)
        ${ARA_ARXMLS_DIR}/platform_components/com/daemon
        ${ARA_ARXMLS_DIR}/development_machines/machine1/processes/nsomeipd 
        ${ARA_ARXMLS_DIR}/development_machines/machine2/processes/nsomeipd 

        # startup depends on sm
        ${ARA_ARXMLS_DIR}/platform_components/sm/daemon/app_design.arxml 
        ${ARA_ARXMLS_DIR}/development_machines/machine1/processes/smd/sm/integration/app_deployment.arxml
        ${ARA_ARXMLS_DIR}/development_machines/machine2/processes/smd/sm/integration/app_deployment2.arxml 

        # dm
        ${ARA_ARXMLS_DIR}/platform_components/diag/daemon
        ${ARA_ARXMLS_DIR}/development_machines/machine1/processes/dmd
)
aragen_helper_add_build_helper_library()

include(FindPkgConfig)
include(CMakePackageConfigHelpers)

find_package(isoft-cmake-common REQUIRED)
find_package(isoft-manifestreader REQUIRED)
find_package(isoft-serialize REQUIRED)
find_package(nai REQUIRED)
find_package(isoft-naicpp REQUIRED)
find_package(isoft-ipc REQUIRED)
find_package(isoft-ara-fsh REQUIRED)
find_package(ara-core REQUIRED)
find_package(ara-log REQUIRED)
find_package(ara-exec-execution-client REQUIRED)
find_package(ara-sm-fg-state-notify-client REQUIRED)
find_package(ara-sm-internal-utils REQUIRED)

include(${ARA_PHM_ROOT_DIR}/src/phmd/misc/config.cmake)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

if(NOT TARGET target-commoncom-client)
    add_subdirectory(${ARA_PHM_ROOT_DIR}/src/common ${CMAKE_BINARY_DIR}/src/common)
endif()

add_subdirectory(config_manager)
add_subdirectory(watchdog)
add_subdirectory(healthchannel_manager)
add_subdirectory(supervision_manager)

# Compile phm_context.cpp into an object file
set(TARGET_OBJECT_NAME "target-phmcontext")

add_library(${TARGET_OBJECT_NAME} OBJECT
    phm_context.cpp
)

target_include_directories(${TARGET_OBJECT_NAME}
PRIVATE
    $<BUILD_INTERFACE:${ARA_PHM_ROOT_DIR}/src/common/src/util/include>
    $<BUILD_INTERFACE:${ARA_PHM_ROOT_DIR}/src/common/src/base/include>
    $<BUILD_INTERFACE:${ARA_PHM_ROOT_DIR}/src/common/src/svcom/include>
    $<BUILD_INTERFACE:${ARA_PHM_ROOT_DIR}/src/common/src/hccom/include>
    $<BUILD_INTERFACE:${ARA_PHM_ROOT_DIR}/src/common/src/racom/include>
    $<BUILD_INTERFACE:${ARA_PHM_ROOT_DIR}/src/common/src/raextcom/include>
    $<BUILD_INTERFACE:${ARA_PHM_ROOT_DIR}/src/common/src/hcacom/include>
    $<BUILD_INTERFACE:${ARA_PHM_ROOT_DIR}/src/common/src/hcaextcom/include>
    $<BUILD_INTERFACE:${ARA_PHM_ROOT_DIR}/src/phm_client/include>
    $<BUILD_INTERFACE:${ARA_PHM_ROOT_DIR}/src/recovery_action/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/config_manager/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/watchdog/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/healthchannel_manager/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/supervision_manager/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
)

target_link_libraries(${TARGET_OBJECT_NAME}
PRIVATE
    ara::core-types
    isoft::naicpp
    isoft::ipccpp
    isoft::ara_fsh
    isoft::manifestreader
)

# Compile phmd executable
set(TARGET_EXECUTABLE_NAME "phmd")

add_executable(${TARGET_EXECUTABLE_NAME} main.cpp)

target_include_directories(${TARGET_EXECUTABLE_NAME}
PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/>
    $<BUILD_INTERFACE:${ARA_PHM_ROOT_DIR}/src/common/src/util/include>
    $<BUILD_INTERFACE:${ARA_PHM_ROOT_DIR}/src/common/src/base/include>
    $<BUILD_INTERFACE:${ARA_PHM_ROOT_DIR}/src/common/src/svcom/include>
    $<BUILD_INTERFACE:${ARA_PHM_ROOT_DIR}/src/common/src/hccom/include>
    $<BUILD_INTERFACE:${ARA_PHM_ROOT_DIR}/src/common/src/racom/include>
    $<BUILD_INTERFACE:${ARA_PHM_ROOT_DIR}/src/common/src/raextcom/include>
    $<BUILD_INTERFACE:${ARA_PHM_ROOT_DIR}/src/common/src/hcacom/include>
    $<BUILD_INTERFACE:${ARA_PHM_ROOT_DIR}/src/common/src/hcaextcom/include>
    $<BUILD_INTERFACE:${ARA_PHM_ROOT_DIR}/src/phm_client/include>
    $<BUILD_INTERFACE:${ARA_PHM_ROOT_DIR}/src/recovery_action/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/config_manager/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/watchdog/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/healthchannel_manager/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/supervision_manager/include>
)

target_link_libraries(${TARGET_EXECUTABLE_NAME}
PRIVATE
    ara::log
    ara::core-types
    ara::core
    ara::exec::execution_client
    ara::exec::find_process_client
    isoft::manifestreader
    ${IAM_LIB}
    ara::sm::fg_state_notify_client
    $<BUILD_INTERFACE:target-util-log>
    $<BUILD_INTERFACE:target-util-timer>
    $<BUILD_INTERFACE:target-phmcontext>
    $<BUILD_INTERFACE:target-configmanager>
    $<BUILD_INTERFACE:target-watchdog>
    $<BUILD_INTERFACE:target-healthchannel-manager>
    $<BUILD_INTERFACE:target-supervision-manager>
    $<BUILD_INTERFACE:target-commoncom-client>
    $<BUILD_INTERFACE:target-commoncom-server>
    $<BUILD_INTERFACE:target-svcom-server>
    $<BUILD_INTERFACE:target-hccom-server>
    $<BUILD_INTERFACE:target-racom-client>
    $<BUILD_INTERFACE:target-hcacom-client>
    $<BUILD_INTERFACE:target-raextcom-server>
    $<BUILD_INTERFACE:target-hcaextcom-server>
)

# TODO Why is it not necessary to write here to install
# install(
#     TARGETS ${TARGET_EXECUTABLE_NAME}
#     RUNTIME DESTINATION ${CMAKE_INSTALL_SBINDIR} 
# )

# TODO Is it necessary?
aragen_helper_register_target(TARGET ${TARGET_EXECUTABLE_NAME} TYPE EXE)