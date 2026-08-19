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
/// @file       common_api.cpp
/// @brief      AutoSar-AP
/// @details    Some common data
/// @date       2025-04-15
/// @author     Han Yuxin
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author          <th>Description
/// <tr><td>2025-04-15 <td>0.1 <td>CheJinzhao <td>Created initial version
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/IAM/IAM-COMMON
/// @interface_level=Unit
/// @trace_id_sr=SR_IAM_
/// @unit_name=PAraCommon
/// @endcode
///
/// ================================================================
///
/// rights reserved.
///
/// ================================================================

#include "common_api.h"

#include <isoft/manifestreader/tps_enumeration.h>

#include "ara_common.h"
#include "ara_log.h"
#include "config_iam_machine.h"

namespace ara {
namespace iam {
namespace internal {
namespace common {
//********************************/
/// @brief Convert PID to ProcessName
/// @param pid Process ID
/// @return Process name FQN
ara::core::String Pid2Name(uint32_t const &pid) noexcept
{
    common::PAraCommon const cmCrypto{};
    if ((pid <= static_cast< uint32_t >(0))) {
        common::IamLogger().LogError() << "pid value  == 0 ,please check parameter!";
        return ara::core::String{};
    }
#if defined(ARA_IAM_DEBUG_WITHOUT_EM)
    ara::core::String stProcess = "/homeTest/Client";
#else
    #ifdef ARA_WITH_EXEC
    PConfigIam_Machine::Uptr pConfigMachine{MakeSinglePtr< PConfigIam_Machine >()};
    isoft::manifestreader::tps::TrustedPlatformExecutableLaunchBehaviorEnum eTrustedPlatform{
        pConfigMachine->GetTrustedPlatform()};
    ara::core::String stProcess{cmCrypto.PidTranslate(pid, eTrustedPlatform)};
    if (stProcess.empty()) {
        common::IamLogger().LogError() << "Get ProcessName Failed.";
    } else {
        common::IamLogger().LogDebug() << "Get ProcessName Success [" << stProcess.c_str() << "].";
    }
    #else
    ara::core::String stProcess{""};
    #endif
#endif
    return stProcess;
}

//********************************/
}  // namespace common
}  // namespace internal
}  // namespace iam
}  // namespace ara