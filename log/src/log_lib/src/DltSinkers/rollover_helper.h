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
/// @file       rollover_helper.h
/// @brief      Common helpers for file-based log rollover.
/// @details
/// @date       2024-06-28
/// @author     yangjinbiao
/// @version    1.2.0
///
/// ================================================================

#ifndef __LOG_INTERNAL_ROLLOVER_HELPER__
#define __LOG_INTERNAL_ROLLOVER_HELPER__

#include <nai/os/nai_dirent.h>
#include <nai/os/nai_file.h>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <list>
#include <string>
#include <vector>

namespace ara {
namespace log {
namespace internal {

struct RolloverConfig
{
    std::string logDir{};
    std::string cleanName{};
    std::string suffix{".dlt"};
    bool compress{false};
    std::size_t loopFileCount{5U};
};

class RolloverHelper
{
public:
    struct RolloverEntry
    {
        std::size_t index{0U};
        std::string ts{};
        std::string filename{};
    };

    RolloverHelper() = default;
    explicit RolloverHelper(RolloverConfig cfg) noexcept { UpdateConfig(cfg); }

    void UpdateConfig(RolloverConfig cfg) noexcept { config_ = std::move(cfg); }
    void SetCompress(bool compress) noexcept { config_.compress = compress; }
    void SetLoopFileCount(std::size_t count) noexcept { config_.loopFileCount = count; }
    void SetNames(std::string logDir, std::string cleanName, std::string suffix) noexcept
    {
        config_.logDir    = std::move(logDir);
        config_.cleanName = std::move(cleanName);
        config_.suffix    = std::move(suffix);
    }

    std::string RolloverSuffix() noexcept;

    std::list< std::string > GetFileList() noexcept;
    bool MatchLoopName(std::string const& str) noexcept;

    std::vector< RolloverEntry > CollectIndexedFiles(std::string const& rolloverSuffix) noexcept;
    bool RotateIndexedFiles(std::string const& rolloverSuffix) noexcept;
    std::int32_t RemoveOldestFiles() noexcept;

private:
    RolloverConfig config_{};
};

}  // namespace internal
}  // namespace log
}  // namespace ara

#endif
