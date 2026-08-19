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
/// @file       process.cpp
/// @brief
/// @details
/// @date       2024-09-11
/// @author     mazelin
/// @version    1.2.0
///
/// ================================================================

#include "isoft/ara_fsh/process.h"

#include <cstdlib>

namespace isoft {
namespace ara_fsh {
Process::Process() noexcept
{
    constexpr auto kEnvKeyConfig{"ISOFT_ARA_FSH_PROC_CONFIG_DIR"};
    auto* config{getenv(kEnvKeyConfig)};  // NOLINT -- compatibility>[concurrency-mt-unsafe]
    if (config == nullptr) {
        config_ = "./";
    } else {
        config_ = config;
        if (config_.back() != '/') {
            config_ += '/';
        }
    }
}

std::string Process::GetConfig(std::string const& file) const noexcept { return config_ + file; }
}  // namespace ara_fsh
}  // namespace isoft