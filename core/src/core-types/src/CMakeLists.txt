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
# @date       2021-04-14
# @author     jiawei
# @version    1.2.0
#
# ================================================================

find_package(isoft-cmake-common REQUIRED)
include(GNUInstallDirs)
set(LIBRARY_NAME "core-types")


if(NOT DEFINED ARA_CORE_ROOT_DIR)
    get_filename_component(ARA_CORE_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../" ABSOLUTE)
endif()
message(STATUS "======== Begin : ${PROJECT_NAME}.${LIBRARY_NAME} ========")
message(STATUS "  **  ARA_SYSROOT               = ${ARA_SYSROOT} ")
message(STATUS "  **  ARA_CORE_ROOT_DIR         = ${ARA_CORE_ROOT_DIR} ")
message(STATUS "  **  CMAKE_SOURCE_DIR          = ${CMAKE_SOURCE_DIR}")
message(STATUS "  **  PROJECT_SOURCE_DIR        = ${PROJECT_SOURCE_DIR} ")
message(STATUS "  **  CMAKE_CURRENT_SOURCE_DIR  = ${CMAKE_CURRENT_SOURCE_DIR} ")


#set(ARA_WITH_ASAN TRUE)

if (${ARA_WITH_ASAN})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
    set(ASAN_LD_FLAGS "-fsanitize=address -fno-omit-frame-pointer")
endif()


if(CMAKE_COMPILER_IS_GNUCXX)
    #add_compile_options(-Werror)
endif()

add_compile_options(-Wconversion -Wsign-conversion)

# find_package(Boost REQUIRED)
file(GLOB_RECURSE SRC_PH_LIST CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
list(APPEND SRC_PH_LIST "${CMAKE_CURRENT_SOURCE_DIR}/phsl/types/bad_variant_access.cc")

add_library(${LIBRARY_NAME}     ${SRC_PH_LIST} )

set_target_properties(${LIBRARY_NAME}
    PROPERTIES
        SOVERSION ${PROJECT_VERSION_MAJOR}
        VERSION ${PROJECT_VERSION}
    OUTPUT_NAME ara_core_types
)

target_link_libraries(${LIBRARY_NAME}
PUBLIC
        ${ASAN_LD_FLAGS}  # public asn flags
        # Boost::headers
PRIVATE
        atomic # for clang link error
)
target_include_directories(${LIBRARY_NAME}
PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
    # $<BUILD_INTERFACE:${Boost_INCLUDE_DIR}>
    $<INSTALL_INTERFACE:include>
)

add_library(ara::${LIBRARY_NAME} ALIAS ${LIBRARY_NAME})

# -----------------------------

include(FindPkgConfig)
if(PKG_CONFIG_FOUND)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../misc/${PROJECT_NAME}.pc.in ${PROJECT_NAME}.pc @ONLY)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
endif()

# Create *-config.cmake file
include(CMakePackageConfigHelpers)
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/../misc/${PROJECT_NAME}-config.cmake.in
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
    TARGETS ${LIBRARY_NAME}
    EXPORT ${PROJECT_NAME}-targets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
install(
    EXPORT ${PROJECT_NAME}-targets
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
    NAMESPACE ara::
)

# Install header files
install(
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    COMPONENT dev
    FILES_MATCHING PATTERN "*.h"
)

message(STATUS "======== End   : ${PROJECT_NAME}.${LIBRARY_NAME} ========")
