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
# @date       2022-07-14
# @author     hanjingjing
# @version    1.2.0
#
# ================================================================

cmake_minimum_required(VERSION 3.16)
project(ara-crypto LANGUAGES C CXX VERSION 1.1.0)

message(STATUS "=================== Begin : ${PROJECT_NAME} ================")
message(STATUS "  **  CMAKE_SOURCE_DIR          = ${CMAKE_SOURCE_DIR}")
message(STATUS "  **  CMAKE_INSTALL_PREFIX      = ${CMAKE_INSTALL_PREFIX}")
SET( CRYPTO_ROOT_DIR ${CMAKE_SOURCE_DIR} )
message(STATUS " ***  CRYPTO_ROOT_DIR           = ${CRYPTO_ROOT_DIR}")
message(STATUS "  **  LD_LIBRARY_PATH           = $ENV{LD_LIBRARY_PATH}")

option(ARA_ENABLE_DEMO "is enable demo"  OFF)
option(ARA_ENABLE_FORMAT_CODE_BEFORE_BUILD "code format before build" ON)

# message(STATUS "${PROJECT_NAME}: chejinzo PROJECT_SOURCE_DIR = ${PROJECT_SOURCE_DIR}")

if(ARA_ENABLE_EXAMPLE)
	add_subdirectory(example)
endif(ARA_ENABLE_EXAMPLE)

if(ARA_WITH_CRYPTO_HSM_NXP)
    message(STATUS "CMake Build NXP Mode : ARA_WITH_CRYPTO_HSM_NXP == true")
else()
    message(STATUS "CMake Build NXP Mode : ARA_WITH_CRYPTO_HSM_NXP == null")
endif()

if(ARA_WITH_CRYPTO_HSM_BST)
    message(STATUS "CMake Build BST Mode : ARA_WITH_CRYPTO_HSM_BST == true")
else()
    message(STATUS "CMake Build BST Mode : ARA_WITH_CRYPTO_HSM_BST == null")
endif()

add_subdirectory(src)

message(STATUS "=================== End   : ${PROJECT_NAME} ================")