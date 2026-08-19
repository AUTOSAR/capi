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
/// @file       iam_grant_raw.cpp
/// @brief      IAM-com module logic processing function
/// @details
/// @date       2025-04-18
/// @author     Han Yuxin
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author     <th>Description
/// <tr><td>2025-04-18 <td>1.0 <td>Han Yuxin <td>Refactored based on version 2.2
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/IAM/IAM-COM
/// @interface_level=module
/// @trace_id_sr=SR_IAM_00201
/// @unit_name=IAM_COM
/// @unit_description=Check verification provided by IAM for the Com module: Raw
/// @endcode
///
/// ================================================================
///
/// rights reserved.
///
/// ================================================================

#include "ara/iam/internal/raw/iam_grant_raw.h"

#include "ara/iam/internal/grant/serialization_error_domain.h"
#include "common/ara_common.h"
#include "common/ara_log.h"
#include "common/common_api.h"
#include "grant_check_raw.h"
namespace {
//********************************/
/// @brief Checks if the event given grant exists
/// @param pid process  pid
/// @return process name
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_IAM_00101
/// @needwork = dd
/// @endcode
ara::core::String CheckGrant_Pid(uint32_t const &pid)  // NOLINT
{
    if (pid <= static_cast< uint32_t >(0)) {
        ara::iam::internal::common::IamLogger().LogError() << "pid value  == 0 ,please check parameter!";
        return ara::core::String{""};
    }
    ara::core::String stProcessName{ara::iam::internal::common::Pid2Name(pid)};
    if (stProcessName.empty()) {
        ara::iam::internal::common::IamLogger().LogError() << "Raw check failed, pid-->fqn check failed.";
        return ara::core::String{""};
    }
    ara::iam::internal::com::PGrantCheck_Raw::Uptr pGrantCheck{
        ara::iam::internal::common::MakeSinglePtr< ara::iam::internal::com::PGrantCheck_Raw >()};
    if (!pGrantCheck) {
        ara::iam::internal::common::IamLogger().LogInfo() << "PGrantCheck_Raw Is Nullptr Point!";
        return ara::core::String{""};
    }
    if (false == pGrantCheck->IsReady()) {
        ara::iam::internal::common::IamLogger().LogInfo() << "PGrantCheck_Raw Is Not Ready!";
        return ara::core::String{""};
    }
    return stProcessName;
}
//********************************/
}  // namespace
namespace ara {
namespace iam {
namespace internal {
namespace com {
//********************************/
/// @brief Initialize
/// @return true/false
ara::core::Result< void > PIamGrant_Raw::Initialize() noexcept
{
    PGrantCheck_Raw::Uptr pGrantCheck = common::MakeSinglePtr< PGrantCheck_Raw >();
    if (!pGrantCheck) {
        common::IamLogger().LogInfo() << "PGrantCheck_Raw Is Nullptr Point!";
        return ara::core::Result< void >::FromError(
            ara::iam::internal::grant::GrantSerializationErrc::kSingleGrantCheckPtr);
    }
    if (pGrantCheck->IsReady()) {
        common::IamLogger().LogInfo() << "PGrantCheck_Raw Is Ready!";
        return ara::core::Result< void >::FromValue();
    }
    bool bReturn{pGrantCheck->Initialize()};
    if (false == bReturn) {
        common::IamLogger().LogInfo() << "PGrantCheck_Raw Loaded Field !";
        return ara::core::Result< void >::FromError(pGrantCheck->GetErrorCode());
    }
    common::IamLogger().LogInfo() << "PGrantCheck_Raw Loaded Success !";
    return ara::core::Result< void >::FromValue();
}
/// @brief Clear  All GrantInfo
/// @returns Deinitialization bool result
bool PIamGrant_Raw::Deinitialize() noexcept
{
    PGrantCheck_Raw::Uptr pGrantCheck = common::MakeSinglePtr< PGrantCheck_Raw >();
    if (!pGrantCheck) {
        common::IamLogger().LogInfo() << "PGrantCheck_Raw Is Nullptr Point!";
        return false;
    }
    return pGrantCheck->Deinitialize();
}
//********************************/
/// @brief Checks if the given grant exists
/// @param pid raw client id
/// @param stIp ip
/// @param nTcpPort tcp port
/// @param nUdpPort udp port
/// @param nMulCastUdpPort mulcast port
/// @return  true/false
bool PIamGrant_Raw::HasRawGrant(uint32_t const &pid,                       // NOLINT
                                ara::core::String const &stIp,             // NOLINT
                                uint32_t const &nTcpPort,                  // NOLINT
                                uint32_t const &nUdpPort,                  // NOLINT
                                uint32_t const &nMulCastUdpPort) noexcept  // NOLINT
{
#ifdef ARA_WITH_EXEC
    PIamConfigData_RawDataStreamGrant grantObject;
    #if defined(ARA_IAM_DEBUG_WITHOUT_EM)
    grantObject.stProcessName = "/homeTest/Client";  // NOLINT
    #else
    grantObject.stProcessName = CheckGrant_Pid(pid);
    #endif
    grantObject.stIp            = stIp;
    grantObject.nUdpPort        = static_cast< int16_t >(nUdpPort);
    grantObject.nTcpPort        = static_cast< int16_t >(nTcpPort);
    grantObject.nMulCastUdpPort = static_cast< int16_t >(nMulCastUdpPort);
    PGrantCheck_Raw::Uptr pGrantCheck{common::MakeSinglePtr< PGrantCheck_Raw >()};
    common::IamLogger().LogDebug() << " Event get processname from exec is " << grantObject.stProcessName.c_str()
                                   << " stIp:" << stIp.c_str() << " nUdpPort:" << nUdpPort << " nTcpPort:" << nTcpPort
                                   << " nMulCastUdpPort:" << nMulCastUdpPort;
    return pGrantCheck->GrantCheck_RawDataStream(grantObject);
#else
    return true;
#endif

}  //********************************/
}  // namespace com
}  // namespace internal
}  // namespace iam
}  // namespace ara
