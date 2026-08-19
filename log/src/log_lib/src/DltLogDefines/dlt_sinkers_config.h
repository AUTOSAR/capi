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
/// @file       dlt_sinkers_config.h
/// @brief
/// @details
/// @date       2025-09-03
/// @author     yangjinbiao
/// @version    1.2.0
///
/// ================================================================

#ifndef DLT_SINKERS_CONFIG_H
#define DLT_SINKERS_CONFIG_H
#include <cstdint>
#include <list>
#include <map>
#include <memory>
#include <string>

#include "common.h"

namespace ara {
namespace log {
namespace internal {
namespace DLTV2 {

struct DltSinkersConfig final
{
    std::string sinkerType{kDLT_LOGSINK_CONSOLE};

    /// @brief Log level
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    std::uint8_t defaultLogThreshold{0};

    /// @brief Modeled message flag
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    bool nonVerboseMode{false};

    /// @brief File name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    std::string logFileName{"./app_log.dlt"};

    /// @brief File cache size
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    std::size_t fileBufferSize{0};

    /// @brief Single log file size
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    std::size_t singleFileSize{10240000};

    /// @brief Total number of log files
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    std::size_t fileCount{5};
    /// @brief Whether multiple channels share the same file
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    bool fileShared{false};

    bool plainText{false};

    bool bufferOutput{false};

    /// @brief Whether file compression is enabled
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    bool enableCompress{false};

    /// @brief Compression format (ZIP or TAR_GZ)
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    std::string compressionFormat{"ZIP"};

    /// Enabled by default, need to close when
    bool defaultTraceState{true};
};
using DltSinkersMap       = std::map< std::string, std::shared_ptr< DltSinkersConfig > >;
using DltSinkersConfigPtr = std::shared_ptr< DltSinkersConfig >;
using DltSinkerList       = std::list< std::shared_ptr< DltSinkersConfig > >;
using DltSinkerLevelMap   = std::map< std::string, std::uint8_t >;

}  // namespace DLTV2
}  // namespace internal

}  // namespace log
}  // namespace ara

#endif  // DLT_SINKERS_CONFIG_H