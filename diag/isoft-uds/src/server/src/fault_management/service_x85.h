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
/// @file       service_x85.h
/// @brief      This file provides service_x85 functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/isoft/uds/fault_management
/// @interface_level=software
/// @trace_id_sr=4eb17183-0cdb-4110-a074-57423601826f
/// @unit_name=service_x85.h
/// @unit_description=This file provides service_x85 functionality
/// @endcode
///
/// ================================================================

#ifndef __ISOFT_UDS_FAULT_SERVICE_X85_H__
#define __ISOFT_UDS_FAULT_SERVICE_X85_H__

#include <atomic>
#include <memory>

#include "authentication_management/auth_validator.h"
#include "authentication_management/authentication_manager.h"
#include "common/general_service.h"
#include "common/request_context.h"
#include "common/service_interface.h"
#include "isoft/uds/fault_management/fault_management.h"
#include "security_access_management/securityLevel_validator.h"
#include "security_access_management/security_level_manager.h"
#include "session_management/session_manager.h"
#include "session_management/session_validator.h"
namespace isoft {
namespace uds {
namespace server {

/// @brief FaultManager class forward declaration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02454
/// @needwork = dd
/// @endcode
class FaultManager;

/// @brief
/// The client should use the ControlDTCSetting service to stop or resume the update of DTC status bits in the server.
/// DTC status bits are reported in the statusOfDTC parameter of certain subfunctions of ReadDTCInformation positive response (see D.2 for bit definitions).
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02455
/// @needwork = dd
/// @endcode
class ServiceX85 : public GeneralService
{
public:
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00781
    /// @needwork = dda
    /// @endcode
    ServiceX85(ServiceX85 const&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00782
    /// @needwork = dda
    /// @endcode
    ServiceX85& operator=(ServiceX85 const&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00783
    /// @needwork = dda
    /// @endcode
    ServiceX85(ServiceX85&&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00784
    /// @needwork = dda
    /// @endcode
    ServiceX85& operator=(ServiceX85&&) = delete;

    /// @brief Constructor
    /// @param[in] faultManager Fault manager
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01506
    /// @needwork = dd
    /// @endcode
    explicit ServiceX85(FaultManager& faultManager) noexcept : faultManager_{faultManager} {};

    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01507
    /// @needwork = dd
    /// @endcode
    ~ServiceX85() noexcept override = default;

    /// @brief Initialization, load configuration file
    /// @param[in] config Configuration file
    /// @param[in] authManagerPtr Authentication manager pointer
    /// @param[in] sessionManagerPtr Session manager pointer
    /// @param[in] securityLevelManagerPtr Security level manager pointer
    /// @return 0: Success
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01508
    /// @needwork = dd
    /// @endcode
    std::int32_t Initialize(ControlDTCSettingInstanceConfig& config,
                            std::shared_ptr< AuthenticationManager > const& authManagerPtr,
                            std::shared_ptr< SessionManager > const& sessionManagerPtr,
                            std::shared_ptr< SecurityLevelManager > const& securityLevelManagerPtr) noexcept;

    /// @brief Get session validator
    /// @return Session validator pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01509
    /// @needwork = dd
    /// @endcode
    std::vector< ValidatorPtr > GetSidValidator() noexcept override;

private:
    /// @brief Get size
    /// @param subfunction Parameter
    /// @return Size
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01510
    /// @needwork = dd
    /// @endcode
    std::size_t GetMinimumSize(std::uint8_t subfunction) noexcept override;

    /// @brief IsSupport function
    /// @param subfunction Parameter
    /// @return Operation result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01511
    /// @needwork = dd
    /// @endcode
    bool IsSupport(std::uint8_t subfunction) noexcept override;

    /// @brief Get subfunction authentication validator
    /// @param subfunction Parameter
    /// @return Authentication validator pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01512
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< Validator > GetSubfunctionAuth(std::uint8_t subfunction) noexcept override;

    /// @brief Get subfunction session validator
    /// @param subfunction Parameter
    /// @return Session validator pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01513
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< Validator > GetSubfunctionSession(std::uint8_t subfunction) noexcept override;

    /// @brief Support security level validator
    /// @return Operation result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01514
    /// @needwork = dd
    /// @endcode
    bool SupportSecurityLevel() noexcept override;

    /// @brief Get subfunction security level validator
    /// @param subfunction Parameter
    /// @return Security level validator pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01515
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< Validator > GetSubfunctionSecurityLevel(std::uint8_t subfunction) noexcept override;

    /// @brief ParseAndResponse function
    /// @param[in] context Request context
    /// @return 0: Success
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01516
    /// @needwork = dd
    /// @endcode
    void ParseAndResponse(std::shared_ptr< RequestContext > context) noexcept override;

    /// @brief Subfunction instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01517
    /// @needwork = dd
    /// @endcode
    struct Instance
    {
        /// @brief Subfunction ID
        /// @code{.isoft}
        /// @interface_level=unit
        /// @needwork = dd
        /// @endcode
        std::uint8_t subfunctionNumber{0U};
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

    /// @brief Subfunction instance table
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01518
    /// @needwork = dd
    /// @endcode
    std::map< std::uint8_t, std::shared_ptr< Instance > > instanceTable_{};

    /// @brief Authentication validator
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01519
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< ServerLevelAuthValidator< std::uint8_t > > authValidator_;
    /// @brief Session validator
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01520
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< ServerLevelSessionValidator< std::uint8_t > > sessionValidator_{nullptr};
    /// @brief Security level validator
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01521
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< ServerLevelSecurityLevelValidator< std::uint8_t > > securityLevelValidator_{nullptr};

    /// @brief Fault manager reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01522
    /// @needwork = dd
    /// @endcode
    FaultManager& faultManager_;
};

}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  // ISOFT_UDS_SERVER_SERVICE_X85_H_