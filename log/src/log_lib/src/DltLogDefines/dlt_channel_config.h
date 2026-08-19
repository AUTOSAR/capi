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
/// @file       dlt_channel_config.h
/// @brief
/// @details
/// @date       2025-09-03
/// @author     yangjinbiao
/// @version    1.2.0
///
/// ================================================================

#ifndef DLT_CHANNEL_CONFIG_H
#define DLT_CHANNEL_CONFIG_H

#include <cstdint>
#include <map>
#include <memory>
#include <string>

namespace ara {
namespace log {
namespace internal {

namespace DLTV1 {  /// @brief Mapping of dltchannel configuration information
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_TSYNC_02448
/// @trace_id_dd=DD_TSYNC_02848
/// @needwork = ad
/// @endcode
struct ChannelConfig
{  /// @brief Mode
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00000
    /// @trace_id_dd=DD_LOG_01630
    /// @needwork = dda
    /// @endcode
    std::uint8_t mode{0};

    /// @brief applicationDescription_ Application description
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00000
    /// @trace_id_dd=DD_LOG_01631
    /// @needwork = dda
    /// @endcode
    std::string applicationDescription{};

    /// @brief applicationId_ Application ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00000
    /// @trace_id_dd=DD_LOG_01632
    /// @needwork = dda
    /// @endcode
    std::string applicationId{};

    /// @brief contextDescription Channel description
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00000
    /// @trace_id_dd=DD_LOG_01633
    /// @needwork = dda
    /// @endcode
    std::string contextDescription{};

    /// @brief contextId Channel ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00000
    /// @trace_id_dd=DD_LOG_01634
    /// @needwork = dda
    /// @endcode
    std::string contextId{};

    /// @brief Session ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00000
    /// @trace_id_dd=DD_LOG_01635
    /// @needwork = dda
    /// @endcode
    std::uint32_t sessionId{0};

    /// @brief Log level
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00000
    /// @trace_id_dd=DD_LOG_01636
    /// @needwork = dda
    /// @endcode
    std::uint8_t logLevel{0};

    /// @brief Modeled message flag
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00000
    /// @trace_id_dd=DD_LOG_01637
    /// @needwork = dda
    /// @endcode
    bool nonVerboseMode{true};

    /// @brief File name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00000
    /// @trace_id_dd=DD_LOG_01638
    /// @needwork = dda
    /// @endcode
    std::string logFileName{};

    /// @brief File cache size
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00000
    /// @trace_id_dd=DD_LOG_01639
    /// @needwork = dda
    /// @endcode
    std::size_t fileBufferSize{0};

    /// @brief Single log file size
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00000
    /// @trace_id_dd=DD_LOG_01640
    /// @needwork = dda
    /// @endcode
    std::size_t singleFileSize{0};

    /// @brief Total number of log files
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00000
    /// @trace_id_dd=DD_LOG_01641
    /// @needwork = dda
    /// @endcode
    std::size_t fileCount{0};

    /// @brief serviceInstance_ Service instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00000
    /// @trace_id_dd=DD_LOG_01642
    /// @needwork = dda
    /// @endcode
    std::string serviceInstance{};

    /// @brief Whether multiple channels share the same file
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00000
    /// @trace_id_dd=DD_LOG_01643
    /// @needwork = dda
    /// @endcode
    bool fileShared{false};

    /// @brief Whether it is a deterministic channel flag
    /// @needwork = dda
    bool determConfiged{false};
};
/// @brief Configuration Map
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_LOG_00320
/// @trace_id_dd=DD_LOG_01716
/// @needwork = ad
/// @endcode
using ChannelConfigMap = std::map< std::string, std::shared_ptr< ChannelConfig > >;
/// @brief Configuration Map pointer
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_LOG_00321
/// @trace_id_dd=DD_LOG_01717
/// @needwork = ad
/// @endcode
using ChannelConfigPtr = std::shared_ptr< ChannelConfig >;
}  // namespace DLTV1
}  // namespace internal
}  // namespace log
}  // namespace ara
#endif  // DLT_CHANNEL_CONFIG_H
