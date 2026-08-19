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
/// @file       dlt_default_config.h
/// @brief
/// @details
/// @date       2025-09-03
/// @author     yangjinbiao
/// @version    1.2.0
///
/// ================================================================

#ifndef DLT_DEFAULT_CONFIG_H
#define DLT_DEFAULT_CONFIG_H

#include <cstdint>
#include <string>

namespace ara {
namespace log {
namespace internal {

struct DLtDefautConfig final
{
    std::string appid{"DEFT"};
    std::string appdesc{"APPDESC"};
    std::string commandServerName{"LTCM"};
    std::string ecuId{"ECU1"};
    std::string timebaseName{"TIMEBASE"};
    std::string dltVersion{"1.0"};
    std::uint8_t defaultLogLevel{3U};
    std::uint8_t priLogLeve{0U};
    std::size_t maxFileCount{15U};
    std::size_t maxFileBufferSize{1024U * 1024 * 4U};
    std::size_t maxFingleFileSize{1024U * 1024 * 1000U};
    std::size_t fileCount{5U};
    std::size_t singleFileSize{1024U * 1024 * 4U};
    std::size_t fileBufferSize{0U};
    std::uint8_t mode{4U};
    bool sessionIdSupport{false};
    bool fileShared{false};
    std::uint32_t defDetermFreeCount{1024U};
};
static const struct DLtDefautConfig DLT_DEFAULT_CONFIG;

}  // namespace internal
}  // namespace log
}  // namespace ara

#endif  // DLT_DEFAULT_CONFIG_H