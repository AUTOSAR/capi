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
/// @file       service_x31.h
/// @brief      This file provides service_x31 functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/isoft/uds/routine_management
/// @interface_level=software
/// @trace_id_sr=c112411e-7929-493a-a287-4d7bfb07c627
/// @unit_name=service_x31.h
/// @unit_description=This file provides service_x31 functionality
/// @endcode
///
/// ================================================================

#ifndef ISOFT_UDS_SERVER_SERVICE_X31_H_
#define ISOFT_UDS_SERVER_SERVICE_X31_H_

#include <cstddef>
#include <memory>

#include "authentication_management/auth_validator.h"
#include "authentication_management/authentication_manager.h"
#include "common/request_context.h"
#include "common/service_interface.h"
#include "data_management/environmental_condition.h"
#include "isoft/uds/routine_management/routine_management.h"
#include "security_access_management/securityLevel_validator.h"
#include "security_access_management/security_level_manager.h"
#include "session_management/session_manager.h"
#include "session_management/session_validator.h"
namespace isoft {
namespace uds {
namespace server {

/// @brief The client uses the RoutineControl service to execute defined sequences of steps and obtain any relevant results.
/// This service has great flexibility, but typical uses may include functions such as erasing memory, resetting or learning adaptive data, running self-tests,
/// overriding normal server control strategies, and controlling server values to change over time, including predefined sequences (e.g., closing a convertible roof).
/// Generally, when used for output control, this service is used for more complex types of control, while inputOutputControlByldentifier is used for relatively simple (e.g., static) output control.
///
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02450
/// @needwork = dd
/// @endcode
class ServiceX31 : public ServiceInterface
{
public:
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00768
    /// @needwork = dda
    /// @endcode
    ServiceX31(ServiceX31 const& other) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00769
    /// @needwork = dda
    /// @endcode
    ServiceX31& operator=(ServiceX31 const& other) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00770
    /// @needwork = dda
    /// @endcode
    ServiceX31(ServiceX31&&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00771
    /// @needwork = dda
    /// @endcode
    ServiceX31& operator=(ServiceX31&&) = delete;

    /// @brief Constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    explicit ServiceX31() noexcept;

    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01471
    /// @needwork = dd
    /// @endcode
    ~ServiceX31() noexcept override = default;

    /// @brief Initialization function
    /// @param[in] config Configuration file
    /// @param[in] authManagerPtr Authentication manager pointer
    /// @param[in] sessionManagerPtr Session manager pointer
    /// @param[in] securityLevelManagerPtr Security level manager pointer
    /// @return 0: Success
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01472
    /// @needwork = dd
    /// @endcode
    std::int32_t Initialize(RoutineManagement& config,
                            std::shared_ptr< AuthenticationManager > const& authManagerPtr,
                            std::shared_ptr< SessionManager > const& sessionManagerPtr,
                            std::shared_ptr< SecurityLevelManager > const& securityLevelManagerPtr) noexcept;

    /// @brief Get session ID validator
    /// @return Session ID validator pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01473
    /// @needwork = dd
    /// @endcode
    std::vector< ValidatorPtr > GetSidValidator() noexcept override;

    /// @brief Process function
    /// @param[in] context Request context
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01474
    /// @needwork = dd
    /// @endcode
    void Process(std::shared_ptr< RequestContext > context) noexcept override;

    /// @brief Confirm function
    /// @param status Parameter
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01475
    /// @needwork = dd
    /// @endcode
    void Confirm(RequestContext& request, ResponseStatusType status) noexcept override;

    /// @brief Get P4 server max value
    /// @param[in] subfunction Subfunction
    /// @param[in] assistParam Assist parameter
    /// @return P4 server max value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01476
    /// @needwork = dd
    /// @endcode
    virtual std::uint32_t GetP4ServerMax(std::int16_t const subfunction = -1,
                                         std::int32_t const assistParam = -1) noexcept;

private:
    /// @brief Subfunction
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01477
    /// @needwork = dd
    /// @endcode
    enum class Subfunction : std::uint8_t
    {
        kStarRoutine           = 0x01U,
        kStopRoutine           = 0x02U,
        kRequestRoutineResults = 0x03U,
    };
    /// @brief Instance
    /// @param[in] id Instance ID
    /// @param[in] routineInfo Routine information
    /// @param[in] authValidator Authentication validator pointer
    /// @param[in] sessionValidator Session validator pointer
    /// @param[in] securityLevelValidator Security level validator pointer
    /// @param[in] subfunctionConditionMap Subfunction condition mapping
    /// @param[in] p4ServerMax P4 server max value
    /// @param[in] startP4ServerMax Start P4 server max value
    /// @param[in] stopP4ServerMax Stop P4 server max value
    /// @param[in] resultP4ServerMax Result P4 server max value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01478
    /// @needwork = dd
    /// @endcode
    struct Instance
    {
        std::uint16_t id{0U};
        std::int16_t routineInfo{0U};
        std::shared_ptr< SubfunctionLevelAuthValidator< std::uint16_t > > authValidator{nullptr};
        std::shared_ptr< SubfunctionLevelSessionValidator< std::uint16_t > > sessionValidator{nullptr};
        std::shared_ptr< SubfunctionLevelSecurityLevelValidator< std::uint16_t > > securityLevelValidator{nullptr};
        std::map< Subfunction, std::shared_ptr< EnvironmentalCondition > > subfunctionConditionMap;

        std::uint32_t p4ServerMax{0U};
        std::uint32_t startP4ServerMax{0U};
        std::uint32_t stopP4ServerMax{0U};
        std::uint32_t resultP4ServerMax{0U};
    };

    /// @brief Instance table
    /// @name instanceTable_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01479
    /// @needwork = dd
    /// @endcode
    ara::core::Map< std::uint16_t, std::shared_ptr< Instance > > instanceTable_{};

    /// @name authValidator_
    /// @brief Authentication validator pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01480
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< ServerLevelAuthValidator< std::uint16_t > > authValidator_{nullptr};
    /// @name sessionValidator_
    /// @brief Session validator pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01481
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< ServerLevelSessionValidator< std::uint16_t > > sessionValidator_{nullptr};
    /// @name securityLevelValidator_
    /// @brief Security level validator pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01482
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< ServerLevelSecurityLevelValidator< std::uint16_t > > securityLevelValidator_{nullptr};

    /// @brief Routine interface pointer
    /// @name interface_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01483
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< GenericRoutineInterface > interface_{nullptr};
};

}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  // ISOFT_UDS_SERVER_SERVICE_X31_H_