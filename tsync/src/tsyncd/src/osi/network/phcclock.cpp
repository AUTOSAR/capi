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
/// @file       phcclock.cpp
/// @brief      OSI network module raw socket class
/// @details
/// @date       2023-01-09
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#include "isoft/osi/network/phcclock.h"

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

/// @brief destructor
PhcClock::~PhcClock() noexcept { _close(); }
/// @brief destructor
void PhcClock::_close() noexcept
{
    if (nullptr != adjObj_) {
        adjObj_ = nullptr;
    }
    if (-1 != clkid_) {
        ClosePhcClock(clkid_);
        clkid_ = -1;
    }
}
/// @brief open phc clock
/// @param deviceName - network device name
/// @return <0 indicates failure.
std::int32_t PhcClock::Open(std::string const &deviceName) noexcept
{
    if (deviceName.empty()) {
        return ara::tsync::internal::kRET_E1;
    }
    ifName_ = deviceName;
    std::int32_t const phcIndex{GetPhcIndex(ifName_.c_str())};
    if (0 > phcIndex) {
        LOG().Error() << ifName_.c_str() << ", GetPhcIndex: " << ara::tsync::internal::GetErrString();
        return ara::tsync::internal::kRET_E2;
    }
    std::stringstream ss;
    ss << "/dev/ptp";
    ss << phcIndex;
    ptpName_ = ss.str();
    LOG().Verbose() << ifName_.c_str() << ", GetPhcIndex ptpName_= " << ptpName_.c_str();

    clkid_ = _openPhcClock(ptpName_.c_str());
    if (-1 == clkid_) {
        LOG().Error() << "cant open " << ptpName_.c_str() << ", for : " << ifName_.c_str();
        return ara::tsync::internal::kRET_E5;
    }
    std::int32_t const maxAdj{_getPhcMaxadj(clkid_)};
    if (0 == maxAdj) {
        LOG().Error() << "_getPhcMaxadj error,  " << ptpName_.c_str() << ", for : " << ifName_.c_str();
        return ara::tsync::internal::kRET_E6;
    }
    adjObj_ = std::make_shared< PhcClockAdj >();
    if (nullptr != adjObj_) {
        if (0 != adjObj_->InitClockAdj(clkid_, maxAdj)) {
            adjObj_ = nullptr;
        }
    }

    if (nullptr == adjObj_) {
        LOG().Error() << "failed to InitClockAdj " << ifName_.c_str();
        return ara::tsync::internal::kRET_E7;
    }
    return 0;
}

/// @brief adjust phc clock
/// @param offset - deviation value
/// @param ts - clock value
/// @return <0 indicates failure.
std::int32_t PhcClock::AdjustClock(int64_t offset, int64_t ts) noexcept { return adjObj_->AdjustClock(offset, ts); }

/// @brief measure the deviation between phc clock and system clock
/// @param offset - deviation value
/// @param delay - delay
/// @param ts - phc clock
/// @return <0 indicates failure.
std::int32_t PhcClock::GetClockSysOffset(int64_t &offset, int64_t &delay, int64_t &ts) noexcept
{
    return adjObj_->GetClockSysOffset(offset, delay, ts);
}

/// @brief set clock
/// @param value  - nano-second
/// @return <0 indicates failure.
std::int32_t PhcClock::InitPhcClockValue(int64_t value) noexcept
{
    timespec tsReal{};
    tsReal.tv_sec  = (value + kUTC_TAI_OFFSET_NS) / kTS_NSPERSEC;
    tsReal.tv_nsec = value % kTS_NSPERSEC;
    std::int32_t err{clock_settime(clkid_, &tsReal)};
    if (0 != err) {
        LOG().Error() << "PhcClock::InitPhcClockValue, " << ifName_
                      << ",clock_gettime or clock_settime error:" << ara::tsync::internal::GetErrString();
    }
    return err;
}

