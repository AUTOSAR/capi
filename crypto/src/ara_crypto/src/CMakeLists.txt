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
# @brief      Encryption/Decryption module compilation
# @details
# @date       2021-12-17
# @author     hanjingjing
# @version    1.2.0
#
# ================================================================

# 1. Name and call find_package


setup_find_packages(IN_ARA_CRYPTO)
set(LIBRARY_NAME "ara_crypto")
set(LIBRARY_FILE_NAME "ara_crypto_ph")

message(STATUS "======== Begin : ${PROJECT_NAME}.${LIBRARY_NAME} ========")
message(STATUS " ***  ARA_SYSROOT               = ${ARA_SYSROOT} ")
message(STATUS " ***  CRYPTO_ROOT_DIR           = ${CRYPTO_ROOT_DIR} ")
message(STATUS "  **  CMAKE_SOURCE_DIR          = ${CMAKE_SOURCE_DIR}")
message(STATUS "  **  PROJECT_SOURCE_DIR        = ${PROJECT_SOURCE_DIR} ")
message(STATUS "  **  CMAKE_CURRENT_SOURCE_DIR  = ${CMAKE_CURRENT_SOURCE_DIR} ")

# ① CMAKE_BINARY_DIR / PROJECT_BINARY_DIR: Location of generated libraries, executables, etc., project top-level directory (in) / project build directory (out).
# ② CMAKE_SOURCE_DIR / PROJECT_SOURCE_DIR: Project top-level directory.
# ③ CMAKE_CURRENT_SOURCE_DIR: Path where the current CMakeLists.txt is located.
# ④ CMAKE_CURRENT_BINARY_DIR: Path of the build directory where the current CMakeLists.txt is located.
# ⑤ CMAKE_LIBRARY_OUTPUT_DIRECTORY: Output directory for library files.
# ⑥ CMAKE_RUNTIME_OUTPUT_DIRECTORY: Output directory for executable files.

# 2. Define the source list (ara_crypto own source + required OBJECT libraries)
file(GLOB_RECURSE SRC_PH_LIST CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
print_source_list("${SRC_PH_LIST}" "srcFile111")

# 3. Build the shared library uniformly (including all conditionally appended object files)
if(ARA_WITH_CRYPTO_BUILD_SHARED_LIBS)
	add_library(${LIBRARY_NAME} SHARED ${SRC_PH_LIST} )
else()
    add_library(${LIBRARY_NAME} STATIC ${SRC_PH_LIST})
endif()
# Alias the name
add_library(ara::${LIBRARY_NAME} ALIAS ${LIBRARY_NAME})
add_library(ara::crypto ALIAS ${LIBRARY_NAME})

# 4. Link object libraries (via target_sources) append OBJECT target files
target_sources(${LIBRARY_NAME} PRIVATE
    $<TARGET_OBJECTS:ara::crypto::lib-isoft>
    $<TARGET_OBJECTS:ara::crypto::lib-x509>
)
# Conditionally append OBJECT target files for lib-hsmnxp
if(ARA_WITH_CRYPTO_HSM_NXP)
	target_sources(${LIBRARY_NAME} PRIVATE
		$<TARGET_OBJECTS:ara::crypto::lib-hsmnxp>
		$<TARGET_OBJECTS:ara::crypto::lib-pkcs>
	)
endif()
# Conditionally append OBJECT target files for lib-hsmbst
if(ARA_WITH_CRYPTO_HSM_BST)
	target_sources(${LIBRARY_NAME} PRIVATE
		$<TARGET_OBJECTS:ara::crypto::lib-hsmbst>
	)
endif()

# 5. Add preprocessor macros to be passed to C++
add_precompiled_macros(${LIBRARY_NAME})
# Other C++ macros
if(ARA_WITH_CRYPTO_HSM_NXP)
	target_compile_definitions(${LIBRARY_NAME}
		PRIVATE
			ARA_WITH_CRYPTO_HSM_NXP=1
	)
endif()
if(ARA_WITH_CRYPTO_HSM_BST)
	target_compile_definitions(${LIBRARY_NAME}
		PRIVATE
			ARA_WITH_CRYPTO_HSM_BST=1
	)
endif()
target_compile_definitions(${LIBRARY_NAME}
PUBLIC
	HAS_ARA_CRYPTO # for check library , see ara::core::Initialize
)

# 6. Link libraries
target_link_libraries(${LIBRARY_NAME}
    ${ARA_GEN_BUILD_HELPER_LIBRARY}
)
if(ARA_WITH_CRYPTO_HSM_BST)
	set_link_libraries(${LIBRARY_NAME} IN_ARA_CRYPTO WITH_BST)
else()
	set_link_libraries(${LIBRARY_NAME} IN_ARA_CRYPTO)
endif()

# 7. Include file paths
target_include_directories( ${LIBRARY_NAME}
PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
	$<BUILD_INTERFACE:${CRYPTO_ROOT_DIR}/src/lib_isoft/include>
	$<BUILD_INTERFACE:${CRYPTO_ROOT_DIR}/src/lib_x509/include>
)
# System paths must be written separately: only this way can suppress warnings
target_include_directories(${LIBRARY_NAME}
SYSTEM PRIVATE
	${OPENSSL_INCLUDE_DIR}  # Using OpenSSL header files
	$<BUILD_INTERFACE:${CMAKE_INSTALL_PREFIX}/include>	# Path here is: ~/ara-sysroot/ara/framework/1.0.0
)
# 8. Compilation dependencies
# add_dependencies must occur after add_library
add_dependencies(${LIBRARY_NAME}
	lib-isoft
	lib-x509
)

# 9. Conditionally compiled header files and compilation dependencies
if(ARA_WITH_CRYPTO_HSM_NXP)
	target_include_directories(${LIBRARY_NAME}
	PRIVATE
		$<BUILD_INTERFACE:${CRYPTO_ROOT_DIR}/src/hsm_nxp/lib_hsmnxp/include>
	)
    add_dependencies( ${LIBRARY_NAME}	lib-hsmnxp)
endif()

if(ARA_WITH_CRYPTO_HSM_BST)
    add_dependencies( ${LIBRARY_NAME}	lib-hsmbst)
	target_include_directories(${LIBRARY_NAME}
	PRIVATE
		$<BUILD_INTERFACE:${CRYPTO_ROOT_DIR}/src/hsm_bst/lib_hsmbst/include> # Path here is: ~/ap-all/crypto/src/lib_isoft/include
	)
endif()

# 10. Add compile and link attributes
set_target_properties(${LIBRARY_NAME} PROPERTIES
	POSITION_INDEPENDENT_CODE ON
	OUTPUT_NAME ${LIBRARY_FILE_NAME}
)

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

# Install the actual targets
install(
	TARGETS ${LIBRARY_NAME}
	EXPORT "${PROJECT_NAME}-targets"
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	)

# Install exported targets, i.e. *-targets.cmake
install(
	EXPORT "${PROJECT_NAME}-targets"
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
	NAMESPACE ara::
    )

# Install header files
install(
	DIRECTORY ${CRYPTO_ROOT_DIR}/src/ara_crypto/include/
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
	COMPONENT dev
	FILES_MATCHING PATTERN "*.h"
	)

message(STATUS "======== End   : ${PROJECT_NAME}.${LIBRARY_NAME} ========")
