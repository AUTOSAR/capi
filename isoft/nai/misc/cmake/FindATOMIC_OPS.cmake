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
# @file       FindATOMIC_OPS.cmake
# @brief
# @details
# @date       2021-06-22
# @author     xn
# @version    1.2.0
#
# ================================================================
#
# - Find atomic_ops
# Find the native ATOMIC_OPS headers and libraries.
# ATOMIC_OPS_INCLUDE_DIRS - where to find atomic_ops.h, etc.
# ATOMIC_OPS_LIBRARIES    - List of libraries when using atomic_ops.
# ATOMIC_OPS_FOUND        - True if atomic_ops found.
# Look for the header file.
#
# ================================================================

FIND_PATH(ATOMIC_OPS_INCLUDE_DIR NAMES atomic_ops.h)

# Look for the library.
FIND_LIBRARY(ATOMIC_OPS_LIBRARY NAMES atomic_ops)
FIND_LIBRARY(ATOMIC_OPS_STATIC_LIBRARY NAMES 
    ${CMAKE_STATIC_LIBRARY_PREFIX}atomic_ops${CMAKE_STATIC_LIBRARY_SUFFIX})
IF(ATOMIC_OPS_LIBRARY)
    LIST(APPEND ATOMIC_OPS_LIBRARIES ${ATOMIC_OPS_LIBRARY})
ENDIF()
IF(ATOMIC_OPS_STATIC_LIBRARY)
    LIST(APPEND ATOMIC_OPS_LIBRARIES ${ATOMIC_OPS_STATIC_LIBRARY})
ENDIF()

message(STATUS "lib suffix: " ${CMAKE_STATIC_LIBRARY_SUFFIX})

# handle the QUIETLY and REQUIRED arguments and set ATOMIC_OPS_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(ATOMIC_OPS DEFAULT_MSG 
    ATOMIC_OPS_LIBRARIES ATOMIC_OPS_INCLUDE_DIR)

# Copy the results to the output variables.
IF(ATOMIC_OPS_FOUND)
  SET(ATOMIC_OPS_LIBRARIES ${ATOMIC_OPS_LIBRARY})
  SET(ATOMIC_OPS_STATIC_LIBRARIES ${ATOMIC_OPS_STATIC_LIBRARY})
  SET(ATOMIC_OPS_INCLUDE_DIRS ${ATOMIC_OPS_INCLUDE_DIR})
ELSE(ATOMIC_OPS_FOUND)
  SET(ATOMIC_OPS_LIBRARIES)
  SET(ATOMIC_OPS_INCLUDE_DIRS)
ENDIF(ATOMIC_OPS_FOUND)

MARK_AS_ADVANCED(ATOMIC_OPS_INCLUDE_DIR ATOMIC_OPS_LIBRARY)

