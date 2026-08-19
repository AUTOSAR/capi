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
# @date       2021-12-17
# @author     hanjingjing
# @version    1.2.0
#
# ================================================================

cmake_minimum_required(VERSION 3.16)
project(ara-crypto LANGUAGES C CXX VERSION 1.1.0)

find_package(isoft-cmake-common REQUIRED) 

option(ARA_BUILD_CRYPTO_KEY_DAEMON  "Enable build with cryptod" OFF)
option(ARA_BUILD_CRYPTO_X509_DAEMON "Enable build with cryptox509d" OFF)
option(ARA_WITH_CRYPTO_HSM_NXP      "Enable build with hsm_nxp" OFF)
option(ARA_WITH_CRYPTO_HSM_BST      "Enable build with hsm_bst" OFF)
option(ARA_WITH_CRYPTO_PLUGIN       "Enable build with crypto_plugin" OFF)

add_subdirectory(ara_crypto)

add_subdirectory(lib_isoft)

add_subdirectory(lib_x509)

if(ARA_BUILD_CRYPTO_KEY_DAEMON)
    add_subdirectory(cryptod)
endif()

if(ARA_BUILD_CRYPTO_X509_DAEMON)
    add_subdirectory(cryptox509d)
endif()

if(ARA_WITH_CRYPTO_HSM_NXP)
    message(STATUS "****** ENABLE ARA_CRYPTO_HSM_NXP ******")
    add_subdirectory(hsm_nxp)
endif()

if(ARA_WITH_CRYPTO_HSM_BST)
    message(STATUS "****** ENABLE ARA_CRYPTO_HSM_BST ******")
    add_subdirectory(hsm_bst)
endif()

if(ARA_WITH_CRYPTO_PLUGIN)
    message(STATUS "****** ENABLE opensssl crypto_plugin ******")
    add_subdirectory(crypto_plugin)
endif()
