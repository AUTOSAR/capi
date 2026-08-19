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
# @date       2025-10-14
# @author     jiangzhiyuan
# @version    1.2.0
#
#
# ================================================================

cmake_minimum_required(VERSION 3.10)
project(FwKernelModule)

# Automatically detect kernel version
execute_process(
    COMMAND uname -r
    OUTPUT_VARIABLE KERNEL_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Set kernel build directory
set(KERNEL_DIR /lib/modules/${KERNEL_VERSION}/build)

# Set your kernel module name, excluding '.ko' suffix
set(MODULE_NAME kfirewall_app)

# Add custom target to build kernel module
add_custom_target(${MODULE_NAME}_module ALL
    COMMAND make -C ${KERNEL_DIR} M=${CMAKE_CURRENT_SOURCE_DIR} modules
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Building kernel module ${MODULE_NAME}.ko using Kbuild"
)

# Optional: add clean target
add_custom_target(clean_${MODULE_NAME}
    COMMAND make -C ${KERNEL_DIR} M=${CMAKE_CURRENT_SOURCE_DIR} clean
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Cleaning kernel module ${MODULE_NAME}"
)


# 2. Install kernel module to specified location
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/kfirewall_app.ko
    DESTINATION ${CMAKE_INSTALL_BINDIR}
    OPTIONAL
)

install(
	DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/ara/fw/kernalModule
	COMPONENT dev
	FILES_MATCHING PATTERN "*.h"
	)



# # 2. Install kernel module to specified location
# install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/kfirewall_app.ko
#     DESTINATION ${CMAKE_INSTALL_PREFIX}/bin 
#     OPTIONAL
# )

# install(
# 	DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
# 	DESTINATION ${CMAKE_INSTALL_PREFIX}/include/ara/fw/kernalModule
# 	COMPONENT dev
# 	FILES_MATCHING PATTERN "*.h"
# 	)

