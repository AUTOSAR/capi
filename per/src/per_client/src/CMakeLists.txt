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
# @date       2026-01-30
# @author     niuliming
# @version    1.2.0
#
# ================================================================
#
# === Library Basic Information ===
#
# ================================================================

set(LIBRARY_NAME "per")
set(LIBRARY_FILE_NAME "ara_per")

# === Option Configuration ===
option(ARA_PER_DEBUG "Enable ARA persistent debug mode" OFF)
option(ARA_WITH_CRYPTO "Enable crypto support for persistent storage" OFF)  # Explicit declaration for easy external control

# === Path Settings ===
if(NOT DEFINED ARA_PER_CLIENT_ROOT_DIR)
    get_filename_component(ARA_PER_CLIENT_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../" ABSOLUTE)
    message(STATUS "ARA_PER_CLIENT_ROOT_DIR not defined, setting to: ${ARA_PER_CLIENT_ROOT_DIR}")
endif()

# === Status Output ===
message(STATUS "======== Begin : ${PROJECT_NAME}.${LIBRARY_NAME} ========")
message(STATUS "  **  ARA_SYSROOT               = ${ARA_SYSROOT}")
message(STATUS "  **  ARA_PER_CLIENT_ROOT_DIR   = ${ARA_PER_CLIENT_ROOT_DIR}")
message(STATUS "  **  CMAKE_CURRENT_SOURCE_DIR  = ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "  **  CMAKE_INSTALL_PREFIX      = ${CMAKE_INSTALL_PREFIX}")

# === CMake Modules ===
include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

# === Find Dependency Packages ===
find_package(nai REQUIRED)
find_package(ara-core-types REQUIRED)
find_package(ara-log REQUIRED)
find_package(isoft-manifestreader REQUIRED)
find_package(isoft-crc REQUIRED)
find_package(isoft-ara-fsh REQUIRED)
find_package(isoft-naicpp REQUIRED)

# === Crypto Support ===
if(ARA_WITH_CRYPTO)
    message(STATUS "Persistent client compiled with crypto support")
    find_package(ara-crypto REQUIRED)
else()
    message(STATUS "Persistent client compiled without crypto support")
endif()

# === Source File List (Explicit listing recommended, but keep GLOB_RECURSE for rapid development)===
file(GLOB_RECURSE SRC_PH_LIST CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/common/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/file_storage/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/kv_storage/*.cpp"
)

# === Create Library Target ===
if(ARA_WITH_PER_BUILD_SHARED_LIBS)
    add_library(${LIBRARY_NAME} SHARED ${SRC_PH_LIST})
else()
    add_library(${LIBRARY_NAME} STATIC ${SRC_PH_LIST})
endif()

add_library(ara::per ALIAS ${LIBRARY_NAME})

# === Set Library Properties ===
set_target_properties(${LIBRARY_NAME} PROPERTIES
    OUTPUT_NAME "${LIBRARY_FILE_NAME}"
    POSITION_INDEPENDENT_CODE ON  # Enable for STATIC as well, convenient for shared library usage
)

# === Include Directories (Note PUBLIC interface should only include paths of externally exposed headers)===
target_include_directories(${LIBRARY_NAME}
    PUBLIC
        # ${ARA_PER_CLIENT_ROOT_DIR}/include
        # $<INSTALL_INTERFACE:include>
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        ${CMAKE_CURRENT_SOURCE_DIR}/common
        ${CMAKE_CURRENT_SOURCE_DIR}/file_storage
        ${CMAKE_CURRENT_SOURCE_DIR}/kv_storage
        ${ARA_PER_CLIENT_ROOT_DIR}/include
        ${CMAKE_INSTALL_PREFIX}/include  # Note: Hardcoding this path in BUILD_INTERFACE is generally not recommended
)

# === Link Dependencies ===
# Use target_link_libraries directly for clearer scoping instead of intermediate variables
target_link_libraries(${LIBRARY_NAME}
    PUBLIC
        ara::core-types
        ara::log
        isoft::crc
        isoft::manifestreader
        isoft::ara_fsh
        nai
    PRIVATE
        isoft::naicpp
        $<BUILD_INTERFACE:target-kv-engine>
)

# Conditional linking of crypto library
if(ARA_WITH_CRYPTO)
    target_link_libraries(${LIBRARY_NAME} PUBLIC ara::ara_crypto)
    target_compile_definitions(${LIBRARY_NAME} PUBLIC ARA_WITH_CRYPTO)
endif()

# === Compile Definitions ===
# Replace global CMAKE_CXX_FLAGS, use target_compile_definitions
target_compile_definitions(${LIBRARY_NAME}
    PUBLIC
        ARA_LOG_WITHOUT_MODEL_MESSAGE=1  # Required for all users
        HAS_ARA_PER                      # Used for library detection
    PRIVATE
        $<$<BOOL:${ARA_PER_DEBUG}>:
            ARA_DEBUG=1
            ARA_PER_DEBUG=1
            ISOFT_MANIFEST_DEBUG=1
        >
)

if(ARA_PER_DEBUG)
    message(STATUS "Persistent client compiled with debug mode enabled.")
endif()

# === Dependency Relationship (Ensure kv_engine builds first)===
add_subdirectory(kv_engine)
add_dependencies(${LIBRARY_NAME} target-kv-engine)

# === Installation Rules ===
# Install header files
install(
    DIRECTORY ${ARA_PER_CLIENT_ROOT_DIR}/include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    COMPONENT dev
    FILES_MATCHING PATTERN "*.h"
)

# Install library files
install(
    TARGETS ${LIBRARY_NAME}
    EXPORT "${PROJECT_NAME}-targets"
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    # RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}  # Included for completeness though unlikely
)

# Install export targets
install(
    EXPORT "${PROJECT_NAME}-targets"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
    NAMESPACE ara::
)

# === pkg-config Support ===
find_package(PkgConfig QUIET)
if(PKG_CONFIG_FOUND AND EXISTS "${PROJECT_CUR_ROOT_DIR}/misc/${PROJECT_NAME}.pc.in")
    configure_file(
        "${PROJECT_CUR_ROOT_DIR}/misc/${PROJECT_NAME}.pc.in"
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc"
        @ONLY
    )
    install(
        FILES "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
    )
endif()

# === CMake Config Package File ===
configure_package_config_file(
    "${PROJECT_CUR_ROOT_DIR}/misc/${PROJECT_NAME}-config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
    NO_SET_AND_CHECK_MACRO
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake"
    COMPATIBILITY ExactVersion
)

install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
)

message(STATUS "======== End   : ${PROJECT_NAME}.${LIBRARY_NAME} ========")
