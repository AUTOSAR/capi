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
/// @file       event_report.h
/// @brief      Reporting interface provided to AA applications
/// @details
/// @date       2022-12-27
/// @author     niuliming
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/idsm/lib client
/// @interface_level=module
/// @trace_id_sr=SRS_IDSM_0005
/// @unit_name=EventReporter
/// @unit_description=Reporting interface provided to AA applications
/// @endcode
///
/// ================================================================

#ifndef ARA_IDSM_EVENT_REPORT_H_
#define ARA_IDSM_EVENT_REPORT_H_

#include <ara/core/instance_specifier.h>
#include <ara/log/logger.h>

#include "ara/idsm/common.h"

namespace ara {
namespace idsm {
/// @brief {SWS_AIDSM_10101} Security event reporting class.
/// @code{.isoft}
/// export_level=/IntrusionDetectionSystem
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00016
/// @trace_id_dd=DD_IDSM_00042
/// @needwork = ad
/// @endcode
class EventReporter
{
private:
    /* data */
public:
    /// @brief Constructor
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00043
    /// @needwork = dda
    /// @endcode
    EventReporter() = delete;
    /// @brief Destructor
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00044
    /// @needwork = dda
    /// @endcode
    virtual ~EventReporter() = default;
    /// @brief Copy constructor
    /// @param object to be copied
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00045
    /// @needwork = dda
    /// @endcode
    EventReporter(EventReporter const &) = default;
    /// @brief Move constructor
    /// @param object to be moved
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00046
    /// @needwork = dda
    /// @endcode
    EventReporter(EventReporter &&) = default;
    /// @brief Copy assignment operator
    /// @param object to be copied for assignment operator
    /// @return Left operand of the assignment operator
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00047
    /// @needwork = dda
    /// @endcode
    EventReporter &operator=(EventReporter const &) = default;
    /// @brief Move assignment operator
    /// @param object to be moved for assignment operator
    /// @return Left operand of the assignment operator
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00048
    /// @needwork = dda
    /// @endcode
    EventReporter &operator=(EventReporter &&) = default;
    /// @brief  [SWS_AIDSM_10301] Construct a security event reporting object for generating a security event of the specified type.
    /// @param eventType Port instance identifier
    /// @return Void return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00017
    /// @trace_id_dd=DD_IDSM_00049
    /// @needwork = ad
    /// @endcode
    explicit EventReporter(ara::core::InstanceSpecifier eventType) noexcept;

    /// @brief [SWS_AIDSM_10302] Create a security event
    /// @param counter Event counter
    /// @return Void return
    /// @code{.isoft}
    /// export_level=/IntrusionDetectionSystem
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00018
    /// @trace_id_dd=DD_IDSM_00050
    /// @needwork = ad
    /// @endcode
    void ReportEvent(CountType const counter = 1U) const noexcept;

    /// @brief [SWS_AIDSM_10303] Create a security event
    /// @param timestamp Timestamp
    /// @param counter Event counter
    /// @return Void return
    /// @code{.isoft}
    /// export_level=/IntrusionDetectionSystem
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00019
    /// @trace_id_dd=DD_IDSM_00051
    /// @needwork = ad
    /// @endcode
    void ReportEvent(TimestampType const timestamp, CountType const counter = 1U) const noexcept;

    /// @brief [SWS_AIDSM_10304]  Create a security event
    /// @param contextData Event context data
    /// @param counter Event counter
    /// @return Void return
    /// @code{.isoft}
    /// export_level=/IntrusionDetectionSystem
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00020
    /// @trace_id_dd=DD_IDSM_00052
    /// @needwork = ad
    /// @endcode
    void ReportEvent(ContextDataType const &contextData, CountType const counter = 1U) const noexcept;

    /// @brief [SWS_AIDSM_10305] Create a security event
    /// @param contextData Event context data
    /// @param timestamp Timestamp
    /// @param counter Event counter
    /// @return Void return
    /// @code{.isoft}
    /// export_level=/IntrusionDetectionSystem
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00021
    /// @trace_id_dd=DD_IDSM_00053
    /// @needwork = ad
    /// @endcode
    void ReportEvent(ContextDataType const &contextData,
                     TimestampType const timestamp,
                     CountType const counter = 1U) const noexcept;

private:
    /// @name instanceId_
    /// Unique identifier
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00054
    /// @needwork = dda
    /// @endcode
    ara::core::InstanceSpecifier instanceId_;
};

}  // namespace idsm

}  // namespace ara
#endif