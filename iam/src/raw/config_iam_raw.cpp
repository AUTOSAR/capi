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
/// @file       config_iam_raw.cpp
/// @brief      AutoSar-IAM-COM Configuration file
/// @details
/// @date       2025-04-14
/// @author     Han Yuxin
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr><td>2025-04-14 <td>0.1 <td>Han Yuxin <td>Refactored IAM-COM
/// functionality
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/IAM/IAM-COM
/// @interface_level=Unit
/// @trace_id_sr=SR_IAM_
/// @unit_name=IAM_COM
/// @unit_description=Configuration information provided by IAM to the COM
/// module
/// @endcode
///
/// ================================================================
///
/// rights reserved.
///
/// ================================================================

#include "config_iam_raw.h"

#include <isoft/ara_fsh/filesystem_hierarchy.h>
#include <isoft/manifestreader/manifest_node.h>
#include <isoft/manifestreader/manifest_reader.h>

#include <functional>

#include "common/ara_log.h"

namespace {
//********************************/
/// @brief Get configuration file tag string constant:
/// LocalComAccessControlEnabled
/// @return processname
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_IAM_
/// @needwork = dd
/// @endcode
inline constexpr ara::iam::internal::char8_t const *GetJsonKey_LocalComAccessControlEnabled() noexcept
{
    return "LocalComAccessControlEnabled";
}
/// @brief Get configuration file tag string constant: StrictVerificationMode
/// @return processname
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_IAM_
/// @needwork = dd
/// @endcode
inline constexpr ara::iam::internal::char8_t const *GetJsonKey_RawStrictVerificationMode() noexcept
{
    return "StrictVerificationMode";
}
/// @brief Get configuration file tag string constant: RawDataStreamGrants
/// @return slotname
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_IAM_
/// @needwork = dd
/// @endcode
inline constexpr ara::iam::internal::char8_t const *GetJsonKey_RawDataStreamGrants() noexcept
{
    return "RawDataStreamGrants";
}
/// @brief Get configuration file tag string constant: processname
/// @return processname
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_IAM_
/// @needwork = dd
/// @endcode
inline constexpr ara::iam::internal::char8_t const *GetJsonKey_ProcessName() noexcept { return "processname"; }
/// @brief Get configuration file tag string constant: ip
/// @return slotname
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_IAM_
/// @needwork = dd
/// @endcode
inline constexpr ara::iam::internal::char8_t const *GetJsonKey_Ip() noexcept { return "ip"; }
/// @brief Get configuration file tag string constant: udpPort
/// @return slotname
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_IAM_
/// @needwork = dd
/// @endcode
inline constexpr ara::iam::internal::char8_t const *GetJsonKey_UdpPort() noexcept { return "udpPort"; }
/// @brief Get configuration file tag string constant: tcpPort
/// @return slotname
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_IAM_
/// @needwork = dd
/// @endcode
inline constexpr ara::iam::internal::char8_t const *GetJsonKey_TcpPort() noexcept { return "tcpPort"; }
/// @brief Get configuration file tag string constant: mulCastUdpPort
/// @return slotname
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_IAM_
/// @needwork = dd
/// @endcode
inline constexpr ara::iam::internal::char8_t const *GetJsonKey_MulCastUdpPort() noexcept { return "mulCastUdpPort"; }
}  // namespace

//********************************/
MR_HEADER(ara::iam::internal::com::PIamConfigData_RawDataStreamGrant)
MR_FIELD(GetJsonKey_ProcessName(), stProcessName)
MR_FIELD(GetJsonKey_Ip(), stIp)
MR_FIELD(GetJsonKey_UdpPort(), nUdpPort)
MR_FIELD(GetJsonKey_TcpPort(), nTcpPort)
MR_FIELD(GetJsonKey_MulCastUdpPort(), nMulCastUdpPort)
MR_FOOTER
//********************************/
namespace ara {
namespace iam {
namespace internal {
namespace com {
//********************************/
/// @brief Load Raw configuration data
/// @return load success/falied.
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = ad
/// @endcode
bool PConfigIam_Raw::Initialize() noexcept
{
    _ResetData();
    _ClearReady();
    SetErrorCode(ara::iam::internal::grant::GrantSerializationErrc::kInitCode);
    return true;
}
/// @brief Clear  All GrantInfo
/// @returns true
bool PConfigIam_Raw::Deinitialize() noexcept
{
    _ResetData();
    _ClearReady();
    SetErrorCode(ara::iam::internal::grant::GrantSerializationErrc::kInitCode);
    return true;
}
/// @brief Initialize configuration file
/// @param stFileName Configuration file name
/// @return true if init sucess false otherwise
bool PConfigIam_Raw::InitManifest(ara::core::StringView const &stFileName) noexcept
{
    ara::core::Result< std::unique_ptr< isoft::manifestreader::Manifest > > manifestRes{
        isoft::manifestreader::OpenManifest(stFileName)};
    if (!manifestRes.HasValue()) {
        SetErrorCode(ara::iam::internal::grant::GrantSerializationErrc::kFileNotFound);
        return false;
    }
    std::unique_ptr< isoft::manifestreader::Manifest > const pManifestRes{std::move(manifestRes).Value()};
    common::IamLogger().LogInfo() << "PConfigIam_Raw::LoadData ConfigName = " << stFileName.data();

    bLocalComAccessControlEnabled_ = pManifestRes->GetValue(GetJsonKey_LocalComAccessControlEnabled(), false);
    bRawStrictVerificationMode_    = pManifestRes->GetValue(GetJsonKey_RawStrictVerificationMode(), false);
    bool bReady{false};
    bReady = common::ReadMapData< PIamConfigData_RawDataStreamGrant >(
        pManifestRes, ara::core::String(GetJsonKey_RawDataStreamGrants()),
        [this](PIamConfigData_RawDataStreamGrant const &data) -> void {
            setIamConfigRawDataStreamGrant_.insert(data);
        });
    if (false == bReady) {
        common::IamLogger().LogInfo() << "PConfigIam_Raw::user not configuration raw grant ";
    }

    _SetReady();
    SetErrorCode(ara::iam::internal::grant::GrantSerializationErrc::kInitCode);
    return IsReady();
}
//********************************/
/// @brief Check whether data exists: grant Event check.
/// @param grantObject event struct
/// @return  event check success or failed.
bool PConfigIam_Raw::GrantCheck_RawDataStream(PIamConfigData_RawDataStreamGrant const &grantObject) const noexcept
{
    return _T_GrantCheck(bLocalComAccessControlEnabled_, setIamConfigRawDataStreamGrant_, grantObject);
}

//********************************/
/// @brief Reset data
void PConfigIam_Raw::_ResetData()
{
    bLocalComAccessControlEnabled_ = true;
    setIamConfigRawDataStreamGrant_.clear();
}
//********************************/
}  // namespace com
}  // namespace internal
}  // namespace iam
}  // namespace ara