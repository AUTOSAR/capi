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
/// @file       session_model.h
/// @brief      This file provides session_model functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/isoft/uds/session_management
/// @interface_level=software
/// @trace_id_sr=751c320f-ca66-4663-bb8e-a1d2b91d51fa
/// @unit_name=SessionModel
/// @unit_description=This file provides session_model functionality
///
/// ================================================================

#ifndef ISOFT_UDS_SERVER_INCLUDE_SESSIONMANAGEMENTSESSION_SESSIONMODEL_H_
#define ISOFT_UDS_SERVER_INCLUDE_SESSIONMANAGEMENTSESSION_SESSIONMODEL_H_
#include <set>
#include <string>
namespace isoft {
namespace uds {
namespace server {
/// @brief Session model
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01322
/// @trace_id_dd=DD_UDS_02614
/// @needwork = ad
/// @endcode
using SessionId = std::uint8_t;
/// @brief Session model
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01323
/// @trace_id_dd=DD_UDS_02615
/// @needwork = ad
/// @endcode
struct SessionModel
{
    /// @brief Session short name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01006
    /// @trace_id_dd=DD_UDS_02120
    /// @needwork = ad
    /// @endcode
    std::string shortName{""};
    /// @brief Session ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01007
    /// @trace_id_dd=DD_UDS_02121
    /// @needwork = ad
    /// @endcode
    SessionId id{0U};
    /// @brief P2 timeout, unit: milliseconds
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01008
    /// @trace_id_dd=DD_UDS_02122
    /// @needwork = ad
    /// @endcode
    std::uint16_t p2ServerMax{0U};
    /// @brief P2Star timeout, unit: milliseconds
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01009
    /// @trace_id_dd=DD_UDS_02123
    /// @needwork = ad
    /// @endcode
    std::uint16_t p2StarServerMax{0U};
    /// @brief Comparison operator
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01010
    /// @trace_id_dd=DD_UDS_02124
    /// @needwork = ad
    /// @endcode
    bool operator<(const SessionModel &other) const { return id < other.id; }
};

}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  ///ISOFT_UDS_SERVER_INCLUDE_SESSIONMANAGEMENTSESSION_SESSIONMODEL_H_