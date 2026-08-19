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
/// @file       log_stream.h
/// @brief      Template class for encapsulating log parameters
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
/// @unit_name = log_stream
/// @unit_description=Lib interface side of Dlt, providing application interfaces externally for users
/// @endcode
///
/// ================================================================

#ifndef LOG_STREAM_H
#define LOG_STREAM_H

#include <chrono>
#include <iomanip>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

#include "ara/core/error_code.h"
#include "ara/core/instance_specifier.h"
#include "ara/core/span.h"
#include "argument.h"
#include "common.h"
#include "log_api_define_v1.h"

namespace ara {
namespace log {
class DltLogger;
class Logger;

namespace internal {
namespace tests {
class LogStreamDirectTest;

}  // namespace tests
class DltPlainEncoder;
class DltProtocolEncoder;
class DeterminedExecutor;
struct LogStreamContext;
using LogStreamContextPtr = std::shared_ptr< LogStreamContext >;

}  // namespace internal

/*  
/// @brief Actual log printing class used to combine multiple inputs
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_LOG_00025
/// @trace_id_dd=DD_LOG_00025
/// @needwork =  
/// @endcode 
*/
class LogStream
{
public:
    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00026
    /// @trace_id_dd=DD_LOG_00026
    /// @needwork = ad
    /// @endcode
    ~LogStream() noexcept;

    /// @brief Move copy
    /// @param[in]  src Original LogStream
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00027
    /// @trace_id_dd=DD_LOG_00027
    /// @needwork = ad
    /// @endcode
    LogStream(LogStream &&src) noexcept;

    /// @brief Flush the buffer.
    /// This method ends the current log message and creates a new log message after pushing the buffer to the relevant handler.
    /// @code{.isoft}
    /// @uptrace={SWS_LOG_00002}
    /// @uptrace={SWS_LOG_00039}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00028
    /// @trace_id_dd=DD_LOG_00028
    /// @needwork = ad
    /// @endcode
    void Flush() noexcept;

    void FlushAndDestroy() noexcept;

    /// @brief Append data of type bool to a log message.
    ///
    /// @param[in]  value The data to be appended to the LogStream
    /// @return The LogStream object that has been appended with value
    ///
    /// @code{.isoft}
    ///
    /// @uptrace={SWS_LOG_00002}
    /// @uptrace={SWS_LOG_00040}
    ///
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00029
    /// @trace_id_dd=DD_LOG_00029
    /// @needwork = ad
    /// @endcode
    LogStream &operator<<(bool value) noexcept;

    /// @brief Append data of type uint8_t to a log message.
    ///
    /// @param[in]  value The data to be appended to the LogStream
    /// @return The LogStream object that has been appended with value
    ///
    /// @code{.isoft}
    ///
    /// @uptrace={SWS_LOG_00002}
    /// @uptrace={SWS_LOG_00041}
    ///
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00030
    /// @trace_id_dd=DD_LOG_00030
    /// @needwork = ad
    /// @endcode
    LogStream &operator<<(std::uint8_t value) noexcept;

    /// @brief Append data of type uint16_t to a log message.
    ///
    /// @param[in]  value The data to be appended to the LogStream
    /// @return The LogStream object that has been appended with value
    ///
    /// @code{.isoft}
    ///
    /// @uptrace={SWS_LOG_00002}
    /// @uptrace={SWS_LOG_00042}
    ///
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00031
    /// @trace_id_dd=DD_LOG_00031
    /// @needwork = ad
    /// @endcode
    LogStream &operator<<(std::uint16_t value) noexcept;

    /// @brief Append data of type uint32_t to a log message.
    ///
    /// @param[in]  value The data to be appended to the LogStream
    /// @return The LogStream object that has been appended with value
    ///
    /// @code{.isoft}
    ///
    /// @uptrace={SWS_LOG_00002}
    /// @uptrace={SWS_LOG_00043}
    ///
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00032
    /// @trace_id_dd=DD_LOG_00032
    /// @needwork = ad
    /// @endcode
    LogStream &operator<<(std::uint32_t value) noexcept;

    /// @brief Append data of type uint64_t to a log message.
    ///
    /// @param[in]  value The data to be appended to the LogStream
    /// @return The LogStream object that has been appended with value
    ///
    /// @code{.isoft}
    ///
    /// @uptrace={SWS_LOG_00002}
    /// @uptrace={SWS_LOG_00044}
    ///
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00033
    /// @trace_id_dd=DD_LOG_00033
    /// @needwork = ad
    /// @endcode
    LogStream &operator<<(std::uint64_t value) noexcept;

