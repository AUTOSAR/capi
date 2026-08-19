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

set(LIBRARY_NAME "isoft_manifestreader")
set(TARGET_NAME manifestreader)

file(GLOB_RECURSE SRC_LIST CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
message( STATUS "PROJECT_NAME = ${PROJECT_NAME}")
foreach( srcFileName ${SRC_LIST})
    message(STATUS "      srcFileName = ${srcFileName}")
endforeach()

add_library( ${TARGET_NAME} SHARED
	${SRC_LIST}
)
target_include_directories( ${TARGET_NAME}
	PUBLIC
		$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/>
		$<INSTALL_INTERFACE:include>
	PRIVATE
		$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/private/>
)

target_link_libraries( ${TARGET_NAME}
	PUBLIC
		ara::core-types
		RapidJSON
)

set_target_properties( ${TARGET_NAME}
	PROPERTIES
		SOVERSION ${PROJECT_VERSION_MAJOR}
		VERSION ${PROJECT_VERSION}
		OUTPUT_NAME ${LIBRARY_NAME}
)

add_library(isoft::manifestreader ALIAS ${TARGET_NAME})

add_dependencies(${TARGET_NAME}
    ara::core-types
	isoft::common
)

if(PKG_CONFIG_FOUND)
	configure_file(${PROJECT_SOURCE_DIR}/misc/${PROJECT_NAME}.pc.in ${PROJECT_NAME}.pc @ONLY)
	install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc" DESTINATION ${CMAKE_INSTALL_LIBDIR})
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

install(
	TARGETS ${TARGET_NAME}
	EXPORT ${PROJECT_NAME}-targets
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Install the imported target definitions
install(EXPORT ${PROJECT_NAME}-targets
    NAMESPACE isoft::
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
	COMPONENT dev
)

# Install header files
install(DIRECTORY
	${PROJECT_SOURCE_DIR}/include/
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
	COMPONENT dev
)

