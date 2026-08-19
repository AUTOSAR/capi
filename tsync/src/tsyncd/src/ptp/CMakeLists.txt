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
# @date       2021-10-26
# @author     fengjian
# @version    1.2.0
#
# ================================================================

set(TARGET_NAME "target-ptp")

add_library(${TARGET_NAME} OBJECT
    networkmanager.cpp
    network.cpp
    ptpmanager.cpp
   	followup.cpp
   	subtlvtimesecured.cpp
   	tlvautosar.cpp
)

target_compile_options(${TARGET_NAME} PUBLIC "-Wno-invalid-offsetof" "-Wno-old-style-cast")

target_include_directories(${TARGET_NAME}
PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
)

get_target_property(osi_libs target-osi-network LINK_LIBRARIES)
set(all_libs ${osi_libs})
list(REMOVE_DUPLICATES all_libs)
target_link_libraries(${TARGET_NAME}
PUBLIC
    nai
    ara::core-types
    ${all_libs}
    ara::tsync-pub
    $<BUILD_INTERFACE:target-osi-network>
)

add_dependencies( ${TARGET_NAME}
    target-osi-network
    ara::tsync-pub
)

