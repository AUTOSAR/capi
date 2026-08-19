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
/// @file       service_x14.h
/// @brief      This file provides service_x14 functionality
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
/// @trace_id_sr=d5025337-8827-4698-9619-8ff7730742d2
/// @unit_name=service_x14.h
/// @unit_description=This file provides service_x14 functionality
/// @endcode
///
/// ================================================================

#ifndef __ISOFT_UDS_FAULT_SERVICE_X14_H__
#define __ISOFT_UDS_FAULT_SERVICE_X14_H__

#include <atomic>
#include <memory>

#include "authentication_management/auth_validator.h"
#include "authentication_management/authentication_manager.h"
#include "common/request_context.h"
#include "common/service_interface.h"
#include "fault_management/fault_manager.h"
#include "isoft/uds/fault_management/fault_management.h"
#include "security_access_management/securityLevel_validator.h"
#include "security_access_management/security_level_manager.h"
#include "session_management/session_manager.h"
#include "session_management/session_validator.h"
namespace isoft {
namespace uds {
namespace server {

/// @brief Server class forward declaration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02553
/// @needwork = dd
/// @endcode
class Server;

/// @brief The client uses the ClearDiagnosticlnformation service to clear diagnostic information in one or more server memories.
/// When the ClearDiagnosticInformation service is fully processed, the server should send a positive response.
/// Even if no dtc is stored, the server should send a positive response.
/// If the server supports multiple copies of DTC status information in memory (e.g., one copy in RAM, one copy in EEPROM),
/// the server should clear the copy used by the ReadDTCInformation status reporting service.
/// Additional copies, such as backup copies in long-term memory, are updated according to appropriate backup strategies (e.g., during power latch phase).
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02554
/// @needwork = dd
/// @endcode
class ServiceX14 : public ServiceInterface
{
public:
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00811
    /// @needwork = dda
    /// @endcode
    ServiceX14(ServiceX14 const&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00812
    /// @needwork = dda
    /// @endcode
    ServiceX14& operator=(ServiceX14 const&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00813
    /// @needwork = dda
    /// @endcode
    ServiceX14(ServiceX14&&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00814
    /// @needwork = dda
    /// @endcode
    ServiceX14& operator=(ServiceX14&&) = delete;

    /// @brief Constructor
    /// @param[in] conversationManager Session management
    /// @param[in] eventManager DID management
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00815
    /// @needwork = dda
    /// @endcode
    explicit ServiceX14(FaultManager& faultManager) noexcept : faultManager_{faultManager} {}

    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01897
    /// @needwork = dd
    /// @endcode
    ~ServiceX14() noexcept override = default;

    /// @brief Initialization, load configuration file
    /// @param[in] dmc Configuration file
    /// @param[in] node File file node
    /// @param[in] apt Access permission table
    /// @return 0: Success, other values: Failure
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01898
    /// @needwork = dd
    /// @endcode
    std::int32_t Initialize(ClearDiagnosticInformationInstanceConfig& config,
                            std::shared_ptr< AuthenticationManager > const& authManagerPtr,
                            std::shared_ptr< SessionManager > const& sessionManagerPtr,
                            std::shared_ptr< SecurityLevelManager > const& securityLevelManagerPtr) noexcept;

    /// @brief Get session ID validator
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01998
    /// @needwork = dd
    /// @endcode
    std::vector< ValidatorPtr > GetSidValidator() noexcept override;

    /// @brief Process request message
    /// @param[in] context Request context
    /// @param[in] sessionId Session ID
    /// @param[in] securityLevelId Security level ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01899
    /// @needwork = dd
    /// @endcode
    void Process(std::shared_ptr< RequestContext > context) noexcept override;

    /// @brief Confirm function
    /// @param status Parameter
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01900
    /// @needwork = dd
    /// @endcode
    void Confirm(RequestContext& request, ResponseStatusType status) noexcept override;

private:
    /// @brief Authentication validator
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01999
    /// @needwork = dd
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_02000
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< ServerLevelAuthValidator< std::uint8_t > > authValidator_;

    /// @brief Session validator
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_02001
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< ServerLevelSessionValidator< std::uint8_t > > sessionValidator_;

    /// @brief Security level validator
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01901
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< ServerLevelSecurityLevelValidator< std::uint8_t > > securityLevelValidator_;

    /// @brief Fault manager
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01902
    /// @needwork = dd
    /// @endcode
    FaultManager& faultManager_;

    /// @brief Whether processing request
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01903
    /// @needwork = dd
    /// @endcode
    std::atomic_bool inProgress_{false};
};

}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  // ISOFT_UDS_SERVER_SERVICE_X14_H_