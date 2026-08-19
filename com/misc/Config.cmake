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
# @file       Config.cmake
# @brief
# @details
# @date       2022-01-06
# @author     mazelin
# @version    1.2.0
#
# ================================================================

include(GNUInstallDirs)
macro(Init)
	if(NOT DEFINED GLOB_CUSTOM_CONFIG)
		set(GLOB_CUSTOM_CONFIG ${CMAKE_CURRENT_LIST_FILE})
	endif()
	if(NOT DEFINED GLOB_SOURCE_DIR)
		set(GLOB_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
	endif()

	if(NOT DEFINED ARA_SYSROOT)
		if(DEFINED ENV{ARA_SYSROOT})
			set(ARA_SYSROOT "$ENV{ARA_SYSROOT}")
		else()
			set(ARA_SYSROOT "$ENV{HOME}/ara-sysroot")
		endif()
	endif()

	set(ENABLE_OUTER_PROJECT OFF)

	if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
		if(ANDROID)
			set(__android__ 1)
			add_compile_definitions(__android__)
		else()
			set(__linux__ 1)
		endif()
	endif()
	if(${CMAKE_SYSTEM_NAME} MATCHES "QNX")
		set(__QNX__ 1)
	else()
		# set(CMAKE_CXX_STANDARD 14)
		# set(CMAKE_CXX_STANDARD_REQUIRED ON)
		# set(CMAKE_CXX_EXTENSIONS OFF)
		# set(CMAKE_C_STANDARD 99)
		# set(CMAKE_C_STANDARD_REQUIRED ON)
		# set(CMAKE_C_EXTENSIONS OFF)
	endif()

	# add_compile_options("-Werror")
	# add_compile_options("-Wno-pedantic")
	# add_compile_options("-Wno-old-style-cast")
	# add_compile_options("-Wno-unknown-pragmas")

	option(BUILD_ALWAYS "Build always" ON)
	option(BUILD_SHARED_LIBS "Build shared libraries" ON)
	option(ARA_ENABLE_COM_NSOMEIP "Enable ara com nsomeip binding" ON)
	option(ARA_ENABLE_COM_FSOMEIP "Enable ara com fsomeip binding" OFF)
	option(ARA_ENABLE_COM_FASTDDS "Enable ara com fastdds binding" ON)
	option(ARA_ENABLE_COM_NPC "Enable ara com npc binding" ON)
	option(ARA_ENABLE_COM_ICC "Enable ara com icc binding" ON)
	option(ARA_ENABLE_COM_TIMER "Enable ara com timer" ON)
	option(ARA_ENABLE_COM_E2E "Enable ara com E2E" ON)
	option(ARA_ENABLE_COM_SECOC "Enable ara com SecOC" ON)
	option(ARA_ENABLE_COM_CG "Enable ara com CG" ON)
	option(ARA_ENABLE_COM_RAW "Enable ara com RAW" ON)
	option(ARA_ENABLE_COM_ROUTING "Enable ara com Routing" ON)
	option(ARA_ENABLE_COM_MONITOR "Enable ara com monitor" ON)
	option(ARA_ENABLE_COM_NET_FILTER "Enable ara com net filter" ON)
	option(ARA_ENABLE_COM_EVENT_SEND_TRIGGER "Enable ara com event send trigger" ON)
	option(ARA_ENABLE_COM_EVENT_LOST_CHECKER "Enable ara com event lost checker" ON)
	option(ARA_ENABLE_FSH "Enable ara fsh" ON)
	option(ARA_ENABLE_LOG "Enable ara log" ${ARA_WITH_LOG})
	option(ARA_ENABLE_IAM "Enable ara IAM" ${ARA_WITH_IAM})
	option(ARA_ENABLE_EXEC "Enable ara exec" ${ARA_WITH_EXEC})
	option(ARA_ENABLE_EXAMPLES "Enable ara examples" ${ARA_ENABLE_SAMPLES})
	option(ARA_ENABLE_TESTS "Enable ara tests" ON)

	message(STATUS "BUILD_ALWAYS=${BUILD_ALWAYS}")
	message(STATUS "BUILD_SHARED_LIBS=${BUILD_SHARED_LIBS}")
	message(STATUS "ARA_SYSROOT=${ARA_SYSROOT}")
	message(STATUS "ARA_ENABLE_COM_NSOMEIP=${ARA_ENABLE_COM_NSOMEIP}")
	message(STATUS "ARA_ENABLE_COM_FSOMEIP=${ARA_ENABLE_COM_FSOMEIP}")
	message(STATUS "ARA_ENABLE_COM_FASTDDS=${ARA_ENABLE_COM_FASTDDS}")
	message(STATUS "ARA_ENABLE_COM_NPC=${ARA_ENABLE_COM_NPC}")
	message(STATUS "ARA_ENABLE_COM_ICC=${ARA_ENABLE_COM_ICC}")
	message(STATUS "ARA_ENABLE_COM_TIMER=${ARA_ENABLE_COM_TIMER}")
	message(STATUS "ARA_ENABLE_COM_E2E=${ARA_ENABLE_COM_E2E}")
	message(STATUS "ARA_ENABLE_COM_SECOC=${ARA_ENABLE_COM_SECOC}")
	message(STATUS "ARA_ENABLE_COM_CG=${ARA_ENABLE_COM_CG}")
	message(STATUS "ARA_ENABLE_COM_RAW=${ARA_ENABLE_COM_RAW}")
	message(STATUS "ARA_ENABLE_COM_ROUTING=${ARA_ENABLE_COM_ROUTING}")
	message(STATUS "ARA_ENABLE_COM_MONITOR=${ARA_ENABLE_COM_MONITOR}")
	message(STATUS "ARA_ENABLE_COM_NET_FILTER=${ARA_ENABLE_COM_NET_FILTER}")
	message(STATUS "ARA_ENABLE_COM_EVENT_SEND_TRIGGER=${ARA_ENABLE_COM_EVENT_SEND_TRIGGER}")
	message(STATUS "ARA_ENABLE_COM_EVENT_LOST_CHECKER=${ARA_ENABLE_COM_EVENT_LOST_CHECKER}")
	message(STATUS "ARA_ENABLE_FSH=${ARA_ENABLE_FSH}")
	message(STATUS "ARA_ENABLE_LOG=${ARA_ENABLE_LOG}")
	message(STATUS "ARA_ENABLE_IAM=${ARA_ENABLE_IAM}")
	message(STATUS "ARA_ENABLE_EXEC=${ARA_ENABLE_EXEC}")
	message(STATUS "ARA_ENABLE_EXAMPLES=${ARA_ENABLE_EXAMPLES}")
	message(STATUS "ARA_ENABLE_TESTS=${ARA_ENABLE_TESTS}")

	set(ARA_CMAKE_ARGS
		${CMAKE_ARGS}
		-DCMAKE_VERBOSE_MAKEFILE=\"${CMAKE_VERBOSE_MAKEFILE}\" # ON|OFF
		-DCMAKE_BUILD_TYPE=\"${CMAKE_BUILD_TYPE}\" # Debug|Release|RelWithDebInfo|MinSizeRel
		-DCMAKE_BUILD_PARALLEL_LEVEL=\"${CMAKE_BUILD_PARALLEL_LEVEL}\" # `cat /proc/cpuinfo | grep processor | wc -l`
		-DCMAKE_PREFIX_PATH=\"${CMAKE_PREFIX_PATH}\" # ${ARA_SYSROOT}/usr
		-DCMAKE_MODULE_PATH=\"${CMAKE_MODULE_PATH}\" # ${ARA_SYSROOT}/usr
		-DCMAKE_INSTALL_PREFIX=\"${CMAKE_INSTALL_PREFIX}\" # ${ARA_SYSROOT}/ara/framework/x.x.x
		-DCMAKE_INSTALL_LIBDIR=\"${CMAKE_INSTALL_LIBDIR}\" # lib
		-DCMAKE_FIND_ROOT_PATH=\"${CMAKE_FIND_ROOT_PATH}\" # ${ARA_SYSROOT}
		-DBUILD_ALWAYS=\"${BUILD_ALWAYS}\" # ON|OFF
		-DBUILD_SHARED_LIBS=\"${BUILD_SHARED_LIBS}\" # ON|OFF
		-DARA_SYSROOT=\"${ARA_SYSROOT}\"
		-DARA_ENABLE_COM_NSOMEIP=\"${ARA_ENABLE_COM_NSOMEIP}\" # ON|OFF
		-DARA_ENABLE_COM_FSOMEIP=\"${ARA_ENABLE_COM_FSOMEIP}\" # ON|OFF
		-DARA_ENABLE_COM_FASTDDS=\"${ARA_ENABLE_COM_FASTDDS}\" # ON|OFF
		-DARA_ENABLE_COM_NPC=\"${ARA_ENABLE_COM_NPC}\" # ON|OFF
		-DARA_ENABLE_COM_ICC=\"${ARA_ENABLE_COM_ICC}\" # ON|OFF
		-DARA_ENABLE_COM_TIMER=\"${ARA_ENABLE_COM_TIMER}\" # ON|OFF
		-DARA_ENABLE_COM_E2E=\"${ARA_ENABLE_COM_E2E}\" # ON|OFF
		-DARA_ENABLE_COM_SECOC=\"${ARA_ENABLE_COM_SECOC}\" # ON|OFF
		-DARA_ENABLE_COM_CG=\"${ARA_ENABLE_COM_CG}\" # ON|OFF
		-DARA_ENABLE_COM_RAW=\"${ARA_ENABLE_COM_RAW}\" # ON|OFF
		-DARA_ENABLE_COM_ROUTING=\"${ARA_ENABLE_COM_ROUTING}\" # ON|OFF
		-DARA_ENABLE_COM_MONITOR=\"${ARA_ENABLE_COM_MONITOR}\" # ON|OFF
		-DARA_ENABLE_COM_NET_FILTER=\"${ARA_ENABLE_COM_NET_FILTER}\" # ON|OFF
		-DARA_ENABLE_COM_EVENT_SEND_TRIGGER=\"${ARA_ENABLE_COM_EVENT_SEND_TRIGGER}\" # ON|OFF
		-DARA_ENABLE_COM_EVENT_LOST_CHECKER=\"${ARA_ENABLE_COM_EVENT_LOST_CHECKER}\" # ON|OFF
		-DARA_ENABLE_FSH=\"${ARA_ENABLE_FSH}\" # ON|OFF
		-DARA_ENABLE_LOG=\"${ARA_ENABLE_LOG}\" # ON|OFF
		-DARA_ENABLE_IAM=\"${ARA_ENABLE_IAM}\" # ON|OFF
		-DARA_ENABLE_EXEC=\"${ARA_ENABLE_EXEC}\" # ON|OFF
		-DARA_ENABLE_EXAMPLES=\"${ARA_ENABLE_EXAMPLES}\" # ON|OFF
		-DARA_ENABLE_TESTS=\"${ARA_ENABLE_TESTS}\" # ON|OFF
		-DARA_LOG_WITHOUT_MODEL_MESSAGE=1 # adapt for log module
	)
endmacro()

include(ExternalProject)
function(OuterProject)
	set(options ENABLE_TEST)
	set(oneValueArgs NAME SOURCE_DIR)
	set(multiValueArgs DOWNLOAD_COMMAND BUILD_COMMAND INSTALL_COMMAND CMAKE_ARGS DEPENDS)
	cmake_parse_arguments(PRJ "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})
	if(DEFINED PRJ_DOWNLOAD_COMMAND)
		set(DOWNLOAD_COMMAND ${PRJ_DOWNLOAD_COMMAND})
	else()
		set(DOWNLOAD_COMMAND echo "no download step for ${PRJ_NAME}")
	endif()
	if(DEFINED PRJ_BUILD_COMMAND)
		set(BUILD_COMMAND ${PRJ_BUILD_COMMAND})
	else()
		set(BUILD_COMMAND ${CMAKE_COMMAND} --build <BINARY_DIR> --parallel 10 -j 20)
	endif()
	if(DEFINED PRJ_INSTALL_COMMAND)
		set(INSTALL_COMMAND ${PRJ_INSTALL_COMMAND})
	else()
		set(INSTALL_COMMAND echo "no install step for ${PRJ_NAME}")
	endif()
	message("NAME: " ${PRJ_NAME})
	message("SOURCE_DIR: " ${PRJ_SOURCE_DIR})
	ExternalProject_Add(
		${PRJ_NAME}
		DOWNLOAD_COMMAND ${PRJ_DOWNLOAD_COMMAND}
		SOURCE_DIR ${PRJ_SOURCE_DIR}
		CMAKE_ARGS ${ARA_CMAKE_ARGS} ${PRJ_CMAKE_ARGS}
		BUILD_ALWAYS ${BUILD_ALWAYS}
		BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS}
		BUILD_COMMAND ${BUILD_COMMAND}
		INSTALL_COMMAND ${INSTALL_COMMAND}
		DEPENDS ${PRJ_DEPENDS}
	)
endfunction()

Init()