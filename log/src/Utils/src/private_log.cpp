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
/// @file       private_log.cpp
/// @brief      Template class for encapsulating log parameters
/// @details
/// @date       2024-06-28
/// @author     yangjinbiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/LOG/Utils
/// @interface_level = unit
/// @trace_id_sr=LOG_SR_00006,LOG_SR_00015
/// @unit_name = private_log
/// @unit_description=Internal support function of Dlt lib, utility class, used for internal log debugging
/// @endcode
///
/// ================================================================

#include "private_log.h"

#include <sys/time.h>

#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#ifdef __android__
    #include <android/log.h>
#endif

namespace ara {
namespace log {
namespace internal {
PriLogLevelType PrivateLogger::s_Log_Level_{PriLogLevelType::kDlt_Log_Off};
std::string PrivateLogger::s_Appid_;
/// @brief

/// @brief
PrivateLogger::~PrivateLogger() noexcept { End(); }
/// @brief
/// @return
std::shared_ptr< PrivateLogger > ara::log::internal::PrivateLogger::Instance() noexcept
{
    return std::make_shared< PrivateLogger >();
}
/// @brief
/// @param[in]  level
/// @param[in]  msg
/// @return
PrivateLogger &PrivateLogger::Inter_Log(PriLogLevelType const &level, std::string const &msg) noexcept
{
    lineLogLeve_ = level;
    if (CanLog()) {
        std::chrono::system_clock::time_point const now{std::chrono::system_clock::now()};
        // Get timestamp
        std::chrono::system_clock::duration const remindeTime{
            std::chrono::duration_cast< std::chrono::microseconds >(now.time_since_epoch()) % 1000000};
        // Convert to time_t
        std::time_t const tt{std::chrono::system_clock::to_time_t(now)};
        // Format output
        std::int32_t const int6{6};
        std::int32_t const int1000{1000};
        std::ostringstream oss;
        char const zeroChar{'0'};
        std::tm tmBuf{};
        std::tm *tmPtr = localtime_r(&tt, &tmBuf);
        oss << std::put_time(tmPtr, "%Y/%m/%d %H:%M:%S") << "." << std::setw(int6) << std::setfill(zeroChar)
            << remindeTime.count() / int1000 << " " << PrivateLogger::s_Appid_ << " #LLB "
            << " [ ";

        // std::ignore = msg_.append();
        std::ignore = msg_.append(oss.str());
        std::ignore = msg_.append(msg);
    }
    return *this;
}
/// @brief
/// @param[in]  level
/// @param[in]  msg
/// @return
PrivateLogger &PrivateLogger::Inter_Log(PriLogLevelType const &level, std::int64_t const &msg) noexcept
{
    lineLogLeve_ = level;
    if (CanLog()) {
        std::ignore = msg_.append(std::to_string(msg));
    }
    return *this;
}
/// @brief
/// @param[in]  level
/// @param[in]  msg1
/// @param[in]  msg2
/// @return
PrivateLogger &PrivateLogger::Inter_Log(PriLogLevelType const &level,
                                        std::string const &msg1,
                                        std::string const &msg2) noexcept
{
    lineLogLeve_ = level;
    if (CanLog()) {
        std::cout << msg1 << msg2 << std::endl;
    }
    return *this;
}
/// @brief
/// @param[in]  msg
/// @return
PrivateLogger &PrivateLogger::Inter_Log(std::string const &msg) noexcept
{
    if (CanLog()) {
        std::ignore = msg_.append(msg);
#ifdef __android__
        __android_log_print(ANDROID_LOG_INFO, "LOGTAG", msg.c_str());
#endif
    }
    return *this;
}

/// @brief
void PrivateLogger::End() noexcept
{
    if (CanLog()) {
        std::cout << msg_ << " ]  " << std::endl;
#ifdef __android__
        msg_.append(" ]  ");
        __android_log_print(ANDROID_LOG_INFO, "LOGTAG", msg_.c_str());
#endif
    }
    msg_.clear();
}
/// @brief
/// @param[in]  value
/// @return
PrivateLogger &ara::log::internal::PrivateLogger::operator<<(std::string const &value) noexcept
{
    if (CanLog()) {
        std::ignore = msg_.append(" ");
        std::ignore = msg_.append(value);
    }
    return *this;
}
/// @brief
/// @param[in]  value
/// @return
PrivateLogger &PrivateLogger::operator<<(char const *value) noexcept
{
    if (CanLog()) {
        std::ignore = msg_.append(value);
    }

    return *this;
}
/// @brief
/// @param[in]  value
/// @return
PrivateLogger &PrivateLogger::operator<<(std::int32_t const &value) noexcept
{
    if (CanLog()) {
        std::ignore = msg_.append(std::to_string(value));
    }

    return *this;
}
PrivateLogger &PrivateLogger::operator<<(std::uint32_t const &value) noexcept
{
    if (CanLog()) {
        std::ignore = msg_.append(std::to_string(value));
    }

    return *this;
}
/// @brief
/// @param[in]  value
/// @return
PrivateLogger &PrivateLogger::operator<<(std::uint64_t const &value) noexcept
{
    if (CanLog()) {
        std::ignore = msg_.append(std::to_string(value));
    }

    return *this;
}
/// @brief
/// @param[in]  value
/// @return
PrivateLogger &PrivateLogger::operator<<(std::int64_t const &value) noexcept
{
    if (CanLog()) {
        std::ignore = msg_.append(std::to_string(value));
    }

    return *this;
}
/// @brief
/// @return
bool PrivateLogger::CanLog() const noexcept
{
    if ((lineLogLeve_ > PriLogLevelType::kDlt_Log_Off) && (lineLogLeve_ <= PrivateLogger::s_Log_Level_)) {
        return true;
    }
    return false;
}
/// @brief
/// @param[in]  loglevel
bool PrivateLogger::SetPrivateLogLevel(PriLogLevelType const &loglevel) noexcept
{
    PrivateLogger::s_Log_Level_ = loglevel;
    return true;
}
bool PrivateLogger::Set_Appid(std::string const &appId) noexcept
{
    PrivateLogger::s_Appid_ = appId;
    return true;
}
}  // namespace internal
}  // namespace log
}  // namespace ara
