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

set(TARGET_NAME ${ECU_STATE_COMM})

# Find all source files in the src directory
file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

# Set the library to be generated
add_library(${TARGET_NAME} OBJECT ${SRC_FILES})

target_link_libraries(${TARGET_NAME}
PUBLIC
    $<BUILD_INTERFACE:${ARA_GEN_BUILD_HELPER_LIBRARY}>
)
add_dependencies(${TARGET_NAME} ${ARA_GEN_BUILD_HELPER_LIBRARY})