    /// @brief Append data of type int8_t to a log message.
    ///
    /// @param[in]  value The data to be appended to the LogStream
    /// @return The LogStream object that has been appended with value
    ///
    /// @code{.isoft}
    ///
    /// @uptrace={SWS_LOG_00002}
    /// @uptrace={SWS_LOG_00045}
    ///
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00034
    /// @trace_id_dd=DD_LOG_00034
    /// @needwork = ad
    /// @endcode
    LogStream &operator<<(std::int8_t value) noexcept;

    /// @brief Append data of type int16_t to a log message.
    ///
    /// @param[in]  value The data to be appended to the LogStream
    /// @return The LogStream object that has been appended with value
    ///
    /// @code{.isoft}
    ///
    /// @uptrace={SWS_LOG_00002}
    /// @uptrace={SWS_LOG_00046}
    ///
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00035
    /// @trace_id_dd=DD_LOG_00035
    /// @needwork = ad
    /// @endcode
    LogStream &operator<<(std::int16_t value) noexcept;

    /// @brief Append data of type int32_t to a log message.
    ///
    /// @param[in]  value The data to be appended to the LogStream
    /// @return The LogStream object that has been appended with value
    ///
    /// @code{.isoft}
    ///
    /// @uptrace={SWS_LOG_00002}
    /// @uptrace={SWS_LOG_00047}
    ///
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00036
    /// @trace_id_dd=DD_LOG_00036
    /// @needwork = ad
    /// @endcode
    LogStream &operator<<(std::int32_t value) noexcept;

    /// @brief Append data of type int64_t to a log message.
    ///
    /// @param[in]  value The data to be appended to the LogStream
    /// @return The LogStream object that has been appended with value
    ///
    /// @code{.isoft}
    ///
    /// @uptrace={SWS_LOG_00002}
    /// @uptrace={SWS_LOG_00048}
    ///
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00037
    /// @trace_id_dd=DD_LOG_00037
    /// @needwork = ad
    /// @endcode
    LogStream &operator<<(std::int64_t value) noexcept;

    /// @brief Append data of type float to a log message.
    ///
    /// @param[in]  value The data to be appended to the LogStream
    /// @return The LogStream object that has been appended with value
    ///
    /// @code{.isoft}
    ///
    /// @uptrace={SWS_LOG_00002}
    /// @uptrace={SWS_LOG_00049}
    ///
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00038
    /// @trace_id_dd=DD_LOG_00038
    /// @needwork = ad
    /// @endcode
    LogStream &operator<<(float value) noexcept;

    /// @brief Append data of type double to a log message.
    ///
    /// @param[in]  value The data to be appended to the LogStream
    /// @return The LogStream object that has been appended with value
    ///
    /// @code{.isoft}
    ///
    /// @uptrace={SWS_LOG_00002}
    /// @uptrace={SWS_LOG_00050}
    ///
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00039
    /// @trace_id_dd=DD_LOG_00039
    /// @needwork = ad
    /// @endcode
    LogStream &operator<<(double value) noexcept;

    /// @brief Append data of type LogHex8 to a log message.
    ///
    /// @param[in]  value The data to be appended to the LogStream
    /// @return The LogStream object that has been appended with value
    ///
    /// @code{.isoft}
    ///
    /// @uptrace={SWS_LOG_00002}
    /// @uptrace={SWS_LOG_00053}
    ///
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00040
    /// @trace_id_dd=DD_LOG_00040
    /// @needwork = ad
    /// @endcode
    LogStream &operator<<(LogHex8 const &value) noexcept;

    /// @brief Append data of type LogHex16 to a log message.
    ///
    /// @param[in]  value The data to be appended to the LogStream
    /// @return The LogStream object that has been appended with value
    ///
    /// @code{.isoft}
    ///
    /// @uptrace={SWS_LOG_00002}
    /// @uptrace={SWS_LOG_00054}
    ///
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00041
    /// @trace_id_dd=DD_LOG_00041
    /// @needwork = ad
    /// @endcode
    LogStream &operator<<(LogHex16 const &value) noexcept;

