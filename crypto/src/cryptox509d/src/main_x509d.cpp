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
/// @file       main_x509d.cpp
/// @brief      Main function entry point.
/// @details    Main function entry point.
/// @date       2022-08-17
/// @author     Chang Zheng
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Certificate Manager/Certificate Manager
/// @interface_level=/crypto/Certificate Manager/Certificate Manager
/// @trace_id_sr=SR_CRYPTO_03001
/// @unit_name=main
/// @unit_description=Main function entry point
/// @endcode
///
/// ================================================================

#include "ara/core/initialization.h"
#include "ara/crypto/common/isoft_log_api.h"
#include "ara/crypto/internal/ara_core_init.h"
#include "ara/crypto/ipc/isoft_ipc_protocol.h"
#include "ara/crypto/x509/isoft_x509_manager.h"
#include "ara/per/internal/common/isoft_file_opt.h"

//********************************/
/// @brief Main program entry for X509 Key Manager
/// @param argc Number of main function parameters
/// @param argv Main function parameter list
/// @return 0
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03319
/// @trace_id_dd=DD_CRYPTO_06629
/// @needwork = ad
/// @endcode
int32_t main(int32_t const argc, ara::crypto::char8_t** const argv) noexcept
{
    ara::crypto::isoft_def::LogInfo() << "Running main() from " << __FILE__;
    std::ignore = argc;
    std::ignore = argv;
    ara::core::String const nFileName{ara::crypto::keys::isoft_def::GetIpcFile_Server_X509()};
    // Initialize CoreInit
    ara::crypto::internal::MAraCoreInit testInit{true, {nFileName.data(), nFileName.size()}};
    ara::core::StringView const stVersion{"2024-01-03"};
    ara::crypto::isoft_def::LogInfo() << "---- AP_Crypto.cryptox509d.Initialize Success, Versino = " << stVersion.data()
                                      << "> ----";
    ara::crypto::isoft_def::LogInfo() << "---- AP_Crypto.cryptox509d.WorkPath = "
                                      << ara::per::isoftkv::PFileOpt::GetWorkPath().data();

    ara::crypto::keys::isoft_def::PX509_Manager const ipcManager;
    ipcManager.Running();

    return 0;
}
//********************************/
