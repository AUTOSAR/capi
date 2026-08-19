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
# Define target name
#
# ================================================================

set(TARGET_NAME "target-kv-engine")

# Set the default value for ARA_PER_CLIENT_ROOT_DIR (if not defined)
if(NOT DEFINED ARA_PER_CLIENT_ROOT_DIR)
    get_filename_component(ARA_PER_CLIENT_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../" ABSOLUTE)
    message(STATUS "ARA_PER_CLIENT_ROOT_DIR not defined, setting to: ${ARA_PER_CLIENT_ROOT_DIR}")
endif()

# Print build information
message(STATUS "======== Begin : ${PROJECT_NAME}.${TARGET_NAME} ========")
message(STATUS "  **  ARA_SYSROOT               = ${ARA_SYSROOT}")
message(STATUS "  **  ARA_PER_CLIENT_ROOT_DIR   = ${ARA_PER_CLIENT_ROOT_DIR}")
message(STATUS "  **  CMAKE_CURRENT_SOURCE_DIR  = ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "  **  CMAKE_INSTALL_PREFIX      = ${CMAKE_INSTALL_PREFIX}")

# Find dependent packages
find_package(ara-core REQUIRED)
find_package(ara-core-types REQUIRED)

# Optional encryption support
option(ARA_WITH_CRYPTO "Enable crypto support for persistent KV engine" OFF)

if(ARA_WITH_CRYPTO)
    message(STATUS "Persistent client KV engine compiled with crypto support")
    find_package(ara-crypto REQUIRED)
else()
    message(STATUS "Persistent client KV engine compiled without crypto support")
endif()

# Collect source files (recommended to list source files explicitly, but glob is retained for rapid prototyping)
file(GLOB_RECURSE SRC_LIST CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

# Create OBJECT library
add_library(${TARGET_NAME} OBJECT ${SRC_LIST})

# Set C++ standard (if not already set, it is recommended to add)
# set_target_properties(${TARGET_NAME} PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

# Link dependencies: all set to PRIVATE because this is an internal implementation
target_link_libraries(${TARGET_NAME}
    PRIVATE
        ara::core
        ara::core-types
)

if(ARA_WITH_CRYPTO)
    target_link_libraries(${TARGET_NAME} PRIVATE ara::ara_crypto)
    target_compile_definitions(${TARGET_NAME} PRIVATE ARA_WITH_CRYPTO)
endif()

# Set include directories
target_include_directories(${TARGET_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
        ${ARA_PER_CLIENT_ROOT_DIR}/include
        ${ARA_SYSROOT}/usr/include
        # Note: Do not include ${CMAKE_INSTALL_PREFIX}/include — this is usually unnecessary; dependencies should be resolved via find_package and target_link_libraries
)

# NOTE: This is an internal OBJECT library. No install() rules are defined.
message(STATUS "======== End   : ${PROJECT_NAME}.${TARGET_NAME} ========")
