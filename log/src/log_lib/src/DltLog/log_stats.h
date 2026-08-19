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
/// @file       log_stats.h
/// @brief      Log statistics class, for internal debugging
/// @details
/// @date       2024-06-28
/// @author     yangjinbiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/LOG
/// @interface_level = module
/// @trace_id_sr=LOG_SR_00001,LOG_SR_00002,LOG_SR_00003,LOG_SR_00004,LOG_SR_00006,LOG_SR_00010,LOG_SR_00011,LOG_SR_00012,LOG_SR_00013,LOG_SR_00014
/// @unit_name = log_stats
/// @unit_description=Log statistics class, for internal debugging
/// @endcode
///
/// ================================================================

#ifndef __INTERNAL_LOG_STATICSTIC_LOGANDTRACE__
#define __INTERNAL_LOG_STATICSTIC_LOGANDTRACE__

#include <csignal>
#include <iostream>
#include <map>
#include <mutex>
#include <string>

namespace ara {
namespace log {
namespace internal {

class LogStatistic
{
    using SessionMap  = std::map< std::int32_t, std::int32_t >;
    using Context_Map = std::map< std::string, std::int32_t >;

private:
    /* data */
public:
    LogStatistic(/* args */);
    LogStatistic(const LogStatistic&) = delete;
    LogStatistic& operator=(const LogStatistic&) = delete;

    // Disable move construction and move assignment
    LogStatistic(LogStatistic&&) = delete;
    LogStatistic& operator=(LogStatistic&&) = delete;

    /// @brief
    /// @return
    bool InitStatistic() noexcept;

    /// @brief
    bool StartStatis() noexcept;

    /// @brief
    bool StopStatis() noexcept;

    /// @brief
    bool IsStatis() const noexcept;

    /// @brief
    /// @return
    static LogStatistic& GetInstance()
    {
        static LogStatistic s_Instance;
        return s_Instance;
    }
    /// @brief
    /// @return
    bool DeIinitStatistic() noexcept;

    /// @brief
    /// @return
    static void StatisticTimerCallback(std::int32_t sigNum) noexcept;
    /// @brief
    /// @return
    bool OutputStaitc() noexcept;

    /// @brief
    /// @param appId
    /// @param msgId
    /// @return
    bool AddMessageId(std::string const& appId, std::uint32_t const& msgId) noexcept;

    /// @brief
    /// @param appId
    /// @param len
    /// @return
    bool AddMessageSize(std::string const& appId, std::int32_t const& len) noexcept;

    /// @brief
    /// @param appId
    /// @param msgId
    /// @return
    bool PrintTime(std::string const& appId, std::uint32_t const& msgId, std::string const& info) noexcept;
    /// @brief
    ~LogStatistic();

private:
    static timer_t s_Timerid_;
    bool isStatis_{false};
    std::mutex mMtx_;
    std::map< std::string, std::int32_t > mAppCountMap_;
    std::map< std::string, std::uint32_t > mMsgIDMap_;
    std::map< std::string, std::int32_t > mAppLenMap_;
};

}  // namespace internal
}  // namespace log
}  // namespace ara
#endif