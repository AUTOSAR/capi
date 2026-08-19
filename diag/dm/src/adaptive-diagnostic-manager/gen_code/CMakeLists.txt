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
# @date       2026-03-02
# @author     xuhua
# @version    1.2.0
#
# ================================================================

cmake_minimum_required(VERSION 3.14)

set(TARGET_NAME gen_code_service_obj)

find_package(ara-core-types REQUIRED)

# -------------------------------- service gen code --------------------------------
file(GLOB_RECURSE SRC_SERVICE_AGENT_LIST CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*/serviceAgent/*.cpp")

# ===================== Compile service as object target =====================
add_library(gen_code_service_obj OBJECT
    ${SRC_SERVICE_AGENT_LIST}
)

# server's own header file directory
target_include_directories(gen_code_service_obj
    PRIVATE 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../>
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/../../../common/include>
)

target_link_libraries(gen_code_service_obj
    PUBLIC
        ara::core-types
        ${DIAG_COMMON_LIB}
)

# Export server_obj target
set_target_properties(gen_code_service_obj PROPERTIES EXPORT_NAME server_obj)
