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
/// @file       config_iam_com.h
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

#ifndef ARA_IAM_COM_CONFIG_IAM_COM_H_
#define ARA_IAM_COM_CONFIG_IAM_COM_H_

#include <ara/core/instance_specifier.h>
#include <ara/core/string.h>
#include <ara/core/string_view.h>

#include <map>
#include <set>

#include "common/config_iam_base.h"
#include "common/data_type.h"
#include "config_iam_com_data.h"

namespace ara {
namespace iam {
namespace internal {
namespace com {
/// @brief Get configuration file tag string constant: Configuration file name
/// @return ./etc/machine_iam_phm.json
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_IAM_
/// @needwork = ad
/// @endcode
inline constexpr ara::iam::internal::char8_t const *GetConfigFileName_IamCom() noexcept
{
    return "./etc/machine_iam_com.json";
}
/// @brief Get configuration file tag string constant: Configuration file name
/// @return ./etc/machine_iam_phm.json
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_IAM_
/// @needwork = ad
/// @endcode
inline constexpr ara::iam::internal::char8_t const *GetConfigFileName_Machine() noexcept
{
    return "./etc/machine_manifest.json";
}
//********************************/
/// @brief Data structure after Manifest configuration interpretation
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = ad
/// @endcode
class PConfigIam_Com : public common::PConfigIam_Base
{
public:
    /// @brief ComEventGrant storage MAP
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    using SET_IamConfig_ComEventGrant = std::set< PIamConfigData_ComEventGrant >;
    /// @brief ComMethodGrant storage MAP
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    using SET_IamConfig_ComMethodGrant = std::set< PIamConfigData_ComMethodGrant >;
    /// @brief ComOfferServiceGrant storage MAP
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    using SET_IamConfig_ComOfferServiceGrant = std::set< PIamConfigData_ComOfferServiceGrant >;
    /// @brief ComFindServiceGrant storage MAP
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    using SET_IamConfig_ComFindServiceGrant = std::set< PIamConfigData_ComFindServiceGrant >;
    /// @brief ComRemoteEventGrant storage MAP
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    using MAP_IamConfig_ComRemoteEventGrant = std::multimap< ara::core::String, PIamConfigData_ComRemoteEventGrant >;
    /// @brief ComRemoteEventGrant storage MAP
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    using MAP_IamConfig_ComRemoteMethodGrant = std::multimap< ara::core::String, PIamConfigData_ComRemoteMethodGrant >;
    /// @brief ComTlsMethodGrant storage MAP
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    using SET_IamConfig_ComTlsMethodGrant = std::set< PIamConfigData_ComTlsMethodGrant >;
    /// @brief ComTlsEventGrant storage MAP
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    using SET_IamConfig_ComTlsEventGrant = std::set< PIamConfigData_ComTlsEventGrant >;

public:
    /// @brief the constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    PConfigIam_Com() noexcept = default;
    /// @brief Default destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    virtual ~PConfigIam_Com() = default;
    /// @brief Default copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    PConfigIam_Com(PConfigIam_Com const &other) = delete;
    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    PConfigIam_Com(PConfigIam_Com &&other) = delete;
    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    PConfigIam_Com &operator=(PConfigIam_Com const &other) = delete;
    /// @brief Default move assignment function
    /// @param other Another object instance of this class
    /// @return  *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    PConfigIam_Com &operator=(PConfigIam_Com &&other) = delete;

public:
    /// @brief Load Com configuration data
    /// @return load success/falied.
    /// @code{.isoft}
    /// @interface_level=unit
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
    /// @trace_id_sr=SR_IAM_00103
    /// @needwork = ad
    /// @endcode
    bool GrantCheck_ComEvent(PIamConfigData_ComEventGrant const &grantObject) const noexcept;

    /// @brief grant Method check.
    /// @param grantObject method struct
    /// @return method check success or failed.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_IAM_00104
    /// @needwork = ad
    /// @endcode
    bool GrantCheck_ComMethod(PIamConfigData_ComMethodGrant const &grantObject) const noexcept;

    /// @brief grant offerservice check.OfferServiceGrantCheck
    /// @param grantObject offer service struct
    /// @return offerservice check success or failed.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_IAM_00102
    /// @needwork = ad
    /// @endcode
    bool GrantCheck_ComOfferService(PIamConfigData_ComOfferServiceGrant const &grantObject) const noexcept;

