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
/// @file       iam_grant_com.h
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
/// @unit_description=Check verification provided by IAM for the Com module
/// @endcode
///
/// ================================================================
///
/// rights reserved.
///
/// ================================================================

#ifndef ARA_IAM_COM_IAM_GRANT_COM_H_
#define ARA_IAM_COM_IAM_GRANT_COM_H_

#include <ara/core/result.h>
#include <ara/core/string.h>

#include <cstdint>
namespace ara {
namespace iam {
namespace internal {
namespace com {
//********************************/
/// @brief Interface for the IAMGrantQueryClient. This class establishes grant
/// init and check process.
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = ad
/// @endcode
class PIamGrant_Com final
{
public:
    /// @brief Construct a new PIamGrant_Com object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = ad
    /// @endcode
    PIamGrant_Com() = default;

    /// @brief Destroy the PIamGrant_Com object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = ad
    /// @endcode
    ~PIamGrant_Com() = default;

    /// @brief copy constructor of PIamGrant_Com.
    /// @param other reference of object.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = ad
    /// @endcode
    explicit PIamGrant_Com(PIamGrant_Com &other) = delete;

    /// @brief move constructor of PIamGrant_Com
    /// @param other reference of object.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = ad
    /// @endcode
    explicit PIamGrant_Com(PIamGrant_Com &&other) = delete;

    /// @brief assignment operator of PIamGrant_Com.
    /// @param other reference of object.
    /// @returns reference of this object.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = ad
    /// @endcode
    PIamGrant_Com &operator=(PIamGrant_Com const &other) = delete;

    /// @brief move assignment operator of PIamGrant_Com.
    /// @param other reference of object.
    /// @returns reference of this object.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = ad
    /// @endcode
    PIamGrant_Com &operator=(PIamGrant_Com &&other) = delete;

    /// @brief grant init store
    /// @returns bool
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_IAM_00101
    /// @needwork = ad
    /// @endcode
    static ara::core::Result< void > Initialize() noexcept;

    /// @brief Clear  All GrantInfo
    /// @returns Deinitialization bool result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = ad
    /// @endcode
    static bool Deinitialize() noexcept;

public:
    /// @brief Checks if the event given grant exists
    /// @param pid process  pid
    /// @param nServiceID event service id
    /// @param nInstanceID inst id
    /// @param nEventID event id
    /// @return true/false
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_IAM_00103
    /// @needwork = ad
    /// @endcode
    static bool HasEventGrant(uint32_t const &pid,
                              uint16_t const &nServiceID,
                              uint16_t const &nInstanceID,
                              uint16_t const &nEventID) noexcept;

    /// @brief Checks if the method given grant exists
    /// @param pid pid
    /// @param nServiceID method service id
    /// @param nInstanceID inst id
    /// @param nMethodID method id
    /// @return true/false
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_IAM_00104
    /// @needwork = ad
    /// @endcode
    static bool HasMethodGrant(uint32_t const &pid,
                               uint16_t const &nServiceID,
                               uint16_t const &nInstanceID,
                               uint16_t const &nMethodID) noexcept;
    /// @brief Checks if the findservice given grant exists
    /// @param pid fs process pid
    /// @param nServiceID fs service id
    /// @param nInstanceID inst id
    /// @return true/false
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_IAM_00102
    /// @needwork = ad
    /// @endcode
    static bool HasFindServiceGrant(uint32_t const &pid,
                                    uint16_t const &nServiceID,
                                    uint16_t const &nInstanceID) noexcept;

    /// @brief Checks if the offerservice given grant exists
    /// @param pidoffer service process pid
    /// @param nServiceID offer service id
    /// @param nInstanceID inst id
    /// @return true/false
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_IAM_00102
    /// @needwork = ad
    /// @endcode
    static bool HasOfferServiceGrant(uint32_t const &pid,
                                     uint16_t const &nServiceID,
                                     uint16_t const &nInstanceID) noexcept;

    /// @brief Checks if the remote event given grant exists
    /// @param stRemoteIp remote ip
    /// @param nPortRemote remote port
    /// @param stLocalIp local ip
    /// @param nPortLocal local port
    /// @param nServiceID service id
    /// @param nInstanceID instance id
    /// @param nEventID evnet id
    /// @return true/false
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_IAM_00103
    /// @needwork = ad
    /// @endcode
    static bool HasEventRemoteIpGrant(ara::core::String const &stRemoteIp,
                                      uint32_t const &nPortRemote,
                                      ara::core::String const &stLocalIp,
                                      int32_t const &nPortLocal,
                                      uint16_t const &nServiceID,
                                      uint16_t const &nInstanceID,
                                      uint16_t const &nEventID) noexcept;
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
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_IAM_00104
    /// @needwork = ad
    /// @endcode
    static bool HasMethodRemoteIpGrant(ara::core::String const &stRemoteIp,
                                       ara::core::String const &stProtocol,
                                       uint32_t const &nPortRemote,
                                       ara::core::String const &stLocalIp,
                                       int32_t const &nPortLocal,
                                       uint16_t const &nServiceID,
                                       uint16_t const &nInstanceID,
                                       uint16_t const &nMethodID) noexcept;
    /// @brief HasEventTlsGrant
    /// @param stTlsName tls name
    /// @param nServiceID tls service id
    /// @param nInstanceID inst id
    /// @param nEventID tls event id
    /// @return true/false
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_IAM_00103
    /// @needwork = ad
    /// @endcode
    static bool HasEventTlsGrant(ara::core::String const &stTlsName,
                                 uint16_t const &nServiceID,
                                 uint16_t const &nInstanceID,
                                 uint16_t const &nEventID) noexcept;
    /// @brief HasMethodTlsGrant
    /// @param stTlsName name
    /// @param nServiceID tls service id
    /// @param nInstanceID inst id
    /// @param nMethodID method id
    /// @return true/false
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_IAM_00104
    /// @needwork = ad
    /// @endcode
    static bool HasMethodTlsGrant(ara::core::String const &stTlsName,
                                  uint16_t const &nServiceID,
                                  uint16_t const &nInstanceID,
                                  uint16_t const &nMethodID) noexcept;
};
//********************************/
}  // namespace com
}  // namespace internal
}  // namespace iam
}  // namespace ara

#endif  // ARA_IAM_COM_IAM_GRANT_COM_H_
