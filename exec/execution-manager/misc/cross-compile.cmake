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
# @file       cross-compile.cmake
# @brief
# @details
# @date       2021-06-21
# @author     james
# @version    1.2.0
#
# ================================================================

SET(CROSS_COMPILE ON)
SET(ARA_SYSROOT $ENV{HOME}/ara-sysroot)

IF(CROSS_COMPILE)  
    # Path to cross-compilation toolchain
    SET(TOOLCHAIN_DIR "$ENV{HOME}/toolchain/gcc-linaro-6.3.1")
    # Installation and search paths for runtime libraries, headers, etc. after cross-compilation
    SET(SYSROOT_PATH ${ARA_SYSROOT})

    SET(CMAKE_SYSTEM_NAME Linux)
    set(CMAKE_CXX_COMPILER ${TOOLCHAIN_DIR}/bin/aarch64-linux-gnu-g++)
    set(CMAKE_C_COMPILER   ${TOOLCHAIN_DIR}/bin/aarch64-linux-gnu-gcc)
    #set(GNU_FLAGS "-mfpu=vfp -fPIC")
    set(CMAKE_CXX_FLAGS "${GNU_FLAGS} ")
    set(CMAKE_C_FLAGS "${GNU_FLAGS}  ")

    SET(CMAKE_SYSROOT ${SYSROOT_PATH})
    SET(CMAKE_FIND_ROOT_PATH  
        ${SYSROOT_PATH}/
        #${TOOLCHAIN_DIR}/aarch64-linux-gnu/libc/usr/include
        #${TOOLCHAIN_DIR}/aarch64-linux-gnu/include/c++/6.3.1
        )

    #include_directories(${SYSROOT_PATH}/usr/include)
    #link_directories(/home/zchx/Downloads/boost_1_49_0_arm/stage/lib)
ENDIF(CROSS_COMPILE)
