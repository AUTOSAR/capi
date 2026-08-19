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
# @brief      fw module compilation
# @details
# @date       2024-11-26
# @author     jiangzhiyuan
# @version    1.2.0
#
#
# ================================================================

set(LIBRARY_NAME ara-fw-engine)
set(LIBRARY_FILE_NAME ara_fw_engine)
set(EXPORT_NAME fw_engine)
#Define project version
set(FW_ENGINE_PROJECT_VERSION 1.0.0)
message(STATUS "<<<<< Begin : ${LIBRARY_NAME} Src > >> >>")

message(STATUS "<<<<< ARA_SYSROOT = ${ARA_SYSROOT} > >> >>")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include)

file(GLOB_RECURSE SRC_PH_LIST CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

message(STATUS "<<<<< SRC_PH_LIST = ${CMAKE_CURRENT_SOURCE_DIR}")
foreach( srcFileName ${SRC_PH_LIST})
    message(STATUS "      srcFileName = ${srcFileName}")
endforeach()

find_package(ara-log REQUIRED)
find_package(ara-core-types REQUIRED)

# 4. Unified build shared library (contains all conditionally appended target files)
if(ARA_WITH_FW_ENGINE_BUILD_SHARED_LIBS)
	add_library(${LIBRARY_NAME} SHARED ${SRC_PH_LIST})
else()
    add_library(${LIBRARY_NAME} STATIC ${SRC_PH_LIST})
endif()

add_library(ara::fw::${EXPORT_NAME} ALIAS ${LIBRARY_NAME})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-stringop-overflow")

# Include file paths
target_include_directories(${LIBRARY_NAME}
	PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../libfwclient/include>
		$<INSTALL_INTERFACE:include>
)

# Link extensions
target_link_libraries(${LIBRARY_NAME} 
	ara::core-types
	ara::log
)

set_target_properties(${LIBRARY_NAME} PROPERTIES
	OUTPUT_NAME ${LIBRARY_FILE_NAME}
	EXPORT_NAME ${EXPORT_NAME}
)


include(FindPkgConfig)
if(PKG_CONFIG_FOUND)
    set(LIBRARIES "-l${LIBRARY_FILE_NAME}")
	configure_file(${FW_ENGINE_ROOT_DIR}/misc/ara-firewall.pc.in ${LIBRARY_NAME}.pc @ONLY)
	install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${LIBRARY_NAME}.pc DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
endif()

#Create *-config.cmake file
include(CMakePackageConfigHelpers)
configure_package_config_file(
	${FW_ENGINE_ROOT_DIR}/misc/ara-firewall-config.cmake.in
	${CMAKE_CURRENT_BINARY_DIR}/${LIBRARY_NAME}-config.cmake
	INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${LIBRARY_NAME}
	NO_CHECK_REQUIRED_COMPONENTS_MACRO
)
# Create *-config-version.cmake file
write_basic_package_version_file(
	${CMAKE_CURRENT_BINARY_DIR}/${LIBRARY_NAME}-config-version.cmake
	VERSION ${FW_ENGINE_PROJECT_VERSION} 
	COMPATIBILITY ExactVersion
)
# Install *-config.cmake and *-version.cmake files
install(
	FILES
		${CMAKE_CURRENT_BINARY_DIR}/${LIBRARY_NAME}-config.cmake
		${CMAKE_CURRENT_BINARY_DIR}/${LIBRARY_NAME}-config-version.cmake
	DESTINATION
		${CMAKE_INSTALL_LIBDIR}/cmake/${LIBRARY_NAME}
)

# Install the actual targets
install(
	TARGETS ${LIBRARY_NAME}
	EXPORT ${LIBRARY_NAME}
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	)

# Install exported targets, i.e. *-targets.cmake
install(
	EXPORT ${LIBRARY_NAME}
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${LIBRARY_NAME}
    NAMESPACE ara::fw::
    FILE ara-fw-engine-targets.cmake
    )

# Install header files
install(
	DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../include/
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
	COMPONENT dev
	FILES_MATCHING PATTERN "*.h"
	)

message(STATUS "=====   End : ${LIBRARY_NAME} =====")
