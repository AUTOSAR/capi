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
/// @file       log_sinker_interface.h
/// @brief      Template class for encapsulating log parameters
/// @details
/// @date       2024-06-28
/// @author     yangjinbiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/LOG/DltSinkers
/// @interface_level = unit
/// @trace_id_sr=LOG_SR_00007,LOG_SR_00009
/// @unit_name = log_sinker_interface
/// @unit_description==Common abstract interface of Dlt lib's implemented log sinker for writing to files and sending to DLT
/// @endcode
///
/// ================================================================

#ifndef __LOG__INTERNAL_ILOGSINKER__
#define __LOG__INTERNAL_ILOGSINKER__

#include <cstdint>
#include <string>
#include <tuple>

#include "common.h"
namespace ara {
namespace log {
namespace internal {
using Char8_T = char;
/// @brief Sinkers interface class
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_LOG_00163
/// @trace_id_dd=DD_LOG_00248
/// @needwork = ad
/// @endcode
class IlogSinker
{
public:
    /// @brief  Destructor for releasing memory
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00164
    /// @trace_id_dd=DD_LOG_00249
    /// @needwork = ad
    /// @endcode
    virtual ~IlogSinker() noexcept = default;

    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_02356
    /// @trace_id_dd=DD_LOG_02755
    /// @needwork = ad
    /// @endcode
    IlogSinker() = default;

    /// @brief Constructor
    /// @param other
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_02357
    /// @trace_id_dd=DD_LOG_02756
    /// @needwork = ad
    /// @endcode
    IlogSinker(IlogSinker const& other) = default;

    /// @brief Default assignment
    /// @param other
    /// @return Reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_02352
    /// @trace_id_dd=DD_LOG_02751
    /// @needwork = ad
    /// @endcode

    IlogSinker& operator=(IlogSinker const& other) = default;

    /// @brief other
    /// @param other
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_02353
    /// @trace_id_dd=DD_LOG_02752
    /// @needwork = ad
    /// @endcode
    IlogSinker(IlogSinker&& other) = delete;

    /// @brief other
    /// @param other
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_02358
    /// @trace_id_dd=DD_LOG_02757
    /// @needwork = ad
    /// @endcode
    IlogSinker& operator=(IlogSinker&& other) = delete;

    /// @brief Write log
    /// @param[in]  buffer1  Cache pointer
    /// @param[in]  len1  Cache length
    /// @param[in]  buffer2  Cache pointer
    /// @param[in]  len2 Cache length
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00165
    /// @trace_id_dd=DD_LOG_00250
    /// @needwork = ad
    /// @endcode
    virtual int32_t SaveLog(std::uint8_t* buffer1,
                            std::size_t const& len1,
                            std::uint8_t* buffer2,
                            std::size_t const& len2) noexcept
    {
        std::ignore = buffer1;
        std::ignore = len1;
        std::ignore = buffer2;
        std::ignore = len2;
        return 0;
    };
    virtual std::int32_t SavePlainTextLog(std::uint8_t* buffer1, std::size_t const& len1) noexcept
    {
        std::ignore = buffer1;
        std::ignore = len1;
        return 0;
    };
    /// @brief Write log
    /// @param[in]  buffer1  Cache pointer
    /// @param[in]  len1  Cache length
    /// @param[in]  buffer2  Cache pointer
    /// @param[in]  len2 Cache length
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00165
    /// @trace_id_dd=DD_LOG_00250
    /// @needwork = ad
    /// @endcode
    virtual int32_t SaveLogWithTimeStamp(std::uint8_t* buffer1,
                                         std::size_t const& len1,
                                         std::uint8_t* buffer2,
                                         std::size_t const& len2,
                                         DltTimeStamp const& dltTimestamp) noexcept
    {
        std::ignore = buffer1;
        std::ignore = len1;
        std::ignore = buffer2;
        std::ignore = len2;
        std::ignore = dltTimestamp;
        return 0;
    };
    /// @brief
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00166
    /// @trace_id_dd=DD_LOG_00251
    /// @needwork = ad
    /// @endcode
    virtual bool FlushBuffer() noexcept = 0;

    /// @brief
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = ad
    /// @endcode
    virtual bool SetLogLevel(std::string const& contextId, std::uint8_t const& logLevel) noexcept = 0;
};
}  // namespace internal

}  // namespace log

}  // namespace ara
#endif