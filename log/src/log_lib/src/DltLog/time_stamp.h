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
/// @file       time_stamp.h
/// @brief      Template class for encapsulating log parameters
/// @details
/// @date       2024-06-28
/// @author     yangjinbiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/LOG/DltLog
/// @interface_level = unit
/// @trace_id_sr=LOG_SR_00001,LOG_SR_00002,LOG_SR_00003,LOG_SR_00004,LOG_SR_00006,LOG_SR_00010,LOG_SR_00011,LOG_SR_00012,LOG_SR_00013,LOG_SR_00014
/// @unit_name = LogTimeStamp
/// @unit_description=Get timestamp based on configuration: if time base is configured, get time base time; otherwise get system time
/// @endcode
///
/// ================================================================

#ifndef __LOG_TOOLS_TIMESTAMP__
#define __LOG_TOOLS_TIMESTAMP__
#include <sys/time.h>

#include <cstdint>
#include <list>
#include <memory>
#include <mutex>
#include <string>
#ifdef ARA_WITH_TSYNC
    #include <ara/tsync/synchronized_time_base_consumer.h>
#endif
namespace ara {
namespace log {
namespace internal {
/// @brief Utility class
/// @brief Get timestamp based on configuration: if time base is configured, get time base time; otherwise get system time
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_LOG_00153
/// @trace_id_dd=DD_LOG_00235
/// @needwork = ad
/// @endcode
class LogTimeStamp final
{
public:
    /// @brief
    /// @param[in]  timebaseName
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00154
    /// @trace_id_dd=DD_LOG_00236
    /// @needwork = ad
    /// @endcode
    explicit LogTimeStamp(std::string timebaseName) noexcept;

    /// @brief Destructor, default
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00155
    /// @trace_id_dd=DD_LOG_00237
    /// @needwork = ad
    /// @endcode
    ~LogTimeStamp() = default;

    /// @brief Copy constructor, deleted
    /// @param[in]  other
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00156
    /// @trace_id_dd=DD_LOG_00238
    /// @needwork = ad
    /// @endcode
    LogTimeStamp(LogTimeStamp const& other) = delete;

    /// @brief Move copy
    /// @param[in]  other
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00157
    /// @trace_id_dd=DD_LOG_00239
    /// @needwork = ad
    /// @endcode
    LogTimeStamp(LogTimeStamp&& other) = delete;

    /// @brief Copy assignment, deleted
    /// @param[in]  other
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00158
    /// @trace_id_dd=DD_LOG_00240
    /// @needwork = ad
    /// @endcode
    LogTimeStamp& operator=(LogTimeStamp const& other) = delete;

    /// @brief Move assignment
    /// @param[in]  other
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00159
    /// @trace_id_dd=DD_LOG_00241
    /// @needwork = ad
    /// @endcode
    LogTimeStamp& operator=(LogTimeStamp&& other) = delete;

    /// @brief Get timestamp
    /// @return Timestamp in microseconds
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00160
    /// @trace_id_dd=DD_LOG_00242
    /// @needwork = ad
    /// @endcode
    std::uint64_t TSYNCGetTimeStamp() noexcept;

    bool TsyncEnabled() noexcept
    {
        if (timebaseName_.empty()) {
            return false;
        }
        return true;
    }

private:
    /// @brief timebaseName_ Stores the time base. If the process starts before tsyncd, the consumer cannot open it; subsequent attempts are needed.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00153
    /// @trace_id_dd=DD_LOG_00243
    /// @needwork = dda
    /// @endcode
    std::string timebaseName_;

#ifdef ARA_WITH_TSYNC
    /// @brief timebaseConsumer_ Pointer to the time base consumer
    std::shared_ptr< ara::tsync::SynchronizedTimeBaseConsumer > timebaseConsumer_{nullptr};
#endif

    /// @brief consumerMutex_ During startup, if tsyncd has not started yet, Consumer objects will be recreated frequently; need to prevent multi-threaded contention
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00153
    /// @trace_id_dd=DD_LOG_00244
    /// @needwork = dda
    /// @endcode
    std::mutex consumerMutex_;

    /// @brief
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00153
    /// @trace_id_dd=DD_LOG_00245
    /// @needwork = dda
    /// @endcode
    bool isInited_{false};
};

}  // namespace internal
}  // namespace log
}  // namespace ara
#endif