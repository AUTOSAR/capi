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
/// @file       main_kspd.cpp
/// @brief      Main function entry point.
/// @details    Main function entry point.
/// @date       2023-09-01
/// @author     Che Jinzhao
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr><td>2023-09-019  <td>1.0.0    <td>Che Jinzhao      <td>Created initial version
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Key Manager/Key Slot Manager
/// @interface_level=/crypto/Key Manager/Key Slot Manager
/// @trace_id_sr=SR_CRYPTO_01008
/// @unit_name=main
/// @unit_description=Main function entry point
/// @endcode
///
/// ================================================================

#include <isoft/ipccpp/debug.h>

#include "ara/core/initialization.h"
#include "ara/crypto/common/isoft_log_api.h"
#include "ara/crypto/internal/ara_core_init.h"
#include "ara/crypto/ipc/isoft_ipc_protocol.h"
#include "ara/crypto/ksp/isoft_keys_manager.h"
#include "ara/per/internal/common/isoft_file_opt.h"

//********************************/

/// @brief Key manager main function
/// @param argc Number of main function parameters
/// @param argv Main function parameter list
/// @return 0
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03320
/// @trace_id_dd=DD_CRYPTO_06632
/// @needwork = ad
/// @endcode
int32_t main(int32_t const argc, ara::crypto::char8_t** const argv) noexcept
{
    if (argc == 0) {
    }
    if (argv == nullptr) {
    }
    ara::core::String const nFileName{ara::crypto::keys::isoft_def::GetIpcFile_Server_Keys()};
    // Initialize CoreInit
    ara::crypto::internal::MAraCoreInit const testInit{
        true, ara::crypto::T_StringView(nFileName)};  // {nFileName.data(),nFileName.size()}};
    ara::crypto::isoft_def::LogInfo() << "---- AP_Crypto.cryptod.Initialize Success, Versino = <"
                                      << ara::crypto::keys::isoft_def::GetstVersion() << "> ----";
    ara::crypto::isoft_def::LogInfo() << "---- AP_Crypto.cryptod.WorkPath = "
                                      << ara::per::isoftkv::PFileOpt::GetWorkPath().data();

    ara::crypto::keys::isoft_def::PKeys_Manager const ipcManager;
    ipcManager.Running();

    return 0;
}
//********************************/
