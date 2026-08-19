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
/// @file       ara_core_init.h
/// @brief      AutoSar-AP Data Persistent Storage Module
/// @details    AP-Crypto Initialization
/// @date       2022-09-27
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr><td>2022-09-27  <td>1.0.0    <td>hanjingjing <td>Created initial version
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Reusable Functions/Reusable Functions Module
/// @interface_level=software
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=MAraCoreInit
/// @endcode
///
/// ================================================================

#ifndef PH_CRYPTO_ARA_CORE_INIT_H_
#define PH_CRYPTO_ARA_CORE_INIT_H_

#include <ara/exec/execution_client.h>
#include <isoft/ipccpp/debug.h>
#include <isoft/naicpp/terminating_handler.h>

#include "ara/core/initialization.h"
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03261
/// @trace_id_dd=DD_CRYPTO_06503
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
#define PH_ASSERT_INIT(exp)                                                                                            \
    if (!(exp)) {                                                                                                      \
        std::ignore = fprintf(stderr, "PH_ASSERT : %s:%d\n", __FILE__, static_cast< int32_t >(__LINE__));              \
        assert(exp);                                                                                                   \
    }

namespace ara {
namespace crypto {
namespace internal {
//********************************/
/// @brief CoreInit operations encapsulated within the Crypto module
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02548
/// @trace_id_dd=DD_CRYPTO_05350
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_description=Initialization environment operations encapsulated within the Crypto module
/// @needwork = ad
/// @endcode
class MAraCoreInit
{
public:
    /// @brief Constructor with parameters
    /// @param bUseThread Whether to use a thread
    /// @param stIpcFileName IPC file name
    /// @throws Function execution exception
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02549
    /// @trace_id_dd=DD_CRYPTO_05351
    /// @needwork = ad
    /// @endcode
    MAraCoreInit(bool const bUseThread, ara::core::StringView const& stIpcFileName) noexcept
    {
        if (bUseThread) {
            // Initialize EvLoop: If blocking in the main thread is required, EvLoop must be initialized manually using the kUserThread parameter before core::init
            std::ignore = ::isoft::naicpp::GlobalGeneralEvLoop::Initialize(
                ::isoft::naicpp::GlobalGeneralEvLoop::Mode::kUserThread);
        }
        ara::core::Result< void > const resultInit{ara::core::Initialize()};
        PH_ASSERT_INIT(resultInit.HasValue())
        if (bUseThread) {
            pLifeCycle_ = std::make_unique< isoft::naicpp::TerminatingHandler >();
            std::ignore = pLifeCycle_->Register();
        }
        // Initialize ipc
        std::string stSocketFile(stIpcFileName.data(), stIpcFileName.size());
        pIpcDebugInit_ = std::make_unique< isoft::ipc::debug::MIpcDebugInit >(stSocketFile);
        PH_ASSERT_INIT(false == pIpcDebugInit_->IsHaveError())
        //Report status to the execution manager
        pExeClient_ = std::make_unique< ara::exec::ExecutionClient >();
        std::ignore = pExeClient_->ReportExecutionState(ara::exec::ExecutionState::kRunning);
        std::cout << "ReportExecutionState success : " << __FILE__ << ":" << static_cast< int32_t >(__LINE__);
    }
    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02550
    /// @trace_id_dd=DD_CRYPTO_05352
    /// @needwork = ad
    /// @endcode
    virtual ~MAraCoreInit() noexcept
    {
        pIpcDebugInit_.reset();
        pExeClient_.reset();
        pLifeCycle_.reset();
        std::ignore = ara::core::Deinitialize();
    }
    /// @brief Default move constructor
    /// @param other Another instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02551
    /// @trace_id_dd=DD_CRYPTO_05353
    /// @needwork = ad
    /// @endcode
    MAraCoreInit(MAraCoreInit&& other) = delete;
    /// @brief Default move assignment operator
    /// @param other Another instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02552
    /// @trace_id_dd=DD_CRYPTO_05354
    /// @needwork = ad
    /// @endcode
    MAraCoreInit& operator=(MAraCoreInit&& other) = delete;
    /// @brief Default copy assignment operator
    /// @param other Another instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02553
    /// @trace_id_dd=DD_CRYPTO_05355
    /// @needwork = ad
    /// @endcode
    MAraCoreInit& operator=(MAraCoreInit const& other) = delete;
    /// @brief Default copy constructor
    /// @param other Another instance of this class The return value of this function
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02554
    /// @trace_id_dd=DD_CRYPTO_05356
    /// @needwork = ad
    /// @endcode
    MAraCoreInit(MAraCoreInit const& other) = delete;

private:
    /// @brief continueExecution_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05357
    /// @needwork = dda
    /// @endcode
    std::atomic_bool continueExecution_{true};
    /// @brief ipc initialization
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05358
    /// @needwork = dda
    /// @endcode
    std::unique_ptr< isoft::ipc::debug::MIpcDebugInit > pIpcDebugInit_{nullptr};
    /// @brief Execute client
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05359
    /// @needwork = dda
    /// @endcode
    std::unique_ptr< ara::exec::ExecutionClient > pExeClient_{nullptr};
    /// @brief Lifecycle
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05360
    /// @needwork = dda
    /// @endcode
    std::unique_ptr< isoft::naicpp::TerminatingHandler > pLifeCycle_{nullptr};
};
//********************************/
}  // namespace internal
}  // namespace crypto
}  // namespace ara

#endif
