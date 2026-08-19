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
/// @file       ipc_name.h
/// @brief      fw
/// @details    ipc communication name
/// @date       2024-12-16
/// @author     jzy
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/fw/firewall initialization
/// @interface_level=module
/// export_level=module
/// @trace_id_sr=SR_FW_0001
/// @unit_name=IpcName
/// @endcode
///
/// ================================================================
///
///
/// ================================================================

#ifndef _ARA_FW_IPC_NAME_H_
#define _ARA_FW_IPC_NAME_H_
#include "ara/fw/common/common.h"

namespace ara {
namespace fw {
namespace internal {

/// @brief get  FW IPC server file name.
/// @return  file name.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00387
/// @trace_id_dd=DD_FW_00519
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkFWIpcFile_Server() noexcept { return "fwdm"; }

/// @brief get  FW IPC client file name.
/// @return  file name.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00388
/// @trace_id_dd=DD_FW_00520
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkFWIpcFile_Client() noexcept { return "fwd/fwdm"; }

/// @brief FW IPC server test name.
/// @return  file name.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00389
/// @trace_id_dd=DD_FW_00521
/// @needwork = ad
/// @endcode
inline constexpr char8_t const *GetkFWIpcFile_ServerTest() noexcept { return "fwd"; }

/// @brief Get string
/// @return "fwdn"
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00390
/// @trace_id_dd=DD_FW_00522
/// @needwork = ad
/// @endcode
inline std::string GetFWIpcFile_Server_Keys() noexcept
{
    std::string stReturn{GetkFWIpcFile_Server()};
    return stReturn;
}

}  // namespace internal
}  // namespace fw
}  // namespace ara
#endif