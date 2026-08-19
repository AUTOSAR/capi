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
# @date       2021-05-21
# @author     james.feng
# @version    1.2.0
#
# ================================================================

set(EXPORT_NAME state_client)
set(TARGET_NAME target-state-client)
set(ARA_CMAKE_CONFIG ara-exec-state-client)
set(LIBRARY_FILE_NAME ara_exec_state_client)

add_library(${TARGET_NAME}
    src/state_client.cpp
    src/function_group.cpp
    src/function_group_state.cpp
)

add_library(ara::exec::${EXPORT_NAME} ALIAS ${TARGET_NAME})

target_compile_definitions(${TARGET_NAME}
PUBLIC
    HAS_ARA_EXEC_STATE_CLIENT
)

target_include_directories(${TARGET_NAME}
PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
	$<BUILD_INTERFACE:${CMAKE_INSTALL_PREFIX}/include>
)

get_target_property(CONFIG_LIBS target-config LINK_LIBRARIES)
get_target_property(SMS_CLIENT_LIBS target-sms-client LINK_LIBRARIES)

target_link_libraries(${TARGET_NAME}
PUBLIC
    ara::log
    isoft::ara_fsh
    ara::core-types
    ${CONFIG_LIBS}
    ${SMS_CLIENT_LIBS}
PRIVATE
    $<BUILD_INTERFACE:target-log>
    $<BUILD_INTERFACE:target-config>
    $<BUILD_INTERFACE:target-sms-client>
    $<BUILD_INTERFACE:target-string-util>
    $<BUILD_INTERFACE:target-public-header>
)

# Create *.pc file
if(PKG_CONFIG_FOUND)
    set(LIBRARIES "-l${LIBRARY_FILE_NAME} -lisoft_ara_fsh -lnaicpp -lisoft_ipc")
    configure_file(${PROJECT_SOURCE_DIR}/misc/pkgconfig/ara-exec.pc.in ${ARA_CMAKE_CONFIG}.pc @ONLY)
    install(
        FILES ${CMAKE_CURRENT_BINARY_DIR}/${ARA_CMAKE_CONFIG}.pc
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
    )
endif()

# Create *-config.cmake file
configure_package_config_file(
    ${PROJECT_SOURCE_DIR}/misc/cmake/ara-exec-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/${ARA_CMAKE_CONFIG}-config.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${ARA_CMAKE_CONFIG}
    NO_CHECK_REQUIRED_COMPONENTS_MACRO
)
# Create *-config-version.cmake file
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/${ARA_CMAKE_CONFIG}-config-version.cmake
    COMPATIBILITY ExactVersion
)
# Install *-config.cmake and *-version.cmake files
install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/${ARA_CMAKE_CONFIG}-config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/${ARA_CMAKE_CONFIG}-config-version.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${ARA_CMAKE_CONFIG}
)

# Install header files
install(
    DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    COMPONENT dev
    FILES_MATCHING PATTERN "*.h"
)

set_target_properties(${TARGET_NAME} PROPERTIES
    OUTPUT_NAME ${LIBRARY_FILE_NAME}
    #PUBLIC_HEADER ${CMAKE_CURRENT_LIST_DIR}/include/ara/exec/state_client.h
    EXPORT_NAME ${EXPORT_NAME}
)

# Install lib
install(
    TARGETS ${TARGET_NAME}
    EXPORT ${TARGET_NAME}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/ara/exec
)

# Install ara-exec-targets.cmake
install(
    EXPORT ${TARGET_NAME}
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${ARA_CMAKE_CONFIG}
    NAMESPACE ara::exec::
    FILE ara-exec-targets.cmake
)