    /// @brief Append data of type LogHex32 to a log message.
    ///
    /// @param[in]  value The data to be appended to the LogStream
    /// @return The LogStream object that has been appended with value
    ///
    /// @code{.isoft}
    ///
    /// @uptrace={SWS_LOG_00002}
    /// @uptrace={SWS_LOG_00055}
    ///
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00042
    /// @trace_id_dd=DD_LOG_00042
    /// @needwork = ad
    /// @endcode
    LogStream &operator<<(LogHex32 const &value) noexcept;

    /// @brief Append data of type LogHex64 to a log message.
    ///
    /// @param[in]  value The data to be appended to the LogStream
    /// @return The LogStream object that has been appended with value
    ///
    /// @code{.isoft}
    ///
    /// @uptrace={SWS_LOG_00002}
    /// @uptrace={SWS_LOG_00056}
    ///
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00043
    /// @trace_id_dd=DD_LOG_00043
    /// @needwork = ad
    /// @endcode
    LogStream &operator<<(LogHex64 const &value) noexcept;

    /// @brief Append data of type LogBin8 to a log message.
    ///
    /// @param[in]  value The data to be appended to the LogStream
    /// @return The LogStream object that has been appended with value
    ///
    /// @code{.isoft}
    ///
    /// @uptrace={SWS_LOG_00002}
    /// @uptrace={SWS_LOG_00057}
    ///
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00044
    /// @trace_id_dd=DD_LOG_00044
    /// @needwork = ad
    /// @endcode
    LogStream &operator<<(LogBin8 const &value) noexcept;

    /// @brief Append data of type LogBin16 to a log message.
    ///
    /// @param[in]  value The data to be appended to the LogStream
    /// @return The LogStream object that has been appended with value
    ///
    /// @code{.isoft}
    ///
    /// @uptrace={SWS_LOG_00002}
    /// @uptrace={SWS_LOG_00058}
    ///
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00045
    /// @trace_id_dd=DD_LOG_00045
    /// @needwork = ad
    /// @endcode
    LogStream &operator<<(LogBin16 const &value) noexcept;

    /// @brief Append data of type LogBin32 to a log message.
    ///
    /// @param[in]  value The data to be appended to the LogStream
    /// @return The LogStream object that has been appended with value
    ///
    /// @code{.isoft}
    ///
    /// @uptrace={SWS_LOG_00002}
    /// @uptrace={SWS_LOG_00059}
    ///
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00046
    /// @trace_id_dd=DD_LOG_00046
    /// @needwork = ad
    /// @endcode
    LogStream &operator<<(LogBin32 const &value) noexcept;

    /// @brief Append data of type LogBin64 to a log message.
    ///
    /// @param[in]  value The data to be appended to the LogStream
    /// @return The LogStream object that has been appended with value
    ///
    /// @code{.isoft}
    ///
    /// @uptrace={SWS_LOG_00002}
    /// @uptrace={SWS_LOG_00060}
    ///
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00047
    /// @trace_id_dd=DD_LOG_00047
    /// @needwork = ad
    /// @endcode
    LogStream &operator<<(LogBin64 const &value) noexcept;

    /// @brief Append data of type ara::core::StringView to a log message.
    ///
    /// @param[in]  value The data to be appended to the LogStream
    /// @return The LogStream object that has been appended with value
    ///
    /// @code{.isoft}
    ///
    /// @uptrace={SWS_LOG_00002}
    /// @uptrace={SWS_LOG_00062}
    ///
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00048
    /// @trace_id_dd=DD_LOG_00048
    /// @needwork = ad
    /// @endcode
    LogStream &operator<<(ara::core::StringView const &value) noexcept;

    /// @brief Append data of type char* to a log message.
    ///
    /// @param[in]  value The data to be appended to the LogStream
    /// @return The LogStream object that has been appended with value
    ///
    /// @code{.isoft}
    ///
    /// @uptrace={SWS_LOG_00002}
    /// @uptrace={SWS_LOG_00051}
    ///
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00049
    /// @trace_id_dd=DD_LOG_00049
    /// @needwork = ad
    /// @endcode
    LogStream &operator<<(const char *const value) noexcept;

