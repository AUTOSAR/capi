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
/// @file       time_stamp.cpp
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
/// @interface_level=unit
/// @trace_id_sr=LOG_SR_00001,LOG_SR_00002,LOG_SR_00003,LOG_SR_00004,LOG_SR_00006,LOG_SR_00010,LOG_SR_00011,LOG_SR_00012,LOG_SR_00013,LOG_SR_00014
/// @unit_name = time_stamp
/// @unit_description=Get timestamp based on configuration: if time base is configured, get time base time; otherwise get system time
/// @endcode
///
/// ================================================================

#include "time_stamp.h"

#include <time.h>

#include <cstdint>
#include <utility>

#include "Utils/src/private_log.h"
namespace ara {
namespace log {
namespace internal {

/// @brief Ten thousand
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_LOG_00153
/// @trace_id_dd=DD_LOG_01699
/// @needwork = dda
/// @endcode
static std::uint32_t const kTenThousand{10000U};

/// @brief One hundred thousand
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_LOG_00153
/// @trace_id_dd=DD_LOG_01700
/// @needwork = dda
/// @endcode
static std::uint32_t const kHundredThousand{100000U};

LogTimeStamp::LogTimeStamp(std::string timebaseName) noexcept : timebaseName_(std::move(timebaseName)) {}

/// This can only be used in the console and file in version 2.2.
std::uint64_t LogTimeStamp::TSYNCGetTimeStamp() noexcept
{
    if (timebaseName_.empty() == false) {
#ifdef ARA_WITH_TSYNC
        if (isInited_ == false) {
            std::lock_guard< decltype(consumerMutex_) > const lock{consumerMutex_};
            if (isInited_ == false) {
                timebaseConsumer_.reset();
                ara::core::String timebase{timebaseName_};
                timebaseConsumer_ = std::make_shared< ara::tsync::SynchronizedTimeBaseConsumer >(timebase);
                ara::tsync::Timestamp const gtm{timebaseConsumer_->GetCurrentTime()};
                std::chrono::microseconds const tsS{
                    std::chrono::duration_cast< std::chrono::microseconds >(gtm.time_since_epoch())};
                std::int64_t tmpCount = {static_cast< std::int64_t >(tsS.count())};
                if (tmpCount > 0) {
                    isInited_ = true;
                }
            }
        }
        if (isInited_) {
            ara::tsync::Timestamp const gtm{timebaseConsumer_->GetCurrentTime()};
            std::chrono::microseconds const tsS{
                std::chrono::duration_cast< std::chrono::microseconds >(gtm.time_since_epoch())};
            return {static_cast< std::uint64_t >(tsS.count())};
        }
#endif
    }
    return 0U;
}

}  // namespace internal
}  // namespace log
}  // namespace ara
