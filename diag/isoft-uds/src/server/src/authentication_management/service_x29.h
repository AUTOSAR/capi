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
/// @file       service_x29.h
/// @brief      This file provides service_x29 functionality
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
/// @trace_id_sr=d6f4612e-af8e-452c-a69f-ccf71d1b967f
/// @unit_name=service_x29.h
/// @unit_description=This file provides service_x29 functionality
/// @endcode
///
/// ================================================================

#ifndef ISOFT_UDS_SERVER_SERVICE_X29_H_
#define ISOFT_UDS_SERVER_SERVICE_X29_H_

#include <memory>

#include "authentication_manager.h"
#include "common/general_service.h"
#include "security_access_management/securityLevel_validator.h"
#include "session_management/session_validator.h"

namespace isoft {
namespace uds {
namespace server {

/// @brief Authentication processing service
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02444
/// @needwork = dd
/// @endcode
class ServiceX29 : public ServiceInterface
{
public:
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00699
    /// @needwork = dda
    /// @endcode
    ServiceX29(ServiceX29 const &) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00700
    /// @needwork = dda
    /// @endcode
    ServiceX29 &operator=(ServiceX29 const &) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00701
    /// @needwork = dda
    /// @endcode
    ServiceX29(ServiceX29 &&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00702
    /// @needwork = dda
    /// @endcode
    ServiceX29 &operator=(ServiceX29 &&) = delete;

    /// @brief Constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01366
    /// @needwork = dd
    /// @endcode
    ServiceX29() noexcept = default;

    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01367
    /// @needwork = dd
    /// @endcode
    ~ServiceX29() noexcept override = default;

    /// @brief Initialization function
    /// @param[in] transmitCertificate Authentication transmission certificate
    /// @param[in] interfacePtr Authentication interface pointer
    /// @param[in] manager Authentication manager pointer
    /// @return 0: Success
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01368
    /// @needwork = dd
    /// @endcode
    std::int32_t Initialize(AuthTransmitCertificate const &transmitCertificate,
                            std::shared_ptr< AuthenticationInterface > &interfacePtr,
                            std::shared_ptr< AuthenticationManager > manager) noexcept;

    /// @brief Confirm function
    /// @param requestContext Request context
    /// @param status Parameter
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01369
    /// @needwork = dd
    /// @endcode
    void Confirm(RequestContext &requestContext, ResponseStatusType status) noexcept override;

    /// @brief Process function
    /// @param context Request context
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01370
    /// @needwork = dd
    /// @endcode
    void Process(std::shared_ptr< RequestContext > context) noexcept override;

    /// @brief Get service level validator
    /// @return Validator pointer vector
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01371
    /// @needwork = dd
    /// @endcode
    std::vector< ValidatorPtr > GetSidValidator() noexcept override { return {}; }

private:
    /// @brief Check whether subfunction is supported
    /// @param subfunction Subfunction ID
    /// @return Operation result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01372
    /// @needwork = dd
    /// @endcode
    bool _IsSupport(std::uint8_t subfunction) const noexcept;

    /// @brief Authentication unbind function
    /// @param context Request context
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01373
    /// @needwork = dd
    /// @endcode
    void _DeAuthenticate(std::shared_ptr< RequestContext > &context) noexcept;

    /// @brief Verify unidirectional authentication certificate function
    /// @param context Request context
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01374
    /// @needwork = dd
    /// @endcode
    void _VerifyCertificateUnidirectional(std::shared_ptr< RequestContext > &context) noexcept;

    /// @brief Verify bidirectional authentication certificate function
    /// @param context Request context
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01375
    /// @needwork = dd
    /// @endcode
    void _VerifyCertificateBidirectional(std::shared_ptr< RequestContext > &context) noexcept;

    /// @brief Proof of ownership
    /// @param context Request context
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01376
    /// @needwork = dd
    /// @endcode
    void _ProofOfOwnership(std::shared_ptr< RequestContext > &context) noexcept;

    /// @brief Transmit authentication certificate function
    /// @param context Request context
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01377
    /// @needwork = dd
    /// @endcode
    void _TransmitCertificate(std::shared_ptr< RequestContext > &context) noexcept;

    /// @brief Authentication configuration function
    /// @param context Request context
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01378
    /// @needwork = dd
    /// @endcode
    void _AuthenticationConfiguration(std::shared_ptr< RequestContext > &context) noexcept;

private:
    /// @brief Authentication transmission certificate
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01379
    /// @needwork = dd
    /// @endcode
    AuthTransmitCertificate transmitCertificate_{};

    /// @brief Authentication interface pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01380
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< AuthenticationInterface > interfacePtr_{};

    /// @brief Authentication manager pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01381
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< AuthenticationManager > manager_{};
};

}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  // ISOFT_UDS_SERVER_SERVICE_X10_H_