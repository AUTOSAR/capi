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

cmake_minimum_required(VERSION 3.16)

project(logModule)

# Enable CTest/ctest integration once at the top level so subprojects can register tests.

# Stop on compilation error - treat compilation warnings as errors
add_compile_options(-Werror)

# set(ARA_LOG_HAS_REMOTE ON)
option(ARA_ENABLE_TESTS "test switch" ON)

# set(ARA_ENABLE_AP_ENV ON)
message(STATUS "Parent: ARA_WITH_TSYNC = ${ARA_WITH_TSYNC}")

add_subdirectory(src)
