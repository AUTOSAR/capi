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
/// @file       service_x11.h
/// @brief      This file provides service_x11 functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/isoft/uds/ecu_reset_management
/// @interface_level=software
/// @trace_id_sr=a58af830-c7ef-4bcf-968e-c96363d8cb28
/// @unit_name=service_x11.h
/// @unit_description=This file provides service_x11 functionality
/// @endcode
///
/// ================================================================

#ifndef ISOFT_UDS_SERVER_SERVICE_X11_H_
#define ISOFT_UDS_SERVER_SERVICE_X11_H_

#include <cstdint>
#include <memory>

#include "authentication_management/auth_validator.h"
#include "authentication_management/authentication_manager.h"
#include "common/general_service.h"
#include "common/persistence_service.h"
#include "common/request_context.h"
#include "isoft/uds/ecu_reset_management/ecu_reset_management.h"
#include "security_access_management/securityLevel_validator.h"
#include "security_access_management/security_level_manager.h"
#include "session_management/session_manager.h"
#include "session_management/session_validator.h"
namespace isoft {
namespace uds {
namespace server {

/// @brief This service requests the server to effectively perform a server reset based on the content of the resetType parameter value embedded in the ECUReset request message.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02427
/// @needwork = dd
/// @endcode
class ServiceX11 : public GeneralService
{
public:
    /// @brief Notify session re-establishment
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01185
    /// @needwork = dd
    /// @endcode
    using NotifyOfReestablishment = std::function< void(std::shared_ptr< Channel >) >;

