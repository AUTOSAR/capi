#!/bin/sh

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
# @file       run.sh
# @brief
# @details
# @date       2024-06-01
# @author     yanglong.wang
# @version    1.2.0
#
# ================================================================

echo $(pwd)
if [ "$#" -ne 2 ]; then
    echo "Error: Two parameters are required."
    echo "Usage: $0 <ARA_SYSROOT_PATH> <EXE_PATH>"
    exit 1
fi

export ARA_SYSROOT=$1
export LD_LIBRARY_PATH=${ARA_SYSROOT}/usr/lib:${ARA_SYSROOT}/ara/framework/1.0.0/lib:${ARA_SYSROOT}/ara/framework/1.0.0/lib64:${LD_LIBRARY_PATH}
export ISOFT_ARA_FSH_SYSROOT=${ARA_SYSROOT}
export ISOFT_ARA_FSH_CONFIG_FILE_NAME=ara_ver1.json
export ISOFT_ARA_FSH_PLATFORM_VERSION=1.0.0
export ISOFT_ARA_FSH_SWCL_NAME=core
export ISOFT_ARA_FSH_SWCL_VERSION=1.2.0
export ISOFT_ARA_FSH_PROC_NAME=phmd_process1
export ISOFT_ARA_RUNTIME_DIR=/tmp/ara/
# export PHM_CONTRIBUTION_PATH_ONLY_IT=$(pwd)/phm_contribute.json

$2
