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
/// @file       service_x2a.h
/// @brief      This file provides service_x2a functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/isoft/uds/data_management
/// @interface_level=unit
/// @trace_id_sr=d4b9ac8d-2b1b-4b5b-a3b9-52740cb6106f
/// @unit_name=service_x2a.h
/// @unit_description=This file provides service_x2a functionality
/// @endcode
///
/// ================================================================

#ifndef ISOFT_UDS_SERVER_SERVICE_X2A_H_
#define ISOFT_UDS_SERVER_SERVICE_X2A_H_

#include <cstddef>
#include <cstdint>

#include "common/request_context.h"
#include "common/service_interface.h"
#include "common/timer.h"
#include "diagnostic_data_manager.h"
#include "isoft/uds/data_management/diagnostic_data_model_manager.h"
#include "service_x22.h"
namespace isoft {
namespace uds {
namespace server {



/// @brief Service X2A class
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = dd
/// @endcode
class ServiceX2A : public ServiceInterface
{
public:
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00749
    /// @needwork = dda
    /// @endcode
    ServiceX2A(ServiceX2A const&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00750
    /// @needwork = dda
    /// @endcode
    ServiceX2A& operator=(ServiceX2A const&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00751
    /// @needwork = dda
    /// @endcode
    ServiceX2A(ServiceX2A&&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00752
    /// @needwork = dda
    /// @endcode
    ServiceX2A& operator=(ServiceX2A&&) = delete;

    /// @brief Constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    ServiceX2A() noexcept;

    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    ~ServiceX2A() noexcept override = default;

    /// @brief Initialization function
    /// @param[in] model Model
    /// @param[in] serviceX22Ptr Service X22 pointer
    /// @param[in] sessionManagerPtr Session management pointer
    /// @param[in] securityLevelManagerPtr Security level management pointer
    /// @param[in] diagnosticDataManagerPtr Diagnostic data management pointer
    /// @return 0: Success
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    std::int32_t Initialize(ServiceX2AModel& model,
                            std::shared_ptr< ServiceX22 >& serviceX22Ptr,
                            std::shared_ptr< SessionManager >& sessionManagerPtr,
                            std::shared_ptr< SecurityLevelManager >& securityLevelManagerPtr,
                            std::shared_ptr< DiagnosticDataManager >& diagnosticDataManagerPtr) noexcept;

    /// @brief Get service level validator
    /// @return Service level validator array
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    std::vector< ValidatorPtr > GetSidValidator() noexcept override;

    /// @brief Process function
    /// @param context Parameter
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    void Process(std::shared_ptr< RequestContext > context) noexcept override;

    /// @brief Confirm function
    /// @param status Parameter
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    void Confirm(RequestContext& request, ResponseStatusType status) noexcept override;

private:
    /// @brief Get PDID table callback function
    /// @param[in] category Service level
    /// @return PDID vector
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    using GetPDIDTableCallback = std::function< std::vector< std::uint8_t >(DiagnosticPeriodicRateCategoryEnum) >;

    /// @brief Task scheduler class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    class Scheduler final
    {
    public:
        /// @brief Constructor
        /// @return Return value
        /// @code{.isoft}
        /// @interface_level=unit
        /// @needwork = dd
        /// @endcode
        Scheduler() noexcept;

        /// @brief Destructor
        /// @return Return value
        /// @code{.isoft}
        /// @interface_level=unit
        /// @needwork = dd
        /// @endcode
        ~Scheduler() noexcept = default;

        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_UDS_00000
        /// @trace_id_dd=DD_UDS_00753
        /// @needwork = dda
        /// @endcode
        Scheduler(Scheduler const&) = delete;
        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_UDS_00000
        /// @trace_id_dd=DD_UDS_00754
        /// @needwork = dda
        /// @endcode
        Scheduler& operator=(Scheduler const&) = delete;
        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_UDS_00000
        /// @trace_id_dd=DD_UDS_00755
        /// @needwork = dda
        /// @endcode
        Scheduler(Scheduler&&) = delete;
        /// @code{.isoft}
        /// @interface_level=unit
        /// @trace_id_ad=AD_UDS_00000
        /// @trace_id_dd=DD_UDS_00756
        /// @needwork = dda
        /// @endcode
        Scheduler& operator=(Scheduler&&) = delete;

        /// @brief Initialization function
        /// @param[in] rates Service level
        /// @param[in] getPDIDTableCallback Get PDID table callback function
        /// @param[in] dataIdentifierManager Diagnostic data management pointer
        /// @return 0: Success
        /// @code{.isoft}
        /// @interface_level=unit
        /// @needwork = dd
        /// @endcode
        std::int32_t Initialize(DiagnosticPeriodicRate rates,
                                GetPDIDTableCallback getPDIDTableCallback,
                                std::shared_ptr< DiagnosticDataManager > dataIdentifierManager) noexcept;

        /// @brief Start function
        /// @param[in] context Context pointer
        /// @return Return value
        /// @code{.isoft}
        /// @interface_level=unit
        /// @needwork = dd
        /// @endcode
        void Start(std::shared_ptr< RequestContext > context) noexcept;

        /// @brief Stop function
        /// @return Return value
        /// @code{.isoft}
        /// @interface_level=unit
        /// @needwork = dd
        /// @endcode
        void Stop() noexcept;

    private:
        /// @brief Timer timeout handling
        /// @return Return value
        /// @code{.isoft}
        /// @interface_level=unit
        /// @needwork = dd
        /// @endcode
        void _timeOut() noexcept;



        /// @brief Service level
        /// @code{.isoft}
        /// @interface_level=unit
        /// @needwork = dd
        /// @endcode
        DiagnosticPeriodicRate rate_{};

        /// @brief Timer
        /// @code{.isoft}
        /// @interface_level=unit
        /// @needwork = dd
        /// @endcode
        Timer timer_{};

        /// @brief Get PDID table callback function
        /// @code{.isoft}
        /// @interface_level=unit
        /// @needwork = dd
        /// @endcode
        GetPDIDTableCallback getPDIDTableCallback_{nullptr};

        /// @brief Diagnostic data management pointer
        /// @code{.isoft}
        /// @interface_level=unit
        /// @needwork = dd
        /// @endcode
        std::shared_ptr< DiagnosticDataManager > dataIdentifierManager_{nullptr};

        /// @brief Context pointer
        /// @code{.isoft}
        /// @interface_level=unit
        /// @needwork = dd
        /// @endcode
        std::shared_ptr< RequestContext > context_{nullptr};
    };
    /// @brief Task scheduler class pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    using SchedulerPtr = std::shared_ptr< Scheduler >;

private:
    /// @brief Task scheduler class mapping table
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    std::map< std::uint8_t, DiagnosticPeriodicRateCategoryEnum > periodicIdentifierStateTable_{};
    /// @brief Task scheduler class mapping table mutex
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    std::mutex periodicIdentifierStateMutex_{};

    /// @brief Check Session and SecurityLevel of each source ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    bool checkPerSourceId_{false};

    /// @brief Maximum number of task schedulers
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    std::uint8_t maxPeriodicDidToRead_{0U};

    /// @brief Maximum number of task schedulers
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    std::uint8_t schedulerMaxNumber_{0U};

    /// @brief Dynamically define data identifier subfunction table
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    std::set< DiagnosticDynamicallyDefineDataIdentifierSubfunctionEnum > subfunctionTable_{};

    /// @brief Diagnostic data management pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< DiagnosticDataManager > dataIdentifierManager_{nullptr};

    /// @brief Task scheduler class pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< ServiceX22 > serviceX22Ptr_{nullptr};

    /// @brief Task scheduler class mapping table
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< ServerLevelSessionValidator< std::uint16_t > > sessionValidator_{nullptr};

    /// @brief Task scheduler class mapping table
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< ServerLevelSecurityLevelValidator< std::uint16_t > > securityLevelValidator_{nullptr};

    /// @brief Task scheduler class mapping table
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    std::map< DiagnosticPeriodicRateCategoryEnum, SchedulerPtr > schedulerTable_{};

private:
    /// @brief Task scheduler class mapping table
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    void _add(DiagnosticPeriodicRateCategoryEnum rate, std::vector< std::uint8_t >& pdidTable) noexcept;

    /// @brief Task scheduler class mapping table
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    bool _remove(std::vector< std::uint8_t >& pdidTable) noexcept;

    /// @brief Task scheduler class mapping table
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    void _clearAll() noexcept;

    /// @brief Task scheduler class mapping table
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    std::vector< std::uint8_t > _get(DiagnosticPeriodicRateCategoryEnum rate) noexcept;

    /// @brief Task scheduler class mapping table
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    void _clearWhenSessionChange(SessionId, SessionId newSessionId) noexcept;

    /// @brief Task scheduler class mapping table
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    void _clearWhenSecurityLevelChange(SecurityLevelId id) noexcept;
};

}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  // ISOFT_UDS_SERVER_SERVICE_X2A_H_