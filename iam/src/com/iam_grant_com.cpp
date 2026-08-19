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
/// @file       iam_grant_com.cpp
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
/// <tr><td>2025-04-18 <td>1.0.0 <td>Han Yuxin <td>Refactored based on
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/IAM/IAM-COM
/// @interface_level=module
/// @trace_id_sr=SR_IAM_00201
/// @unit_name=IAM_COM
/// @unit_description=Check verification provided by IAM for the COM module
/// @endcode
///
/// ================================================================
///
/// rights reserved.
///
/// ================================================================

#include "ara/iam/internal/com/iam_grant_com.h"

#include "ara/iam/internal/grant/serialization_error_domain.h"
#include "common/ara_common.h"
#include "common/ara_log.h"
#include "common/common_api.h"
#include "grant_check_com.h"
namespace {
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
        ara::iam::internal::common::IamLogger().LogError() << "Com check failed, pid-->fqn check failed.";
        return ara::core::String{""};
    }
    ara::iam::internal::com::PGrantCheck_Com::Uptr pGrantCheck{
        ara::iam::internal::common::MakeSinglePtr< ara::iam::internal::com::PGrantCheck_Com >()};
    if (!pGrantCheck) {
        ara::iam::internal::common::IamLogger().LogInfo() << "PGrantCheck_Com Is Nullptr Point!";
        return ara::core::String{""};
    }
    if (false == pGrantCheck->IsReady()) {
        ara::iam::internal::common::IamLogger().LogInfo() << "PGrantCheck_Com Is Not Ready!";
        return ara::core::String{""};
    }
    return stProcessName;
}
}  // namespace
namespace ara {
namespace iam {
namespace internal {
namespace com {
//********************************/
/// @brief Initialize
/// @return true/false
ara::core::Result< void > PIamGrant_Com::Initialize() noexcept
{
    PGrantCheck_Com::Uptr pGrantCheck = common::MakeSinglePtr< PGrantCheck_Com >();
    if (!pGrantCheck) {
        common::IamLogger().LogInfo() << "PGrantCheck_Com Is Nullptr Point!";
        return ara::core::Result< void >::FromError(
            ara::iam::internal::grant::GrantSerializationErrc::kSingleGrantCheckPtr);
    }
    if (pGrantCheck->IsReady()) {
        common::IamLogger().LogInfo() << "PGrantCheck_Com Is Ready!";
        return ara::core::Result< void >::FromValue();
    }
    bool bReturn{pGrantCheck->Initialize()};
    if (false == bReturn) {
        common::IamLogger().LogInfo() << "PGrantCheck_Com Loaded Field !";
        return ara::core::Result< void >::FromError(pGrantCheck->GetErrorCode());
    }
    common::IamLogger().LogInfo() << "PGrantCheck_Com Loaded Success !";
    return ara::core::Result< void >::FromValue();
}
/// @brief Clear  All GrantInfo
/// @returns Deinitialization bool result
bool PIamGrant_Com::Deinitialize() noexcept
{
    PGrantCheck_Com::Uptr pGrantCheck = common::MakeSinglePtr< PGrantCheck_Com >();
    if (!pGrantCheck) {
        common::IamLogger().LogInfo() << "PGrantCheck_Com Is Nullptr Point!";
        return false;
    }
    return pGrantCheck->Deinitialize();
}
//********************************/
/// @brief Checks if the event given grant exists
/// @param pid process  pid
/// @param nServiceID event service id
/// @param nInstanceID inst id
/// @param nEventID event id
/// @return true/false
bool PIamGrant_Com::HasEventGrant(uint32_t const &pid,                // NOLINT
                                  uint16_t const &nServiceID,         // NOLINT
                                  uint16_t const &nInstanceID,        // NOLINT
                                  uint16_t const &nEventID) noexcept  // NOLINT
{
#ifdef ARA_WITH_EXEC
    PIamConfigData_ComEventGrant grantObject;
    grantObject.stProcessName = CheckGrant_Pid(pid);
    if (grantObject.stProcessName.empty()) {
        return false;
    }
    grantObject.nServiceID  = nServiceID;
    grantObject.nInstanceID = nInstanceID;
    grantObject.nEventID    = nEventID;
    PGrantCheck_Com::Uptr pGrantCheck{common::MakeSinglePtr< PGrantCheck_Com >()};
    common::IamLogger().LogDebug() << " Event get processname from exec is " << grantObject.stProcessName.c_str()
                                   << "serviceId:" << nServiceID << "instanceId:" << nInstanceID
                                   << "eventid:" << nEventID;
    return pGrantCheck->GrantCheck_ComEvent(grantObject);
#else
    return true;
#endif
}

/// @brief Checks if the method given grant exists
/// @param pidprocess pid
/// @param nServiceID method service id
/// @param nInstanceID inst id
/// @param nMethodID method id
/// @return true/false
bool PIamGrant_Com::HasMethodGrant(uint32_t const &pid,                 // NOLINT
                                   uint16_t const &nServiceID,          // NOLINT
                                   uint16_t const &nInstanceID,         // NOLINT
                                   uint16_t const &nMethodID) noexcept  // NOLINT
{
#ifdef ARA_WITH_EXEC
    PIamConfigData_ComMethodGrant grantObject;
    grantObject.stProcessName = CheckGrant_Pid(pid);
    if (grantObject.stProcessName.empty()) {
        return false;
    }
    grantObject.nServiceID  = nServiceID;
    grantObject.nInstanceID = nInstanceID;
    grantObject.nMethodID   = nMethodID;
    PGrantCheck_Com::Uptr pGrantCheck{common::MakeSinglePtr< PGrantCheck_Com >()};
    common::IamLogger().LogDebug() << " Event get processname from exec is " << grantObject.stProcessName.c_str()
                                   << "serviceId:" << nServiceID << "instanceId:" << nInstanceID
                                   << "methodid:" << nMethodID;
    return pGrantCheck->GrantCheck_ComMethod(grantObject);
#else
    return true;
#endif
}

/// @brief Checks if the offerservice given grant exists
/// @param pidoffer service process pid
/// @param nServiceID offer service id
/// @param nInstanceID inst id
/// @return true/false
bool PIamGrant_Com::HasOfferServiceGrant(uint32_t const &pid,                   // NOLINT
                                         uint16_t const &nServiceID,            // NOLINT
                                         uint16_t const &nInstanceID) noexcept  // NOLINT
{
#ifdef ARA_WITH_EXEC
    PIamConfigData_ComOfferServiceGrant grantObject;
    grantObject.stProcessName = CheckGrant_Pid(pid);
    if (grantObject.stProcessName.empty()) {
        return false;
    }
    grantObject.nServiceID  = nServiceID;
    grantObject.nInstanceID = nInstanceID;
    PGrantCheck_Com::Uptr pGrantCheck{common::MakeSinglePtr< PGrantCheck_Com >()};
    common::IamLogger().LogDebug() << " OfferService get processname from exec is " << grantObject.stProcessName.c_str()
                                   << "serviceId:" << nServiceID << "instanceId:" << nInstanceID;
    return pGrantCheck->GrantCheck_ComOfferService(grantObject);
#else
    return true;
#endif
}

/// @brief Checks if the findservice given grant exists
/// @param pid fs process pid
/// @param nServiceID fs service id
/// @param nInstanceID inst id
/// @return true/false
bool PIamGrant_Com::HasFindServiceGrant(uint32_t const &pid,                   // NOLINT
                                        uint16_t const &nServiceID,            // NOLINT
                                        uint16_t const &nInstanceID) noexcept  // NOLINT
{
#ifdef ARA_WITH_EXEC
    PIamConfigData_ComFindServiceGrant grantObject;
    grantObject.stProcessName = CheckGrant_Pid(pid);
    if (grantObject.stProcessName.empty()) {
        return false;
    }
    grantObject.nServiceID  = nServiceID;
    grantObject.nInstanceID = nInstanceID;
    PGrantCheck_Com::Uptr pGrantCheck{common::MakeSinglePtr< PGrantCheck_Com >()};
    common::IamLogger().LogDebug() << " FindService get processname from exec is " << grantObject.stProcessName.c_str()
                                   << "serviceId:" << nServiceID << "instanceId:" << nInstanceID;
    return pGrantCheck->GrantCheck_ComFindService(grantObject);
#else
    return true;
#endif
}

/// @brief Checks if the remote event given grant exists
/// @param stRemoteIp remote ip
/// @param nPortRemote remote port
/// @param stLocalIp local ip
/// @param nPortLocal local port
/// @param nServiceID service id
/// @param nInstanceID instance id
/// @param nEventID evnet id
/// @return true/false
bool PIamGrant_Com::HasEventRemoteIpGrant(ara::core::String const &stRemoteIp,
                                          uint32_t const &nPortRemote,
                                          ara::core::String const &stLocalIp,
                                          int32_t const &nPortLocal,
                                          uint16_t const &nServiceID,
                                          uint16_t const &nInstanceID,
                                          uint16_t const &nEventID) noexcept
{
    if (stRemoteIp.empty()) {
        common::IamLogger().LogError() << "RemoteEvent: remote ip empty! ";
        return false;
    }
    common::IamLogger().LogDebug() << "remote event check RemoteIp :" << stRemoteIp.c_str()
                                   << " RemotePort:" << nPortRemote;
    common::IamLogger().LogDebug() << "remote event check LocalIp :" << stLocalIp.c_str()
                                   << " LocalPort:" << nPortLocal;
    common::IamLogger().LogDebug() << "remote Event service: " << nServiceID << " instanceId:" << nInstanceID
                                   << " eventid:" << nEventID;
    PGrantCheck_Com::Uptr pGrantCheck{common::MakeSinglePtr< PGrantCheck_Com >()};
    if (!pGrantCheck) {
        common::IamLogger().LogInfo() << "PGrantCheck_Com Is Nullptr Point!";
        return false;
    }
    if (false == pGrantCheck->IsReady()) {
        common::IamLogger().LogInfo() << "PGrantCheck_Com Is Not Ready!";
        return false;
    }
    return pGrantCheck->GrantCheck_RemoteIpEvent(stRemoteIp, nPortRemote, stLocalIp, nPortLocal, nServiceID,
                                                 nInstanceID, nEventID);
}
/// @brief Checks if the remote method given grant exists
/// @param stRemoteIp remote ip
/// @param stProtocol protocol(TCP/UDP)
/// @param nPortRemote remote port
/// @param stLocalIp local ip
/// @param nPortLocal local port
/// @param nServiceID service id
/// @param nInstanceID instance id
/// @param nMethodID method id
/// @return true/false
bool PIamGrant_Com::HasMethodRemoteIpGrant(ara::core::String const &stRemoteIp,
                                           ara::core::String const &stProtocol,
                                           uint32_t const &nPortRemote,
                                           ara::core::String const &stLocalIp,
                                           int32_t const &nPortLocal,
                                           uint16_t const &nServiceID,
                                           uint16_t const &nInstanceID,
                                           uint16_t const &nMethodID) noexcept
{
    if (stRemoteIp.empty()) {
        common::IamLogger().LogError() << "RemoteEvent: remote ip empty! ";
        return false;
    }
    common::IamLogger().LogDebug() << "remote event check RemoteIp :" << stRemoteIp.c_str()
                                   << " RemotePort:" << nPortRemote << " RemoteProtocol:" << stProtocol.c_str();
    common::IamLogger().LogDebug() << "remote event check LocalIp :" << stLocalIp.c_str()
                                   << " LocalPort:" << nPortLocal;
    common::IamLogger().LogDebug() << "remote Event service: " << nServiceID << " instanceId:" << nInstanceID
                                   << " methodID:" << nMethodID;
    PGrantCheck_Com::Uptr pGrantCheck{common::MakeSinglePtr< PGrantCheck_Com >()};
    if (!pGrantCheck) {
        common::IamLogger().LogInfo() << "PGrantCheck_Com Is Nullptr Point!";
        return false;
    }
    if (false == pGrantCheck->IsReady()) {
        common::IamLogger().LogInfo() << "PGrantCheck_Com Is Not Ready!";
        return false;
    }
    return pGrantCheck->GrantCheck_RemoteIpMethod(stRemoteIp, nPortRemote, stLocalIp, nPortLocal, nServiceID,
                                                  nInstanceID, nMethodID, stProtocol);
}
/// @brief HasEventTlsGrant
/// @param stTlsName tls name
/// @param nServiceID tls service id
/// @param nInstanceID inst id
/// @param nEventID tls event id
/// @return true/false
bool PIamGrant_Com::HasEventTlsGrant(ara::core::String const &stTlsName,
                                     uint16_t const &nServiceID,
                                     uint16_t const &nInstanceID,
                                     uint16_t const &nEventID) noexcept
{
    if (stTlsName.empty()) {
        common::IamLogger().LogError() << "HasEventTlsGrant: TlsName ip empty! ";
        return false;
    }
    PGrantCheck_Com::Uptr pGrantCheck{common::MakeSinglePtr< PGrantCheck_Com >()};
    if (!pGrantCheck) {
        common::IamLogger().LogInfo() << "PGrantCheck_Com Is Nullptr Point!";
        return false;
    }
    if (false == pGrantCheck->IsReady()) {
        common::IamLogger().LogInfo() << "PGrantCheck_Com Is Not Ready!";
        return false;
    }
    PIamConfigData_ComTlsEventGrant grantObject;
    grantObject.stTlsName   = stTlsName;
    grantObject.nServiceID  = nServiceID;
    grantObject.nInstanceID = nInstanceID;
    grantObject.nEventID    = nEventID;

    common::IamLogger().LogDebug() << "EventTls TlsName: " << stTlsName.c_str() << " serviceId:" << nServiceID
                                   << " instanceId:" << nInstanceID << " eventId:" << nEventID;
    return pGrantCheck->GrantCheck_ComTlsEvent(grantObject);
}

/// @brief HasMethodTlsGrant
/// @param stTlsName name
/// @param nServiceID tls service id
/// @param nInstanceID inst id
/// @param nMethodID method id
/// @return true/false
bool PIamGrant_Com::HasMethodTlsGrant(ara::core::String const &stTlsName,
                                      uint16_t const &nServiceID,
                                      uint16_t const &nInstanceID,
                                      uint16_t const &nMethodID) noexcept
{
    if (stTlsName.empty()) {
        common::IamLogger().LogError() << "HasEventTlsGrant: TlsName ip empty! ";
        return false;
    }
    PGrantCheck_Com::Uptr pGrantCheck{common::MakeSinglePtr< PGrantCheck_Com >()};
    if (!pGrantCheck) {
        common::IamLogger().LogInfo() << "PGrantCheck_Com Is Nullptr Point!";
        return false;
    }
    if (false == pGrantCheck->IsReady()) {
        common::IamLogger().LogInfo() << "PGrantCheck_Com Is Not Ready!";
        return false;
    }
    PIamConfigData_ComTlsMethodGrant grantObject;
    grantObject.stTlsName   = stTlsName;
    grantObject.nServiceID  = nServiceID;
    grantObject.nInstanceID = nInstanceID;
    grantObject.nMethodID   = nMethodID;

    common::IamLogger().LogDebug() << "EventTls TlsName: " << stTlsName.c_str() << " serviceId:" << nServiceID
                                   << " instanceId:" << nInstanceID << " methodId:" << nMethodID;
    return pGrantCheck->GrantCheck_ComTlsMethod(grantObject);
}
//********************************/
}  // namespace com
}  // namespace internal
}  // namespace iam
}  // namespace ara
