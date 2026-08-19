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
/// @file       com_management.h
/// @brief      This file provides com_management functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/isoft/uds/com_management
/// @interface_level=software
/// @trace_id_sr=583e20ff-9f0a-48fe-a022-5526a497bfdc
/// @unit_name=ComManagement
/// @unit_description=This file provides com_management functionality
/// @endcode
///
/// ================================================================

#ifndef ISOFT_UDS_SERVER_CONFIG_DIAGNOSTICCOMMANAGEMENT_H_
#define ISOFT_UDS_SERVER_CONFIG_DIAGNOSTICCOMMANAGEMENT_H_
#include <memory>
#include <set>

#include "communication_control.h"
#include "isoft/uds/authentication_management/authentication_state.h"
#include "isoft/uds/common.h"
#include "isoft/uds/security_access_management/security_level.h"
#include "isoft/uds/session_management/session_model.h"
namespace isoft {
namespace uds {
namespace server {
/// @brief Define subfunction values that allow receiving and sending
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_
/// @trace_id_dd=DD_UDS_00960
/// @needwork = dd
/// @endcode
constexpr std::uint8_t kEnableRxAndTx = 0x00;
/// @brief Define subfunction values that only allow receiving
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_
/// @trace_id_dd=DD_UDS_00961
/// @needwork = dd
/// @endcode
constexpr std::uint8_t kEnableRxAndDisableTx = 0x01;
/// @brief Define subfunction values that only allow sending
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_
/// @trace_id_dd=DD_UDS_00962
/// @needwork = dd
/// @endcode
constexpr std::uint8_t kDisableRxAndEnableTx = 0x02;
/// @brief Define subfunction values that do not allow receiving or sending
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_
/// @trace_id_dd=DD_UDS_00963
/// @needwork = dd
/// @endcode
constexpr std::uint8_t kDisableRxAndTx = 0x03;
/// @brief Define subfunction values that only allow receiving specific address information
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_
/// @trace_id_dd=DD_UDS_00964
/// @needwork = dd
/// @endcode
constexpr std::uint8_t kEnableRxAndDisableTxWithEnhancedAddressInformation = 0x04;
/// @brief Define subfunction values that only allow sending specific address information
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_
/// @trace_id_dd=DD_UDS_00965
/// @needwork = dd
/// @endcode
constexpr std::uint8_t kDisableRxAndEnableTxWithEnhancedAddressInformation = 0x05;
/// @brief Define subfunction values that allow receiving and sending specific address information
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_
/// @trace_id_dd=DD_UDS_00966
/// @needwork = dd
/// @endcode
constexpr std::uint8_t kEnableRxAndTxWithEnhancedAddressInformation = 0x06;

/// @brief Define communication control instance configuration structure
/// @param subfunctionNumber Subfunction value
/// @param accessPermissionAuth Access permission authentication status
/// @param accessPermissionSession Access permission session ID set
/// @param accessPermissionSecurityLevel Access permission security level ID set
/// @param operator<( Comparison operator
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = ad
/// @endcode
struct ComControlInstanceConfig
{
public:
    /// @brief Subfunction value
    std::uint8_t subfunctionNumber{0U};
    /// @brief Access permission authentication status
    EnableAuth accessPermissionAuth{};
    /// @brief Access permission session ID set
    std::set< SessionId > accessPermissionSession{};
    /// @brief Access permission security level ID set
    std::set< SecurityLevelId > accessPermissionSecurityLevel{};
    /// @brief Comparison operator
    /// @param other Other instance configuration
    /// @return true if the current instance configuration is less than the other instance configuration, otherwise false
    bool operator<(const ComControlInstanceConfig &other) const { return subfunctionNumber < other.subfunctionNumber; }
};
/// @brief Define communication management configuration structure
/// @param instanceTable Instance table
/// @param interface Communication control interface
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = ad
/// @endcode
class ComManagementConfig : public ServiceConfigBase
{
public:
    /// @brief Instance table
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00967
    /// @needwork = dda
    /// @endcode
    std::set< ComControlInstanceConfig > instanceTable{};
    /// @brief Communication control interface
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00968
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< CommunicationControlInterface > interface{nullptr};
};

}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  ///ISOFT_UDS_SERVER_CONFIG_DIAGNOSTICCOMMANAGEMENT_H_