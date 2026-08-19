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
/// @file       private_log.h
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

#ifndef __INTERNAL_LOG_PRIVATELOG_LOGANDTRACE__
#define __INTERNAL_LOG_PRIVATELOG_LOGANDTRACE__
// clang-format off
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
// clang-format on

namespace ara {
namespace log {
namespace internal {
/// @brief Enum
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_LOG_00099
/// @trace_id_dd=DD_LOG_00146
/// @needwork = ad
/// @endcode
enum class PriLogLevelType : uint8_t
{
    /// @brief  DLT_LOG_OFF
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_TSYNC_02404
    /// @trace_id_dd=DD_TSYNC_02803
    /// @needwork = ad
    /// @endcode
    kDlt_Log_Off = 0x00,

    /// @brief  DLT_LOG_ERROR
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_TSYNC_02405
    /// @trace_id_dd=DD_TSYNC_02804
    /// @needwork = ad
    /// @endcode
    kDlt_Log_Error = 0x02,

    /// @brief  DLT_LOG_VERBOSE
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_TSYNC_02406
    /// @trace_id_dd=DD_TSYNC_02805
    /// @needwork = ad
    /// @endcode
    kDlt_Log_Verbose = 0x06,

    /// @brief  DLT_LOG_MAX
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_TSYNC_02407
    /// @trace_id_dd=DD_TSYNC_02806
    /// @needwork = ad
    /// @endcode
    kDlt_Log_Max = 0x10
};
/// @brief char definition
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_LOG_00317
/// @trace_id_dd=DD_LOG_01713
/// @needwork = ad
/// @endcode
using Char8_T = char;

/// @brief Internal private log
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_LOG_00100
/// @trace_id_dd=DD_LOG_00147
/// @needwork = ad
/// @endcode
class PrivateLogger final
{
public:
    /// @brief Constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00101
    /// @trace_id_dd=DD_LOG_00148
    /// @needwork = ad
    /// @endcode
    PrivateLogger() = default;
    /// @brief Constructor
    /// @param[in]  other  Other
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00102
    /// @trace_id_dd=DD_LOG_00149
    /// @needwork = ad
    /// @endcode
    PrivateLogger(PrivateLogger const &other) noexcept = default;
    /// @brief Copy assignment
    /// @param[in]  other   Other
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00103
    /// @trace_id_dd=DD_LOG_00150
    /// @needwork = ad
    /// @endcode
    PrivateLogger &operator=(PrivateLogger const &other) noexcept = default;

    PrivateLogger(PrivateLogger &&other) noexcept = default;
    /// @brief Copy assignment
    /// @param[in]  other   Other
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00103
    /// @trace_id_dd=DD_LOG_00150
    /// @needwork = ad
    /// @endcode
    PrivateLogger &operator=(PrivateLogger &&other) noexcept = default;
    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00104
    /// @trace_id_dd=DD_LOG_00151
    /// @needwork = ad
    /// @endcode
    ~PrivateLogger() noexcept;
    /// @brief Get internal log instance
    /// @return Instance pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00105
    /// @trace_id_dd=DD_LOG_00152
    /// @needwork = ad
    /// @endcode
    static std::shared_ptr< PrivateLogger > Instance() noexcept;
    /// @brief  Internal log interface
    /// @param[in]  level  Log level
    /// @param[in]  msg  Message
    /// @return  Reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00106
    /// @trace_id_dd=DD_LOG_00153
    /// @needwork = ad
    /// @endcode
    PrivateLogger &Inter_Log(PriLogLevelType const &level, std::string const &msg) noexcept;
    /// @brief  Internal log interface
    /// @param[in]  level Log level
    /// @param[in]  msg Message
    /// @return  Reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00107
    /// @trace_id_dd=DD_LOG_00154
    /// @needwork = ad
    /// @endcode
    PrivateLogger &Inter_Log(PriLogLevelType const &level, std::int64_t const &msg) noexcept;
    /// @brief Internal log interface
    /// @param[in]  level Log level
    /// @param[in]  msg1 Message1
    /// @param[in]  msg2 Message2
    /// @return Reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00108
    /// @trace_id_dd=DD_LOG_00155
    /// @needwork = ad
    /// @endcode
    PrivateLogger &Inter_Log(PriLogLevelType const &level, std::string const &msg1, std::string const &msg2) noexcept;

    /// @brief Internal log interface
    /// @param[in]  msg Message
    /// @return  Reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00109
    /// @trace_id_dd=DD_LOG_00156
    /// @needwork = ad
    /// @endcode
    PrivateLogger &Inter_Log(std::string const &msg) noexcept;

