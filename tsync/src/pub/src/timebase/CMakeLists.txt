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

set(TARGET_NAME "target-timebase")

add_subdirectory(resource)
add_subdirectory(proxy)

if (ARA_TSYNC_WITH_PER)
    add_library(${TARGET_NAME} OBJECT
        timekvstorage.cpp
        timefilestorage.cpp
        tbmanager.cpp
    )
else()
    add_library(${TARGET_NAME} OBJECT
        tbmanager.cpp
    )
endif()

target_include_directories(${TARGET_NAME}
PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../../../tsync_client/src/include/public>
)

get_target_property(common_libs target-common LINK_LIBRARIES)
get_target_property(log_libs target-log LINK_LIBRARIES)
get_target_property(config_libs target-config LINK_LIBRARIES)
get_target_property(timedomain_libs target-timedomain LINK_LIBRARIES)
get_target_property(timebase_resource_libs target-timebase-resource LINK_LIBRARIES)
get_target_property(timebase_proxy_libs target-timebase-proxy LINK_LIBRARIES)
set(all_libs ${common_libs} ${log_libs} ${config_libs} ${timedomain_libs} ${timebase_resource_libs} ${timebase_proxy_libs})
list(REMOVE_DUPLICATES all_libs)


if (ARA_TSYNC_WITH_PER)
    target_link_libraries(${TARGET_NAME}
    PUBLIC
        ara::core-types
        ara::per
        ${all_libs}
        $<BUILD_INTERFACE:target-common>
        $<BUILD_INTERFACE:target-log>
        $<BUILD_INTERFACE:target-config>
        $<BUILD_INTERFACE:target-timedomain>
        $<BUILD_INTERFACE:target-timebase-resource>
        $<BUILD_INTERFACE:target-timebase-proxy>
    )
else()
    target_link_libraries(${TARGET_NAME}
    PUBLIC
        ara::core-types
        ${all_libs}
        $<BUILD_INTERFACE:target-common>
        $<BUILD_INTERFACE:target-log>
        $<BUILD_INTERFACE:target-config>
        $<BUILD_INTERFACE:target-timedomain>
        $<BUILD_INTERFACE:target-timebase-resource>
        $<BUILD_INTERFACE:target-timebase-proxy>
    )
endif()


add_dependencies(${TARGET_NAME}
    target-common
    target-log
    target-config
    target-timedomain
    target-timebase-resource
    target-timebase-proxy
)


