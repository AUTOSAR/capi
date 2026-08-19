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
# @date       2024-10-12
# @author     kai.ju
# @version    1.2.0
#
# ================================================================
#
# server library
#
# ================================================================

set(ROLE_TYPE server)
set(TARGET_NAME uds-${ROLE_TYPE})

find_package(nai REQUIRED)
find_package(isoft-naicpp REQUIRED)
find_package(ara-core-types REQUIRED)
find_package(ara-log REQUIRED)

set(DIAG_COMMON_LIB diag-common)
# Check if the target exists
if(TARGET diag-common)
    message(STATUS "Target 'diag-common' exists, can link safely")
else()
    find_package(diag-common REQUIRED)
    set(DIAG_COMMON_LIB ara::diag-common)
endif()

################################################################################

file(GLOB FAULT_MANAGEMENT_SOURCES ./fault_management/*.cpp)

set(SRC_LIST
    ${FAULT_MANAGEMENT_SOURCES}
    authentication_management/authentication_manager.cpp
    authentication_management/client_authentication.cpp
    authentication_management/client_authentication_controller.cpp
    authentication_management/service_x29.cpp
    authentication_management/client_authentication_controller_impl.cpp
    common/general_service.cpp
    ${PROJECT_SOURCE_DIR}/../common/src/thread_pool/thread_pool.cpp
    common/timer.cpp
    common/request_context.cpp
    common/log.cpp
    common/persistence_service.cpp
    session_management/service_x10.cpp
    session_management/session_manager.cpp
    transfer_managment/service_x34.cpp
    transfer_managment/service_x35.cpp
    transfer_managment/service_x36.cpp
    transfer_managment/service_x37.cpp
    transfer_managment/service_x38.cpp    

    transfer_managment/transfer_manager.cpp
    custom_management/service_custom.cpp
    # dtc_management/service_x14.cpp
    # dtc_management/service_x85.cpp
    # dtc_management/service_x19.cpp
    roe_management/diagnostic_roe_manager.cpp
    roe_management/service_x86.cpp
    com_management/service_x28.cpp
    ecu_reset_management/service_x11.cpp
    data_management/service_x2c.cpp
    data_management/service_x2e.cpp
    data_management/service_x2a.cpp
    data_management/service_x22.cpp
    data_management/environmental_condition.cpp
    data_management/diagnostic_data_manager.cpp
    data_management/environmental_condition_manager.cpp
    routine_management/service_x31.cpp
    # routine_management/routine.cpp
    security_access_management/security_level_manager.cpp
    security_access_management/service_x27.cpp
    cancellation_handler.cpp
    cancellation_state.cpp
    server.cpp
    server_manager.cpp
    server_impl.cpp
    message.cpp
    service_validation_service.cpp
)

if(ARA_WITH_DIAG_UDS_BUILD_SHARED_LIBS)
    add_library(${TARGET_NAME} SHARED ${SRC_LIST})
else()
    add_library(${TARGET_NAME} STATIC ${SRC_LIST})
endif()
add_library(isoft::uds::${TARGET_NAME} ALIAS ${TARGET_NAME})

target_include_directories(${TARGET_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src/server/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src/server/include>
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src/server/src>
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src/server/src/common>
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/../common/include>
)

target_link_libraries(${TARGET_NAME} 
    PRIVATE
        nai
        isoft::naicpp
        ara::core-types
        ara::log
    PRIVATE
        ${DIAG_COMMON_LIB}
)

set_target_properties(${TARGET_NAME}
    PROPERTIES
        SOVERSION ${PROJECT_VERSION_MAJOR}
        VERSION ${PROJECT_VERSION}
        OUTPUT_NAME isoft_uds_${ROLE_TYPE}
)

set(INSTALL_CONFIG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../misc)
# General install
include(FindPkgConfig)
if(PKG_CONFIG_FOUND)
    configure_file(${INSTALL_CONFIG_DIR}/${PROJECT_NAME}-${ROLE_TYPE}.pc.in
        ${PROJECT_NAME}-${ROLE_TYPE}.pc @ONLY)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-${ROLE_TYPE}.pc DESTINATION
        ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
endif()

# Create *-config.cmake file
include(CMakePackageConfigHelpers)
configure_package_config_file(
    ${INSTALL_CONFIG_DIR}/${PROJECT_NAME}-${ROLE_TYPE}-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-${ROLE_TYPE}-config.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}-${ROLE_TYPE}
    NO_SET_AND_CHECK_MACRO
)
# Create *-config-version.cmake file
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-${ROLE_TYPE}-config-version.cmake
    COMPATIBILITY ExactVersion
)
# Install *-config.cmake and *-version.cmake files
install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-${ROLE_TYPE}-config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-${ROLE_TYPE}-config-version.cmake
    DESTINATION
        ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}-${ROLE_TYPE}
)

# Install exported targets, i.e. *-targets.cmake
install(
    TARGETS ${TARGET_NAME}
    EXPORT ${PROJECT_NAME}-${ROLE_TYPE}-targets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
install(
    EXPORT ${PROJECT_NAME}-${ROLE_TYPE}-targets
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}-${ROLE_TYPE}
    NAMESPACE isoft::uds::
)

################################################################################
# Install header files
install(
    DIRECTORY ${PROJECT_SOURCE_DIR}/src/server/include/isoft
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
