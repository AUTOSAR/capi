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
/// @file       config_iam_com_data.h
/// @brief      AutoSar-IAM-COM Configuration file data structure
/// @details
/// @date       2025-04-17
/// @author     Han Yuxin
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr><td>2025-04-17 <td>0.1 <td>Han Yuxin <td>Refactored IAM-COM
/// functionality
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/IAM/IAM-COM
/// @interface_level=Unit
/// @trace_id_sr=SR_IAM_
/// @unit_name=IAM_COM
/// @unit_description=Configuration information provided by IAM to the COM
/// module
/// @endcode
///
/// ================================================================
///
/// rights reserved.
///
/// ================================================================

#ifndef ARA_IAM_COM_CONFIG_IAM_COM_DATA_H_
#define ARA_IAM_COM_CONFIG_IAM_COM_DATA_H_

#include <ara/core/string.h>
#include <ara/core/string_view.h>

#include "common/data_type.h"

namespace ara {
namespace iam {
namespace internal {
namespace com {
//********************************/
#pragma pack(push, 4)
/// @brief  ComEventGrants
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = ad
/// @endcode
struct PIamConfigData_ComEventGrant final
{
public:
    /// @brief  processname
    ara::core::String stProcessName{};
    /// @brief  serviceid
    uint16_t nServiceID{0U};
    /// @brief  instanceid
    uint16_t nInstanceID{0U};
    /// @brief  eventid
    uint16_t nEventID{0U};
};
/// @brief  ComMethodGrants
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = ad
/// @endcode
struct PIamConfigData_ComMethodGrant final
{
public:
    /// @brief  processname
    ara::core::String stProcessName{};
    /// @brief  serviceid
    uint16_t nServiceID{0U};
    /// @brief  instanceid
    uint16_t nInstanceID{0U};
    /// @brief  methodid
    uint16_t nMethodID{0U};
};
/// @brief  ComOfferServiceGrants
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = ad
/// @endcode
struct PIamConfigData_ComOfferServiceGrant final
{
public:
    /// @brief  processname
    ara::core::String stProcessName{};
    /// @brief  serviceid
    uint16_t nServiceID{0U};
    /// @brief  instanceid
    uint16_t nInstanceID{0U};
};
/// @brief  ComFindServiceGrants
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = ad
/// @endcode
struct PIamConfigData_ComFindServiceGrant final
{
public:
    /// @brief  processname
    ara::core::String stProcessName{};
    /// @brief  serviceid
    uint16_t nServiceID{0U};
    /// @brief  instanceid
    uint16_t nInstanceID{0U};
};
/// @brief  ComRemoteEventGrants
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = ad
/// @endcode
struct PIamConfigData_ComRemoteEventGrant final
{
public:
    /// @brief  LocalIp
    ara::core::String stLocalIp{};
    /// @brief  LocalPortStart
    int32_t nLocalPortStart{0U};
    /// @brief  LocalPortEnd
    int32_t nLocalPortEnd{0U};
    /// @brief  RemoteIp
    ara::core::String stRemoteIp{};
    /// @brief  RemotePortStart
    uint32_t nRemotePortStart{0U};
    /// @brief  RemotePortEnd
    uint32_t nRemotePortEnd{0U};
    /// @brief  serviceid
    uint16_t nServiceID{0U};
    /// @brief  instanceid
    uint16_t nInstanceID{0U};
    /// @brief  eventid
    uint16_t nEventID{0U};
};
/// @brief  ComRemoteMethodGrants
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = ad
/// @endcode
struct PIamConfigData_ComRemoteMethodGrant final
{
public:
    /// @brief  LocalIp
    ara::core::String stLocalIp{};
    /// @brief  LocalPortStart
    int32_t nLocalPortStart{0U};
    /// @brief  LocalPortEnd
    int32_t nLocalPortEnd{0U};
    /// @brief  RemoteIp
    ara::core::String stRemoteIp{};
    /// @brief  RemotePortStart
    uint32_t nRemotePortStart{0U};
    /// @brief  RemotePortEnd
    uint32_t nRemotePortEnd{0U};
    /// @brief  Protocol
    ara::core::String stProtocol{};
    /// @brief  serviceid
    uint16_t nServiceID{0U};
    /// @brief  instanceid
    uint16_t nInstanceID{0U};
    /// @brief  methodid
    uint16_t nMethodID{0U};
};
/// @brief  ComTlsEventGrants
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = ad
/// @endcode
struct PIamConfigData_ComTlsEventGrant final
{
public:
    /// @brief  TlsName
    ara::core::String stTlsName{};
    /// @brief  serviceid
    uint16_t nServiceID{0U};
    /// @brief  instanceid
    uint16_t nInstanceID{0U};
    /// @brief  eventid
    uint16_t nEventID{0U};
};
/// @brief  ComTlsMethodGrants
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = ad
/// @endcode
struct PIamConfigData_ComTlsMethodGrant final
{
public:
    /// @brief  TlsName
    ara::core::String stTlsName{};
    /// @brief  serviceid
    uint16_t nServiceID{0U};
    /// @brief  instanceid
    uint16_t nInstanceID{0U};
    /// @brief  methodid
    uint16_t nMethodID{0U};
};
#pragma pack(pop)
//********************************/
/// @brief operator < compare.
/// @param lhs left-hand side left operand
/// @param rhs right-hand side right operand
/// @return true/false
/// @exception noexcept
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_IAM_00103
/// @needwork = ad
/// @endcode
inline bool operator<(PIamConfigData_ComEventGrant const &lhs, PIamConfigData_ComEventGrant const &rhs) noexcept
{
    if (lhs.stProcessName != rhs.stProcessName) {
        return lhs.stProcessName < rhs.stProcessName;
    }
    if (lhs.nServiceID != rhs.nServiceID) {
        return lhs.nServiceID < rhs.nServiceID;
    }
    if (lhs.nInstanceID != rhs.nInstanceID) {
        return lhs.nInstanceID < rhs.nInstanceID;
    }
    return lhs.nEventID < rhs.nEventID;
}
/// @brief  operator < compare
/// @param lhs  other left
/// @param rhs  other right
/// @return true/false
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_IAM_00104
/// @needwork = ad
/// @endcode
inline bool operator<(PIamConfigData_ComMethodGrant const &lhs, PIamConfigData_ComMethodGrant const &rhs) noexcept
{
    if (lhs.stProcessName != rhs.stProcessName) {
        return lhs.stProcessName < rhs.stProcessName;
    }
    if (lhs.nServiceID != rhs.nServiceID) {
        return lhs.nServiceID < rhs.nServiceID;
    }
    if (lhs.nInstanceID != rhs.nInstanceID) {
        return lhs.nInstanceID < rhs.nInstanceID;
    }
    return lhs.nMethodID < rhs.nMethodID;
}
/// @brief operator < compare
/// @param lhs left hand
/// @param rhs  right hand
/// @return bool
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_IAM_00102
/// @needwork = ad
/// @endcode
inline bool operator<(PIamConfigData_ComOfferServiceGrant const &lhs,
                      PIamConfigData_ComOfferServiceGrant const &rhs) noexcept
{
    if (lhs.stProcessName != rhs.stProcessName) {
        return lhs.stProcessName < rhs.stProcessName;
    }
    if (lhs.nServiceID != rhs.nServiceID) {
        return lhs.nServiceID < rhs.nServiceID;
    }
    return lhs.nInstanceID < rhs.nInstanceID;
}
/// @brief  operator < compare
/// @param lhs left hand
/// @param rhs right hand
/// @return true/false
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_IAM_00102
/// @needwork = ad
/// @endcode
inline bool operator<(PIamConfigData_ComFindServiceGrant const &lhs,
                      PIamConfigData_ComFindServiceGrant const &rhs) noexcept
{
    if (lhs.stProcessName != rhs.stProcessName) {
        return lhs.stProcessName < rhs.stProcessName;
    }
    if (lhs.nServiceID != rhs.nServiceID) {
        return lhs.nServiceID < rhs.nServiceID;
    }
    return lhs.nInstanceID < rhs.nInstanceID;
}
/// @brief operator < compare.
/// @param lhs left hand
/// @param rhs right hand
/// @return true/false
/// @exception noexcept
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_IAM_00104
/// @needwork = ad
/// @endcode
inline bool operator<(PIamConfigData_ComTlsMethodGrant const &lhs, PIamConfigData_ComTlsMethodGrant const &rhs) noexcept
{
    if (lhs.stTlsName != rhs.stTlsName) {
        return lhs.stTlsName < rhs.stTlsName;
    }
    if (lhs.nServiceID != rhs.nServiceID) {
        return lhs.nServiceID < rhs.nServiceID;
    }
    if (lhs.nInstanceID != rhs.nInstanceID) {
        return lhs.nInstanceID < rhs.nInstanceID;
    }
    return lhs.nMethodID < rhs.nMethodID;
}

/// @brief operator < compare.
/// @param lhs left hand
/// @param rhs right hand
/// @return true/false
/// @exception noexcept
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_IAM_00103
/// @needwork = ad
/// @endcode
inline bool operator<(PIamConfigData_ComTlsEventGrant const &lhs, PIamConfigData_ComTlsEventGrant const &rhs) noexcept
{
    if (lhs.stTlsName != rhs.stTlsName) {
        return lhs.stTlsName < rhs.stTlsName;
    }
    if (lhs.nServiceID != rhs.nServiceID) {
        return lhs.nServiceID < rhs.nServiceID;
    }
    if (lhs.nInstanceID != rhs.nInstanceID) {
        return lhs.nInstanceID < rhs.nInstanceID;
    }
    return lhs.nEventID < rhs.nEventID;
}
//********************************/
}  // namespace com
}  // namespace internal
}  // namespace iam
}  // namespace ara

#endif  // ARA_IAM_COM_CONFIG_IAM_COM_DATA_H_
