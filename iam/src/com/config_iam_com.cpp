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
/// @file       config_iam_com.cpp
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

#include "config_iam_com.h"

#include <isoft/ara_fsh/filesystem_hierarchy.h>
#include <isoft/manifestreader/manifest_node.h>
#include <isoft/manifestreader/manifest_reader.h>

#include <functional>

#include "common/ara_log.h"

namespace {
//********************************/
/// @brief Get configuration file tag string constant:
/// trustedPlatformExecutableLaunchBehavior
/// @return processname
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_IAM_
/// @needwork = dd
/// @endcode
inline constexpr ara::iam::internal::char8_t const *
GetJsonKey_TrustedPlatformExecutableLaunchBehavior() noexcept  // NOLINT
{
    return "trustedPlatformExecutableLaunchBehavior";
}
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
/// @brief Get configuration file tag string constant:
/// RemoteAccessControlEnabled
/// @return processname
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_IAM_
/// @needwork = dd
/// @endcode
inline constexpr ara::iam::internal::char8_t const *GetJsonKey_RemoteAccessControlEnabled() noexcept
{
    return "RemoteAccessControlEnabled";
}
/// @brief Get configuration file tag string constant: StrictVerificationMode
/// @return processname
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_IAM_
/// @needwork = dd
/// @endcode
inline constexpr ara::iam::internal::char8_t const *GetJsonKey_StrictVerificationMode() noexcept
{
    return "StrictVerificationMode";
}
/// @brief Get configuration file tag string constant: ComEventGrants
/// @return processname
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_IAM_
/// @needwork = dd
/// @endcode
inline constexpr ara::iam::internal::char8_t const *GetJsonKey_ComEventGrants() noexcept { return "ComEventGrants"; }
/// @brief Get configuration file tag string constant: ComMethodGrants
/// @return processname
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_IAM_
/// @needwork = dd
/// @endcode
inline constexpr ara::iam::internal::char8_t const *GetJsonKey_ComMethodGrants() noexcept { return "ComMethodGrants"; }
/// @brief Get configuration file tag string constant: ComOfferServiceGrants
/// @return processname
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_IAM_
/// @needwork = dd
/// @endcode
inline constexpr ara::iam::internal::char8_t const *GetJsonKey_ComOfferServiceGrants() noexcept
{
    return "ComOfferServiceGrants";
}
/// @brief Get configuration file tag string constant: ComFindServiceGrants
/// @return processname
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_IAM_
/// @needwork = dd
/// @endcode
inline constexpr ara::iam::internal::char8_t const *GetJsonKey_ComFindServiceGrants() noexcept
{
    return "ComFindServiceGrants";
}
/// @brief Get configuration file tag string constant: ComRemoteEventGrants
/// @return processname
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_IAM_
/// @needwork = dd
/// @endcode
inline constexpr ara::iam::internal::char8_t const *GetJsonKey_ComRemoteEventGrants() noexcept
{
    return "ComRemoteEventGrants";
}
/// @brief Get configuration file tag string constant: ComRemoteMethodGrants
/// @return processname
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_IAM_
/// @needwork = dd
/// @endcode
inline constexpr ara::iam::internal::char8_t const *GetJsonKey_ComRemoteMethodGrants() noexcept
{
    return "ComRemoteMethodGrants";
}
/// @brief Get configuration file tag string constant: ComTlsMethodGrants
/// @return processname
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_IAM_
/// @needwork = dd
/// @endcode
inline constexpr ara::iam::internal::char8_t const *GetJsonKey_ComTlsMethodGrants() noexcept
{
    return "ComTlsMethodGrants";
}
/// @brief Get configuration file tag string constant: ComTlsEventGrants
/// @return processname
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_IAM_
/// @needwork = dd
/// @endcode
inline constexpr ara::iam::internal::char8_t const *GetJsonKey_ComTlsEventGrants() noexcept
{
    return "ComTlsEventGrants";
}
/// @brief Get configuration file tag string constant: processname
/// @return processname
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_IAM_
/// @needwork = dd
/// @endcode
inline constexpr ara::iam::internal::char8_t const *GetJsonKey_ProcessName() noexcept { return "processname"; }
/// @brief Get configuration file tag string constant: serviceid
/// @return slotname
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_IAM_
/// @needwork = dd
/// @endcode
inline constexpr ara::iam::internal::char8_t const *GetJsonKey_ServiceID() noexcept { return "serviceid"; }
/// @brief Get configuration file tag string constant: instanceid
/// @return slotname
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_IAM_
/// @needwork = dd
/// @endcode
inline constexpr ara::iam::internal::char8_t const *GetJsonKey_InstanceID() noexcept { return "instanceid"; }
/// @brief Get configuration file tag string constant: eventid
/// @return slotname
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_IAM_
/// @needwork = dd
/// @endcode
inline constexpr ara::iam::internal::char8_t const *GetJsonKey_EventID() noexcept { return "eventid"; }
/// @brief Get configuration file tag string constant: methodid
/// @return slotname
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_IAM_
/// @needwork = dd
/// @endcode
inline constexpr ara::iam::internal::char8_t const *GetJsonKey_MethodID() noexcept { return "methodid"; }
/// @brief Get configuration file tag string constant: LocalIp
/// @return slotname
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_IAM_
/// @needwork = dd
/// @endcode
inline constexpr ara::iam::internal::char8_t const *GetJsonKey_LocalIp() noexcept { return "LocalIp"; }
/// @brief Get configuration file tag string constant: LocalPortStart
/// @return slotname
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_IAM_
/// @needwork = dd
/// @endcode
inline constexpr ara::iam::internal::char8_t const *GetJsonKey_LocalPortStart() noexcept { return "LocalPortStart"; }
/// @brief Get configuration file tag string constant: LocalPortEnd
/// @return slotname
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_IAM_
/// @needwork = dd
/// @endcode
inline constexpr ara::iam::internal::char8_t const *GetJsonKey_LocalPortEnd() noexcept { return "LocalPortEnd"; }
/// @brief Get configuration file tag string constant: Protocol
/// @return slotname
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_IAM_
/// @needwork = dd
/// @endcode
inline constexpr ara::iam::internal::char8_t const *GetJsonKey_LocalProtocol() noexcept { return "Protocol"; }
/// @brief Get configuration file tag string constant: RemoteIp
/// @return slotname
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_IAM_
/// @needwork = dd
/// @endcode
inline constexpr ara::iam::internal::char8_t const *GetJsonKey_RemoteIp() noexcept { return "RemoteIp"; }
/// @brief Get configuration file tag string constant: RemotePortStart
/// @return slotname
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_IAM_
/// @needwork = dd
/// @endcode
inline constexpr ara::iam::internal::char8_t const *GetJsonKey_RemotePortStart() noexcept { return "RemotePortStart"; }
/// @brief Get configuration file tag string constant: LocalPortEnd
/// @return slotname
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_IAM_
/// @needwork = dd
/// @endcode
inline constexpr ara::iam::internal::char8_t const *GetJsonKey_RemotePortEnd() noexcept { return "RemotePortEnd"; }
/// @brief Get configuration file tag string constant: TlsName
/// @return slotname
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_IAM_
/// @needwork = dd
/// @endcode
inline constexpr ara::iam::internal::char8_t const *GetJsonKey_TlsName() noexcept { return "TlsName"; }
}  // namespace

