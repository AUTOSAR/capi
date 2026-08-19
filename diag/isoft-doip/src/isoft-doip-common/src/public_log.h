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
/// @file       public_log.h
/// @brief      This file provides DOIP common log related interface definitions
/// @details
/// @date       2022-08-29
/// @author     gaohuiming
/// @version    1.2.0
///
/// ================================================================

#ifndef ISOFT_DOIP_LOG_H_
#define ISOFT_DOIP_LOG_H_
#include <ara/log/log_stream.h>

namespace isoft {
namespace doip {
/// @brief Get Error level LogStream object
/// @return LogStream object
ara::log::LogStream LogError() noexcept;

/// @brief Get Warn level LogStream object
/// @return LogStream object
ara::log::LogStream LogWarn() noexcept;

/// @brief Get Info level LogStream object
/// @return LogStream object
ara::log::LogStream LogInfo() noexcept;

/// @brief Get debug level LogStream object
/// @return LogStream object
ara::log::LogStream LogDebug() noexcept;
}  // namespace doip
}  // namespace isoft

#endif  // ISOFT_DOIP_LOG_H_