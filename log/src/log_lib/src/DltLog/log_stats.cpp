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
/// @file       log_stats.cpp
/// @brief      No description provided.
/// @details
/// @date       2024-06-28
/// @author     yangjinbiao
/// @version    1.2.0
///
/// ================================================================

#include "log_stats.h"

#include <csignal>
#include <tuple>

#include "logger_api.h"
namespace ara {
namespace log {
namespace internal {

timer_t LogStatistic::s_Timerid_ = nullptr;
LogStatistic::LogStatistic()     = default;

bool LogStatistic::AddMessageSize(std::string const& appId, std::int32_t const& len) noexcept
{
    std::lock_guard< decltype(mMtx_) > lockGd{mMtx_};
    std::map< std::string, std::int32_t >::iterator it = this->mAppLenMap_.find(appId);
    if (it != this->mAppLenMap_.end()) {
        it->second += len;
    } else {
        this->mAppLenMap_.emplace(appId, len);
    }
    return true;
}

bool LogStatistic::PrintTime(std::string const& appId, std::uint32_t const& msgId, std::string const& info) noexcept
{
    std::stringstream stream;
    // note : qac can not be
    char const zeroChar{'0'};
    std::int32_t const kMaxLen{8};
    stream << appId << "_"
           << "0x" << std::setfill(zeroChar) << std::setw(kMaxLen) << std::hex << msgId;
    std::string key                                 = stream.str();
    std::chrono::system_clock::time_point const now = std::chrono::system_clock::now();
    // Convert to millisecond timestamp (Unix timestamp)
    const std::int64_t timestamp
        = std::chrono::duration_cast< std::chrono::milliseconds >(now.time_since_epoch()).count();
    std::cout << "\r\n" << key << "_" << info << "_" << timestamp << std::endl;
    return true;
}

LogStatistic::~LogStatistic() { internal::LogStatistic::GetInstance().StopStatis(); }
bool LogStatistic::InitStatistic() noexcept
{
    struct sigevent sev
    {
    };
    sev.sigev_value.sival_ptr = nullptr;
    sev.sigev_signo           = SIGALRM;
    sev.sigev_notify          = SIGEV_SIGNAL;

    timer_create(CLOCK_REALTIME, &sev, &LogStatistic::s_Timerid_);
    std::int32_t const kTime{10};
    struct itimerspec its
    {
        {kTime, 0}, { kTime, 0 }
    };
    std::ignore = signal(SIGALRM, StatisticTimerCallback);
    timer_settime(LogStatistic::s_Timerid_, 0, &its, nullptr);

    return false;
}

bool LogStatistic::DeIinitStatistic() noexcept
{
    if (LogStatistic::s_Timerid_ != nullptr) {
        timer_delete(LogStatistic::s_Timerid_);
    }
    internal::LogStatistic::GetInstance().StopStatis();

    return true;
}

bool LogStatistic::StartStatis() noexcept
{
    InitStatistic();
    isStatis_ = true;
    return true;
}
bool LogStatistic::StopStatis() noexcept
{
    isStatis_ = false;
    return true;
}
bool LogStatistic::IsStatis() const noexcept { return isStatis_; }

void LogStatistic::StatisticTimerCallback(std::int32_t sigNum) noexcept
{
    std::ignore = sigNum;
    LogStatistic::GetInstance().OutputStaitc();
    return;
}

bool LogStatistic::OutputStaitc() noexcept
{
    // std::lock_guard< std::mutex > lockGd(mMtx_);
    if (this->isStatis_) {
        // Use a range-based for loop
        std::cout << "-------messageid--log statis-----------" << std::endl;
        for (std::map< std::string, std::uint32_t >::const_reference pair : this->mMsgIDMap_) {
            std::cout << "Key: " << pair.first << ", Value: " << pair.second << std::endl;
        }
        std::cout << "-------------appid-all-message-count-------------" << std::endl;
        for (std::map< std::string, std::int32_t >::const_reference pair : this->mAppCountMap_) {
            std::cout << "Key: " << pair.first << ", Value: " << pair.second << std::endl;
        }
        std::cout << "-------------appid-all-message-len-------------" << std::endl;
        for (std::map< std::string, std::int32_t >::const_reference pair : this->mAppLenMap_) {
            std::cout << "Key: " << pair.first << ", Value: " << pair.second << std::endl;
        }
    }

    return true;
}

bool LogStatistic::AddMessageId(std::string const& appId, std::uint32_t const& msgId) noexcept
{
    std::stringstream stream;
    // note : qac can not be
    char const zeroChar{'0'};
    std::int32_t const kMaxLen{8};
    stream << appId << "_"
           << "0x" << std::setfill(zeroChar) << std::setw(kMaxLen) << std::hex << msgId;
    std::string key = stream.str();

    // const std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    // Convert to millisecond timestamp (Unix timestamp)
    // const std::int64_t timestamp =
    //     std::chrono::duration_cast< std::chrono::milliseconds >(now.time_since_epoch()).count();
    // std::cout << key << "_lib_time_callstart_" << timestamp << std::endl;

    {
        std::lock_guard< decltype(mMtx_) > lockGd{mMtx_};
        {
            std::map< std::string, std::uint32_t >::iterator it = this->mMsgIDMap_.find(key);
            if (it != this->mMsgIDMap_.end()) {
                ++it->second;
            } else {
                this->mMsgIDMap_.emplace(key, 1);
            }
        }

        {
            std::map< std::string, std::int32_t >::iterator it = this->mAppCountMap_.find(appId);
            if (it != this->mAppCountMap_.end()) {
                ++it->second;
            } else {
                this->mAppCountMap_.emplace(appId, 1);
            }
        }
    }

    return true;
}
}  // namespace internal
}  // namespace log
}  // namespace ara
