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
/// @file       determ_log_meta_data.h
/// @brief      Memory model for deterministic log output
/// @details
/// @date       2024-06-28
/// @author     yangjinbiao
/// @version    1.2.0
///
/// ================================================================

#ifndef __LOG_INTERNAL_DETERM_LOG_META_DATA_H__
#define __LOG_INTERNAL_DETERM_LOG_META_DATA_H__

#include <array>

#include "DltSinkers/file_sinker_no_locker.h"
#include "DltSinkers/file_sinker_withLocker.h"
#include "DltSinkers/remote_sinker.h"
#include "internal/dlt_constants.h"
#include "internal/dlt_structures.h"

namespace ara {
namespace log {
namespace internal {
/// @brief Deterministic log data
class DltProtocolEncoder;
class IlogSinker;
struct LogMetaData
{
    /// @brief If output to console, it is a string; if output to file or remote, it is a formatted DLT message
    std::array< std::uint8_t, kLogBufferSize > buffer{};
    std::size_t size{0U};
    std::uint8_t argsNum{0U};
    IlogSinker* rawFileSinker{nullptr};

    LogLevel logLevel{LogLevel::kVerbose};
    std::uint64_t nowTime{0U};
    std::uint8_t msgCount{0U};
    std::uint32_t timeStamp{0U};
    std::uint32_t mMsgid{0U};

    /// @brief Cached message
    /// @needwork = dda
    internal::DltMessage message{};
    bool toConsle{false};
    bool toFile{false};
    bool toRemote{false};
};
}  // namespace internal
}  // namespace log
}  // namespace ara
#endif