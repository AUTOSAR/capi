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

add_library(${PROJECT_NAME}
	proxy/event.cpp
	proxy/field.cpp
	proxy/method.cpp
	proxy/proxy.cpp
	skeleton/event.cpp
	skeleton/field.cpp
	skeleton/method.cpp
	skeleton/skeleton.cpp
	base.cpp
	runtime.cpp
	threadpool.cpp
	types.cpp
)
target_compile_definitions(${PROJECT_NAME}
	PUBLIC
	HAS_ARA_COM
)
if((NOT ARA_MAJOR_VERSION) OR (ARA_MAJOR_VERSION STREQUAL 1))
target_compile_definitions(${PROJECT_NAME}
	PUBLIC
	R2011
)
else()
target_compile_definitions(${PROJECT_NAME}
	PUBLIC
	R2311
)
endif()
if(ARA_ENABLE_COM_EVENT_SEND_TRIGGER)
target_compile_definitions(${PROJECT_NAME}
	PUBLIC
	HAS_COM_EVENT_SEND_TRIGGER
)
endif()
if(ARA_ENABLE_COM_EVENT_LOST_CHECKER)
target_compile_definitions(${PROJECT_NAME}
	PUBLIC
	HAS_COM_EVENT_LOST_CHECKER
)
endif()
target_include_directories(${PROJECT_NAME}
	PUBLIC
	$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
	$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

find_package(ara-core-types REQUIRED)
find_package(isoft-manifestreader REQUIRED)
target_link_libraries(${PROJECT_NAME}
	PUBLIC
	ara::core-types
	PRIVATE
	isoft::manifestreader
)
if(NOT TARGET ara_com_log)
find_package(ara-com-log REQUIRED)
endif()
target_link_libraries(${PROJECT_NAME}
	PUBLIC
	ara_com_log
)
if(ARA_ENABLE_COM_TIMER)
target_sources(${PROJECT_NAME}
	PRIVATE
	time.cpp
)
target_compile_definitions(${PROJECT_NAME}
	PUBLIC
	HAS_COM_TIMER
)
endif()
if(ARA_ENABLE_COM_E2E)
if(NOT TARGET ara_com_e2e)
find_package(ara-com-e2e REQUIRED)
endif()
target_link_libraries(${PROJECT_NAME}
	PUBLIC
	ara_com_e2e
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
