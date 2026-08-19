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
# @brief      cmake configuration file
# @details
# @date       2021-10-26
# @author     fengjian
# @version    1.2.0
#
# ================================================================
#
# Debugging self-compilation of lib & tsyncd used, two projects for ap-all, because the trimming macros corresponding to lib & tsyncd are different: ara_per, ara_log
#
# ================================================================

cmake_minimum_required(VERSION 3.10)
project(None) # placeholder only
set(pub_add TRUE)
add_subdirectory(pub)
add_subdirectory(tsync_client)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(ARA_CMAKE_INSTALL_BIN "bin")
option(ARA_WITH_TSYNC_DAEMON "" ON)
if (ARA_WITH_TSYNC_DAEMON)
    add_subdirectory(tsyncd)
endif()