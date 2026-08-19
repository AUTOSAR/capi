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
/// @file       ecu_reset_management.h
/// @brief      This file provides ecu_reset_management functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/isoft/uds/ecu_reset_management
/// @interface_level=software
/// @trace_id_sr=e54942f9-12eb-4cfb-9030-6b28ad0e8e04
/// @unit_name=EcuResetManagement
/// @unit_description=This file provides ecu_reset_management functionality
/// @endcode
///
/// ================================================================

#ifndef ISOFT_UDS_SERVER_CONFIG_SECURITYACCESS_H_
#define ISOFT_UDS_SERVER_CONFIG_SECURITYACCESS_H_
#include <memory>

#include "ecu_reset_request.h"
#include "isoft/uds/authentication_management/authentication_state.h"
#include "isoft/uds/common.h"
#include "isoft/uds/security_access_management/security_level.h"
#include "isoft/uds/session_management/session_model.h"
namespace isoft {
namespace uds {
namespace server {
/// @brief Define hard reset variable

/// @interface_level=unit
/// @needwork=ad
/// @endcode
constexpr std::uint8_t kHardReset = 0x01;
/// @brief Define key/on/off reset variable
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01332
/// @trace_id_dd=DD_UDS_02624
/// @needwork = ad
/// @endcode
constexpr std::uint8_t kKeyOffOnReset = 0x02;
/// @brief Define soft reset variable
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01333
/// @trace_id_dd=DD_UDS_02625
/// @needwork = ad
/// @endcode
constexpr std::uint8_t kSoftReset = 0x03;
/// @brief Define enable fast shutdown variable
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01334
/// @trace_id_dd=DD_UDS_02626
/// @needwork = ad
/// @endcode
constexpr std::uint8_t kEnableRapidPowerShutDown = 0x04;
/// @brief Define disable fast shutdown variable
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01335
/// @trace_id_dd=DD_UDS_02627
/// @needwork = ad
/// @endcode
constexpr std::uint8_t kDisableRapidPowerShutDown = 0x05;

/// @brief Define ECU reset instance configuration structure
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01336
/// @trace_id_dd=DD_UDS_02628
/// @needwork = ad
/// @endcode
struct EcuResetInstanceConfig
{
    /// @brief Subfunction
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01041
    /// @trace_id_dd=DD_UDS_02155
    /// @needwork = ad
    /// @endcode
    std::uint8_t subfunctionNumber{0U};
    /// @brief Access permission
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01042
    /// @trace_id_dd=DD_UDS_02156
    /// @needwork = ad
    /// @endcode
    EnableAuth accessPermissionAuth{};
    /// @brief Access permission session ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01043
    /// @trace_id_dd=DD_UDS_02157
    /// @needwork = ad
    /// @endcode
    std::set< SessionId > accessPermissionSession{};
    /// @brief Access permission security level ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01044
    /// @trace_id_dd=DD_UDS_02158
    /// @needwork = ad
    /// @endcode
    std::set< SecurityLevelId > accessPermissionSecurityLevel{};

    bool operator<(const EcuResetInstanceConfig &other) const { return subfunctionNumber < other.subfunctionNumber; }
};

/// @brief Define diagnostic response ECU reset enumeration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01337
/// @trace_id_dd=DD_UDS_02629
/// @needwork = ad
/// @endcode
enum class DiagnosticResponseToEcuResetEnum
{
    /// @brief Response after reset
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01045
    /// @trace_id_dd=DD_UDS_02159
    /// @needwork = ad
    /// @endcode
    kRespondAfterReset,
    /// @brief Response before reset
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01046
    /// @trace_id_dd=DD_UDS_02160
    /// @needwork = ad
    /// @endcode
    kRespondBeforeReset
};

/// @brief Define ECU reset management structure
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01338
/// @trace_id_dd=DD_UDS_02630
/// @needwork = ad
/// @endcode
struct EcuResetManagement : public ServiceConfigBase
{
public:
    /// @brief Reset response
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01047
    /// @trace_id_dd=DD_UDS_02161
    /// @needwork = ad
    /// @endcode
    DiagnosticResponseToEcuResetEnum respondToReset{DiagnosticResponseToEcuResetEnum::kRespondAfterReset};
    /// @brief Instance table
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01048
    /// @trace_id_dd=DD_UDS_02162
    /// @needwork = ad
    /// @endcode
    std::set< EcuResetInstanceConfig > instanceTable{};
    /// @brief Reset request interface pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01049
    /// @trace_id_dd=DD_UDS_02163
    /// @needwork = ad
    /// @endcode
    std::shared_ptr< EcuResetRequestInterface > interfacePtr{nullptr};
};
}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  ///ISOFT_UDS_SERVER_CONFIG_SECURITYACCESS_H_