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

find_package(ara-crypto REQUIRED)
find_package(ara-ucm-libs REQUIRED)

file(GLOB_RECURSE FSM_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/fsm/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/fsm/approvals/*.cpp 
	${CMAKE_CURRENT_SOURCE_DIR}/fsm/campaign_steps/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/fsm/package_transfer/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/fsm/parsing/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/fsm/states/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/fsm/storage/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/fsm/task_executor/*.cpp
) 

file(GLOB_RECURSE PKGMNG_SERVICE_SOURCES 
	${CMAKE_CURRENT_SOURCE_DIR}/package_management_service/*.cpp
)

file(GLOB_RECURSE SERVICES_SOURCES 
	${CMAKE_CURRENT_SOURCE_DIR}/services/vehicle_driver_application/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/services/vehicle_package_management/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/services/vehicle_state_manager/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/services/*.cpp
)

file(GLOB_RECURSE UTILS_SOURCES 
	${CMAKE_CURRENT_SOURCE_DIR}/utils/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/other/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/utils/config*.cpp
)

include_directories( ${CMAKE_CURRENT_SOURCE_DIR})
set(EXE_target ${ARA_GEN_EXECUTABLE})
set(UCMM_MAIN_COMMON common_${EXE_target})

# ucmm object lib for internal use
add_library(${UCMM_MAIN_COMMON} STATIC
	${FSM_SOURCES}
	${PKGMNG_SERVICE_SOURCES}
	${SERVICES_SOURCES}
	${UTILS_SOURCES}
	main.cpp
	vehicle_package_application.cpp
)

target_link_libraries(${UCMM_MAIN_COMMON}
	PUBLIC
	${ARA_GEN_BUILD_HELPER_LIBRARY}
	ara::ara_ucm_libtransfer
	ara::ara_ucm_libextraction
	ara::ara_ucm_libcrypto
)

add_library(ara::lib_ucmm_main ALIAS ${UCMM_MAIN_COMMON})

set(EXE_src
    main.cpp
)
add_executable(${EXE_target} ${EXE_src})

set_target_properties(${EXE_target}
    PROPERTIES
        OUTPUT_NAME ${ARA_GEN_EXECUTABLE}
    )

target_link_libraries(${EXE_target}
	PUBLIC
    ${UCMM_MAIN_COMMON}
	${ARA_GEN_BUILD_HELPER_LIBRARY}
	ara::ara_ucm_libtransfer
	ara::ara_ucm_libextraction
)

add_dependencies(${EXE_target} ${ARA_GEN_BUILD_HELPER_LIBRARY})
# isoft-ap unified handling for target
aragen_helper_register_target(TARGET ${EXE_target} TYPE EXE)

