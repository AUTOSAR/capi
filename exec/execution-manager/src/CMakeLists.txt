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
# @date       2022-11-02
# @author     james.feng
# @version    1.2.0
#
# ================================================================

add_subdirectory(osi)
add_subdirectory(log)
add_subdirectory(ipc)
add_subdirectory(utils)
add_subdirectory(config)

add_subdirectory(fps)
add_subdirectory(rds)
add_subdirectory(ems)
add_subdirectory(sms)
add_subdirectory(ums)
add_subdirectory(emd)

add_subdirectory(state-client)
add_subdirectory(update-client)
add_subdirectory(execution-client)
add_subdirectory(find-process-client)

option(ARA_ENABLE_DETERMINISTIC_EXECUTION "enable deterministic execution" OFF)
if (ARA_ENABLE_DETERMINISTIC_EXECUTION)
    # add_subdirectory(deterministic-execution)
endif()

set(TARGET_PUBLIC_HEADER target-public-header)
add_library(${TARGET_PUBLIC_HEADER} INTERFACE)
target_include_directories(${TARGET_PUBLIC_HEADER} INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Install header files
install(
    DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    COMPONENT dev
    FILES_MATCHING PATTERN "*.h"
)