/// @brief get ptp clock index of the specified network card
/// @param name - network device name
/// @return ptp clock index <0 - failure
std::int32_t PhcClock::GetPhcIndex(ara::tsync::internal::char8_t const *const name) noexcept
{
    ethtool_ts_info info{};
    ifreq ifr{};
    std::int32_t fd{0};
    std::int32_t err{0};
    std::int32_t const phcIndex{-1};
    std::ignore  = memset(&ifr, 0, sizeof(ifr));
    std::ignore  = memset(&info, 0, sizeof(info));
    info.cmd     = static_cast< std::uint32_t >(ETHTOOL_GET_TS_INFO);
    std::ignore  = strncpy(static_cast< ara::tsync::internal::char8_t * >(&ifr.ifr_name[0]), name,
                          static_cast< std::size_t >(IFNAMSIZ - 1));
    std::ignore  = ifr.ifr_name[0];
    ifr.ifr_data = reinterpret_cast< ara::tsync::internal::char8_t * >(&info);
    fd           = socket(AF_INET, SOCK_DGRAM, 0);

    if (fd < 0) {
        LOG().Error() << "GetPhcIndex socket failed:" << name;
        return phcIndex;
    }

    err         = ioctl(fd, static_cast< std::uint64_t >(SIOCETHTOOL), &ifr);
    std::ignore = close(fd);
    if (err < 0) {
        LOG().Error() << "GetPhcIndex ioctl SIOCETHTOOL failed:" << name;
        return phcIndex;
    }
    return info.phc_index;
}

/// @brief open PTP hardware clock
/// @param phcName PTP device name
/// @return legal clock ID on success, CLOCK_INVALID on failure.
clockid_t PhcClock::_openPhcClock(ara::tsync::internal::char8_t const *const phcName) const noexcept
{
    clockid_t clockId{0};
    timespec ts{};
    timex tx{};
    std::int32_t fd{0};

    std::ignore = memset(&tx, 0, sizeof(tx));

    fd = open(phcName, O_RDWR | O_CLOEXEC);
    if (fd < 0) {
        return kCLOCK_INVALID;
    }

    clockId = FD_TO_CLOCKID(fd);
    /* check if clockId is valid */
    if (0 != clock_gettime(clockId, &ts)) {
        std::ignore = close(fd);
        return kCLOCK_INVALID;
    }
    if (0 != clock_adjtime(clockId, &tx)) {
        std::ignore = close(fd);
        return kCLOCK_INVALID;
    }
    std::ignore = ts;
    std::ignore = tx;
    return clockId;
}

/// @brief close PTP hardware clock
/// @param clockId clock id.
void PhcClock::ClosePhcClock(clockid_t const clockId) noexcept
{
    if (clockId == kCLOCK_INVALID) {
        return;
    }

    std::ignore = close(CLOCKID_TO_FD(clockId));
}

/// @brief get PTP hardware clock capabilities
/// @param clockId clock id.
/// @param caps PTP hardware clock capabilities.
/// @return 0 - success other - failure
std::int32_t PhcClock::_getPhcCaps(clockid_t const clockId, struct ptp_clock_caps *const caps) const noexcept
{
    std::int32_t const fd{CLOCKID_TO_FD(clockId)};
    std::int32_t err{0};

    err = ioctl(fd, static_cast< std::size_t >(PTP_CLOCK_GETCAPS), caps);
    if (0 != err) {
        LOG().Error() << "_getPhcCaps:" << ifName_ << "," << ara::tsync::internal::GetErrString();
    }
    return err;
}

/// @brief query the maximum frequency adjustment of the PTP hardware clock.
/// @param clockId clock id.
/// @return maximum frequency adjustment, 0 on failure
std::int32_t PhcClock::_getPhcMaxadj(clockid_t const clockId) const noexcept
{
    std::int32_t max{0};
    ptp_clock_caps caps{};
    std::ignore = memset(&caps, 0, sizeof(caps));

    if (0 != _getPhcCaps(clockId, &caps)) {
        return 0;
    }

    max = caps.max_adj;

    if ((kBITS_OF_CPU32 == kBITS_OF_CPU32) && (max > kMAX_PPB_32)) {
        max = kMAX_PPB_32;
    }
    return max;
}

}  // namespace network
}  // namespace osi
}  // namespace isoft