//********************************/
MR_HEADER(ara::iam::internal::com::PIamConfigData_ComEventGrant)
MR_FIELD(GetJsonKey_ProcessName(), stProcessName)
MR_FIELD(GetJsonKey_ServiceID(), nServiceID)
MR_FIELD(GetJsonKey_InstanceID(), nInstanceID)
MR_FIELD(GetJsonKey_EventID(), nEventID)
MR_FOOTER
//****************/
MR_HEADER(ara::iam::internal::com::PIamConfigData_ComMethodGrant)
MR_FIELD(GetJsonKey_ProcessName(), stProcessName)
MR_FIELD(GetJsonKey_ServiceID(), nServiceID)
MR_FIELD(GetJsonKey_InstanceID(), nInstanceID)
MR_FIELD(GetJsonKey_MethodID(), nMethodID)
MR_FOOTER
//****************/
MR_HEADER(ara::iam::internal::com::PIamConfigData_ComOfferServiceGrant)
MR_FIELD(GetJsonKey_ProcessName(), stProcessName)
MR_FIELD(GetJsonKey_ServiceID(), nServiceID)
MR_FIELD(GetJsonKey_InstanceID(), nInstanceID)
MR_FOOTER
//****************/
MR_HEADER(ara::iam::internal::com::PIamConfigData_ComFindServiceGrant)
MR_FIELD(GetJsonKey_ProcessName(), stProcessName)
MR_FIELD(GetJsonKey_ServiceID(), nServiceID)
MR_FIELD(GetJsonKey_InstanceID(), nInstanceID)
MR_FOOTER
//****************/
MR_HEADER(ara::iam::internal::com::PIamConfigData_ComRemoteEventGrant)
MR_FIELD(GetJsonKey_LocalIp(), stLocalIp)
MR_FIELD(GetJsonKey_LocalPortStart(), nLocalPortStart)
MR_FIELD(GetJsonKey_LocalPortEnd(), nLocalPortEnd)
MR_FIELD(GetJsonKey_RemoteIp(), stRemoteIp)
MR_FIELD(GetJsonKey_RemotePortStart(), nRemotePortStart)
MR_FIELD(GetJsonKey_RemotePortEnd(), nRemotePortEnd)
MR_FIELD(GetJsonKey_ServiceID(), nServiceID)
MR_FIELD(GetJsonKey_InstanceID(), nInstanceID)
MR_FIELD(GetJsonKey_EventID(), nEventID)
MR_FOOTER
//****************/
MR_HEADER(ara::iam::internal::com::PIamConfigData_ComRemoteMethodGrant)
MR_FIELD(GetJsonKey_LocalIp(), stLocalIp)
MR_FIELD(GetJsonKey_LocalPortStart(), nLocalPortStart)
MR_FIELD(GetJsonKey_LocalPortEnd(), nLocalPortEnd)
MR_FIELD(GetJsonKey_RemoteIp(), stRemoteIp)
MR_FIELD(GetJsonKey_RemotePortStart(), nRemotePortStart)
MR_FIELD(GetJsonKey_RemotePortEnd(), nRemotePortEnd)
MR_FIELD(GetJsonKey_LocalProtocol(), stProtocol)
MR_FIELD(GetJsonKey_ServiceID(), nServiceID)
MR_FIELD(GetJsonKey_InstanceID(), nInstanceID)
MR_FIELD(GetJsonKey_MethodID(), nMethodID)
MR_FOOTER
//****************/
MR_HEADER(ara::iam::internal::com::PIamConfigData_ComTlsMethodGrant)
MR_FIELD(GetJsonKey_TlsName(), stTlsName)
MR_FIELD(GetJsonKey_ServiceID(), nServiceID)
MR_FIELD(GetJsonKey_InstanceID(), nInstanceID)
MR_FIELD(GetJsonKey_MethodID(), nMethodID)
MR_FOOTER
//****************/
MR_HEADER(ara::iam::internal::com::PIamConfigData_ComTlsEventGrant)
MR_FIELD(GetJsonKey_TlsName(), stTlsName)
MR_FIELD(GetJsonKey_ServiceID(), nServiceID)
MR_FIELD(GetJsonKey_InstanceID(), nInstanceID)
MR_FIELD(GetJsonKey_EventID(), nEventID)
MR_FOOTER
//********************************/
namespace ara {
namespace iam {
namespace internal {
namespace com {
//********************************/
/// @brief Compare input information with configuration information
/// PIamConfigData_ComRemoteEventGrant
/// @param configGrant Configuration information
/// @param nPortRemote remote port
/// @param stLocalIp local ip
/// @param nPortLocal local port
/// @param nServiceID service id
/// @param nInstanceID instance id
/// @param nEventID evnet id
/// @return Comparison result true or false
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = dda
/// @endcode
bool CompareConfig(PIamConfigData_ComRemoteEventGrant const &configGrant,
                   uint32_t const &nPortRemote,
                   ara::core::String const &stLocalIp,
                   int32_t const &nPortLocal,
                   uint16_t const &nServiceID,
                   uint16_t const &nInstanceID,
                   uint16_t const &nEventID) noexcept
{
    if (stLocalIp != configGrant.stLocalIp) {
        return false;
    }
    if ((nPortLocal < configGrant.nLocalPortStart) || (nPortLocal > configGrant.nLocalPortEnd)) {
        return false;
    }
    if (nServiceID != configGrant.nServiceID) {
        return false;
    }
    if (nInstanceID != configGrant.nInstanceID) {
        return false;
    }
    if (nEventID != configGrant.nEventID) {
        return false;
    }
    if ((nPortRemote < configGrant.nRemotePortStart) || (nPortRemote > configGrant.nRemotePortEnd)) {
        return false;
    }
    return true;
}
/// @brief Compare input information with configuration information
/// PIamConfigData_ComRemoteEventGrant
/// @param configGrant Configuration information
/// @param nPortRemote remote port
/// @param stLocalIp local ip
/// @param nPortLocal local port
/// @param nServiceID service id
/// @param nInstanceID instance id
/// @param nMethodID method id
/// @param stProtocol protocol(TCP/UDP)
/// @return Comparison result true or false
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = dda
/// @endcode
bool CompareConfig(PIamConfigData_ComRemoteMethodGrant const &configGrant,
                   uint32_t const &nPortRemote,
                   ara::core::String const &stLocalIp,
                   int32_t const &nPortLocal,
                   uint16_t const &nServiceID,
                   uint16_t const &nInstanceID,
                   uint16_t const &nMethodID,
                   ara::core::String const &stProtocol) noexcept
{
    if (stLocalIp != configGrant.stLocalIp) {
        return false;
    }
    if ((nPortLocal < configGrant.nLocalPortStart) || (nPortLocal > configGrant.nLocalPortEnd)) {
        return false;
    }
    if (nServiceID != configGrant.nServiceID) {
        return false;
    }
    if (nInstanceID != configGrant.nInstanceID) {
        return false;
    }
    if (nMethodID != configGrant.nMethodID) {
        return false;
    }
    if ((nPortRemote < configGrant.nRemotePortStart) || (nPortRemote > configGrant.nRemotePortEnd)) {
        return false;
    }
    if (stProtocol != configGrant.stProtocol) {
        return false;
    }
    return true;
}
//********************************/
/// @brief Load Com configuration data
/// @return load success/falied.
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = ad
/// @endcode
bool PConfigIam_Com::Initialize() noexcept
{
    _ResetData();
    _ClearReady();
    SetErrorCode(ara::iam::internal::grant::GrantSerializationErrc::kInitCode);
    return true;
}
/// @brief Clear  All GrantInfo
/// @returns true
bool PConfigIam_Com::Deinitialize() noexcept
{
    _ResetData();
    _ClearReady();
    SetErrorCode(ara::iam::internal::grant::GrantSerializationErrc::kInitCode);
    return true;
}
/// @brief Initialize configuration file
/// @param stFileName Configuration file name
/// @return true if init sucess false otherwise
bool PConfigIam_Com::InitManifest(ara::core::StringView const &stFileName) noexcept
{
    ara::core::Result< std::unique_ptr< isoft::manifestreader::Manifest > > manifestRes{
        isoft::manifestreader::OpenManifest(stFileName)};
    if (!manifestRes.HasValue()) {
        SetErrorCode(ara::iam::internal::grant::GrantSerializationErrc::kFileNotFound);
        return false;
    }
    std::unique_ptr< isoft::manifestreader::Manifest > const pManifestRes{std::move(manifestRes).Value()};
    common::IamLogger().LogInfo() << "PConfigIam_Com::LoadData ConfigName = " << stFileName.data();

    bLocalComAccessControlEnabled_ = pManifestRes->GetValue(GetJsonKey_LocalComAccessControlEnabled(), false);
    bRemoteAccessControlEnabled_   = pManifestRes->GetValue(GetJsonKey_RemoteAccessControlEnabled(), false);
    // Get IAM strict verification switch.
    strictVerificationMode_ = pManifestRes->GetValue(GetJsonKey_StrictVerificationMode(), false);

    std::ignore = common::ReadMapData< PIamConfigData_ComEventGrant >(
        pManifestRes, ara::core::String(GetJsonKey_ComEventGrants()),
        [this](PIamConfigData_ComEventGrant const &data) -> void { setIamConfigComEventGrant_.insert(data); });

    std::ignore = common::ReadMapData< PIamConfigData_ComMethodGrant >(
        pManifestRes, ara::core::String(GetJsonKey_ComMethodGrants()),
        [this](PIamConfigData_ComMethodGrant const &data) -> void { setIamConfigComMethodGrant_.insert(data); });

    std::ignore = common::ReadMapData< PIamConfigData_ComOfferServiceGrant >(
        pManifestRes, ara::core::String(GetJsonKey_ComOfferServiceGrants()),
        [this](PIamConfigData_ComOfferServiceGrant const &data) -> void {
            setIamConfigComOfferServiceGrant_.insert(data);
        });

    std::ignore = common::ReadMapData< PIamConfigData_ComFindServiceGrant >(
        pManifestRes, ara::core::String(GetJsonKey_ComFindServiceGrants()),
        [this](PIamConfigData_ComFindServiceGrant const &data) -> void {
            setIamConfigComFindServiceGrant_.insert(data);
        });
    std::ignore = common::ReadMapData< PIamConfigData_ComRemoteEventGrant >(
        pManifestRes, ara::core::String(GetJsonKey_ComRemoteEventGrants()),
        [this](PIamConfigData_ComRemoteEventGrant const &data) -> void {
            if ((data.stLocalIp.empty()) || (-1 == data.nLocalPortStart) || (-1 == data.nLocalPortEnd)) {
                PIamConfigData_ComRemoteEventGrant newData{data};
                newData.stLocalIp.clear();
                newData.nLocalPortStart = -1;
                newData.nLocalPortEnd   = -1;
                mapIamConfigComRemoteEventGrant_.emplace(data.stRemoteIp, newData);
            } else {
                mapIamConfigComRemoteEventGrant_.emplace(data.stRemoteIp, data);
            }
        });

    std::ignore = common::ReadMapData< PIamConfigData_ComRemoteMethodGrant >(
        pManifestRes, ara::core::String(GetJsonKey_ComRemoteMethodGrants()),
        [this](PIamConfigData_ComRemoteMethodGrant const &data) -> void {
            if ((data.stLocalIp.empty()) || (-1 == data.nLocalPortStart) || (-1 == data.nLocalPortEnd)) {
                PIamConfigData_ComRemoteMethodGrant newData{data};
                newData.stLocalIp.clear();
                newData.nLocalPortStart = -1;
                newData.nLocalPortEnd   = -1;
                mapIamConfigComRemoteMethodGrant_.emplace(data.stRemoteIp, newData);
            } else {
                mapIamConfigComRemoteMethodGrant_.emplace(data.stRemoteIp, data);
            }
        });

    std::ignore = common::ReadMapData< PIamConfigData_ComTlsMethodGrant >(
        pManifestRes, ara::core::String(GetJsonKey_ComTlsMethodGrants()),
        [this](PIamConfigData_ComTlsMethodGrant const &data) -> void { setIamConfigComTlsMethodGrant_.insert(data); });

    std::ignore = common::ReadMapData< PIamConfigData_ComTlsEventGrant >(
        pManifestRes, ara::core::String(GetJsonKey_ComTlsEventGrants()),
        [this](PIamConfigData_ComTlsEventGrant const &data) -> void { setIamConfigComTlsEventGrant_.insert(data); });

    _SetReady();
    SetErrorCode(ara::iam::internal::grant::GrantSerializationErrc::kInitCode);
    return IsReady();
}
//********************************/
/// @brief Check if data exists: grant Event check.
/// @param grantObject event struct
/// @return  event check success or failed.
bool PConfigIam_Com::GrantCheck_ComEvent(PIamConfigData_ComEventGrant const &grantObject) const noexcept
{
    return _T_GrantCheck(bLocalComAccessControlEnabled_, setIamConfigComEventGrant_, grantObject);
}

/// @brief Check if data exists: grant Method check.
/// @param grantObject method struct
/// @return method check success or failed.
bool PConfigIam_Com::GrantCheck_ComMethod(PIamConfigData_ComMethodGrant const &grantObject) const noexcept
{
    return _T_GrantCheck(bLocalComAccessControlEnabled_, setIamConfigComMethodGrant_, grantObject);
}

/// @brief Check if data exists: grant offerservice check.
/// OfferServiceGrantCheck
/// @param grantObject offer service struct
/// @return offerservice check success or failed.
bool PConfigIam_Com::GrantCheck_ComOfferService(PIamConfigData_ComOfferServiceGrant const &grantObject) const noexcept
{
    return _T_GrantCheck(bLocalComAccessControlEnabled_, setIamConfigComOfferServiceGrant_, grantObject);
}

/// @brief Check if data exists: grant findservice check.
/// @param grantObject fs struct
/// @return findservice check success or failed.
bool PConfigIam_Com::GrantCheck_ComFindService(PIamConfigData_ComFindServiceGrant const &grantObject) const noexcept
{
    return _T_GrantCheck(bLocalComAccessControlEnabled_, setIamConfigComFindServiceGrant_, grantObject);
}

/// @brief Check if data exists: RemoteIpEventGrant declear
/// @param stRemoteIp remote ip
/// @param nPortRemote remote port
/// @param stLocalIp local ip
/// @param nPortLocal local port
/// @param nServiceID service id
/// @param nInstanceID instance id
/// @param nEventID evnet id
/// @return ture/false
bool PConfigIam_Com::GrantCheck_RemoteIpEvent(ara::core::String const &stRemoteIp,
                                              uint32_t const &nPortRemote,
                                              ara::core::String const &stLocalIp,
                                              int32_t const &nPortLocal,
                                              uint16_t const &nServiceID,
                                              uint16_t const &nInstanceID,
                                              uint16_t const &nEventID) const noexcept
{
    SetErrorCode(ara::iam::internal::grant::GrantSerializationErrc::kInitCode);
    // no iam check. no limitation.
    if (false == bRemoteAccessControlEnabled_) {
        return true;
    }
    // get equal ip from cache.
    auto const getRmIp{mapIamConfigComRemoteEventGrant_.equal_range(stRemoteIp)};

    // Efficiency consideration: In strict verification mode, if no corresponding
    // Grant configuration is found, return directly.
    if (getRmIp.first == getRmIp.second) {
        if (strictVerificationMode_) {
            return false;
        }
    }
    for (typename MAP_IamConfig_ComRemoteEventGrant::const_iterator it{getRmIp.first}; it != getRmIp.second; it++) {
        // Users can choose to configure local port + IP verification or not, IAM is
        // compatible here.
        //  If the user has not configured the local IAM IP and port,
        //  then the remote IAM restriction content does not include the local IAM
        //  configuration.
        if (CompareConfig(it->second, nPortRemote, stLocalIp, nPortLocal, nServiceID, nInstanceID, nEventID)) {
            common::IamLogger().LogInfo() << "remote iam event check success!";
            return true;
        }
    }

    // If no IAM grant configuration is found, i.e., there is no IAM configuration
    // corresponding to this IP, in non-strict verification mode, IAM allows it to
    // pass by default.
    if (!strictVerificationMode_) {
        return true;
    }

    common::IamLogger().LogError() << "remote iam event check failed!";
    SetErrorCode(ara::iam::internal::grant::GrantSerializationErrc::kGrantNotFound);
    return false;
}

/// @brief Check if data exists: RemoteIpMethodGrant declear.
/// @param stRemoteIp remote ip
/// @param nPortRemote remote port
/// @param stLocalIp local ip
/// @param nPortLocal local port
/// @param nServiceID service id
/// @param nInstanceID instance id
/// @param nMethodID method id
/// @param stProtocol protocol(TCP/UDP)
/// @return ture/false
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_IAM_00104
/// @needwork = ad
/// @endcode
bool PConfigIam_Com::GrantCheck_RemoteIpMethod(ara::core::String const &stRemoteIp,
                                               uint32_t const &nPortRemote,
                                               ara::core::String const &stLocalIp,
                                               int32_t const &nPortLocal,
                                               uint16_t const &nServiceID,
                                               uint16_t const &nInstanceID,
                                               uint16_t const &nMethodID,
                                               ara::core::String const &stProtocol) const noexcept
{
    SetErrorCode(ara::iam::internal::grant::GrantSerializationErrc::kInitCode);
    // no iam check. no limitation.
    if (false == bRemoteAccessControlEnabled_) {
        return true;
    }
    // get equal ip from cache.
    auto const getRmIp{mapIamConfigComRemoteMethodGrant_.equal_range(stRemoteIp)};

    // Efficiency consideration: In strict verification mode, if no corresponding
    // Grant configuration is found, return directly.
    if (getRmIp.first == getRmIp.second) {
        if (strictVerificationMode_) {
            return false;
        }
    }

    for (auto it{getRmIp.first}; it != getRmIp.second; it++) {
        // If the user has not configured the local IAM IP and port,
        // then the remote IAM restriction content does not include the local IAM
        // configuration.
        if (CompareConfig(it->second, nPortRemote, stLocalIp, nPortLocal, nServiceID, nInstanceID, nMethodID,
                          stProtocol)) {
            common::IamLogger().LogInfo() << "remote iam method check success!";
            return true;
        }
    }

    // If no IAM grant configuration is found, i.e., there is no IAM configuration
    // corresponding to this IP, in non-strict verification mode, IAM allows it to
    // pass by default.
    if (!strictVerificationMode_) {
        return true;
    }

    common::IamLogger().LogError() << "remote iam method check failed!";
    SetErrorCode(ara::iam::internal::grant::GrantSerializationErrc::kGrantNotFound);
    return false;
}

/// @brief Check if data exists: grant Event check.
/// @param grantObject tls event struct
/// @return  event check success or failed.
bool PConfigIam_Com::GrantCheck_ComTlsMethod(PIamConfigData_ComTlsMethodGrant const &grantObject) const noexcept
{
    bool bReturn{_T_GrantCheck(bRemoteAccessControlEnabled_, setIamConfigComTlsMethodGrant_, grantObject)};
    if (bReturn) {
        common::IamLogger().LogInfo() << "Tls Event Grant check  success!";
    } else {
        common::IamLogger().LogInfo() << "Tls Event Grant check  failed!";
    }
    return bReturn;
}

/// @brief Check if data exists: grant Method check.
/// @param grantObject tls method struct
/// @return method check success or failed.
bool PConfigIam_Com::GrantCheck_ComTlsEvent(PIamConfigData_ComTlsEventGrant const &grantObject) const noexcept
{
    bool bReturn{_T_GrantCheck(bRemoteAccessControlEnabled_, setIamConfigComTlsEventGrant_, grantObject)};
    if (bReturn) {
        common::IamLogger().LogInfo() << "Tls Method Grant check  success!";
    } else {
        common::IamLogger().LogInfo() << "Tls Method Grant check  failed!";
    }
    return bReturn;
}
//********************************/
/// @brief Reset data
void PConfigIam_Com::_ResetData()
{
    bLocalComAccessControlEnabled_ = false;
    bRemoteAccessControlEnabled_   = false;
    strictVerificationMode_        = false;
    setIamConfigComEventGrant_.clear();
    setIamConfigComMethodGrant_.clear();
    setIamConfigComOfferServiceGrant_.clear();
    setIamConfigComFindServiceGrant_.clear();
    mapIamConfigComRemoteEventGrant_.clear();
    mapIamConfigComRemoteMethodGrant_.clear();
    setIamConfigComTlsMethodGrant_.clear();
    setIamConfigComTlsEventGrant_.clear();
}
//********************************/
}  // namespace com
}  // namespace internal
}  // namespace iam
}  // namespace ara