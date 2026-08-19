// Disclaimer
//
// This work (specification and/or software implementation) and the material
// contained in it, as released by AUTOSAR, is for the purpose of information
// only. AUTOSAR and the companies that have contributed to it shall not be
// liable for any use of the work.
//
// The material contained in this work is protected by copyright and other
// types of intellectual property rights. The commercial exploitation of the
// material contained in this work requires a license to such intellectual
// property rights.
//
// This work may be utilized or reproduced without any modification, in any
// form or by any means, for informational purposes only. For any other
// purpose, no part of the work may be utilized or reproduced, in any form
// or by any means, without permission in writing from the publisher.
//
// The work has been developed for automotive applications only. It has
// neither been developed, nor tested for non-automotive applications.
//
// The word AUTOSAR and the AUTOSAR logo are registered trademarks.
// --------------------------------------------------------------------------

/// ================================================================
///
/// File description:
/// ----------------
/// @file       hc_testunit_health_chanel_action.cpp
/// @brief      HealthChannelAction for tyre pressure health channel.
/// @details
/// @date       2024-06-06
/// @author     lianglongxiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/StateManagement/PHMComm
/// @interface_level=uint
/// @trace_id_sr=SR_SM_08001
/// @unit_name=HcTestUnitHealthChannelAction
/// @unit_description=HealthChannelAction for tyre pressure health channel.
/// @endcode
///
/// ================================================================

#include "phm_comm/hc_testunit_health_chanel_action.h"

#include "helper.h"

namespace ara {
namespace sm {
namespace phm_comm {
/// @brief RecoveryHandler called by PHM, implemented on the SM side.
/// @param healthStatusId Abnormal health channel state
void HcTestUnitHealthChannelAction::RecoveryHandler(
    phm::health_channels::hc_testunit::HealthStatuses healthStatusId) noexcept
{
    log_.LogInfo() << "HcTestUnitHealthChannelAction::RecoveryHandler(), healthStatusId number:"
                   << static_cast< uint32_t >(healthStatusId);
}
}  // namespace phm_comm
}  // namespace sm
}  // namespace ara
