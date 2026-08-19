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
# @date       2023-10-30
# @author     zhibo.han
# @version    1.2.0
#
# ================================================================

set(LIBRARY_NAME "ara_ucm_libcrypto")
message("CMAKE_CURRENT_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}")

if(ARA_WITH_UCM_LIBS_BUILD_SHARED_LIBS)
    add_library(
        ${LIBRARY_NAME}
        SHARED
            crypto.cpp
    )
else()
    add_library(
        ${LIBRARY_NAME}
        STATIC
            crypto.cpp
    )
endif()

target_include_directories(
    ${LIBRARY_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    PRIVATE
        ${BOTAN_INCLUDE_DIRS}
)

include_directories(
                    ${CMAKE_INSTALL_PREFIX}/include 
                   )    


target_link_libraries(
    ${LIBRARY_NAME}
    PRIVATE
        ${BOTAN_LINK_LIBRARIES}
        ara::log
    PUBLIC
        ara::core-types
)

target_compile_options(${LIBRARY_NAME} PRIVATE -pthread)

set_target_properties(
    ${LIBRARY_NAME}
    PROPERTIES
        SOVERSION ${PROJECT_VERSION_MAJOR}
        VERSION ${PROJECT_VERSION}
        OUTPUT_NAME ${LIBRARY_NAME}
        EXPORT_NAME ${LIBRARY_NAME}
)

add_library(ara::${LIBRARY_NAME} ALIAS ${LIBRARY_NAME})

install(
    TARGETS ${LIBRARY_NAME}
    EXPORT ${PROJECT_NAME}-targets
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
)

