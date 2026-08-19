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

if(NOT ARA_ENABLE_FSH)
add_subdirectory(fsh)
endif()

add_subdirectory(log)

if(ARA_ENABLE_COM_E2E)
add_subdirectory(e2e)
endif()

if(ARA_ENABLE_COM_SECOC)
add_subdirectory(secoc)
endif()

add_subdirectory(com)

if(ARA_ENABLE_COM_NSOMEIP)
add_subdirectory(nsomeip)
endif()

if(ARA_ENABLE_COM_FSOMEIP)
add_subdirectory(fsomeip)
endif()

if(ARA_ENABLE_COM_FASTDDS)
add_subdirectory(fastdds)
endif()

if(ARA_ENABLE_COM_NPC)
add_subdirectory(npc)
endif()

if(ARA_ENABLE_COM_ICC)
add_subdirectory(icc)
endif()

if(ARA_ENABLE_COM_XPENG)
add_subdirectory(xpeng)
endif()