    /// @brief Data array insertion operator
    /// @param[in]  value Data byte vector
    /// @returns Reference to the current log stream
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00050
    /// @trace_id_dd=DD_LOG_00050
    /// @needwork = ad
    /// @endcode
    LogStream &operator<<(std::vector< std::uint8_t > const &value) noexcept;
    /// @brief Append data of type ara::core::Span<const ara::core::Byte> to a log message.
    /// @param[in]  value
    /// @return
    /// @code{.isoft}
    /// @uptrace={SWS_LOG_00002}
    /// @uptrace={SWS_LOG_00128}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00051
    /// @trace_id_dd=DD_LOG_00051
    /// @needwork = ad
    /// @endcode
    LogStream &operator<<(core::Span< core::Byte const > const &value) noexcept;

    /// @brief Log stream at a certian file and a certian line within the file
    /// @param[in]  file File name
    /// @param[in]  line Line number
    /// @returns Reference to the current log stream
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00052
    /// @trace_id_dd=DD_LOG_00052
    /// @needwork = ad
    /// @endcode
    LogStream &WithLocation(ara::core::StringView file, std::int32_t line) noexcept;

    /// @brief Arugment insertion operator
    /// @param[in]  arg An agrgument
    /// @returns Reference to the current log stream
    /// @code{.isoft}
    /// @tparam T Argument playload type
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00053
    /// @trace_id_dd=DD_LOG_00053
    /// @needwork = ad
    /// @endcode
    template < typename T >
    LogStream &operator<<(Argument< T > const &arg)
    {
        *this << arg.name << " " << arg.arg << " " << arg.unit;
        return *this;
    }

    /// @brief
    /// @param[in]  value
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00054
    /// @trace_id_dd=DD_LOG_00054
    /// @needwork = ad
    /// @endcode
    LogStream &operator<<(std::string const &value) noexcept;

    /// @brief Copy constructor

    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00025
    /// @trace_id_dd=DD_LOG_00055
    /// @needwork = dda
    /// @endcode
    LogStream(LogStream const &other) = delete;
    /// @brief Copy assignment

    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00025
    /// @trace_id_dd=DD_LOG_00056
    /// @needwork = dda
    /// @endcode
    LogStream &operator=(LogStream const &other) = delete;
    /// @brief Move assignment

    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00025
    /// @trace_id_dd=DD_LOG_00057
    /// @needwork = dda
    /// @endcode
    LogStream &operator=(LogStream &&other) = delete;

    /// ####################ext###########################
    /// @brief Get the current message timestamp
    std::uint64_t ExtGetTimeStamp() noexcept;
    /// @brief Set the current message timestamp, requires microseconds
    /// @param timeStamp Timestamp
    /// @return true setting succeeded
    /// @return false setting failed
    void ExtSetTimeStamp(std::uint64_t seconds, std::uint64_t useconds) noexcept;

private:
    friend class ara::log::DltLogger;
    friend class Logger;
    friend class ara::log::internal::tests::LogStreamDirectTest;
    /// @brief Custom constructor
    /// @param[in]  logLevel Log level
    /// @param[in]  logCtx LogStream input context (built by Logger)
    /// @needwork = dda
    explicit LogStream(LogLevel logLevel, internal::LogStreamContextPtr logCtx) noexcept;

    void _CreateDltEncoder() noexcept;
    void _FlushDetermined() noexcept;
    void _FlushNonDetermined() noexcept;
    /// @brief
    /// @param[in]  decimal
    /// @return
    /// @code{.isoft}
    /// @tparam T
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00025
    /// @trace_id_dd=DD_LOG_00059
    /// @needwork = dda
    /// @endcode
    template < typename T >
    std::string _decimalToHexString(T const &decimal)
    {
        using UnsignedT = typename std::make_unsigned< T >::type;

        constexpr std::size_t kHexDigits = sizeof(T) * 2U;
        constexpr char kHexMap[]         = "0123456789abcdef";

        UnsignedT const value = static_cast< UnsignedT >(decimal);
        std::string result(2U + kHexDigits, '0');
        result[0] = '0';
        result[1] = 'x';

        for (std::size_t i = 0U; i < kHexDigits; ++i) {
            std::size_t const shift = (kHexDigits - 1U - i) * 4U;
            result[2U + i]          = kHexMap[(value >> shift) & static_cast< UnsignedT >(0xFU)];  // NOLINT
        }

        return result;
    }

