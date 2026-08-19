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
# @date       2021-07-19
# @author     jiawei
# @version    1.2.0
#
# ================================================================

set(LIBRARY_NAME "ara_ucm_libtransfer")
message("CMAKE_CURRENT_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}")
find_package(ara-per REQUIRED)

set(lib_src
    transfer_instance.cpp
	software_package_data.cpp
	per_transfer_status_storage.cpp
)

if(ARA_WITH_UCM_LIBS_BUILD_SHARED_LIBS)
	add_library(
		${LIBRARY_NAME}
		SHARED
			${lib_src}
	)
else()
	add_library(
		${LIBRARY_NAME}
		STATIC
			${lib_src}
	)
endif()

target_include_directories(
	${LIBRARY_NAME}
	PUBLIC
		$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_link_libraries(
	${LIBRARY_NAME}
	PRIVATE
		ara::log
		ara::per
		ara::core-types
)


target_compile_options(${LIBRARY_NAME} PRIVATE -pthread)

set_target_properties(
	${LIBRARY_NAME}
	PROPERTIES
		SOVERSION ${PROJECT_VERSION_MAJOR}
		VERSION ${PROJECT_VERSION}
		OUTPUT_NAME ${LIBRARY_NAME}
		EXPORT_NAME ${LIBRARY_NAME}
)

add_library(ara::${LIBRARY_NAME} ALIAS ${LIBRARY_NAME})

install(
	TARGETS ${LIBRARY_NAME}
	EXPORT ${PROJECT_NAME}-targets
	ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
	LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
)