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
/// @file       trace.cpp
/// @brief
/// @details
/// @date       2025-03-17
/// @author     jiusen.cui
/// @version    1.2.0
///
/// ================================================================

#include "ara/com/internal/trace/trace.h"

#include "ara/com/internal/log/log.h"
#include "isoft/ara_fsh/process.h"
#include "isoft/manifestreader/manifest_reader.h"
#include "nai/os/nai_stat.h"

namespace ara {
namespace com {
namespace internal {
namespace trace {
constexpr uint32_t kMaxPayloadSize{1024 * 60};
// MonitorConfig ComTrace::config_{};
ara::core::Result< void > ComTrace::LoadConfig(ara::core::StringView const path) noexcept
{
    std::ignore  = path;
    using Result = ara::core::Result< void >;
    ComLogTrace("load com trace");
    Result result{};
    nai_stat_t s;
    auto r{nai_stat(path.data(), &s, NAI_STAT_BASIC)};
    if (r < 0) {
        ComLogDebug("load com trace ignored: config not exist", GenArg(path));
        return result;
    }
    auto manifestRes{isoft::manifestreader::OpenManifest(path)};
    if (!manifestRes) {
        ComLogError("load com trace error: open config failed", GenArg(manifestRes), GenArg(path));
        result.EmplaceError(std::move(manifestRes).Error());
        return result;
    }
    ara::core::StringView const stUseCom{"use_com"};
    ara::core::StringView const stUseCmm{"use_cmm"};
    ara::core::StringView const stPrintDebugLog{"print_debug_log"};
    ara::core::StringView const stPayloadOutputLevel{"payload_output_level"};
    ara::core::StringView const stMaxPayloadSize{"max_payload_size"};
    if (isoft::kSuccess != manifestRes.Value()->Load(stUseCom, config_.useCOMLogger)) {
        config_.useCOMLogger = false;
    }
    if (isoft::kSuccess != manifestRes.Value()->Load(stUseCmm, config_.useCMMLogger)) {
        config_.useCMMLogger = true;
    }
    if (isoft::kSuccess != manifestRes.Value()->Load(stPrintDebugLog, config_.printDebugLog)) {
        config_.printDebugLog = false;
    }
    if (isoft::kSuccess != manifestRes.Value()->Load(stPayloadOutputLevel, config_.payloadOutputLevel)) {
        config_.payloadOutputLevel = 0;
    }
    if (isoft::kSuccess != manifestRes.Value()->Load(stMaxPayloadSize, config_.maxPayloadSize)) {
        config_.maxPayloadSize = kMaxPayloadSize;
    }
    ComLogDebug("load com trace done", GenArg(config_.ToString()));
    return result;
}
}  // namespace trace
}  // namespace internal
}  // namespace com
}  // namespace ara