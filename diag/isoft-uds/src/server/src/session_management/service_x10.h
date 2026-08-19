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
/// @file       service_x10.h
/// @brief      This file provides service_x10 functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/isoft/uds/session_management
/// @interface_level=software
/// @trace_id_sr=e92eaaab-66c1-458b-9027-28efb734f324
/// @unit_name=service_x10.h
/// @unit_description=This file provides service_x10 functionality
/// @endcode
///
/// ================================================================

#ifndef ISOFT_UDS_SERVER_SERVICE_X10_H_
#define ISOFT_UDS_SERVER_SERVICE_X10_H_

#include <memory>

#include "authentication_management/auth_validator.h"
#include "authentication_management/authentication_manager.h"
#include "common/general_service.h"
#include "isoft/uds/session_management/session_management.h"
#include "security_access_management/securityLevel_validator.h"
#include "session_management/session_validator.h"
#include "session_manager.h"
namespace isoft {
namespace uds {
namespace server {

/// @brief DiagnosticSessionControl service is used to enable different diagnostic sessions in the server.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02416
/// @needwork = dd
/// @endcode
class ServiceX10 : public GeneralService
{
public:
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00611
    /// @needwork = dda
    /// @endcode
    ServiceX10(ServiceX10 const&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00612
    /// @needwork = dda
    /// @endcode
    ServiceX10& operator=(ServiceX10 const&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00613
    /// @needwork = dda
    /// @endcode
    ServiceX10(ServiceX10&&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00614
    /// @needwork = dda
    /// @endcode
    ServiceX10& operator=(ServiceX10&&) = delete;

    /// @brief Constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01087
    /// @needwork = dd
    /// @endcode
    ServiceX10() noexcept;

    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01088
    /// @needwork = dd
    /// @endcode
    ~ServiceX10() noexcept override = default;

    /// @brief Initialization function
    /// @param table Instance table
    /// @param authManagerPtr Authentication manager pointer
    /// @param sessionManagerPtr Session manager pointer
    /// @param securityLevelManagerPtr Security level manager pointer
    /// @param p4ServerMax P4 server max value
    /// @return 0: Success
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01089
    /// @needwork = dd
    /// @endcode
    std::int32_t Initialize(std::set< SessionControlInstanceConfig >& table,
                            std::shared_ptr< AuthenticationManager > const& authManagerPtr,
                            std::shared_ptr< SessionManager > const& sessionManagerPtr,
                            std::shared_ptr< SecurityLevelManager > const& securityLevelManagerPtr,
                            std::uint32_t const p4ServerMax) noexcept;

    /// @brief Confirm function
    /// @param requestContext Request context
    /// @param status Status
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01090
    /// @needwork = dd
    /// @endcode
    void Confirm(RequestContext& requestContext, ResponseStatusType status) noexcept override;

    /// @brief Get session validator pointer
    /// @return Session validator pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01091
    /// @needwork = dd
    /// @endcode
    std::vector< ValidatorPtr > GetSidValidator() noexcept override;

private:
    /// @brief Get minimum message length
    /// @param subfunction Parameter
    /// @return Size
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01092
    /// @needwork = dd
    /// @endcode
    std::size_t GetMinimumSize(std::uint8_t subfunction) noexcept override;

    /// @brief Subfunction support determination
    /// @param subfunction Parameter
    /// @return Operation result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01093
    /// @needwork = dd
    /// @endcode
    bool IsSupport(std::uint8_t subfunction) noexcept override;

    /// @brief Get authentication validator pointer
    /// @param subfunction Parameter
    /// @return Authentication validator pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01094
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< Validator > GetSubfunctionAuth(std::uint8_t subfunction) noexcept override;

    /// @brief Get session validator pointer
    /// @param subfunction Parameter
    /// @return Session validator pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01095
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< Validator > GetSubfunctionSession(std::uint8_t subfunction) noexcept override;

    /// @brief SupportSecurityLevel function
    /// @return Operation result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01096
    /// @needwork = dd
    /// @endcode
    bool SupportSecurityLevel() noexcept override;

    /// @brief Get security level validator pointer
    /// @param subfunction Parameter
    /// @return Security level validator pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01097
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< Validator > GetSubfunctionSecurityLevel(std::uint8_t subfunction) noexcept override;

    /// @brief ParseAndResponse function
    /// @param context Request context pointer
    /// @return Response context pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01098
    /// @needwork = dd
    /// @endcode
    void ParseAndResponse(std::shared_ptr< RequestContext > context) noexcept override;

private:
    /// @brief Instance structure
    /// @param id Instance ID
    /// @param authValidator Authentication validator pointer
    /// @param sessionValidator Session validator pointer
    /// @param securityLevelValidator Security level validator pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01099
    /// @needwork = dd
    /// @endcode
    struct Instance
    {
        /// @brief Instance ID
        /// @name id_
        /// @code{.isoft}
        /// @interface_level=unit
        /// @needwork = dd
        /// @endcode
        std::shared_ptr< Session > id{nullptr};
        /// @brief Authentication validator pointer
        /// @name authValidator_
        /// @code{.isoft}
        /// @interface_level=unit
        /// @needwork = dd
        /// @endcode
        std::shared_ptr< SubfunctionLevelAuthValidator< std::uint8_t > > authValidator{nullptr};
        /// @brief Session validator pointer
        /// @name sessionValidator_
        /// @code{.isoft}
        /// @interface_level=unit
        /// @needwork = dd
        /// @endcode
        std::shared_ptr< SubfunctionLevelSessionValidator<> > sessionValidator{nullptr};
        /// @brief Security level validator pointer
        /// @name securityLevelValidator_
        /// @code{.isoft}
        /// @interface_level=unit
        /// @needwork = dd
        /// @endcode
        std::shared_ptr< SubfunctionLevelSecurityLevelValidator<> > securityLevelValidator{nullptr};
    };
    /// @brief Authentication validator pointer
    /// @name authValidator_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01100
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< ServerLevelAuthValidator< std::uint8_t > > authValidator_{nullptr};
    /// @brief Session validator pointer
    /// @name sessionValidator_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01101
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< ServerLevelSessionValidator<> > serverLevelSessionValidator_{nullptr};
    /// @brief Security level validator pointer
    /// @name securityLevelValidator_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01102
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< ServerLevelSecurityLevelValidator<> > serverLevelSecurityLevelValidator_{nullptr};

    /// @brief New session ID
    /// @name newSessionId_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01103
    /// @needwork = dd
    /// @endcode
    SessionId newSessionId_{0U};
    /// @brief Instance table
    /// @name instanceTable_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00615
    /// @needwork = dda
    /// @endcode
    std::map< SessionId, std::shared_ptr< Instance > > instanceTable_{};
    /// @brief Session manager pointer
    /// @name diagnosticSessionManagerPtr_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01104
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< SessionManager > diagnosticSessionManagerPtr_{nullptr};
};

}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  // ISOFT_UDS_SERVER_SERVICE_X10_H_