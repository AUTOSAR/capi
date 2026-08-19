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
# @date       2023-10-08
# @author     zhanglipeng
# @version    1.2.0
#
# ================================================================

cmake_minimum_required(VERSION 3.4)

include_directories(
    PUBLIC
        $<INSTALL_INTERFACE:include>
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/isoft-doip-common/src/>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../common/include>
)

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DARA_LOG_WITHOUT_MODEL_MESSAGE=1")

# If the external switch is passed, default to compiling as a shared library
if(NOT DEFINED ARA_WITH_DIAG_DOIP_BUILD_SHARED_LIBS)
    set(ARA_WITH_DIAG_DOIP_BUILD_SHARED_LIBS ON)
endif()

# DoIP common library
add_subdirectory(isoft-doip-common)

# DoIP server library
if(ARA_WITH_DIAG_DOIP_SERVER)
    add_subdirectory(isoft-doip-server)
endif()
# DoIP client library
if(ARA_WITH_DIAG_DOIP_CLIENT)
    add_subdirectory(isoft-doip-client)
endif()
