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
/// @file       phm_log.cpp
/// @brief
/// @details
/// @date       2024-06-20
/// @author     wangyanlong
/// @version    1.2.0
///
/// ================================================================

#include "ara/phm/internal/phm_log.h"

namespace ara {
namespace phm {
namespace internal {

/// @brief returns logger.
/// @return logger.
ara::log::Logger& GetDefaultLogger() noexcept
{
    ara::log::Logger& logger{ara::log::CreateLogger(std::move(ara::core::StringView("#PHM")),
                                                    std::move(ara::core::StringView("phmd run context")),
                                                    ara::log::LogLevel::kVerbose)};
    return logger;
}

}  // namespace internal
}  // namespace phm
}  // namespace ara