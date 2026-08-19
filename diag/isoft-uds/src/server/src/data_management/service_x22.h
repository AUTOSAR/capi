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
/// @file       service_x22.h
/// @brief      This file provides service_x22 functionality
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
/// @trace_id_sr=f4cb17cc-0c50-4150-805c-6a5d901cdc3a
/// @unit_name=service_x22.h
/// @unit_description=This file provides service_x22 functionality
/// @endcode
///
/// ================================================================

#ifndef ISOFT_UDS_SERVER_SERVICE_X22_H_
#define ISOFT_UDS_SERVER_SERVICE_X22_H_

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

/// @brief 22 service class
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02449
/// @needwork = dd
/// @endcode
class ServiceX22 : public ServiceInterface
{
public:
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00763
    /// @needwork = dda
    /// @endcode
    ServiceX22(ServiceX22 const&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00764
    /// @needwork = dda
    /// @endcode
    ServiceX22& operator=(ServiceX22 const&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00765
    /// @needwork = dda
    /// @endcode
    ServiceX22(ServiceX22&&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00766
    /// @needwork = dda
    /// @endcode
    ServiceX22& operator=(ServiceX22&&) = delete;

    /// @brief Constructor
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01451
    /// @needwork = dd
    /// @endcode
    ServiceX22() noexcept;

    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00767
    /// @needwork = dda
    /// @endcode
    ~ServiceX22() noexcept override = default;

    /// @brief Initialization function
    /// @param model Model
    /// @param authManagerPtr Authentication manager pointer
    /// @param sessionManagerPtr Session manager pointer
    /// @param securityLevelManagerPtr Security level manager pointer
    /// @param envirConditionManagerPtr Environmental condition manager pointer
    /// @param diagnosticDataManagerPtr Diagnostic data data manager pointer
    /// @return 0: Success
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01452
    /// @needwork = dd
    /// @endcode
    std::int32_t Initialize(ServiceX22Model& model,
                            std::shared_ptr< AuthenticationManager > const& authManagerPtr,
                            std::shared_ptr< SessionManager >& sessionManagerPtr,
                            std::shared_ptr< SecurityLevelManager >& securityLevelManagerPtr,
                            std::shared_ptr< EnvironmentalConditionManager >& envirConditionManagerPtr,
                            std::shared_ptr< DiagnosticDataManager >& diagnosticDataManagerPtr) noexcept;

    /// @brief Get session validator
    /// @param id Session ID
    /// @return Session validator pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01453
    /// @needwork = dd
    /// @endcode
    std::vector< ValidatorPtr > GetSidValidator() noexcept override;

    /// @brief Get session validator
    /// @param id Session ID
    /// @return Session validator pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01454
    /// @needwork = dd
    /// @endcode
    std::vector< ValidatorPtr > GetSid2AValidator() noexcept;

    /// @brief Get session validator
    /// @param id Session ID
    /// @return Session validator pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01455
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< SubfunctionLevelSessionValidator< std::uint16_t > > GetSessionValidator(std::uint16_t id) noexcept;

    /// @brief Get security level validator
    /// @param id Security level ID
    /// @return Security level validator pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01456
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< SubfunctionLevelSecurityLevelValidator< std::uint16_t > > GetSecurityLevelValidator(
        std::uint16_t id) noexcept;

    /// @brief Request processing function
    /// @param context Parameter
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01457
    /// @needwork = dd
    /// @endcode
    void Process(std::shared_ptr< RequestContext > context) noexcept override;

    /// @brief Confirm function
    /// @param request Parameter
    /// @param status Parameter
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01458
    /// @needwork = dd
    /// @endcode
    void Confirm(RequestContext& request, ResponseStatusType status) noexcept override;

    /// @brief Check session
    /// @param dataIdentifier Parameter
    /// @return Operation result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01459
    /// @needwork = dd
    /// @endcode
    bool CheckDataIdentifierSession(std::uint16_t dataIdentifier) noexcept;

    /// @brief Check security level
    /// @param dataIdentifier Parameter
    /// @return Operation result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01460
    /// @needwork = dd
    /// @endcode
    bool CheckDataIdentifierSecurityLevel(std::uint16_t dataIdentifier) noexcept;

    /// @brief Check environmental condition
    /// @param dataIdentifier Parameter
    /// @return Operation result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01461
    /// @needwork = dd
    /// @endcode
    bool CheckDataIdentifierCondition(
        std::uint16_t dataIdentifier,
        const isoft::uds::server::MetaInfoMap& metaInfo,
        std::shared_ptr< isoft::uds::server::CancellationState > const& cancellationState) noexcept;

private:
    /// @brief Instance structure
    /// @param id Instance ID
    /// @param authValidator Authentication validator pointer
    /// @param sessionValidator Session validator pointer
    /// @param securityLevelValidator Security level validator pointer
    /// @param envCondition Environmental condition pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01462
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
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01463
    /// @needwork = dd
    /// @endcode
    std::map< std::uint16_t, std::shared_ptr< Instance > > instanceTable_{};

    /// @brief Maximum session ID to read
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01464
    /// @needwork = dd
    /// @endcode
    std::uint16_t maxDidToRead_{0U};

    /// @brief Check whether each source ID reads session only once
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01465
    /// @needwork = dd
    /// @endcode
    bool checkPerSourceId_{false};

    /// @brief Data manager pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01466
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< DiagnosticDataManager > dataIdentifierManager_{nullptr};

    /// @brief Authentication validator pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01467
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< ServerLevelAuthValidator< std::uint16_t > > authValidator_;
    /// @brief Session validator pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01468
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< ServerLevelSessionValidator< std::uint16_t > > sessionValidator_{nullptr};
    /// @brief Security level validator pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01469
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< ServerLevelSecurityLevelValidator< std::uint16_t > > securityLevelValidator_{nullptr};

    /// @brief Filter function
    /// @param context Parameter
    /// @param rawDataIdentifierTable Parameter
    /// @param validDataIdentifierTable Parameter
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01470
    /// @needwork = dd
    /// @endcode
    NrcErrc _filter(std::shared_ptr< RequestContext >& context,
                    std::set< std::uint16_t >& rawDataIdentifierTable,
                    std::vector< std::uint16_t >& validDataIdentifierTable) noexcept;
};

}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  // ISOFT_UDS_SERVER_SERVICE_X22_H_