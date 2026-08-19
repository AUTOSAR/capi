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
/// @file       idsm_trans_limit.h
/// @brief      Security event remote transmission check class
/// @details
/// @date       2023-02-15
/// @author     niuliming
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/idsm/qualified security event storage
/// @interface_level=unit
/// @trace_id_sr=SR_IDSM_0013
/// @unit_description=Security event remote transmission check class
/// @endcode
///
/// ================================================================

#ifndef ARA_IDSM_TRANS_LIMIT_H_
#define ARA_IDSM_TRANS_LIMIT_H_
#include <memory>
#include <mutex>

#include "ara/idsm/internal/idsm_error_domain.h"
namespace ara {
namespace idsm {

/// @brief Default period of rate limiting and traffic shaping filters
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00267
/// @trace_id_dd=DD_IDSM_00817
/// @needwork = dd
/// @endcode
static time_t const kTimeIntervalConst{1024};
/// @brief Traffic shaping class
/// @code{.isoft}
/// @unit_name=TrafficChecker
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00267
/// @trace_id_dd=DD_IDSM_00818
/// @needwork = ad
/// @endcode
class TrafficChecker final
{
public:
    /// @brief Check if traffic exceeds threshold
    /// @param bytes Number of bytes to transmit
    /// @param limited_counter Number of times traffic has been limited
    /// @return Returns false if traffic exceeds threshold, otherwise returns true
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00819
    /// @needwork = dda
    /// @endcode
    bool Check(uint64_t const bytes, uint8_t& limitedCounter) noexcept;
    /// @brief Set the time interval and traffic limit for the traffic shaping class
    /// @param intervals Time interval
    /// @param bytes Traffic limit
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00820
    /// @needwork = dda
    /// @endcode
    void SetIntervalAndBytes(time_t const intervals, uint64_t const bytes) noexcept
    {
        timeInterval_ = intervals;
        maxBytes_     = bytes;
    }

public:
    /// @brief Parameterless constructor
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00821
    /// @needwork = dda
    /// @endcode
    TrafficChecker() noexcept : TrafficChecker{0U, kTimeIntervalConst, 0U, 0, 0U} {}
    /// @brief Parameterized constructor
    /// @param intervals Time interval
    /// @param bytes Maximum number of bytes that can be transmitted within the time interval
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00822
    /// @needwork = dda
    /// @endcode
    TrafficChecker(time_t const intervals, uint64_t const bytes) noexcept : TrafficChecker{bytes, intervals, 0U, 0, 0U}
    {
    }

private:
    /// @brief Parameterized constructor
    /// @param bytes Upper limit on the number of bytes that can be sent within the period
    /// @param intervals Traffic shaping period
    /// @param sum Number of bytes already sent within the period
    /// @param timeStamp Start timestamp of the period
    /// @param times Number of times traffic has been limited within the period
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00823
    /// @needwork = dda
    /// @endcode
    TrafficChecker(uint64_t const bytes,
                   time_t const intervals,
                   uint64_t const sum,
                   time_t const timeStamp,
                   uint8_t const times) noexcept
        : maxBytes_{bytes}, timeInterval_{intervals}, bytesSum_{sum}, lastTimestamp_{timeStamp}, limitedTimes_{times}
    {
    }

private:
    /// @name maxBytes
    /// @brief Maximum number of bytes that can be transmitted within the time interval
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00824
    /// @needwork = dda
    /// @endcode
    uint64_t maxBytes_;
    /// @name timeInterval
    /// @brief Time interval
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00825
    /// @needwork = dda
    /// @endcode
    time_t timeInterval_;
    /// @name bytesSum
    /// @brief Number of bytes already transmitted within the time interval
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00826
    /// @needwork = dda
    /// @endcode
    uint64_t bytesSum_;
    /// @name lastTimestamp
    /// @brief Start timestamp of the current time interval
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00827
    /// @needwork = dda
    /// @endcode
    time_t lastTimestamp_;
    /// @brief Number of times traffic has been limited within the period, at most once per period
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00828
    /// @needwork = dda
    /// @endcode
    uint8_t limitedTimes_;
};

/// @brief Rate limiting class
/// @code{.isoft}
/// @unit_name=RateChecker
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00268
/// @trace_id_dd=DD_IDSM_00829
/// @needwork = ad
/// @endcode
class RateChecker final
{
public:
    /// @brief Check if rate exceeds threshold
    /// @param events Number of events to transmit
    /// @param limited_counter Number of times rate has been limited
    /// @return Returns false if rate exceeds threshold, otherwise returns true
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00830
    /// @needwork = dda
    /// @endcode
    bool Check(uint32_t const events, uint8_t& limitedCounter) noexcept;
    /// @brief Set the time interval and rate limit for the rate limiting class
    /// @param interval Time interval
    /// @param events Rate limit
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00831
    /// @needwork = dda
    /// @endcode
    void SetIntervalAndEvents(time_t const interval, uint32_t const events) noexcept
    {
        timeInterval_ = interval;
        maxEvents_    = events;
    }

public:
    /// @brief Parameterless constructor
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00832
    /// @needwork = dda
    /// @endcode
    RateChecker() noexcept : RateChecker{0U, kTimeIntervalConst, 0U, 0, 0U} {}
    /// @brief Parameterized constructor
    /// @param interval Time interval
    /// @param events Maximum number of events that can be transmitted within the time interval
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00833
    /// @needwork = dda
    /// @endcode
    RateChecker(time_t const interval, uint32_t const events) noexcept : RateChecker{events, interval, 0U, 0, 0U} {}

private:
    /// @brief Parameterized constructor
    /// @param events Upper limit on the number of security events that can be sent within the period
    /// @param intervals Rate limiting period
    /// @param sum Number of security events already sent within the period
    /// @param timeStamp Start timestamp of the period
    /// @param times Number of times rate has been limited within the period
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00834
    /// @needwork = dda
    /// @endcode
    RateChecker(uint32_t const events,
                time_t const intervals,
                uint32_t const sum,
                time_t const timeStamp,
                uint8_t const times) noexcept
        : maxEvents_{events}, timeInterval_{intervals}, eventsSum_{sum}, lastTimestamp_{timeStamp}, limitedTimes_{times}
    {
    }

private:
    /// @name maxEvents
    /// @brief Maximum number of events that can be transmitted within the time interval
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00835
    /// @needwork = dda
    /// @endcode
    uint32_t maxEvents_;
    /// @name timeInterval
    /// @brief Time interval
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00836
    /// @needwork = dda
    /// @endcode
    time_t timeInterval_;
    /// @name eventsSum
    /// @brief Number of events already transmitted within the time interval
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00837
    /// @needwork = dda
    /// @endcode
    uint32_t eventsSum_;
    /// @name lastTimestamp
    /// @brief Start timestamp of the current time interval
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00838
    /// @needwork = dda
    /// @endcode
    time_t lastTimestamp_;
    /// @brief Number of times rate has been limited within the period, at most once per period
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00839
    /// @needwork = dda
    /// @endcode
    uint8_t limitedTimes_;
};

}  // namespace idsm
}  // namespace ara

#endif
