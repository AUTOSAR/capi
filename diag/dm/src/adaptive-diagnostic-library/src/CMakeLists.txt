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

find_package(ara-gen REQUIRED)
find_package(nsomeip REQUIRED)
find_package(ara-arxmls REQUIRED)
find_package(ara-core-types REQUIRED)
find_package(isoft-ara-fsh REQUIRED)
find_package(isoft-manifestreader REQUIRED)
if(ARA_MAJOR_VERSION EQUAL 3)
    find_package(isoft-nml REQUIRED)    
endif()

set(DIAG_COMMON_LIB diag-common)
# Check if the target exists
if(TARGET diag-common)
    message(STATUS "Target 'diag-common' exists, can link safely")
else()
    find_package(diag-common REQUIRED)
    set(DIAG_COMMON_LIB ara::diag-common)
endif()

set(TARGET_NAME diag)

file(GLOB_RECURSE ARA_DIAG_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

add_library(${TARGET_NAME} SHARED
    ${ARA_DIAG_SOURCES}
    ${NML}
    runtime.cpp
    authentication.cpp
    conversation.cpp
    conversation_manager.cpp
    communication_control.cpp
    cancellation_handler.cpp
    runtime_enviroment.cpp
    meta_info.cpp
    ecu_reset_request.cpp
    generic_routine.cpp
    generic_uds_service.cpp
    download_service.cpp
    upload_service.cpp
    release_handler.cpp
    file_transfer_service.cpp
    service_validation.cpp
    security_access.cpp
    generic_data_identifier.cpp
    raw_data_identifier.cpp
)

target_include_directories(${TARGET_NAME}
    PRIVATE
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/../>
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/>
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/../../../common/include>
)
if(ARA_MAJOR_VERSION EQUAL 3)
    set(ISOFT_NML_LIB isoft::isoft_nml) 
else()
    set(ISOFT_NML_LIB "")    
endif()
target_link_libraries(${TARGET_NAME}
    PUBLIC
        nsomeip
        isoft::manifestreader
        isoft::ara_fsh
        ${ISOFT_NML_LIB}
        ara::core-types
    PRIVATE
        ${DIAG_COMMON_LIB}
        gen_code_client_obj
)

target_compile_definitions(${TARGET_NAME}
    PUBLIC
        HAS_ARA_DIAG
    PRIVATE
        ARA_FC_LIB_DIAG
)

set_target_properties(${TARGET_NAME}
    PROPERTIES
        SOVERSION ${PROJECT_VERSION_MAJOR}
        VERSION ${PROJECT_VERSION}
        OUTPUT_NAME ara_${TARGET_NAME}
)

# General install
include(FindPkgConfig)
if(PKG_CONFIG_FOUND)
    configure_file(${PROJECT_SOURCE_DIR}/misc/${PROJECT_NAME}.pc.in ${PROJECT_NAME}.pc @ONLY)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
endif()

# Create *-config.cmake file
include(CMakePackageConfigHelpers)
configure_package_config_file(
    ${PROJECT_SOURCE_DIR}/misc/${PROJECT_NAME}-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
    NO_SET_AND_CHECK_MACRO
)
# Create *-config-version.cmake file
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake
    COMPATIBILITY ExactVersion
)
# Install *-config.cmake and *-version.cmake files
install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake
    DESTINATION
        ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

# Install exported targets, i.e. *-targets.cmake
install(
    TARGETS ${TARGET_NAME}
    EXPORT ${PROJECT_NAME}-targets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
install(
    EXPORT ${PROJECT_NAME}-targets
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
    NAMESPACE ara::
)

# Install header files
install(
    DIRECTORY ara
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
