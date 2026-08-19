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
/// @file       idsm_sev_proc.h
/// @brief      Process security events reported by client
/// @details
/// @date       2023-02-16
/// @author     niuliming
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/idsm/event receiver handler
/// @interface_level=unit
/// @trace_id_sr=SR_IDSM_0001
/// @unit_name=EventProcer
/// @unit_description=Process security events reported by client
/// @endcode
///
/// ================================================================

#ifndef ARA_IDSM_SEV_PROC_H_
#define ARA_IDSM_SEV_PROC_H_
#include <cstdio>
#include <iostream>

#ifdef ARA_WITH_IAM
    #include "ara/iam/internal/grantquery/idsm.h"
#endif

#include "ara/idsm/internal/event.h"
#include "ara/idsm/internal/idsm_error_domain.h"
#include "ara/idsm/internal/message_process.h"
#include "filter/idsm_filter_chain.h"
#include "log/idsm_log.h"

namespace ara {
namespace idsm {
/// @brief Security event processor
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00146
/// @trace_id_dd=DD_IDSM_00335
/// @needwork = ad
/// @endcode
class EventProcer
{
public:
    /// @brief Set of security events reportable by a process
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00336
    /// @needwork = dda
    /// @endcode
    using ProcessEventVec = ara::core::Vector< uint16_t >;
    /// @brief Initialize event processor
    /// @return 0 on success, -1 on failure
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00337
    /// @needwork = dda
    /// @endcode
    int32_t Init();
    /// @brief Security event processing
    /// @param eventMsg Dynamic attributes of security event reported by AA application
    /// @return 0 on success, non-zero on failure
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00338
    /// @needwork = dda
    /// @endcode
    IdsmErrorCode Process(ClientEventMsg& eventMsg);
    /// @brief Timer processing
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00339
    /// @needwork = dda
    /// @endcode
    void Timerhandler();

public:
    /// @brief Default constructor
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00340
    /// @needwork = dda
    /// @endcode
    EventProcer() = default;
    /// @brief Copy constructor
    /// @param procer Object to be copied
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00341
    /// @needwork = dda
    /// @endcode
    EventProcer(EventProcer const& procer) = delete;
    /// @brief Move constructor
    /// @param procer Object to be moved
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00342
    /// @needwork = dda
    /// @endcode
    EventProcer(EventProcer&& procer) = default;
    /// @brief Copy assignment operator
    /// @param procer Object to be copied for assignment operator
    /// @return Left operand of the assignment operator
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00343
    /// @needwork = dda
    /// @endcode
    EventProcer& operator=(EventProcer const& procer) = delete;
    /// @brief Move assignment operator
    /// @param procer Object to be moved for assignment operator
    /// @return Left operand of the assignment operator
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00344
    /// @needwork = dda
    /// @endcode
    EventProcer& operator=(EventProcer&& procer) = default;
    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00345
    /// @needwork = dda
    /// @endcode
    virtual ~EventProcer() = default;

private:
    /// @brief Get configuration needed for event processing
    /// @return 0 on success, non-zero on failure
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00346
    /// @needwork = dda
    /// @endcode
    int32_t _procerConfig();
    /// @brief Process security event reporting mode
    /// @param event Security event
    /// @return true on success, false on processing error
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00347
    /// @needwork = dda
    /// @endcode
    static bool ReportingModeProc(EventPtr const& event);
    /// @brief IAM check
    /// @param procId Process ID
    /// @param eventId Security event ID
    /// @return 0 on check success, non-zero on check failure
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00348
    /// @needwork = dda
    /// @endcode
    bool _iamCheck(uint32_t const procId, uint16_t const eventId) const noexcept;
    /// @brief Memory check
    /// @param frameSize Stack frame memory
    /// @param contextSize Context memory
    /// @return 0 on check success, non-zero on check failure
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00349
    /// @needwork = dda
    /// @endcode
    IdsmErrorCode _memoryCheck(uint32_t const frameSize, uint64_t const contextSize) const;
    /// @brief Adjust security event timestamp
    /// @param eventMsg Client security event information
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00350
    /// @needwork = dda
    /// @endcode
    void _adjustTimestamp(ClientEventMsg& eventMsg) const;
    /// @brief Combine static and dynamic attributes of security event into a complete security event
    /// @param eventMsg Client security event information
    /// @param eventId Security event ID
    /// @return Complete security event. If failure, the security event is empty
    /// @exception Stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00351
    /// @needwork = dda
    /// @endcode
    EventPtr _assembleEvent(ClientEventMsg& eventMsg, uint16_t const eventId) const;

private:
    /// @brief Mapping between event ID and filter chain. Key is event type ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00352
    /// @needwork = dda
    /// @endcode
    ara::core::Map< uint16_t, FilterChainPtr > filterMap_;
    /// @brief Mapping between port and event type ID. Key is port (not port number)
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00353
    /// @needwork = dda
    /// @endcode
    ara::core::Map< ara::core::String, uint16_t > portMap_;
    /// @brief Mapping between port and process. Key is process
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00354
    /// @needwork = dda
    /// @endcode
    ara::core::Map< ara::core::String, ProcessEventVec > processMap_;
    /// @brief Internal security event for insufficient memory
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00355
    /// @needwork = dda
    /// @endcode
    IdsmInternalEvent internalSev_;
    /// @brief Event stack frame memory limit
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00356
    /// @needwork = dda
    /// @endcode
    uint64_t eventMemTotal_{0U};
    /// @brief Event context memory limit
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00357
    /// @needwork = dda
    /// @endcode
    uint64_t contextMemTotal_{0U};

#ifdef ARA_WITH_IAM
    /// @brief IAM check
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00358
    /// @needwork = dda
    /// @endcode
    std::unique_ptr< ara::iam::internal::grant::IAMGrantIdsmQuery > grantIAMPtr_{nullptr};
#endif

    /// @brief Timestamp format
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00359
    /// @needwork = dda
    /// @endcode
    ara::core::String timestampFormat_{""};
    /// @brief Time base
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00360
    /// @needwork = dda
    /// @endcode
    ara::core::String timeBaseFQN_{""};
};
}  // namespace idsm
}  // namespace ara
#endif