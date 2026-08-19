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
# @file       runall.sh
# @brief
# @details
# @date       2024-06-01
# @author     yanglong.wang
# @version    1.2.0
#
# ================================================================

echo $(pwd)
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <build dir>"
    exit 1
fi

BUILD_DIR=$1

./run.sh /home/wangyanlong/ara-sysroot/  ${BUILD_DIR}/test/util/test-util  

./run.sh /home/wangyanlong/ara-sysroot/  ${BUILD_DIR}/test/com/base/test-common-com-server-default &  
sleep 1
./run.sh /home/wangyanlong/ara-sysroot/  ${BUILD_DIR}/test/com/base/test-common-com-client-default &
wait
  
./run.sh /home/wangyanlong/ara-sysroot/  ${BUILD_DIR}/test/com/base/test-common-com-server-mainloop &
sleep 1
./run.sh /home/wangyanlong/ara-sysroot/  ${BUILD_DIR}/test/com/base/test-common-com-client-mainloop &
wait

./run.sh /home/wangyanlong/ara-sysroot/  ${BUILD_DIR}/test/com/svcom/test-svcom-server &
sleep 1
./run.sh /home/wangyanlong/ara-sysroot/  ${BUILD_DIR}/test/com/svcom/test-svcom-client &
wait

./run.sh /home/wangyanlong/ara-sysroot/  ${BUILD_DIR}/test/com/hccom/test-hccom-server &
sleep 1
./run.sh /home/wangyanlong/ara-sysroot/  ${BUILD_DIR}/test/com/hccom/test-hccom-client &
wait

./run.sh /home/wangyanlong/ara-sysroot/  ${BUILD_DIR}/test/com/racom/test-racom-server & 
sleep 1
./run.sh /home/wangyanlong/ara-sysroot/  ${BUILD_DIR}/test/com/racom/test-racom-client &
wait

./run.sh /home/wangyanlong/ara-sysroot/  ${BUILD_DIR}/test/com/hcacom/test-hcacom-server &
sleep 1
./run.sh /home/wangyanlong/ara-sysroot/  ${BUILD_DIR}/test/com/hcacom/test-hcacom-client &
wait

./run.sh /home/wangyanlong/ara-sysroot/  ${BUILD_DIR}/test/com/raextcom/test-raextcom-server &
sleep 1
./run.sh /home/wangyanlong/ara-sysroot/  ${BUILD_DIR}/test/com/raextcom/test-raextcom-client &
wait

./run.sh /home/wangyanlong/ara-sysroot/  ${BUILD_DIR}/test/com/hcaextcom/test-hcaextcom-server &
sleep 1
./run.sh /home/wangyanlong/ara-sysroot/  ${BUILD_DIR}/test/com/hcaextcom/test-hcaextcom-client &
wait

./run.sh /home/wangyanlong/ara-sysroot/  ${BUILD_DIR}/test/com/svcom/test-svcom-server &
sleep 1
./run.sh /home/wangyanlong/ara-sysroot/  ${BUILD_DIR}/test/libs/phm_client/supervised_entity &
wait

./run.sh /home/wangyanlong/ara-sysroot/  ${BUILD_DIR}/test/libs/phm_client/supervised-entity-move

./run.sh /home/wangyanlong/ara-sysroot/  ${BUILD_DIR}/test/com/hccom/test-hccom-server &
sleep 1
./run.sh /home/wangyanlong/ara-sysroot/  ${BUILD_DIR}/test/libs/phm_client/health-channel &
wait

./run.sh /home/wangyanlong/ara-sysroot/  ${BUILD_DIR}/test/libs/phm_client/health-channel-move

./run.sh /home/wangyanlong/ara-sysroot/  ${BUILD_DIR}/test/libs/recovery_action/recovery-action-notifier &
sleep 1
./run.sh /home/wangyanlong/ara-sysroot/  ${BUILD_DIR}/test/libs/recovery_action/recovery-action &
wait

./run.sh /home/wangyanlong/ara-sysroot/  ${BUILD_DIR}/test/libs/recovery_action/recovery-action-notifier &
sleep 1
./run.sh /home/wangyanlong/ara-sysroot/  ${BUILD_DIR}/test/libs/recovery_action/recovery-action-move-construct &
wait

./run.sh /home/wangyanlong/ara-sysroot/  ${BUILD_DIR}/test/libs/recovery_action/recovery-action-notifier &
sleep 1
./run.sh /home/wangyanlong/ara-sysroot/  ${BUILD_DIR}/test/libs/recovery_action/recovery-action-move-assignment &
wait

./run.sh /home/wangyanlong/ara-sysroot/  ${BUILD_DIR}/test/libs/recovery_action/health-channel-action-notifier &
sleep 1
./run.sh /home/wangyanlong/ara-sysroot/  ${BUILD_DIR}/test/libs/recovery_action/health-channel-action &
wait

./run.sh /home/wangyanlong/ara-sysroot/  ${BUILD_DIR}/test/libs/recovery_action/health-channel-action-notifier &
sleep 1
./run.sh /home/wangyanlong/ara-sysroot/  ${BUILD_DIR}/test/libs/recovery_action/health-channel-action-move-constructor &
wait

./run.sh /home/wangyanlong/ara-sysroot/  ${BUILD_DIR}/test/libs/recovery_action/health-channel-action-notifier &
sleep 1
./run.sh /home/wangyanlong/ara-sysroot/  ${BUILD_DIR}/test/libs/recovery_action/health-channel-action-move-assignment &
wait

./run.sh /home/wangyanlong/ara-sysroot/  ${BUILD_DIR}/test/phmd/config/test-config-manager

./run.sh /home/wangyanlong/ara-sysroot/  ${BUILD_DIR}/test/phmd/healthchannel/health-channel-controller &
sleep 1
./run.sh /home/wangyanlong/ara-sysroot/  ${BUILD_DIR}/test/phmd/healthchannel/health-channel-controller-tester &
wait

./run.sh /home/wangyanlong/ara-sysroot/  ${BUILD_DIR}/test/phmd/supervision/supervision-controller &
sleep 1
./run.sh /home/wangyanlong/ara-sysroot/  ${BUILD_DIR}/test/phmd/supervision/supervision-controller-tester &
wait

./run.sh /home/wangyanlong/ara-sysroot/  ${BUILD_DIR}/test/phmd/watchdog/test-watchdog

./run.sh /home/wangyanlong/ara-sysroot/  ../.build/test/phmd/phm_context/test-phm-context
