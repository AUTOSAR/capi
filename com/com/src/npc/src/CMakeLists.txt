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
# @date       2021-12-31
# @author     mazelin
# @version    1.2.0
#
# ================================================================

file(GLOB_RECURSE SRC_LIST ${PROJECT_SOURCE_DIR}/src/*.cpp )
list(APPEND _GeneratedSrc ${SRC_LIST})
file(GLOB_RECURSE SRC_LIST ${PROJECT_SOURCE_DIR}/src/**/*.cpp )
list(APPEND _GeneratedSrc ${SRC_LIST})

add_library(${PROJECT_NAME}
	${_GeneratedSrc}
)

target_compile_definitions(${PROJECT_NAME}
	PUBLIC
	HAS_NPC_BINDING
)
target_include_directories(${PROJECT_NAME}
	PUBLIC
	$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
	$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

find_package(ara-core-types REQUIRED)
find_package(isoft-manifestreader REQUIRED)
find_package(isoft-naicpp REQUIRED)
find_package(nai REQUIRED)
target_link_libraries(${PROJECT_NAME}
	PUBLIC
	ara::core-types
	nai
	PRIVATE
	isoft::manifestreader
	isoft::naicpp
)
if(NOT TARGET npc)
find_package(npc REQUIRED)
endif()
target_link_libraries(${PROJECT_NAME}
	PUBLIC
	npc
)
target_include_directories(${PROJECT_NAME} SYSTEM
	PUBLIC
	$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/npc/include>
	PRIVATE
)
if(NOT TARGET ara_com)
find_package(ara-com REQUIRED)
endif()
target_link_libraries(${PROJECT_NAME}
	PUBLIC
	ara_com
)
if(ARA_ENABLE_COM_SECOC)
if(NOT TARGET ara_com_secoc)
find_package(ara-com-secoc REQUIRED)
endif()
target_link_libraries(${PROJECT_NAME}
	PUBLIC
	ara_com_secoc
)
endif()
if(ARA_ENABLE_IAM)
find_package(ara-iam REQUIRED)
target_link_libraries(${PROJECT_NAME}
	PRIVATE
	ara::iam
)
target_compile_definitions(${PROJECT_NAME}
	PRIVATE
	HAS_ARA_IAM
)
endif()
if(ARA_ENABLE_FSH)
find_package(isoft-ara-fsh REQUIRED)
target_link_libraries(${PROJECT_NAME}
	PRIVATE
	isoft::ara_fsh
)
else()
if(NOT TARGET ara_com_fsh)
find_package(ara-com-fsh REQUIRED)
endif()
target_link_libraries(${PROJECT_NAME}
	PRIVATE
	ara_com_fsh
)
endif()

include(GNUInstallDirs)
# Install actual targets for runtime(required), i.e. libraries
install(
	TARGETS ${PROJECT_NAME}
	EXPORT ${EXPORT_NAME}-targets
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT runtime
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT runtime
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT runtime
)

# Install header files for development(optional)
install(
	DIRECTORY ${PROJECT_SOURCE_DIR}/include/
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} COMPONENT dev
	FILES_MATCHING
	PATTERN "*.h"
	PATTERN "private" EXCLUDE
)

# Install exported targets for integration(optional), i.e. *-targets.cmake
include(CMakePackageConfigHelpers)
install(
	EXPORT ${EXPORT_NAME}-targets
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${EXPORT_NAME} COMPONENT cmake-config
)
## Create *-config.cmake and *-version.cmake files
configure_package_config_file(
	${PROJECT_SOURCE_DIR}/misc/config.cmake.in
	${PROJECT_BINARY_DIR}/${EXPORT_NAME}-config.cmake
	INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${EXPORT_NAME}
)
write_basic_package_version_file(
	${PROJECT_BINARY_DIR}/${EXPORT_NAME}-config-version.cmake
	COMPATIBILITY ExactVersion
)
## Install *-config.cmake and *-version.cmake files
install(
	FILES ${PROJECT_BINARY_DIR}/${EXPORT_NAME}-config.cmake
	FILES ${PROJECT_BINARY_DIR}/${EXPORT_NAME}-config-version.cmake
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${EXPORT_NAME} COMPONENT cmake-config
)

# Install pkg-config files for integration(optional)
include(FindPkgConfig)
if(PKG_CONFIG_FOUND)
configure_file(
	${PROJECT_SOURCE_DIR}/misc/pc.in
	${PROJECT_BINARY_DIR}/${EXPORT_NAME}.pc
	@ONLY
)
install(
	FILES ${PROJECT_BINARY_DIR}/${EXPORT_NAME}.pc
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig COMPONENT pkgconfig
)
endif()