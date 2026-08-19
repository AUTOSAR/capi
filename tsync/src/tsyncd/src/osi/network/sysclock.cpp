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
/// @file       sysclock.cpp
/// @brief      OSI network module raw socket class
/// @details
/// @date       2023-01-09
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#include "isoft/osi/network/sysclock.h"

#include <fcntl.h>
#include <linux/ethtool.h>
#include <linux/sockios.h>
#include <net/if.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <unistd.h>

#include <chrono>
#include <cstring>
#include <ctime>
#include <iostream>
#include <sstream>
#include <string>

#include "ara/tsync/internal/common.h"
#include "ara/tsync/internal/log/logger.h"

namespace isoft {
namespace osi {
namespace network {
/// @brief log output
/// @return Logger object reference
static inline ara::tsync::internal::tslog::Logger &LOG() noexcept { return ara::tsync::internal::tslog::Log(); }

/// @brief open system clock
/// @return <0 indicates failure.
std::int32_t SysClock::Open() noexcept
{
    clkid_ = CLOCK_REALTIME;
    struct timex tx
    {
    };
    memset(&tx, 0, sizeof(tx));
    tx.modes  = ADJ_STATUS;
    tx.status = 0;
    if (clock_adjtime(clkid_, &tx) < 0) {
        LOG().Error() << "SysClock::clock_adjtime ADJ_STATUS";
    }
    leapBit_ = tx.status;
#ifdef _SC_CLK_TCK
    /* This is USER_HZ in the kernel. */
    realtimeHz_ = sysconf(_SC_CLK_TCK);
    if (realtimeHz_ > 0) {
        /* This is TICK_USEC in the kernel. */
        realtimeNominalTick_ = (kTickFreHzParameter + realtimeHz_ / 2) / realtimeHz_;
    }
#endif
    std::int32_t const maxAdj{_getSysMaxadj(clkid_)};
    if (0 == maxAdj) {
        LOG().Error() << "_getSysMaxadj error,  ";
        return ara::tsync::internal::kRET_E6;
    }
    LOG().Verbose() << "SysClock::Open"
                    << ", maxAdj=" << maxAdj;
    adjObj_ = std::make_shared< SysClockAdj >();
    if (nullptr != adjObj_) {
        if (0 != adjObj_->InitClockAdj(clkid_, maxAdj)) {
            adjObj_ = nullptr;
        }
    }

    if (nullptr == adjObj_) {
        LOG().Error() << "failed to InitSysAdj ";
        return ara::tsync::internal::kRET_E7;
    }
    return 0;
}

/// @brief adjust system clock
/// @param offset - deviation value
/// @param ts - clock value
/// @return <0 indicates failure.
std::int32_t SysClock::AdjustClock(int64_t offset, int64_t ts) noexcept { return adjObj_->AdjustClock(offset, ts); }

/// @brief set clock
/// @param value  - nano-second
/// @return <0 indicates failure.
std::int32_t SysClock::InitSysClockValue(int64_t value) const noexcept
{
    timespec tsReal{};
    tsReal.tv_sec  = value / kTS_NSPERSEC;
    tsReal.tv_nsec = value % kTS_NSPERSEC;
    LOG().Verbose() << "SysClock::InitSysClockValue:" << value;

    std::int32_t err{clock_settime(clkid_, &tsReal)};
    if (0 != err) {
        LOG().Error() << "SysClock::InitSysClockValue, "
                      << ",clock_gettime or clock_settime error:" << ara::tsync::internal::GetErrString()
                      << ", tsReal.tv_sec=" << value;
    }
    return err;
}

/// @brief query the maximum frequency adjustment of the PTP hardware clock.
/// @param clockId clock id.
/// @return maximum frequency adjustment, 0 on failure
std::int32_t SysClock::_getSysMaxadj(clockid_t const clockId) const noexcept
{
    std::int32_t f{0};
    struct timex tx
    {
    };

    memset(&tx, 0, sizeof(tx));
    if (clock_adjtime(clockId, &tx) < 0) {
        LOG().Error() << "SysClock::_getSysMaxadj, failed to read out the clock maximum adjustment";
    } else {
        f = static_cast< std::int32_t >(static_cast< double >(tx.tolerance) / kFreqScaled);
    }

    if (0 == f) {
        f = kDefaultSysMaxadj;
    }

    /* The kernel allows the tick length to be adjusted up to 10%. But use
	 * it only if the overall frequency of the clock can be adjusted
	 * continuously with the tick and freq fields (i.e. hz <= 1000).
	 */
    if ((0 != realtimeNominalTick_) && 2 * f >= kRealtimeHzScaled * realtimeHz_) {
        f = static_cast< std::int32_t >(realtimeNominalTick_ / kRealtimeHzParameter * kRealtimeHzScaled * realtimeHz_);
    }

    return f;
}

}  // namespace network
}  // namespace osi
}  // namespace isoft
