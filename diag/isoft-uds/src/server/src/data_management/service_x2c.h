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
/// @file       service_x2c.h
/// @brief      This file provides service_x2c functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/isoft/uds/data_management
/// @interface_level=software
/// @trace_id_sr=80a7b0a0-b003-481c-8ae4-a5fc20c0af92
/// @unit_name=service_x2c.h
/// @unit_description=This file provides service_x2c functionality
/// @endcode
///
/// ================================================================

#ifndef ISOFT_UDS_SERVER_SERVICE_X2C_H_
#define ISOFT_UDS_SERVER_SERVICE_X2C_H_

#include <cstddef>
#include <cstdint>

#include "common/request_context.h"
#include "common/service_interface.h"
#include "diagnostic_data_manager.h"
#include "environmental_condition_manager.h"
#include "isoft/uds/data_management/diagnostic_data_management.h"
#include "security_access_management/securityLevel_validator.h"
#include "service_x22.h"
#include "session_management/session_validator.h"
namespace isoft {
namespace uds {
namespace server {


/// @brief 2C service class
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = dd
/// @endcode
class ServiceX2C : public ServiceInterface
{
public:
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00736
    /// @needwork = dda
    /// @endcode
    ServiceX2C(ServiceX2C const&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00737
    /// @needwork = dda
    /// @endcode
    ServiceX2C& operator=(ServiceX2C const&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00738
    /// @needwork = dda
    /// @endcode
    ServiceX2C(ServiceX2C&&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00739
    /// @needwork = dda
    /// @endcode
    ServiceX2C& operator=(ServiceX2C&&) = delete;

    /// @brief Constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    ServiceX2C() noexcept;

    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    ~ServiceX2C() noexcept override = default;

    /// @brief Initialization function
    /// @param model Model
    /// @param serviceX22Ptr 22 service pointer
    /// @param authManagerPtr Authentication management pointer
    /// @param sessionManagerPtr Session management pointer
    /// @param securityLevelManagerPtr Security level management pointer
    /// @param diagnosticDataManagerPtr Diagnostic data management pointer
    /// @return 0: Success
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    std::int32_t Initialize(ServiceX2CModel& model,
                            std::shared_ptr< ServiceX22 >& serviceX22Ptr,
                            std::shared_ptr< AuthenticationManager > const& authManagerPtr,
                            std::shared_ptr< SessionManager >& sessionManagerPtr,
                            std::shared_ptr< SecurityLevelManager >& securityLevelManagerPtr,
                            std::shared_ptr< DiagnosticDataManager >& diagnosticDataManagerPtr) noexcept;

    /// @brief Get session ID validator function
    /// @return Session ID validator pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    std::vector< ValidatorPtr > GetSidValidator() noexcept override;

    /// @brief Process function
    /// @param context Request context
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00740
    /// @needwork = dda
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    void Process(std::shared_ptr< RequestContext > context) noexcept override;

    /// @brief Confirm function
    /// @param request Request context
    /// @param status Parameter
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    void Confirm(RequestContext& request, ResponseStatusType status) noexcept override;

private:


    /// @brief Check minimum message length function
    /// @param const Parameter
    /// @return Operation result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    bool _checkMinimumSize(MessagePtr const& reuqestPtr) noexcept;

    /// @brief Check whether the subfunction of the current request is supported function
    /// @param const Parameter
    /// @return Operation result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    bool _checkSubfunctionSupported(MessagePtr const& reuqestPtr) noexcept;

    /// @brief
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00742
    /// @needwork = dda
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    NrcErrc _checkSubfunctionSession(std::shared_ptr< RequestContext > const& context) noexcept;

    /// @brief Check whether security level is supported function
    /// @return Operation result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    bool _supportSecurityLevel() noexcept;

    /// @brief Check whether the subfunction of the current request is allowed to run under the current security level
    /// @param const Parameter
    /// @return Operation result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    bool _checkSubfunctionSecurityLevel(std::shared_ptr< RequestContext > const& context) noexcept;

    /// @brief Parse and response function
    /// @param const Parameter
    /// @return Operation result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    void _parseAndResponse(std::shared_ptr< RequestContext > const& context) noexcept;

    /// @brief Clear and response function
    /// @param const Parameter
    /// @return Operation result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    void _clearAndResponse(std::shared_ptr< RequestContext > const& context) noexcept;

    /// @brief Define and response function
    /// @param const Parameter
    /// @return Operation result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    void _defindAndResponse(std::shared_ptr< RequestContext > const& context) noexcept;

    /// @brief Parse function
    /// @param rawData Parameter
    /// @param dynamicDataModel Parameter
    /// @return Operation result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    NrcErrc _parse(std::vector< std::uint8_t >& rawData, DiagnosticDynamicDataModel& dynamicDataModel) noexcept;

    /// @brief Actively clear defined dynamic data identifiers when Session switches
    /// @param oldSessionId Parameter
    /// @param newSessionId Parameter
    /// @return Operation result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    void _clearWhenSessionChange(SessionId oldSessionId, SessionId newSessionId) noexcept;

    /// @brief Instance structure
    /// @param id Parameter
    /// @param maxSourceElement Parameter
    /// @param authValidator Parameter
    /// @param sessionValidator Parameter
    /// @param securityLevelValidator Parameter
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    struct Instance
    {
        /// @brief Instance ID
        /// @code{.isoft}
        /// @interface_level=unit
        /// @needwork = dd
        /// @endcode
        std::uint16_t id{0U};
        /// @brief Maximum source element ID
        /// @code{.isoft}
        /// @interface_level=unit
        /// @needwork = dd
        /// @endcode
        std::uint32_t maxSourceElement{0U};
        /// @brief Authentication validator
        /// @code{.isoft}
        /// @interface_level=unit
        /// @needwork = dd
        /// @endcode
        std::shared_ptr< SubfunctionLevelAuthValidator< std::uint16_t > > authValidator{nullptr};
        /// @brief Session validator
        /// @code{.isoft}
        /// @interface_level=unit
        /// @needwork = dd
        /// @endcode
        std::shared_ptr< SubfunctionLevelSessionValidator< std::uint16_t > > sessionValidator{nullptr};
        /// @brief Security level validator
        /// @code{.isoft}
        /// @interface_level=unit
        /// @needwork = dd
        /// @endcode
        std::shared_ptr< SubfunctionLevelSecurityLevelValidator< std::uint16_t > > securityLevelValidator{nullptr};
    };

private:
    /// @brief Instance table
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    std::map< std::uint16_t, std::shared_ptr< Instance > > instanceTable_{};

    /// @brief Check whether Session and SecurityLevel of each source ID match
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    bool checkPerSourceId_{false};

    /// @brief Whether already stored
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    bool stored_{false};

    /// @brief Dynamic data identifier table
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    std::set< DiagnosticDynamicallyDefineDataIdentifierSubfunctionEnum > subfunctionTable_{};

    /// @brief Dynamic data identifier manager
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< DiagnosticDataManager > dataIdentifierManager_{nullptr};

    /// @brief Service X22 pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< ServiceX22 > serviceX22Ptr_{nullptr};

    /// @brief Authentication validator
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< ServerLevelAuthValidator< std::uint16_t > > authValidator_;

    /// @brief Session validator
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< ServerLevelSessionValidator< std::uint16_t > > sessionValidator_{nullptr};

    /// @brief Security level validator
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< ServerLevelSecurityLevelValidator< std::uint16_t > > securityLevelValidator_{nullptr};
};

}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  // ISOFT_UDS_SERVER_SERVICE_X2C_H_