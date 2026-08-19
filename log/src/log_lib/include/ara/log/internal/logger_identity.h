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
/// @file       logger_identity.h
/// @brief      Logger identity information (immutable after creation)
/// @details
/// @date       2025-09-03
/// @author     yangjinbiao
/// @version    1.2.0
///
/// ================================================================

#ifndef __LOG_INTERNAL_LOGGER_IDENTITY__
#define __LOG_INTERNAL_LOGGER_IDENTITY__

#include <cstdint>
#include <string>

namespace ara {
namespace log {
namespace internal {

/// @brief Logger identity information (immutable after creation)
/// @note Contains identification information for ECU, application, and context
struct LoggerIdentity
{
    /// @brief ECUID
    std::string ecuId;

    /// @brief APPID
    std::string appId;

    /// @brief Application description
    std::string appDescription;

    /// @brief mContextId Channel ID
    std::string contextId{"none"};

    /// @brief mContextDescription Channel description
    std::string contextDescription{"none"};

    /// @brief Session ID
    std::uint32_t sessionId{0};

    /// @brief Message ID session support
    bool sessionSupport{false};
};

}  // namespace internal
}  // namespace log
}  // namespace ara

#endif  // __LOG_INTERNAL_LOGGER_IDENTITY__
