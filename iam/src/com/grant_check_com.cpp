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
/// @file       grant_check_com.cpp
/// @brief      IAM-com module logic processing function
/// @details
/// @date       2025-04-14
/// @author     Han Yuxin
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author     <th>Description
/// <tr><td>2025-04-14 <td>1.0.0 <td>Han Yuxin <td>Refactored based on
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

#include "grant_check_com.h"

#include <map>
#include <set>

#include "ara/iam/internal/grant/serialization_error_domain.h"
#include "common/ara_common.h"
#include "common/ara_log.h"

namespace ara {
namespace iam {
namespace internal {
namespace com {
//********************************/
/// @brief com module init check flag.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_IAM_00501
/// @needwork = ad
/// @endcode
constexpr uint32_t kComAnyInstance{65535U};
/// @brief Get configuration file tag string constant:
/// LocalComAccessControlEnabled
/// @return processname
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_IAM_
/// @needwork = dd
/// @endcode
inline constexpr ara::iam::internal::char8_t const *GetJsonValue_NoncreditApp() noexcept { return "noncreditapp"; }
//********************************/
/// @brief Load Com configuration data
/// @brief com init  load  data .
/// @return load success/falied.
bool PGrantCheck_Com::Initialize() noexcept
{
    if (iamConfigCom_.IsReady()) {
        return true;
    }
    if (false == iamConfigCom_.Initialize()) {
        return false;
    }
    // load machine manifest.
#if defined(ARA_IAM_DEBUG_WITHOUT_EM)
    ara::core::StringView const &stManifestPath{GetConfigFileName_IamCom()};
#else
    common::PAraCommon const cmDir;
    common::EFileDirectoryType const fileType{common::EFileDirectoryType::kComFileDirectory};
    ara::core::String configFileName{cmDir.GetIamConfigDir(fileType)};
    ara::core::StringView const &stManifestPath{configFileName};
    common::IamLogger().LogInfo() << "com load file:" << stManifestPath;
#endif
    if (false == iamConfigCom_.InitManifest(stManifestPath)) {
        return false;
    }
    return true;
}
/// @brief Clear  All GrantInfo
/// @returns true
bool PGrantCheck_Com::Deinitialize() noexcept { return iamConfigCom_.Deinitialize(); }
/// @brief Whether initialization is successful
/// @return true if has init manifest sucess false otherwise
bool PGrantCheck_Com::IsReady() const noexcept { return iamConfigCom_.IsReady(); }
/// @brief Return error code
/// @return Error code
/// @interface_level=unit
/// @needwork = dda
/// @endcode
ara::iam::internal::grant::GrantSerializationErrc PGrantCheck_Com::GetErrorCode() const noexcept
{
    return iamConfigCom_.GetErrorCode();
}
//********************************/
/// @brief grant Event check.
/// @param grantObject event struct
/// @return  event check success or failed.
bool PGrantCheck_Com::GrantCheck_ComEvent(PIamConfigData_ComEventGrant const &grantObject) const noexcept
{
    iamConfigCom_.SetErrorCode(ara::iam::internal::grant::GrantSerializationErrc::kInitCode);
    // if method's instanceid is ANY(65535) then forbidden method process.
    if (grantObject.nInstanceID == kComAnyInstance) {
        iamConfigCom_.SetErrorCode(ara::iam::internal::grant::GrantSerializationErrc::kInvalidInstanceID);
        return false;
    }
    if (grantObject.stProcessName == GetJsonValue_NoncreditApp()) {
        return true;
    }
    return iamConfigCom_.GrantCheck_ComEvent(grantObject);
}

/// @brief grant Method check.
/// @param grantObject method struct
/// @return method check success or failed.
bool PGrantCheck_Com::GrantCheck_ComMethod(PIamConfigData_ComMethodGrant const &grantObject) const noexcept
{
    iamConfigCom_.SetErrorCode(ara::iam::internal::grant::GrantSerializationErrc::kInitCode);
    // if method's instanceid is ANY(65535) then forbidden method process.
    if (grantObject.nInstanceID == kComAnyInstance) {
        iamConfigCom_.SetErrorCode(ara::iam::internal::grant::GrantSerializationErrc::kInvalidInstanceID);
        return false;
    }
    if (grantObject.stProcessName == GetJsonValue_NoncreditApp()) {
        return true;
    }
    return iamConfigCom_.GrantCheck_ComMethod(grantObject);
}

/// @brief grant offerservice check.OfferServiceGrantCheck
/// @param grantObject offer service struct
/// @return offerservice check success or failed.
bool PGrantCheck_Com::GrantCheck_ComOfferService(PIamConfigData_ComOfferServiceGrant const &grantObject) const noexcept
{
    iamConfigCom_.SetErrorCode(ara::iam::internal::grant::GrantSerializationErrc::kInitCode);
    if (grantObject.stProcessName == GetJsonValue_NoncreditApp()) {
        return true;
    }
    return iamConfigCom_.GrantCheck_ComOfferService(grantObject);
}

/// @brief grant findservice check.
/// @param grantObject fs struct
/// @return findservice check success or failed.
bool PGrantCheck_Com::GrantCheck_ComFindService(PIamConfigData_ComFindServiceGrant const &grantObject) const noexcept
{
    iamConfigCom_.SetErrorCode(ara::iam::internal::grant::GrantSerializationErrc::kInitCode);
    // if method's instanceid is ANY(65535) then forbidden method process.
    if (grantObject.nInstanceID == kComAnyInstance) {
        iamConfigCom_.SetErrorCode(ara::iam::internal::grant::GrantSerializationErrc::kInvalidInstanceID);
        return false;
    }
    if (grantObject.stProcessName == GetJsonValue_NoncreditApp()) {
        return true;
    }
    return iamConfigCom_.GrantCheck_ComFindService(grantObject);
}

/// @brief  RemoteIpEventGrant declear
/// @param stRemoteIp remote ip
/// @param nPortRemote remote port
/// @param stLocalIp local ip
/// @param nPortLocal local port
/// @param nServiceID service id
/// @param nInstanceID instance id
/// @param nEventID evnet id
/// @return ture/false
bool PGrantCheck_Com::GrantCheck_RemoteIpEvent(ara::core::String const &stRemoteIp,
                                               uint32_t const &nPortRemote,
                                               ara::core::String const &stLocalIp,
                                               int32_t const &nPortLocal,
                                               uint16_t const &nServiceID,
                                               uint16_t const &nInstanceID,
                                               uint16_t const &nEventID) const noexcept
{
    return iamConfigCom_.GrantCheck_RemoteIpEvent(stRemoteIp, nPortRemote, stLocalIp, nPortLocal, nServiceID,
                                                  nInstanceID, nEventID);
}

/// @brief RemoteIpMethodGrant declear.
/// @param stRemoteIp remote ip
/// @param nPortRemote remote port
/// @param stLocalIp local ip
/// @param nPortLocal local port
/// @param nServiceID service id
/// @param nInstanceID instance id
/// @param nMethodID method id
/// @param stProtocol protocol(TCP/UDP)
/// @return ture/false
bool PGrantCheck_Com::GrantCheck_RemoteIpMethod(ara::core::String const &stRemoteIp,
                                                uint32_t const &nPortRemote,
                                                ara::core::String const &stLocalIp,
                                                int32_t const &nPortLocal,
                                                uint16_t const &nServiceID,
                                                uint16_t const &nInstanceID,
                                                uint16_t const &nMethodID,
                                                ara::core::String const &stProtocol) const noexcept
{
    return iamConfigCom_.GrantCheck_RemoteIpMethod(stRemoteIp, nPortRemote, stLocalIp, nPortLocal, nServiceID,
                                                   nInstanceID, nMethodID, stProtocol);
}

/// @brief grant Method check.
/// @param grantObject tls method struct
/// @return method check success or failed.
bool PGrantCheck_Com::GrantCheck_ComTlsEvent(PIamConfigData_ComTlsEventGrant const &grantObject) const noexcept
{
    return iamConfigCom_.GrantCheck_ComTlsEvent(grantObject);
}

/// @brief grant Event check.
/// @param grantObject tls event struct
/// @return  event check success or failed.
bool PGrantCheck_Com::GrantCheck_ComTlsMethod(PIamConfigData_ComTlsMethodGrant const &grantObject) const noexcept
{
    return iamConfigCom_.GrantCheck_ComTlsMethod(grantObject);
}
//********************************/
}  // namespace com
}  // namespace internal
}  // namespace iam
}  // namespace ara
