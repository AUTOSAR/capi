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
# @date       2022-06-13
# @author     fengjian
# @version    1.2.0
#
# ================================================================

set(LIBRARY_NAME isoft-ara-fsh)
set(LIBRARY_FILE_NAME isoft_ara_fsh)
set(EXPORT_NAME ara_fsh)
set(TARGET_NAME target-${EXPORT_NAME})

file(GLOB_RECURSE SRC_LIST CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
message( STATUS "PROJECT_NAME = ${PROJECT_NAME}")
foreach( srcFileName ${SRC_LIST})
    message(STATUS "      srcFileName = ${srcFileName}")
endforeach()

option(BUILD_SHARED_LIBS "build shared library instead of static" ON)

if(BUILD_SHARED_LIBS)
    add_library(${TARGET_NAME} SHARED
        ${SRC_LIST}
    )
else()
    add_library(${TARGET_NAME} STATIC
        ${SRC_LIST}
    )
endif()

add_library(isoft::${EXPORT_NAME} ALIAS ${TARGET_NAME})

target_compile_options(${TARGET_NAME} PUBLIC "-Wno-old-style-cast" "-Wno-sizeof-pointer-memaccess")

target_include_directories(${TARGET_NAME}
PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
PRIVATE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src/private>
)

find_package(OpenSSL REQUIRED)
target_link_libraries(${TARGET_NAME} PRIVATE OpenSSL::Crypto)

# Create *.pc file
if(PKG_CONFIG_FOUND)
    set(LIBRARIES "-l${LIBRARY_FILE_NAME}")
    configure_file(${PROJECT_SOURCE_DIR}/misc/${LIBRARY_NAME}.pc.in ${LIBRARY_NAME}.pc @ONLY)
    install(
        FILES ${CMAKE_CURRENT_BINARY_DIR}/${LIBRARY_NAME}.pc
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
    )
endif()

# Create *-config.cmake file
configure_package_config_file(
    ${PROJECT_SOURCE_DIR}/misc/${LIBRARY_NAME}-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/${LIBRARY_NAME}-config.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${LIBRARY_NAME}
    NO_CHECK_REQUIRED_COMPONENTS_MACRO
)
# Create *-config-version.cmake file
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/${LIBRARY_NAME}-config-version.cmake
    COMPATIBILITY ExactVersion
)
# Install *-config.cmake and *-version.cmake files
install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/${LIBRARY_NAME}-config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/${LIBRARY_NAME}-config-version.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${LIBRARY_NAME}
)

# Install header files
install(
    DIRECTORY ${PROJECT_SOURCE_DIR}/include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    COMPONENT dev
    FILES_MATCHING PATTERN "*.h"
)

set_target_properties(${TARGET_NAME} PROPERTIES
    OUTPUT_NAME ${LIBRARY_FILE_NAME}
    EXPORT_NAME ${EXPORT_NAME}
	INSTALL_RPATH "$<TARGET_FILE_DIR:OpenSSL::Crypto>"
)

# Install lib 
install(
    TARGETS ${TARGET_NAME}
    EXPORT ${TARGET_NAME}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/isoft/ipc
)

# Install ara-exec-targets.cmake
install(
    EXPORT ${TARGET_NAME}
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${LIBRARY_NAME}
    NAMESPACE isoft::
    FILE ${LIBRARY_NAME}-targets.cmake
)