    /// @brief
    /// @param[in]  decimal
    /// @return
    /// @code{.isoft}
    /// @tparam T
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00025
    /// @trace_id_dd=DD_LOG_00060
    /// @needwork = dda
    /// @endcode
    template < typename T >
    std::string _decimalToBinaryString(T const &decimal)
    {
        // Convert signed types to unsigned types to avoid sign extension issues with negative right shifts
        using UnsignedT                 = typename std::make_unsigned< T >::type;
        UnsignedT const unsignedDecimal = static_cast< UnsignedT >(decimal);

        constexpr std::size_t kBitsPerByte = 8U;
        constexpr std::size_t kTotalBits   = sizeof(T) * kBitsPerByte;

        std::string result(2U + kTotalBits, '0');
        result[0] = '0';
        result[1] = 'b';

        for (std::size_t i = 0U; i < kTotalBits; ++i) {
            std::size_t const bitIndex = kTotalBits - 1U - i;
            result[2U + i] = ((unsignedDecimal >> bitIndex) & static_cast< UnsignedT >(1U)) != 0U ? '1' : '0';
        }

        return result;
    }

private:
    /// @brief Whether to print to the console
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00025
    /// @trace_id_dd=DD_LOG_00062
    /// @needwork = dda
    /// @endcode
    bool mPlainTextCache_{false};

    /// @brief Whether to cache data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00025
    /// @trace_id_dd=DD_LOG_00063
    /// @needwork = dda
    /// @endcode
    bool mDltEnCache_{false};

    /// @brief Instance of DltProtocolEncoder
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00025
    /// @trace_id_dd=DD_LOG_00061
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< internal::DltProtocolEncoder > mDltEncoder_{nullptr};

    /// @brief locationLoged_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00025
    /// @trace_id_dd=DD_LOG_00064
    /// @needwork = dda
    /// @endcode
    bool mLocationLoged_{false};

    /// @brief mPlainEncoder_
    /// @needwork = dda
    std::shared_ptr< internal::DltPlainEncoder > mPlainEncoder_{nullptr};

    //// ---------------------add-----------

    internal::LogStreamContextPtr logCtx_{nullptr};
};
/// @brief Append log level to log information
/// @param[in]  out
/// @param[in]  value
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_LOG_00055
/// @trace_id_dd=DD_LOG_00065
/// @needwork = ad
/// @endcode
LogStream &operator<<(LogStream &out, LogLevel value) noexcept;
/// @brief Append error code to log information
/// @param[in]  out
/// @param[in]  value
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_LOG_00056
/// @trace_id_dd=DD_LOG_00066
/// @needwork = ad
/// @endcode
LogStream &operator<<(LogStream &out, const core::ErrorCode &value) noexcept;
/// @brief Append time to log information
/// @param[in]  out
/// @param[in]  value
/// @return
/// @code{.isoft}
/// @tparam Rep
/// @tparam Period
/// @interface_level=unit
/// @trace_id_ad=AD_LOG_00057
/// @trace_id_dd=DD_LOG_00067
/// @needwork = ad
/// @endcode
template < typename Rep, typename Period >
LogStream &operator<<(LogStream &out, std::chrono::duration< Rep, Period > const &value) noexcept
{
    std::ostringstream s;
    auto getUnit = [&s]() -> std::string {
        if (std::is_same< Period, std::nano >::value) {
            return "ns";
        }
        if (std::is_same< Period, std::micro >::value) {
            return "us";
        }
        if (std::is_same< Period, std::milli >::value) {
            return "ms";
        }
        if (std::is_same< Period, std::ratio< 1 > >::value) {
            return "s";
        }
        if (std::is_same< Period, std::centi >::value) {
            return "cs";
        }
        if (std::is_same< Period, std::deci >::value) {
            return "ds";
        }
        s << "(" << Period::num << "/" << Period::den << ")s";
        std::string unit = s.str();
        s.str("");
        return unit;
    };
    s << std::fixed;
    s << value.count() << getUnit();
    out << s.str();
    return out;
}
/// @brief Append ara::core::InstanceSpecifier to log information
/// @param[in]  out
/// @param[in]  value
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_LOG_00058
/// @trace_id_dd=DD_LOG_00068
/// @needwork = ad
/// @endcode
LogStream &operator<<(LogStream &out, ara::core::InstanceSpecifier const &value) noexcept;
}  // namespace log
}  // namespace ara

#endif