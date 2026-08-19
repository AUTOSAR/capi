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
/// @file       common.h
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
/// @unit_name = common
/// @unit_description=Lib interface side of Dlt, providing application interfaces externally for users
/// @endcode
///
/// ================================================================

#ifndef COMMON_H
#define COMMON_H

#include <bitset>
#include <cstdint>
#include <cstdio>
#include <functional>
#include <iostream>
#include <sstream>
#include <string>

namespace ara {
namespace log {

/// @brief Log levels.
/// @code{.isoft}
/// @trace_id_sws={SWS_LOG_00018}@tracestatus{draft}
/// @interface_level=unit
/// @trace_id_ad=AD_LOG_00083
/// @trace_id_dd=DD_LOG_00102
/// @needwork = ad
/// @endcode
enum class LogLevel : uint8_t
{
    /// @brief kOff Turn off
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_TSYNC_02359
    /// @trace_id_dd=DD_TSYNC_02758
    /// @needwork = ad
    /// @endcode
    kOff = 0x00,

    /// @brief kFatal Fatal error
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_TSYNC_02360
    /// @trace_id_dd=DD_TSYNC_02759
    /// @needwork = ad
    /// @endcode
    kFatal = 0x01,

    /// @brief kError Error
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_TSYNC_02361
    /// @trace_id_dd=DD_TSYNC_02760
    /// @needwork = ad
    /// @endcode
    kError = 0x02,  ///< Error

    /// @brief kWarn Warning
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_TSYNC_02362
    /// @trace_id_dd=DD_TSYNC_02761
    /// @needwork = ad
    /// @endcode
    kWarn = 0x03,  ///< Warning

    /// @brief kInfo Info
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_TSYNC_02363
    /// @trace_id_dd=DD_TSYNC_02762
    /// @needwork = ad
    /// @endcode
    kInfo = 0x04,  ///< Info

    /// @brief kDebug Debug
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_TSYNC_02364
    /// @trace_id_dd=DD_TSYNC_02763
    /// @needwork = ad
    /// @endcode
    kDebug = 0x05,  ///< Debug

    /// @brief kVerbose Verbose
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_TSYNC_02365
    /// @trace_id_dd=DD_TSYNC_02764
    /// @needwork = ad
    /// @endcode
    kVerbose = 0x06  ///< Verbose
};

/// @brief Log sink mode
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_LOG_00084
/// @trace_id_dd=DD_LOG_00103
/// @needwork = ad
/// @endcode
enum class LogMode : uint8_t
{
    /// @brief kRemote Remote network logging sink
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_TSYNC_02366
    /// @trace_id_dd=DD_TSYNC_02765
    /// @needwork = ad
    /// @endcode
    kRemote = 0x01,  ///< Remote network logging sink

    /// @brief kFile File logging sink for debugging
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_TSYNC_02367
    /// @trace_id_dd=DD_TSYNC_02766
    /// @needwork = ad
    /// @endcode
    kFile = 0x02,  ///< File logging sink for debugging

    /// @brief kConsole Console logging sink for debugging
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_TSYNC_02368
    /// @trace_id_dd=DD_TSYNC_02767
    /// @needwork = ad
    /// @endcode
    kConsole = 0x04  ///< Console logging sink for debugging
};

/// @brief Logging client connection state
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_LOG_00085
/// @trace_id_dd=DD_LOG_00104
/// @needwork = ad
/// @endcode
enum class ClientState : int8_t
{
    /// @brief kUnknown Connection state unknown
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_TSYNC_02369
    /// @trace_id_dd=DD_TSYNC_02768
    /// @needwork = ad
    /// @endcode
    kUnknown = -1,  ///< Connection state unknown

    /// @brief kNotConnected Client is disconnected
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_TSYNC_02370
    /// @trace_id_dd=DD_TSYNC_02769
    /// @needwork = ad
    /// @endcode
    kNotConnected = 0,  ///< Client is disconnected

    /// @brief kConnected Client is connected
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_TSYNC_02371
    /// @trace_id_dd=DD_TSYNC_02770
    /// @needwork = ad
    /// @endcode
    kConnected = 1  ///< Client is connected
};

/// @brief Alias definition
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_TSYNC_02420
/// @trace_id_dd=DD_TSYNC_02819
/// @needwork = ad
/// @endcode
using Char8_T = char;

/// @brief Operation status ok or failed.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_LOG_00315
/// @trace_id_dd=DD_LOG_01711
/// @needwork = ad
/// @endcode
enum class OperState : int8_t
{
    /// @brief kFailure Error
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_TSYNC_02372
    /// @trace_id_dd=DD_TSYNC_02771
    /// @needwork = ad
    /// @endcode
    kFailure = -1,  //< Error

    /// @brief kOk ok
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_TSYNC_02373
    /// @trace_id_dd=DD_TSYNC_02772
    /// @needwork = ad
    /// @endcode
    kOk = 0,  //< ok

};
namespace internal {

struct DltTimeStamp
{
    std::uint64_t seconds{0};
    std::uint64_t uSeconds{0};
};

static const char* const kDLT_LOGSINK_REMOTE  = "DLT_LOGSINK_REMOTE";
static const char* const kDLT_LOGSINK_DLT     = "DLT_LOGSINK_DLT";
static const char* const kDLT_LOGSINK_CONSOLE = "DLT_LOGSINK_CONSOLE";
static const char* const kDLT_LOGSINK_FILE    = "DLT_LOGSINK_FILE";
static const char* const kDLT_LOGSINK_ARTI    = "DLT_LOGSINK_ARTI";

}  // namespace internal

}  // namespace log

}  // namespace ara

#endif