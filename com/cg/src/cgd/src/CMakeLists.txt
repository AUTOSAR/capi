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
# @date       2022-06-27
# @author     jiawei
# @version    1.2.0
#
# ================================================================

set(TARGET_NAME ${ARA_GEN_EXECUTABLE})
file(GLOB_RECURSE SRC_LIST ${PROJECT_SOURCE_DIR}/src/*.cpp )

add_executable(${TARGET_NAME}
	${SRC_LIST}
)
set_target_properties(${TARGET_NAME}
	PROPERTIES
	OUTPUT_NAME ${ARA_GEN_EXECUTABLE}
)

add_dependencies(${TARGET_NAME}
	${ARA_GEN_BUILD_HELPER_LIBRARY}
)
target_link_libraries(${TARGET_NAME}
	${ARA_GEN_BUILD_HELPER_LIBRARY}
	Threads::Threads
)
if(NOT TARGET ara_com)
find_package(ara-com REQUIRED)
endif()
target_link_libraries(${TARGET_NAME}
	ara_com
)
if(NOT TARGET ara_com_nsomeip)
find_package(ara-com-nsomeip REQUIRED)
endif()
target_link_libraries(${TARGET_NAME}
	ara_com_nsomeip
)
if(NOT TARGET ara_com_cg)
find_package(ara-com-cg REQUIRED)
endif()
target_link_libraries(${TARGET_NAME}
	ara_com_cg
)

# isoft-ap unified handling of target
aragen_helper_register_target(TARGET ${TARGET_NAME} TYPE EXE)