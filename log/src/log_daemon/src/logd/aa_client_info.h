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
/// @file       aa_client_info.h
/// @brief      Template class for encapsulating log parameters
/// @details
/// @date       2024-06-28
/// @author     yangjinbiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/LOG/DltLogd
/// @interface_level = unit
/// @trace_id_sr=LOG_SR_00001,LOG_SR_00003,LOG_SR_00009
/// @unit_name = aa_client_info
/// @unit_description=Backend of Dlt module, used to support log and command forwarding.
/// @endcode
///
/// ================================================================

#ifndef __LOG_AACLIENT_INFO__
#define __LOG_AACLIENT_INFO__

#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>

namespace ara {
namespace log {
namespace internal {
/// @brief Used to record information of AA processes
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_LOG_00297
/// @trace_id_dd=DD_LOG_00573
/// @needwork = ad
/// @endcode
struct AAClientChannel
{
    /// @brief contextId
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_TSYNC_02423
    /// @trace_id_dd=DD_TSYNC_02822
    /// @needwork = ad
    /// @endcode
    std::string contextId;

    /// @brief contextDescription
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_TSYNC_02424
    /// @trace_id_dd=DD_TSYNC_02823
    /// @needwork = ad
    /// @endcode
    std::string contextDescription;

    /// @brief logLevel
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_TSYNC_02425
    /// @trace_id_dd=DD_TSYNC_02824
    /// @needwork = ad
    /// @endcode
    std::uint8_t logLevel{};
};

/// @brief  Data map
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_LOG_00297
/// @trace_id_dd=DD_LOG_01573
/// @needwork = ad
/// @endcode
using AAClientChannelMap = std::unordered_map< std::string, std::shared_ptr< AAClientChannel > >;

/// @brief  Client information
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_LOG_00298
/// @trace_id_dd=DD_LOG_00574
/// @needwork = ad
/// @endcode
struct AAClientInfo
{
    /// @brief fd_ File descriptor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_TSYNC_02426
    /// @trace_id_dd=DD_TSYNC_02825
    /// @needwork = ad
    /// @endcode
    std::uint64_t sessionId{};

    /// @brief pid_ Process ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_TSYNC_02427
    /// @trace_id_dd=DD_TSYNC_02826
    /// @needwork = ad
    /// @endcode
    std::int32_t pid{};

    /// @brief appId_ Application ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_TSYNC_02428
    /// @trace_id_dd=DD_TSYNC_02827
    /// @needwork = ad
    /// @endcode
    std::string appId{};

    /// @brief ecuId_ EcuID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_TSYNC_02429
    /// @trace_id_dd=DD_TSYNC_02828
    /// @needwork = ad
    /// @endcode
    std::string ecuId{};

    /// @brief Description of the app
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_TSYNC_02430
    /// @trace_id_dd=DD_TSYNC_02829
    /// @needwork = ad
    /// @endcode
    std::string appDesc{};

    /// @brief channels_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_TSYNC_02431
    /// @trace_id_dd=DD_TSYNC_02830
    /// @needwork = ad
    /// @endcode
    AAClientChannelMap channels;
};
/// @brief Map of client information
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_LOG_00297
/// @trace_id_dd=DD_LOG_01574
/// @needwork = ad
/// @endcode
using AAClientInfoMap = std::map< std::string, std::shared_ptr< AAClientInfo > >;

}  // namespace internal
}  // namespace log

}  // namespace ara
#endif