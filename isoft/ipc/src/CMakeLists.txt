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
# @date       2021-11-18
# @author     gaohuiming
# @version    1.2.0
#
# ================================================================

find_package(nai REQUIRED)
find_package(isoft-ipcbase REQUIRED)

set(LIB_TARGET ipc)
set(LIBRARY_FILE_NAME isoft_ipc)
add_library(${LIB_TARGET}
    buffer.c
    peer_uds.c
    packet.c
    event.c
    channel_uds.c
    client.c
    server.c
    server_session.c
    session.c
    manager.c
    ipc.c
)

set_target_properties(${LIB_TARGET}
	PROPERTIES
        SOVERSION ${PROJECT_VERSION_MAJOR}
        VERSION ${PROJECT_VERSION}
        OUTPUT_NAME ${LIBRARY_FILE_NAME}
)

add_library(isoft::ipc ALIAS ${LIB_TARGET})

target_include_directories(${LIB_TARGET} 
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries( ${LIB_TARGET}
    PUBLIC
        isoft::ipcbase
        pthread
        nai
)

install(TARGETS ${LIB_TARGET} EXPORT ${PROJECT_NAME}-targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

add_subdirectory(cpp)
add_subdirectory(em)

if (ARA_IPC_DEBUG)
    add_definitions(-DIPC_PACKET_DEBUG)
    add_definitions(-DIPC_EVNODE_DEBUG)
    add_definitions(-DIPC_SERVER_SESSION_DEBUG)
    add_definitions(-DIPC_SESSION_DEBUG)
endif()

if(ARA_ENABLE_TESTS AND ARA_IPC_DEBUG)
    add_subdirectory(test)
    add_subdirectory(testcpp)
endif()