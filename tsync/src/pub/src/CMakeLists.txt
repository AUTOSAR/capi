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
# @date       2026-03-03
# @author     hejunwei
# @version    1.2.0
#
# ================================================================

cmake_minimum_required(VERSION 3.10)
set(TARGET_NAME "target-ara-tsync-pub")

find_package(isoft-common REQUIRED)
find_package(nai REQUIRED)
find_package(isoft-naicpp REQUIRED)
find_package(isoft-ipc REQUIRED)
find_package(isoft-ipcbase REQUIRED)
find_package(isoft-ara-fsh REQUIRED)
find_package(isoft-manifestreader REQUIRED)
find_package(ara-core-types REQUIRED)

add_subdirectory(common)
add_subdirectory(log)
add_subdirectory(config)
add_subdirectory(ipc)
add_subdirectory(timedomain)
add_subdirectory(timebase)

get_target_property(common_libs target-common LINK_LIBRARIES)
get_target_property(log_libs target-log LINK_LIBRARIES)
get_target_property(timedomain_libs target-timedomain LINK_LIBRARIES)
get_target_property(config_libs target-config LINK_LIBRARIES)
get_target_property(timebase_libs target-timebase LINK_LIBRARIES)
get_target_property(osiipc_libs target-osi-ipc LINK_LIBRARIES)
set(all_libs ${log_libs} ${osiipc_libs} ${timebase_libs} ${timedomain_libs} ${common_libs} ${config_libs})
list(REMOVE_DUPLICATES all_libs)
add_library(${TARGET_NAME} STATIC
    $<TARGET_OBJECTS:target-common>
    $<TARGET_OBJECTS:target-log>
    $<TARGET_OBJECTS:target-timebase>
    $<TARGET_OBJECTS:target-config>
    $<TARGET_OBJECTS:target-timedomain>
    $<TARGET_OBJECTS:target-osi-ipc>

)


target_link_libraries(${TARGET_NAME} 
PUBLIC
    isoft::naicpp
    ${all_libs}
    $<BUILD_INTERFACE:target-common>
    $<BUILD_INTERFACE:target-log>
    $<BUILD_INTERFACE:target-timebase>
    $<BUILD_INTERFACE:target-config>
    $<BUILD_INTERFACE:target-timedomain>
    $<BUILD_INTERFACE:target-osi-ipc>
)

# Set C++ standard (inherit project settings or explicitly specify)
set_target_properties(${TARGET_NAME} PROPERTIES

    # CXX_STANDARD 14             # Questionable configuration: CXX configuration should be unified with ap-all
    # CXX_STANDARD_REQUIRED ON    # Questionable configuration: CXX configuration should be unified with ap-all
    POSITION_INDEPENDENT_CODE ON # Support compiling into shared library
)

# Create alias for easy use
add_library(ara::tsync-pub ALIAS ${TARGET_NAME})