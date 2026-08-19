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
# @date       2022-11-28
# @author     victoryang
# @version    1.2.0
#
# ================================================================

set(LIB_TARGET ipc_em)
set(LIBRARY_FILE_NAME isoft_ipc_em)
add_library(${LIB_TARGET}
    ipc_em.c
)

set_target_properties(${LIB_TARGET}
    PROPERTIES
        SOVERSION ${PROJECT_VERSION_MAJOR}
        VERSION ${PROJECT_VERSION}
        OUTPUT_NAME ${LIBRARY_FILE_NAME}
)

add_library(isoft::ipc_em ALIAS ${LIB_TARGET})

target_include_directories(${LIB_TARGET}
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries( ${LIB_TARGET}
    PUBLIC
        isoft::ipc
)

install(TARGETS ${LIB_TARGET} EXPORT ${PROJECT_NAME}-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

if (ARA_IPC_DEBUG)
    add_definitions(-DIPC_EM_DEBUG)
endif()