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
/// @file       service_x2e.h
/// @brief      This file provides service_x2e functionality
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
/// @trace_id_sr=ccc6be0c-a528-4c55-927d-c71b320fac2f
/// @unit_name=service_x2e.h
/// @unit_description=This file provides service_x2e functionality
/// @endcode
///
/// ================================================================

#ifndef ISOFT_UDS_SERVER_SERVICE_X2E_H_
#define ISOFT_UDS_SERVER_SERVICE_X2E_H_

#include <cstddef>
#include <cstdint>

#include "authentication_management/auth_validator.h"
#include "authentication_management/authentication_manager.h"
#include "common/request_context.h"
#include "common/service_interface.h"
#include "diagnostic_data_manager.h"
#include "environmental_condition_manager.h"
#include "isoft/uds/data_management/diagnostic_data_management.h"
#include "security_access_management/securityLevel_validator.h"
#include "session_management/session_validator.h"
namespace isoft {
namespace uds {
namespace server {

/// @brief Service class
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = dd
/// @endcode
class ServiceX2E : public ServiceInterface
{
public:
    /// @param[in] other Value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00723
    /// @needwork = dda
    /// @endcode
    ServiceX2E(ServiceX2E const& other) = delete;

    /// @param[in] other Value
    /// @return ServiceX2E&
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00724
    /// @needwork = dda
    /// @endcode
    ServiceX2E& operator=(ServiceX2E const& other) = delete;

    /// @param[in] other Value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00725
    /// @needwork = dda
    /// @endcode
    ServiceX2E(ServiceX2E&& other) = delete;

    /// @param[in] other Value
    /// @return ServiceX2E&
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00726
    /// @needwork = dda
    /// @endcode
    ServiceX2E& operator=(ServiceX2E&& other) = delete;

    /// @brief Constructor
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    ServiceX2E() noexcept;

    /// @brief Destructor
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    ~ServiceX2E() override = default;

    /// @brief Initialization function
    /// @param[in] model Model
    /// @param[in] authManagerPtr Authentication management pointer
    /// @param[in] sessionManagerPtr Session management pointer
    /// @param[in] securityLevelManagerPtr Security level management pointer
    /// @param[in] envirConditionManagerPtr Environmental condition management pointer
    /// @param[in] diagnosticDataManagerPtr Diagnostic data management pointer
    /// @return 0: Success
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    std::int32_t Initialize(ServiceX2EModel& model,
                            std::shared_ptr< AuthenticationManager > const& authManagerPtr,
                            std::shared_ptr< SessionManager >& sessionManagerPtr,
                            std::shared_ptr< SecurityLevelManager >& securityLevelManagerPtr,
                            std::shared_ptr< EnvironmentalConditionManager >& envirConditionManagerPtr,
                            std::shared_ptr< DiagnosticDataManager >& diagnosticDataManagerPtr) noexcept;

    /// @brief Get session ID validator
    /// @return Validator vector
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    std::vector< ValidatorPtr > GetSidValidator() noexcept override;

    /// @brief Request processing function
    /// @param context Parameter
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    void Process(std::shared_ptr< RequestContext > context) noexcept override;

    /// @brief Confirm function
    /// @param request Parameter
    /// @param status Parameter
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00727
    /// @needwork = dda
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    void Confirm(RequestContext& request, ResponseStatusType status) noexcept override;

private:
    /// @brief Instance structure
    /// @param[in] id Instance ID
    /// @param[in] dataSize Data size
    /// @param[in] authValidator Authentication validator pointer
    /// @param[in] sessionValidator Session validator pointer
    /// @param[in] securityLevelValidator Security level validator pointer
    /// @param[in] envCondition Environmental condition pointer
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
        /// @brief Data size
        /// @code{.isoft}
        /// @interface_level=unit
        /// @needwork = dd
        /// @endcode
        std::uint32_t dataSize{0U};
        /// @brief Authentication validator pointer
        /// @code{.isoft}
        /// @interface_level=unit
        /// @needwork = dd
        /// @endcode
        std::shared_ptr< SubfunctionLevelAuthValidator< std::uint16_t > > authValidator{nullptr};
        /// @brief Session validator pointer
        /// @code{.isoft}
        /// @interface_level=unit
        /// @needwork = dd
        /// @endcode
        std::shared_ptr< SubfunctionLevelSessionValidator< std::uint16_t > > sessionValidator{nullptr};
        /// @brief Security level validator pointer
        /// @code{.isoft}
        /// @interface_level=unit
        /// @needwork = dd
        /// @endcode
        std::shared_ptr< SubfunctionLevelSecurityLevelValidator< std::uint16_t > > securityLevelValidator{nullptr};
        /// @brief Environmental condition pointer
        /// @code{.isoft}
        /// @interface_level=unit
        /// @needwork = dd
        /// @endcode
        std::shared_ptr< EnvironmentalCondition > envCondition{nullptr};
    };

    /// @brief Instance table
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    std::map< std::uint16_t, std::shared_ptr< Instance > > instanceTable_;

    /// @brief Diagnostic data management pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< DiagnosticDataManager > dataIdentifierManager_;

    /// @brief Authentication validator pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00728
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< ServerLevelAuthValidator< std::uint16_t > > authValidator_;

    /// @brief Session validator pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00729
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< ServerLevelSessionValidator< std::uint16_t > > sessionValidator_;

    /// @brief Security level validator pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00730
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< ServerLevelSecurityLevelValidator< std::uint16_t > > securityLevelValidator_;
};

}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  // ISOFT_UDS_SERVER_SERVICE_X2E_H_