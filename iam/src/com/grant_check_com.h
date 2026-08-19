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
/// @file       grant_check_com.h
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

#ifndef ARA_IAM_COM_GRANT_CHECK_COM_H_
#define ARA_IAM_COM_GRANT_CHECK_COM_H_
#include <ara/core/result.h>
#include <ara/core/string.h>

#include "config_iam_com.h"
namespace ara {
namespace iam {
namespace internal {
namespace com {
//********************************/
/// @brief IAM-Com checker
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = ad
/// @endcode
class PGrantCheck_Com final
{
public:
    /// @brief Shared pointer
    using Uptr = std::shared_ptr< PGrantCheck_Com >;

public:
    /// @brief PGrantCheck_Com constructor.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = ad
    /// @endcode
    PGrantCheck_Com() = default;

    /// @brief PGrantCheck_Com Destruction.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = ad
    /// @endcode
    ~PGrantCheck_Com() = default;

    /// @brief move constructor of PGrantCheck_Com.
    /// @param other reference of object.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = ad
    /// @endcode
    explicit PGrantCheck_Com(PGrantCheck_Com &&other) = delete;

    /// @brief copy constructor of PGrantCheck_Com.
    /// @param other reference of object.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = ad
    /// @endcode
    explicit PGrantCheck_Com(PGrantCheck_Com const &other) = delete;

    /// @brief assignment operator of PGrantCheck_Com.
    /// @param other reference of object.
    /// @returns reference of this object.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = ad
    /// @endcode
    PGrantCheck_Com &operator=(PGrantCheck_Com const &other) = delete;

    /// @brief move assignment operator of PGrantCheck_Com.
    /// @param other reference of object.
    /// @returns reference of this object.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = ad
    /// @endcode
    PGrantCheck_Com &operator=(PGrantCheck_Com &&other) = delete;

    /// @brief Load Com configuration data
    /// @brief com init  load  data .

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
    /// @brief Whether initialization is successful
    /// @return true if has init manifest sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    bool IsReady() const noexcept;
    /// @brief Return error code
    /// @return Error code
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    ara::iam::internal::grant::GrantSerializationErrc GetErrorCode() const noexcept;

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
    /// @brief Checks if the remote method given grant exists
    /// @param stRemoteIp remote ip
    /// @param nPortRemote remote port
    /// @param stLocalIp local ip
    /// @param nPortLocal local port
    /// @param nServiceID service id
    /// @param nInstanceID instance id
    /// @param nMethodID method id
    /// @param stProtocol protocol(TCP/UDP)
    /// @return true/false
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

    /// @brief grant Method check.
    /// @param grantObject tls method struct
    /// @return method check success or failed.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_IAM_00104
    /// @needwork = ad
    /// @endcode
    bool GrantCheck_ComTlsEvent(PIamConfigData_ComTlsEventGrant const &grantObject) const noexcept;

    /// @brief grant Event check.
    /// @param grantObject tls event struct
    /// @return  event check success or failed.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_IAM_00103
    /// @needwork = ad
    /// @endcode
    bool GrantCheck_ComTlsMethod(PIamConfigData_ComTlsMethodGrant const &grantObject) const noexcept;

private:
    PConfigIam_Com iamConfigCom_{};
};
//********************************/
}  // namespace com
}  // namespace internal
}  // namespace iam
}  // namespace ara
#endif  // ARA_IAM_COM_GRANT_CHECK_COM_H_