    ServiceX11(ServiceX11 const&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00650
    /// @needwork = dda
    /// @endcode
    ServiceX11& operator=(ServiceX11 const&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00651
    /// @needwork = dda
    /// @endcode
    ServiceX11(ServiceX11&&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00652
    /// @needwork = dda
    /// @endcode
    ServiceX11& operator=(ServiceX11&&) = delete;

    /// @brief Block state
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01186
    /// @needwork = dd
    /// @endcode
    enum class BlockState
    {
        kUnblocked = 0,  /// Unblocked
        kBlocked   = 1   /// Blocked
    };



    /// @brief Constructor
    /// @param[in] persistenceService Persistence service
    /// @param[in] notify Block notification, after blocking, reject all requests.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01187
    /// @needwork = dd
    /// @endcode
    ServiceX11(std::shared_ptr< PersistenceService >& persistenceService,
               std::function< void(BlockState) > notify) noexcept;

    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01188
    /// @needwork = dd
    /// @endcode
    ~ServiceX11() noexcept override = default;



    /// @brief Initialization
    /// @param[in] config Configuration file
    /// @param[in] authManagerPtr Authentication manager pointer
    /// @param[in] sessionManagerPtr Session manager pointer
    /// @param[in] securityLevelManagerPtr Security level manager pointer
    /// @return 0: Success
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01189
    /// @needwork = dd
    /// @endcode
    std::int32_t Initialize(EcuResetManagement& config,
                            std::shared_ptr< AuthenticationManager > const& authManagerPtr,
                            std::shared_ptr< SessionManager > const& sessionManagerPtr,
                            std::shared_ptr< SecurityLevelManager > const& securityLevelManagerPtr) noexcept;

    /// @brief Get SID validator
    /// @return Validator pointer vector
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01190
    /// @needwork = dd
    /// @endcode
    std::vector< ValidatorPtr > GetSidValidator() noexcept override;

    /// @brief Notify session re-establishment
    /// @param[in] channel Session pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01191
    /// @needwork = dd
    /// @endcode
    void NotifyReestablishment(std::shared_ptr< Channel > const& channel) noexcept;

    /// @brief Confirm function
    /// @param[in] request Request context
    /// @param[in] status Status
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01192
    /// @needwork = dd
    /// @endcode
    void Confirm(RequestContext& request, ResponseStatusType status) noexcept override;

private:
    /// @brief Get minimum size
    /// @param subfunction Parameter
    /// @return Size
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01193
    /// @needwork = dd
    /// @endcode
    std::size_t GetMinimumSize(std::uint8_t subfunction) noexcept override;

    /// @brief Whether subfunction is supported
    /// @param subfunction Parameter
    /// @return Operation result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01194
    /// @needwork = dd
    /// @endcode
    bool IsSupport(std::uint8_t subfunction) noexcept override;

    /// @brief Get subfunction authentication validator
    /// @param subfunction Parameter
    /// @return Validator pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01195
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< Validator > GetSubfunctionAuth(std::uint8_t subfunction) noexcept override;

    /// @brief Get subfunction session validator
    /// @param subfunction Parameter
    /// @return Validator pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01196
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< Validator > GetSubfunctionSession(std::uint8_t subfunction) noexcept override;

    /// @brief SupportSecurityLevel function
    /// @return Operation result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01197
    /// @needwork = dd
    /// @endcode
    bool SupportSecurityLevel() noexcept override;

    /// @brief Get subfunction security level validator
    /// @param subfunction Parameter
    /// @return Validator pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01198
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< Validator > GetSubfunctionSecurityLevel(std::uint8_t subfunction) noexcept override;

    /// @brief ParseAndResponse function
    /// @param[in] context Request context pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01199
    /// @needwork = dd
    /// @endcode
    void ParseAndResponse(std::shared_ptr< RequestContext > context) noexcept override;

    /// @brief Save context
    /// @param[in] msgPtr Message pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01200
    /// @needwork = dd
    /// @endcode
    void _save(std::shared_ptr< Message > const& msgPtr) noexcept;

    /// @brief Notify block
    /// @param[in] blockState Block state
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01201
    /// @needwork = dd
    /// @endcode
    inline void _blockNotify() noexcept { blockNotify_(BlockState::kBlocked); }

    /// @brief Notify unblock
    /// @param[in] blockState Block state
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01202
    /// @needwork = dd
    /// @endcode
    inline void _unblockNotify() noexcept { blockNotify_(BlockState::kUnblocked); }

    /// @brief Instance class
    /// @param[in] subfunctionNumber Subfunction number
    /// @param[in] authValidator Authentication validator pointer
    /// @param[in] sessionValidator Session validator pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01203
    /// @needwork = dd
    /// @endcode
    struct Instance
    {
        /// @brief Subfunction number
        /// @code{.isoft}
        /// @interface_level=unit
        /// @needwork = dd
        /// @endcode
        std::uint8_t subfunctionNumber{0U};
        /// @brief Authentication validator pointer
        /// @code{.isoft}
        /// @interface_level=unit
        /// @needwork = dd
        /// @endcode
        std::shared_ptr< SubfunctionLevelAuthValidator< std::uint8_t > > authValidator{nullptr};
        /// @brief Session validator pointer
        /// @code{.isoft}
        /// @interface_level=unit
        /// @needwork = dd
        /// @endcode
        std::shared_ptr< SubfunctionLevelSessionValidator< std::uint8_t > > sessionValidator{nullptr};
        /// @brief Security level validator pointer
        /// @code{.isoft}
        /// @interface_level=unit
        /// @needwork = dd
        /// @endcode
        std::shared_ptr< SubfunctionLevelSecurityLevelValidator< std::uint8_t > > securityLevelValidator{nullptr};
    };

private:
    /// @brief Instance table
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01204
    /// @needwork = dd
    /// @endcode
    std::map< std::uint8_t, std::shared_ptr< Instance > > instanceTable_{};

    /// @brief Authentication validator pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01205
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< ServerLevelAuthValidator< std::uint8_t > > authValidator_;
    /// @brief Session validator pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01206
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< ServerLevelSessionValidator< std::uint8_t > > sessionValidator_{nullptr};

    /// @brief Security level validator pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01207
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< ServerLevelSecurityLevelValidator< std::uint8_t > > securityLevelValidator_{nullptr};

    /// @brief EcuResetRequestInterface pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01208
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< EcuResetRequestInterface > interface_{};

    /// @brief Response ECU reset enumeration
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01209
    /// @needwork = dd
    /// @endcode
    DiagnosticResponseToEcuResetEnum respondToReset_{DiagnosticResponseToEcuResetEnum::kRespondAfterReset};

    /// @brief PersistenceService pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01210
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< PersistenceService > persistenceService_{nullptr};



    /// @brief Notify block
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01211
    /// @needwork = dd
    /// @endcode
    std::function< void(BlockState const) > blockNotify_{};


};

}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  // ISOFT_UDS_SERVER_SERVICE_X11_H_