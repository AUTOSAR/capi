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
/// @file       common.h
/// @brief      IPC communication management common types
/// @details
/// @date       2022-06-10
/// @author     hejunwei
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/NetworkManager/lib
/// @interface_level=unit
/// @trace_id_sr=SRS_NM_00001
/// @unit_name=NmIpc
/// @unit_description=IPC communication management
/// @endcode
///
/// ================================================================

#ifndef INTERNAL_LIB_NM_COMMCON_H
#define INTERNAL_LIB_NM_COMMCON_H
#include <ara/core/string.h>

#include <functional>  // includes std::function

namespace ara {
namespace nm {

/// @brief Notification callback type declaration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_00000
/// @trace_id_dd=DD_NM_00909
/// @needwork = dda
/// @endcode
using EtherNotifier = std::function< void(ara::core::String const &) >;

/// @brief Notification callback type declaration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_00000
/// @trace_id_dd=DD_NM_00910
/// @needwork = dda
/// @endcode
using EtherNotifierExecutor = std::function< void(EtherNotifier const &, ara::core::String const &) >;

/// @brief Ethernet notification receiver type declaration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_00000
/// @trace_id_dd=DD_NM_00911
/// @needwork = dda
/// @endcode
using IpcEtherNotifierPair = std::pair< EtherNotifier, EtherNotifierExecutor >;

/// @brief Logical network notification receiver type declaration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_00000
/// @trace_id_dd=DD_NM_00912
/// @needwork = dda
/// @endcode
using IpcChangeNotifier = std::function< void(std::uint32_t const) >;

/// @brief Logical network notification executor type declaration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_00000
/// @trace_id_dd=DD_NM_00913
/// @needwork = dda
/// @endcode
using IpcChangeNotifierExecutor = std::function< void(IpcChangeNotifier const &, std::uint32_t const) >;

/// @brief Logical network notification receiver type declaration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_00000
/// @trace_id_dd=DD_NM_00914
/// @needwork = dda
/// @endcode
using IpcLnNotifierPair = std::pair< IpcChangeNotifier, IpcChangeNotifierExecutor >;

}  // namespace nm
}  // namespace ara
#endif
