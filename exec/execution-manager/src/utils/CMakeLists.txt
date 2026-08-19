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
# @brief      cmake configuration file
# @details
# @date       2025-08-04
# @author     Xueliang Bao
# @version    1.2.0
#
# ================================================================
#
# ############################# Header Util ##############################
#
# ================================================================

set(TARGET_HEADER_UTIL target-header-util)

add_library(${TARGET_HEADER_UTIL} INTERFACE)

target_include_directories(${TARGET_HEADER_UTIL}
INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
)

############################## File Util ##############################
set(TARGET_FILE_UTIL target-file-util)

add_library(${TARGET_FILE_UTIL} OBJECT
    src/file.cpp
)

target_include_directories(${TARGET_FILE_UTIL}
PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
)


############################# String Util ##############################
set(TARGET_STRING_UTIL target-string-util)

add_library(${TARGET_STRING_UTIL} OBJECT
    src/string.cpp
)

target_include_directories(${TARGET_STRING_UTIL}
PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
)


############################# Process Util ##############################
set(TARGET_PROCESS_UTIL target-process-util)

add_library(${TARGET_PROCESS_UTIL} OBJECT
    src/process.cpp
)

target_include_directories(${TARGET_PROCESS_UTIL}
PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
)


############################# Security Util ##############################
set(TARGET_SECURITY_UTIL target-security-util)

pkg_search_module(PKG_CRYPTO REQUIRED libcrypto)

add_library(${TARGET_SECURITY_UTIL} OBJECT
    src/security.cpp
)

target_include_directories(${TARGET_SECURITY_UTIL}
PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
    $<BUILD_INTERFACE:$ENV{PKG_CONFIG_SYSROOT_DIR}/${PKG_CRYPTO_INCLUDEDIR}>
)

target_link_libraries(${TARGET_SECURITY_UTIL}
PUBLIC
    ${PKG_CRYPTO_LIBRARIES}
)