    /// @brief End log
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00111
    /// @trace_id_dd=DD_LOG_00158
    /// @needwork = ad
    /// @endcode
    void End() noexcept;
    /// @brief Override <<
    /// @param[in]  value
    /// @return Reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00112
    /// @trace_id_dd=DD_LOG_00159
    /// @needwork = ad
    /// @endcode
    PrivateLogger &operator<<(std::string const &value) noexcept;
    /// @brief  Override <<
    /// @param[in]  value
    /// @return Reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00113
    /// @trace_id_dd=DD_LOG_00160
    /// @needwork = ad
    /// @endcode
    PrivateLogger &operator<<(char const *value) noexcept;

    /// @brief  Override <<
    /// @param[in]  value
    /// @return Reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00114
    /// @trace_id_dd=DD_LOG_00161
    /// @needwork = ad
    /// @endcode
    PrivateLogger &operator<<(std::int32_t const &value) noexcept;
    /// @brief Override << uint32_t
    /// @param value
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00115
    /// @trace_id_dd=DD_LOG_00162
    /// @needwork = ad
    /// @endcode
    PrivateLogger &operator<<(std::uint32_t const &value) noexcept;

    /// @brief  Override <<
    /// @param[in]  value
    /// @return Reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00116
    /// @trace_id_dd=DD_LOG_00163
    /// @needwork = ad
    /// @endcode
    PrivateLogger &operator<<(std::uint64_t const &value) noexcept;
    /// @brief  Override <<
    /// @param[in]  value
    /// @return Reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00117
    /// @trace_id_dd=DD_LOG_00164
    /// @needwork = ad
    /// @endcode
    PrivateLogger &operator<<(std::int64_t const &value) noexcept;

    /// @brief Whether logging is allowed
    /// @return Result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00118
    /// @trace_id_dd=DD_LOG_00165
    /// @needwork = ad
    /// @endcode
    bool CanLog() const noexcept;
    /// @brief Set log level
    /// @param[in]  loglevel Log level
    /// @return Result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00119
    /// @trace_id_dd=DD_LOG_00166
    /// @needwork = ad
    /// @endcode
    static bool SetPrivateLogLevel(PriLogLevelType const &loglevel) noexcept;
    /// @brief
    /// @param appId
    /// @return Result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00120
    /// @trace_id_dd=DD_LOG_00167
    /// @needwork = ad
    /// @endcode

    static bool Set_Appid(std::string const &appId) noexcept;

private:
    /// @brief
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00100
    /// @trace_id_dd=DD_LOG_00167
    /// @needwork = dd
    /// @endcode
    static PriLogLevelType s_Log_Level_;

    /// @brief
    static std::string s_Appid_;

    /// @brief lineLogLeve_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00099
    /// @trace_id_dd=DD_LOG_00168
    /// @needwork = dda
    /// @endcode
    PriLogLevelType lineLogLeve_{PriLogLevelType::kDlt_Log_Verbose};

    /// @brief
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00099
    /// @trace_id_dd=DD_LOG_00169
    /// @needwork = dda
    /// @endcode
    std::string msg_{"\r\n"};
};

/// @brief Empty log stream used in release mode (does nothing)
class NullInternalStream final
{
public:
    template < typename T >
    constexpr NullInternalStream const &operator<<(T const & /*value*/) const noexcept
    {
        return *this;
    }
};

}  // namespace internal

/// @brief LOGVERBOSE
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_TSYNC_02421
/// @trace_id_dd=DD_TSYNC_02820
/// @needwork = ad
/// @endcode
#if !defined(NDEBUG)
    #define LOGVERBOSE(x)                                                                                              \
        ara::log::internal::PrivateLogger::Instance()->Inter_Log(                                                      \
            ara::log::internal::PriLogLevelType::kDlt_Log_Verbose, x)
#else
    #define LOGVERBOSE(x)                                                                                              \
        ara::log::internal::NullInternalStream {}
#endif

/// @brief LOGERROR
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_TSYNC_02422
/// @trace_id_dd=DD_TSYNC_02821
/// @needwork = ad
/// @endcode
#if !defined(NDEBUG)
    #define LOGERROR(x)                                                                                                \
        ara::log::internal::PrivateLogger::Instance()->Inter_Log(ara::log::internal::PriLogLevelType::kDlt_Log_Error, x)
#else
    #define LOGERROR(x)                                                                                                \
        ara::log::internal::NullInternalStream {}
#endif

}  // namespace log
}  // namespace ara
#endif
