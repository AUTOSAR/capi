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

cmake_minimum_required(VERSION 3.12)
project(ara_com_e2e VERSION 1.1.0 LANGUAGES CXX)
string(REPLACE "_" "-" EXPORT_NAME ${PROJECT_NAME})

add_subdirectory(src)


if(ARA_ENABLE_TESTS)
	enable_testing()
	find_package(GTest REQUIRED CONFIG)
	add_subdirectory(test)
endif(ARA_ENABLE_TESTS)
