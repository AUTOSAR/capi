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
/// @file       authentication_management.h
/// @brief      This file provides authentication_management functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/isoft/uds/authentication_management
/// @interface_level=software
/// @trace_id_sr=SRC_UDS_0001
/// @unit_name=AuenticationManagement
/// @unit_description=This file provides authentication_management functionality
/// @endcode
///
/// ================================================================

#ifndef ISOFT_UDS_SERVER_AUTHENTICATION_MANAGEMENT_H_
#define ISOFT_UDS_SERVER_AUTHENTICATION_MANAGEMENT_H_
#include "authentication.h"
#include "authentication_state.h"
namespace isoft {
namespace uds {
namespace server {
/// @brief Value of cancel authentication subfunction
/// @code{.isoft}
///
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_
/// @trace_id_dd=DD_UDS_00977
/// @needwork = dd
/// @endcode
constexpr std::uint8_t kDeAuthenticate = 0x00;
/// @brief Value of verify certificate unidirectional authentication subfunction
/// @code{.isoft}
///
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_
/// @trace_id_dd=DD_UDS_00978
/// @needwork = dd
/// @endcode
constexpr std::uint8_t kVerifyCertificateUnidirectional = 0x01;
/// @brief Value of verify certificate bidirectional authentication subfunction
/// @code{.isoft}
///
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_
/// @trace_id_dd=DD_UDS_00979
/// @needwork = dd
/// @endcode
constexpr std::uint8_t kVerifyCertificateBidirectional = 0x02;
/// @brief Value of proof of ownership subfunction
/// @code{.isoft}
///
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_
/// @trace_id_dd=DD_UDS_00980
/// @needwork = dd
/// @endcode
constexpr std::uint8_t kProofOfOwnership = 0x03;
/// @brief Value of transmit certificate subfunction
/// @code{.isoft}
///
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_
/// @trace_id_dd=DD_UDS_00981
/// @needwork = dd
/// @endcode
constexpr std::uint8_t kTransmitCertificate = 0x04;
/// @brief Value of authentication configuration subfunction
/// @code{.isoft}
///
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_
/// @trace_id_dd=DD_UDS_00982
/// @needwork = dd
/// @endcode
constexpr std::uint8_t kAuthenticationConfiguration = 0x08;
/// @brief Structure of authentication transmission certificate
///
/// @code{.isoft}
///
/// @interface_level=unit
/// @needwork = ad
///
/// @endcode
struct AuthTransmitCertificateEvaluation
{
    ///
    /// @brief ID of authentication transmission certificate
    std::uint16_t certificateEvaluationId{0U};
    ///
    /// @brief name of authentication transmission certificate
    std::string function{};
};
/// @brief Structure of authentication transmission certificate
///
/// @code{.isoft}
///
/// @interface_level=unit
/// @needwork = ad
///
/// @endcode
struct AuthTransmitCertificate
{
    ///
    /// @brief List of certificate Evaluation
    std::vector< AuthTransmitCertificateEvaluation > certificateEvaluation;
};
/// @brief Structure of external authentication identification
///
/// @code{.isoft}
///
/// @interface_level=unit
/// @needwork = ad
///
/// @endcode
struct ExternalAuthenticationIdentification
{
    ///
    /// @brief Source address code
    std::uint16_t sourceAddressCode;
    ///
    /// @brief Source address mask
    std::uint16_t sourceAddressMask;
};
/// @brief Class of authentication management
///
/// @code{.isoft}
///
/// @interface_level=unit
/// @needwork = ad
///
/// @endcode
class AuthenticationManagement
{
public:
    /// @brief List of external authentication identifications
    /// @code{.isoft}
    ///
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00983
    /// @needwork = dda
    /// @endcode
    std::vector< ExternalAuthenticationIdentification > externalAuthenticationList{};
    /// @brief List of authentication roles
    /// @code{.isoft}
    ///
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00984
    /// @needwork = dda
    /// @endcode
    std::vector< AuthRole > authRoleList{};
    /// @brief Authentication timeout
    /// @code{.isoft}
    ///
    /// @note Unit: milliseconds
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00985
    /// @needwork = dda
    /// @endcode
    std::uint32_t authenticationTimeout{};
    /// @brief Authentication transmission certificate
    /// @code{.isoft}
    ///
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00986
    /// @needwork = dda
    /// @endcode
    AuthTransmitCertificate transmitCertificate{};
    /// @brief Authentication interface
    /// @code{.isoft}
    ///
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00987
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< AuthenticationInterface > interface{nullptr};
};

}  // namespace server
}  // namespace uds
}  // namespace isoft
#endif  //ISOFT_UDS_SERVER_AUTHENTICATION_MANAGEMENT_H_