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
# @date       2022-06-17
# @author     mazelin
# @version    1.2.0
#
# ================================================================

cmake_minimum_required(VERSION 3.4)
project(None) 

include(gen/aragen-helper.cmake)



set(ARA_GEN_ARXMLS ${ARA_GEN_ARXMLS}
	${ARA_ARXMLS_DIR}/common
	${ARA_ARXMLS_DIR}/development_machines/system
	${ARA_ARXMLS_DIR}/development_machines/machine1/common
	${ARA_ARXMLS_DIR}/development_machines/machine2/common

	${ARA_ARXMLS_DIR}/platform_components/com/daemon
	${ARA_ARXMLS_DIR}/development_machines/machine1/processes/nsomeipd
	${ARA_ARXMLS_DIR}/development_machines/machine2/processes/nsomeipd
) # arxmls

aragen_helper_add_build_helper_library()


add_subdirectory(src)

if(ARA_ENABLE_TESTS)
	enable_testing()
	find_package(GTest REQUIRED CONFIG)
	add_subdirectory(test)
endif(ARA_ENABLE_TESTS)
