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
# @date       2021-04-19
# @author     jian.feng
# @version    1.2.0
#
# ================================================================

# Purpose: Used only for the development team to conveniently build isoft-ap as a whole
# Includes: All modules of isoft-ap and dependent modules
# Should not include: Any isoft-ap module configuration logic
# Other notes: This CMakeLists is not part of isoft-ap

cmake_minimum_required(VERSION 3.16)

project(ap-all VERSION 1.0.0 LANGUAGES C CXX)

# Include configuration system
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(Config)
include(Utils)
include(Modules)

# Validate configuration
config_validation()

# Check module third-party external dependencies
module_check_dependency()

# Add all modules to cmake project
module_add_all_projects()


if(ARA_ENABLE_SAMPLES)
    add_subdirectory(samples)
endif()
