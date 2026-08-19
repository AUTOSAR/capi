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
/// @file       stream.h
/// @brief      time synchronization internal log stream definition
/// @details
/// @date       2023-02-09
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// export_level=/TimeSync/LogWrapper
/// module_path=/TimeSync/LogWrapper
/// @endcode
///
/// ================================================================

#ifndef ARA_TSYNC_INTERNAL_LOG_STREAM_H_
#define ARA_TSYNC_INTERNAL_LOG_STREAM_H_

#include <ara/core/string.h>
#include <ara/core/string_view.h>

#include <cstdint>
#include <functional>

#include "ara/tsync/internal/common.h"
#include "ara/tsync/internal/log/defines.h"

namespace ara {
namespace tsync {
namespace internal {
namespace tslog {

/// @brief log stream definition
class Stream final
{
public:
    /// @brief constructor with parameters
    /// @param logAttr - log attributes
    /// @param cb - output callback function
    Stream(LogAttribute logAttr, OutputHandler cb) noexcept
        : logAttribute_{std::move(logAttr)}, logOutputCb_{std::move(cb)}, logContent_{}, bUsed_{true}
    {
    }

    /// @brief default constructor
    Stream() = delete;

    /// @brief copy constructor
    /// @param other - other object
    Stream(Stream const& other) = delete;

    /// @brief move constructor is prohibited
    /// @param other - other object
    Stream(Stream&& other) noexcept
    {
        *this        = std::move(other);
        bUsed_       = true;
        other.bUsed_ = false;
    }

    /// @brief copy assignment
    /// @param other
    /// @return reference to self
    Stream& operator=(Stream const& other) = delete;

    /// @brief move assignment is prohibited
    /// @param other
    /// @exception
    /// @return reference to self
    Stream& operator=(Stream&& other) = default;

    /// @brief destructor
    ~Stream() noexcept
    {
        if (bUsed_) {
            ara::core::StringView const svContent{logContent_.c_str(), logContent_.length()};
            if ((nullptr != logOutputCb_) && (true == logAttribute_.UseAraLog())) {
                logOutputCb_(logAttribute_, svContent);
            } else {
                Output2Console(logAttribute_, svContent);
            }
        }
    }

    /// @brief operator overload, print string
    /// @param msg - message
    /// @return class reference
    Stream& operator<<(ara::core::String const& msg) noexcept
    {
        logContent_ += msg;
        return *this;
    }

    /// @brief operator overload, print string view
    /// @param msg - message
    /// @return class reference
    Stream& operator<<(ara::core::StringView const& msg) noexcept
    {
        ara::core::String const s{msg.data(), msg.size()};
        logContent_ += s;
        return *this;
    }

    /// @brief operator overload, print std::string
    /// @param msg - message
    /// @return class reference
    Stream& operator<<(std::string const& msg) noexcept
    {
        logContent_ += msg.c_str();
        return *this;
    }

    /// @brief operator overload, print C string
    /// @param msg - message
    /// @return class reference
    Stream& operator<<(char8_t const* const msg) noexcept
    {
        logContent_ += msg;
        return *this;
    }

    /// @brief operator overload, print bool
    /// @param value - message
    /// @return class reference
    Stream& operator<<(bool const value) noexcept
    {
        std::uint32_t const val{static_cast< std::uint32_t >(value)};
        logContent_ += ara::core::to_string(val);
        return *this;
    }

    /// @brief operator overload, print uint8_t
    /// @param value - message
    /// @return class reference
    Stream& operator<<(std::uint8_t const value) noexcept
    {
        std::uint32_t const val{value};
        logContent_ += ara::core::to_string(val);
        return *this;
    }

    /// @brief operator overload, print uint16_t
    /// @param value - message
    /// @return class reference
    Stream& operator<<(std::uint16_t const value) noexcept
    {
        std::uint32_t const val{value};
        logContent_ += ara::core::to_string(val);
        return *this;
    }

    /// @brief operator overload, print uint32_t
    /// @param value - message
    /// @return class reference
    Stream& operator<<(std::uint32_t const value) noexcept
    {
        logContent_ += ara::core::to_string(value);
        return *this;
    }

    /// @brief operator overload, print uint64_t
    /// @param value - message
    /// @return class reference
    Stream& operator<<(std::uint64_t const value) noexcept
    {
        logContent_ += ara::core::to_string(value);
        return *this;
    }

    /// @brief operator overload, print int32_t
    /// @param value - message
    /// @return class reference
    Stream& operator<<(std::int32_t const value) noexcept
    {
        logContent_ += ara::core::to_string(value);
        return *this;
    }

    /// @brief operator overload, print int64_t
    /// @param value - message
    /// @return class reference
    Stream& operator<<(std::int64_t const value) noexcept
    {
        logContent_ += ara::core::to_string(value);
        return *this;
    }

    /// @brief operator overload, print double
    /// @param value - message
    /// @return class reference
    Stream& operator<<(double const value) noexcept
    {
        logContent_ += ara::core::to_string(value);
        return *this;
    }
    /// @brief operator overload, print address
    /// @param msg - message
    /// @return class reference
    Stream& operator<<(const void* const msg) noexcept
    {
        logContent_ += ara::core::to_string(reinterpret_cast< std::uintptr_t >(msg));
        return *this;
    }

private:
    /// @brief terminal print
    /// @param logAttr - log attributes
    /// @param logMesg - log content
    static void Output2Console(LogAttribute const& logAttr, ara::core::StringView const& logMesg) noexcept
    {
        switch (logAttr.GetLogLevel()) {
            case LogLevel::kFatal: {
                std::cout << "TSYN[Fatal]: " << logMesg << std::endl;
                break;
            }
            case LogLevel::kError: {
                std::cout << "TSYN[Error]: " << logMesg << std::endl;
                break;
            }
            case LogLevel::kWarn: {
                std::cout << "TSYN[Warn]: " << logMesg << std::endl;
                break;
            }
            case LogLevel::kInfo: {
                std::cout << "TSYN[Info]: " << logMesg << std::endl;
                break;
            }
            case LogLevel::kDebug: {
                std::cout << "TSYN[Debug]: " << logMesg << std::endl;
                break;
            }
            case LogLevel::kVerbose: {
                std::cout << "TSYN[Verbose]: " << logMesg << std::endl;
                break;
            }
            default: {
                break;
            }
        }
    }

private:
    /// @name logAttribute_ - log attributes
    LogAttribute logAttribute_;

    /// @name logOutputCb_ - log output callback function
    OutputHandler logOutputCb_;

    /// @name logContent_ - log content
    ara::core::String logContent_;

    /// @name bUsed_ - whether in use
    bool bUsed_;
};

}  // namespace tslog
}  // namespace internal
}  // namespace tsync
}  // namespace ara

#endif  /// ARA_TSYNC_INTERNAL_LOG_STREAM_H_