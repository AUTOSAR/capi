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
# @date       2021-05-20
# @author     git
# @version    1.2.0
#
# ================================================================

cmake_minimum_required(VERSION 3.4)
project(ara-sm VERSION 1.0.0 LANGUAGES CXX)
option(ARA_WITH_SM_DAEMON                       "build ara with sm daemon" ON)
option(ARA_WITH_SM_BUILD_FG_SHARED_LIBS         "build ara with sm shared libs" ON)
option(ARA_WITH_SM_BUILD_UTILS_SHARED_LIBS      "build ara with utils shared libs" ON)
option(ARA_WITH_SM_COMPILE_PHM "build with sm compile phm" ON)
option(ARA_WITH_SM_USE_INTERNAL_PHM_MODEL "build with sm compile phm and use phm models in sm misc" ON) # Only turn on when compiling smd with phm_comm module separately
add_subdirectory(sm)
set(APP_DIR "${CMAKE_CURRENT_SOURCE_DIR}/app")
if(EXISTS "${APP_DIR}" AND IS_DIRECTORY "${APP_DIR}")
    add_subdirectory(app)
else()
    message(STATUS "Directory does not exist: ${APP_DIR}")
endif()