    /// @brief grant findservice check.
    /// @param grantObject fs struct
    /// @return findservice check success or failed.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_IAM_00102
    /// @needwork = ad
    /// @endcode
    bool GrantCheck_ComFindService(PIamConfigData_ComFindServiceGrant const &grantObject) const noexcept;

    /// @brief  RemoteIpEventGrant declear
    /// @param stRemoteIp remote ip
    /// @param nPortRemote remote port
    /// @param stLocalIp local ip
    /// @param nPortLocal local port
    /// @param nServiceID service id
    /// @param nInstanceID instance id
    /// @param nEventID evnet id
    /// @return ture/false
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_IAM_00103
    /// @needwork = ad
    /// @endcode
    bool GrantCheck_RemoteIpEvent(ara::core::String const &stRemoteIp,
                                  uint32_t const &nPortRemote,
                                  ara::core::String const &stLocalIp,
                                  int32_t const &nPortLocal,
                                  uint16_t const &nServiceID,
                                  uint16_t const &nInstanceID,
                                  uint16_t const &nEventID) const noexcept;
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
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_IAM_00104
    /// @needwork = ad
    /// @endcode
    bool GrantCheck_RemoteIpMethod(ara::core::String const &stRemoteIp,
                                   uint32_t const &nPortRemote,
                                   ara::core::String const &stLocalIp,
                                   int32_t const &nPortLocal,
                                   uint16_t const &nServiceID,
                                   uint16_t const &nInstanceID,
                                   uint16_t const &nMethodID,
                                   ara::core::String const &stProtocol) const noexcept;

    /// @brief grant Event check.
    /// @param grantObject tls event struct
    /// @return  event check success or failed.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_IAM_00103
    /// @needwork = ad
    /// @endcode
    bool GrantCheck_ComTlsMethod(PIamConfigData_ComTlsMethodGrant const &grantObject) const noexcept;

    /// @brief grant Method check.
    /// @param grantObject tls method struct
    /// @return method check success or failed.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_IAM_00104
    /// @needwork = ad
    /// @endcode
    bool GrantCheck_ComTlsEvent(PIamConfigData_ComTlsEventGrant const &grantObject) const noexcept;

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
            // If in non-strict verification mode, IAM will grant passage if the Grant
            // model is not configured.
            if (!strictVerificationMode_) {
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
    /// @brief RemoteAccessControlEnabled
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    bool bRemoteAccessControlEnabled_{false};
    /// @brief strictVerificationMode Strict verification mode flag.
    // If enabled, someip must have all IAM grants configured to pass. If
    // disabled, unconfigured IAM grants passage.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    bool strictVerificationMode_{false};
    /// @brief Interpret ComEventGrants
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    SET_IamConfig_ComEventGrant setIamConfigComEventGrant_{};
    /// @brief Interpret ComMethodGrants
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    SET_IamConfig_ComMethodGrant setIamConfigComMethodGrant_{};
    /// @brief Interpret ComOfferServiceGrants
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    SET_IamConfig_ComOfferServiceGrant setIamConfigComOfferServiceGrant_{};
    /// @brief Interpret ComFindServiceGrants
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    SET_IamConfig_ComFindServiceGrant setIamConfigComFindServiceGrant_{};
    /// @brief Interpret ComRemoteEventGrants
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    MAP_IamConfig_ComRemoteEventGrant mapIamConfigComRemoteEventGrant_{};
    /// @brief Interpret ComRemoteMethodGrants
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    MAP_IamConfig_ComRemoteMethodGrant mapIamConfigComRemoteMethodGrant_{};
    /// @brief Interpret ComTlsMethodGrants
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    SET_IamConfig_ComTlsMethodGrant setIamConfigComTlsMethodGrant_{};
    /// @brief Interpret ComTlsEventGrants
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    SET_IamConfig_ComTlsEventGrant setIamConfigComTlsEventGrant_{};
};
//********************************/
}  // namespace com
}  // namespace internal
}  // namespace iam
}  // namespace ara

#endif  // ARA_IAM_COM_CONFIG_IAM_COM_H_
