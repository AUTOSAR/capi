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
# @brief      Encryption/decryption module compilation
# @details
# @date       2021-12-17
# @author     hanjingjing
# @version    1.2.0
#
# ================================================================
#
# 1. Set a name for Target_name
#
# ================================================================

set(TARGET_NAME "lib-isoft")
message(STATUS "======== Begin : ${PROJECT_NAME}.${TARGET_NAME} ========")
message(STATUS " ***  CRYPTO_ROOT_DIR           = ${CRYPTO_ROOT_DIR}")
message(STATUS "  **  CMAKE_CURRENT_SOURCE_DIR  = ${CMAKE_CURRENT_SOURCE_DIR}")

# Step 2: Add the list of files to be compiled
file(GLOB_RECURSE SRC_PH_LIST CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
print_source_list("${SRC_PH_LIST}" "srcFile")

# 3. Add src to Target_name
add_library(${TARGET_NAME} OBJECT ${SRC_PH_LIST})
# Give an alias
add_library(ara::crypto::${TARGET_NAME} ALIAS ${TARGET_NAME})
# Check whether ARA_CRYPTO_DEBUG macro exists and pass it to C++
add_precompiled_macros(${TARGET_NAME})

target_compile_options(${TARGET_NAME} PRIVATE -Wpedantic)

# 4. Add include to Target_name
target_include_directories(${TARGET_NAME} 
PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
PRIVATE
	$<BUILD_INTERFACE:${CRYPTO_ROOT_DIR}/src/ara_crypto/include>
)

# System paths should be written separately: only this can suppress warnings
target_include_directories(${TARGET_NAME}
SYSTEM PRIVATE
	${OPENSSL_INCLUDE_DIR}  # Use OpenSSL header files
	$<BUILD_INTERFACE:${CMAKE_INSTALL_PREFIX}/include>
)

set_target_properties(${TARGET_NAME} PROPERTIES
	POSITION_INDEPENDENT_CODE True
)

# -----------------------------
message(STATUS "======== End   : ${PROJECT_NAME}.${TARGET_NAME} ========")
