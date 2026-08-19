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
/// @file       service_x86.h
/// @brief      This file provides service_x86 functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/isoft/uds/roe_management
/// @interface_level=software
/// @trace_id_sr=6c5fdea2-dff3-4c67-b7f0-9663d71a2841
/// @unit_name=service_x86.h
/// @unit_description=This file provides service_x86 functionality
/// @endcode
///
/// ================================================================

#ifndef ISOFT_UDS_SERVER_SERVICE_X86_H_
#define ISOFT_UDS_SERVER_SERVICE_X86_H_

#include <cstddef>
#include <cstdint>

#include "authentication_management/auth_validator.h"
#include "authentication_management/authentication_manager.h"
#include "common/general_service.h"
#include "common/request_context.h"
#include "diagnostic_roe_manager.h"
#include "isoft/uds/roe_management/roe_management.h"
#include "security_access_management/securityLevel_validator.h"
#include "security_access_management/security_level_manager.h"
#include "session_management/session_manager.h"
#include "session_management/session_validator.h"
namespace isoft {
namespace uds {
namespace server {

class Server;

/// @brief ResponseOnEvent service requests the server to start or stop the transmission of responses to specified events.
/// This service provides the possibility to automatically execute diagnostic services when specified events occur in the server.
/// The client specifies the event (including optional event parameters) and service (including service parameters) to be executed when the event occurs.
/// @code{.isoft}

/// @interface_level=unit
/// @needwork=dd
/// @endcode
class ServiceX86 : public GeneralService
{
public:
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00656
    /// @needwork = dda
    /// @endcode
    ServiceX86(ServiceX86 const& other) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00657
    /// @needwork = dda
    /// @endcode
    ServiceX86& operator=(ServiceX86 const& other) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00658
    /// @needwork = dda
    /// @endcode
    ServiceX86(ServiceX86&&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00659
    /// @needwork = dda
    /// @endcode
    ServiceX86& operator=(ServiceX86&&) = delete;

    /// @brief Constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01212
    /// @needwork = dd
    /// @endcode
    ServiceX86() noexcept;

    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01213
    /// @needwork = dd
    /// @endcode
    ~ServiceX86() noexcept override = default;

    /// @brief Initialization function
    /// @param[in] dmc Configuration file
    /// @param[in] node File file node
    /// @param[in] apt Access permission table
    /// @return 0: Success
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01214
    /// @needwork = dd
    /// @endcode
    std::int32_t Initialize(RoeManagement& config,
                            std::shared_ptr< AuthenticationManager > const& authManagerPtr,
                            std::shared_ptr< SessionManager >& sessionManagerPtr,
                            std::shared_ptr< SecurityLevelManager >& securityLevelManagerPtr,
                            std::shared_ptr< DiagnosticRoeManager >& diagnosticRoeManagerPtr) noexcept;

    /// @brief Get sid validator
    /// @return Validator
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01989
    /// @needwork = dd
    /// @endcode
    std::vector< ValidatorPtr > GetSidValidator() noexcept override;

private:
    /// @brief Instance structure
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01215
    /// @needwork = dd
    /// @endcode
    struct Instance
    {
        /// @brief Response event event operation
        /// @code{.isoft}
        /// @interface_level=unit
        /// @needwork = dd
        /// @endcode
        ResponseOnEventActionEnum responseOnEventAction{ResponseOnEventActionEnum::kStopResponseOnEvent};
        /// @brief Authentication validator
        /// @code{.isoft}
        /// @interface_level=unit
        /// @needwork = dd
        /// @endcode
        std::shared_ptr< SubfunctionLevelAuthValidator< std::uint8_t > > authValidator{nullptr};
        /// @brief Session validator
        /// @code{.isoft}
        /// @interface_level=unit
        /// @needwork = dd
        /// @endcode
        std::shared_ptr< SubfunctionLevelSessionValidator< std::uint8_t > > sessionValidator{nullptr};
        /// @brief Security level validator
        /// @code{.isoft}
        /// @interface_level=unit
        /// @needwork = dd
        /// @endcode
        std::shared_ptr< SubfunctionLevelSecurityLevelValidator< std::uint8_t > > securityLevelValidator{nullptr};
    };

    /// @brief Get size
    /// @param subfunction Parameter
    /// @return Size
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01216
    /// @needwork = dd
    /// @endcode
    std::size_t GetMinimumSize(std::uint8_t subfunction) noexcept override;

    /// @brief IsSupport function
    /// @param subfunction Parameter
    /// @return Operation result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01217
    /// @needwork = dd
    /// @endcode
    bool IsSupport(std::uint8_t subfunction) noexcept override;

    /// @brief Get subfunction authentication validator
    /// @param subfunction Parameter
    /// @return Validator
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01218
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< Validator > GetSubfunctionAuth(std::uint8_t subfunction) noexcept override;

    /// @brief Get subfunction session validator
    /// @param subfunction Parameter
    /// @return Validator
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01219
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< Validator > GetSubfunctionSession(std::uint8_t subfunction) noexcept override;

    /// @brief SupportSecurityLevel function
    /// @return Operation result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01220
    /// @needwork = dd
    /// @endcode
    bool SupportSecurityLevel() noexcept override;

    /// @brief Get subfunction security level validator
    /// @param subfunction Parameter
    /// @return Validator
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01221
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< Validator > GetSubfunctionSecurityLevel(std::uint8_t subfunction) noexcept override;

    /// @brief ParseAndResponse function
    /// @param context Parameter
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01222
    /// @needwork = dd
    /// @endcode
    void ParseAndResponse(std::shared_ptr< RequestContext > context) noexcept override;

private:
    /// @brief Instance table
    /// @name instanceTable_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01223
    /// @needwork = dd
    /// @endcode
    ara::core::Map< std::uint8_t, std::shared_ptr< Instance > > instanceTable_{};
    /// @brief Authentication validator
    /// @name authValidator_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01224
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< ServerLevelAuthValidator< std::uint8_t > > authValidator_{nullptr};
    /// @brief Session manager pointer
    /// @name sessionManagerPtr_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01225
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< SessionManager > sessionManagerPtr_{nullptr};
    /// @brief Session validator
    /// @name sessionValidator_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01226
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< ServerLevelSessionValidator< std::uint8_t > > sessionValidator_{nullptr};
    /// @brief Security level validator
    /// @name securityLevelValidator_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01227
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< ServerLevelSecurityLevelValidator< std::uint8_t > > securityLevelValidator_{nullptr};

    /// @brief Diagnostic ROE manager pointer
    /// @name diagnosticRoeManagerPtr_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01228
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< DiagnosticRoeManager > diagnosticRoeManagerPtr_{nullptr};
};

}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  // ISOFT_UDS_SERVER_SERVICE_X86_H_