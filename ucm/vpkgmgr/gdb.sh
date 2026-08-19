#!/bin/bash

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
# @file       gdb.sh
# @brief
# @details
# @date       2022-09-19
# @author     zhaoyunfei
# @version    1.2.0
#
# ================================================================
#
# export LD_LIBRARY_PATH=/usr/lib:/usr/lib64:/usr/local/lib:/usr/local/lib64:/home/zhaoyunfei/ara-sysroot/ara/core/1.0.0/lib:/home/zhaoyunfei/ara-sysroot/ara/core/1.0.0/lib64
# export ISOFT_ARA_FSH_SYSROOT="/home/zhaoyunfei/ara-sysroot"
# export ISOFT_ARA_FSH_PLATFORM_VERSION="1.0.0"
# export ISOFT_ARA_FSH_SWCL_NAME="core"
# export ISOFT_ARA_FSH_SWCL_VERSION="1.0.0"
# export ISOFT_ARA_FSH_PROC_NAME="ucmm"
# sudo gdb ./build/src/ucmmd
#
# ================================================================

sudo /usr/bin/gdb "$@"
