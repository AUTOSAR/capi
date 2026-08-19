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
/// @file       security_level.h
/// @brief      This file provides security_level functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/isoft/uds/security_access_management
/// @interface_level=software
/// @trace_id_sr=af103160-9a25-4eb9-82ec-2ac06ee31212
/// @unit_name=SecurityLevel
/// @unit_description=This file provides security_level functionality
/// @endcode
///
/// ================================================================

#ifndef ISOFT_UDS_SERVER_INCLUDE_SECURITY_LEVEL_H_
#define ISOFT_UDS_SERVER_INCLUDE_SECURITY_LEVEL_H_
#include <map>
#include <memory>
#include <string>
namespace isoft {
namespace uds {
namespace server {
/// @brief Security level ID
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01414
/// @trace_id_dd=DD_UDS_02706
/// @needwork = ad
/// @endcode
using SecurityLevelId = std::uint8_t;
/// @brief Security access interface
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01415
/// @trace_id_dd=DD_UDS_02707
/// @needwork = ad
/// @endcode
class SecurityAccessInterface;
/// @brief Security level configuration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01416
/// @trace_id_dd=DD_UDS_02708
/// @needwork = ad
/// @endcode
class SecurityLevelConfig final
{
public:
    /// @brief Security level name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01281
    /// @trace_id_dd=DD_UDS_02395
    /// @needwork = ad
    /// @endcode
    std::string shortName{""};
    /// @brief Access data record size
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01282
    /// @trace_id_dd=DD_UDS_02396
    /// @needwork = ad
    /// @endcode
    std::size_t accessDataRecordSize{0U};
    /// @brief Key size
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01283
    /// @trace_id_dd=DD_UDS_02397
    /// @needwork = ad
    /// @endcode
    std::size_t keySize{0U};
    /// @brief Number of failed security accesses
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01284
    /// @trace_id_dd=DD_UDS_02398
    /// @needwork = ad
    /// @endcode
    std::size_t numFailedSecurityAccess{0U};
    /// @brief Security level delay time, unit: seconds
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=ad
    ///
    /// @endcode
    std::uint32_t securityDelayTime{0};
    /// @brief Seed size
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01285
    /// @trace_id_dd=DD_UDS_02399
    /// @needwork = ad
    /// @endcode
    std::size_t seedSize{0U};
    /// @brief Security level ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01286
    /// @trace_id_dd=DD_UDS_02400
    /// @needwork = ad
    /// @endcode
    SecurityLevelId id{0U};
    /// @brief Security access interface
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01287
    /// @trace_id_dd=DD_UDS_02401
    /// @needwork = ad
    /// @endcode
    std::shared_ptr< SecurityAccessInterface > interfacePtr{nullptr};
    /// @brief Comparison operator
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01288
    /// @trace_id_dd=DD_UDS_02402
    /// @needwork = ad
    /// @endcode
    bool operator<(const SecurityLevelConfig &other) const { return id < other.id; }
};

}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  /// ISOFT_UDS_SERVER_INCLUDE_SECURITY_LEVEL_H_