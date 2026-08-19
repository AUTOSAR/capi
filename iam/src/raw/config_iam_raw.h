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
/// @file       config_iam_raw.h
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
/// module: RAW
/// @endcode
///
/// ================================================================
///
/// rights reserved.
///
/// ================================================================

#ifndef ARA_IAM_RAW_CONFIG_IAM_RAW_H_
#define ARA_IAM_RAW_CONFIG_IAM_RAW_H_

#include <ara/core/instance_specifier.h>
#include <ara/core/string.h>
#include <ara/core/string_view.h>

#include <map>
#include <set>

#include "common/config_iam_base.h"
#include "common/data_type.h"

namespace ara {
namespace iam {
namespace internal {
namespace com {
//********************************/
#pragma pack(push, 4)
/// @brief  RawDataStreamGrants
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = ad
/// @endcode
struct PIamConfigData_RawDataStreamGrant final
{
public:
    /// @brief  processname
    ara::core::String stProcessName{};
    /// @brief  ip
    ara::core::String stIp{};
    /// @brief  udpPort
    int16_t nUdpPort{0U};
    /// @brief  tcpPort
    int16_t nTcpPort{0U};
    /// @brief  mulCastUdpPort
    int16_t nMulCastUdpPort{0U};
};
#pragma pack(pop)
/// @brief operator < compare.
/// @param lhs left hand
/// @param rhs right hand
/// @return bool
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_IAM_00106
/// @needwork = ad
/// @endcode
inline bool operator<(PIamConfigData_RawDataStreamGrant const &lhs,
                      PIamConfigData_RawDataStreamGrant const &rhs) noexcept
{
    if (lhs.stProcessName != rhs.stProcessName) {
        return lhs.stProcessName < rhs.stProcessName;
    }
    if (lhs.stIp != rhs.stIp) {
        return lhs.stIp < rhs.stIp;
    }
    if (lhs.nTcpPort != rhs.nTcpPort) {
        return lhs.nTcpPort < rhs.nTcpPort;
    }
    if (lhs.nTcpPort == static_cast< uint32_t >(0)) {
        return lhs.nMulCastUdpPort < rhs.nMulCastUdpPort;
    }
    if (lhs.nMulCastUdpPort == static_cast< uint32_t >(0)) {
        return lhs.nUdpPort < rhs.nUdpPort;
    }
    return false;
}
//********************************/
/// @brief Get configuration file tag string constant: Configuration file name
/// @return ./etc/machine_iam_phm.json
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_IAM_
/// @needwork = ad
/// @endcode
inline constexpr ara::iam::internal::char8_t const *GetConfigFileName_IamRaw() noexcept
{
    return "./etc/machine_iam_raw.json";
}
//********************************/
/// @brief Data structure after Manifest configuration interpretation
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = ad
/// @endcode
class PConfigIam_Raw : public common::PConfigIam_Base
{
public:
    /// @brief RawEventGrant storage MAP
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    using SET_IamConfig_RawDataStreamGrant = std::set< PIamConfigData_RawDataStreamGrant >;

public:
    /// @brief the constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    PConfigIam_Raw() noexcept = default;
    /// @brief Default destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    virtual ~PConfigIam_Raw() = default;
    /// @brief Default copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    PConfigIam_Raw(PConfigIam_Raw const &other) = delete;
    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    PConfigIam_Raw(PConfigIam_Raw &&other) = delete;
    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    PConfigIam_Raw &operator=(PConfigIam_Raw const &other) = delete;
    /// @brief Default move assignment function
    /// @param other Another object instance of this class
    /// @return  *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    PConfigIam_Raw &operator=(PConfigIam_Raw &&other) = delete;

public:
    /// @brief Load Raw configuration data
    /// @return load success/falied.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_IAM_
    /// @needwork = ad
    /// @endcode
    bool Initialize() noexcept;
    /// @brief Clear  All GrantInfo
    /// @returns true
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = ad
    /// @endcode
    bool Deinitialize() noexcept;
    /// @brief Initialize configuration file
    /// @param stFileName Configuration file
    /// @return true if has init manifest sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    bool InitManifest(ara::core::StringView const &stFileName) noexcept;

public:
    /// @brief grant Event check.
    /// @param grantObject event struct
    /// @return  event check success or failed.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_IAM_00106
    /// @needwork = ad
    /// @endcode
    bool GrantCheck_RawDataStream(PIamConfigData_RawDataStreamGrant const &grantObject) const noexcept;

protected:
    /// @brief Reset data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = ad
    /// @endcode
    void _ResetData();
    /// @brief Permission check template function
    /// @tparam T_GrantList
    /// @tparam T_GrantObject
    /// @param bEnable Whether enabled
    /// @param mapGrant Template permission list
    /// @param grantObject Target check object
    /// @return Whether found
    template < class T_GrantList, class T_GrantObject >
    bool _T_GrantCheck(bool const bEnable, T_GrantList const &mapGrant, T_GrantObject const &grantObject) const noexcept
    {
        SetErrorCode(ara::iam::internal::grant::GrantSerializationErrc::kInitCode);
        if (false == bEnable) {
            return true;
        }
        typename T_GrantList::const_iterator const &itFind = mapGrant.find(grantObject);
        if (itFind == mapGrant.end()) {
            // If in non-strict verification mode, IAM grants passage when Grant model
            // is not configured.
            if (!bRawStrictVerificationMode_) {
                return true;
            }
            SetErrorCode(ara::iam::internal::grant::GrantSerializationErrc::kGrantNotFound);
            return false;
        }
        return true;
    }

private:
    /// @brief LocalComAccessControlEnabled
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    bool bLocalComAccessControlEnabled_{false};
    /// @brief bRawStrictVerificationMode_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    bool bRawStrictVerificationMode_{false};
    /// @brief Interpret RawDataStreamGrants
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    SET_IamConfig_RawDataStreamGrant setIamConfigRawDataStreamGrant_{};
};
//********************************/
}  // namespace com
}  // namespace internal
}  // namespace iam
}  // namespace ara

#endif  // ARA_IAM_RAW_CONFIG_IAM_RAW_H_
