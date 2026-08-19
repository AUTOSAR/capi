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
/// @file       initialization.h
/// @brief      This file describes the global initialization and shutdown functions that initialize resp.
/// @details    deinitialize data structures and threads of the AUTOSAR Runtime for Adaptive Applications.
/// @date       2022-06-24
/// @author     hanzhibo
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_CORE_INITIALIZATION_H_
#define ARA_CORE_INITIALIZATION_H_

#include "ara/core/internal/initialization_ara.h"
#include "ara/core/result.h"

namespace ara {
namespace core {

///
/// @brief Initializes data structures and threads of the AUTOSAR Adaptive Runtime for Applications.
///        Prior to this call, no interaction with the ARA is possible. This call must be made inside of
///        main(), i.e., in a place where it is guaranteed that static memory initialization has completed.
///        Depending on the individual functional cluster specification, the calling application may have to
///        provide additional configuration data (e.g., set an Application ID for Logging) or make additional
///        initailization calls (e.g., start a FindService in ara::com) before other API calls to the respective
///        functional cluster can be made. Such calls must be made after the call to Initialize(). Calls to
///        ARA APIs made before static initialization has completed lead to undefinded behavior.Calls made
///        after static initialization has completed but before Initialize() was called will be rejected by the
///        functional cluster implementation with an error or, if no error to be reported is defined, lead to undefined
///        behavior
///
/// @return Result<void> A Result object that indicates whether the AUTOSAR Adaptive Runtime for Applications was
///         successfully initialized. Note that this is the only way for the ARA to report an error that is guaranteed
///         to be available, e.g., in case ara::log failed to correctly initialize. The user is not expected to be able
///         to recover from such an error. However, the user may have a project-specific way of recording errors during
///         initialization without ara::log.
///
/// @uptrace{SWS_CORE_10001, 4f41350c07ec5c173dc695f1989c6d0a66fab4a4}
///
Result< void > Initialize() noexcept
{
    Result< void > res;
    res = ara::core::internal::Initialize();
    if (!res) {
        return res;
    }

#if defined(HAS_ARA_EXEC_EXECUTION_CLIENT) || defined(HAS_ARA_EXEC_FIND_PROCESS_CLIENT)                                \
    || defined(HAS_ARA_EXEC_STATE_CLIENT) || defined(HAS_ARA_EXEC_UPDATE_CLIENT) || defined(HAS_ARA_PHM_WATCHDOG)      \
    || defined(HAS_ARA_PHM_RECOVERY_ACTION) || defined(HAS_ARA_PHM_CLIENT) || defined(HAS_ARA_TSYNC_TSCP)              \
    || defined(HAS_ARA_CRYPTO) || defined(HAS_ARA_IDSM_CLIENT) || defined(HAS_ARA_FW_CLIENT) || defined(HAS_ARA_LOG)   \
    || defined(HAS_ARA_COM)
    res = isoft::naicpp::GlobalGeneralEvLoop::Initialize(isoft::naicpp::GlobalGeneralEvLoop::Mode::kInsideThread);
    if (!res) {
        return res;
    }
#endif
    // always try to Deinitialize
    ara::core::internal::RegisterDeinitialize([]() {
        isoft::naicpp::GlobalGeneralEvLoop::Deinitialize();
        return ara::core::Result< void >::FromValue();
    });

#if defined(HAS_ARA_EXEC_EXECUTION_CLIENT) || defined(HAS_ARA_EXEC_FIND_PROCESS_CLIENT)                                \
    || defined(HAS_ARA_EXEC_STATE_CLIENT) || defined(HAS_ARA_EXEC_UPDATE_CLIENT) || defined(HAS_ARA_PHM_WATCHDOG)      \
    || defined(HAS_ARA_PHM_RECOVERY_ACTION) || defined(HAS_ARA_PHM_CLIENT) || defined(HAS_ARA_TSYNC_TSCP)              \
    || defined(HAS_ARA_CRYPTO) || defined(HAS_ARA_IDSM_CLIENT) || defined(HAS_ARA_FW_CLIENT) || defined(HAS_ARA_LOG)   \
    || defined(HAS_NSOMEIP_BINDING) || defined(HAS_NPC_BINDING)
    res = isoft::core::Ipc::Initialize();
    if (!res)
        return res;
    ara::core::internal::RegisterDeinitialize(isoft::core::Ipc::Deinitialize);
#endif

#ifdef HAS_ARA_LOG
    res = ara::log::internal::Initialize();
    if (!res)
        return res;
    ara::core::internal::RegisterDeinitialize(ara::log::internal::Deinitialize);
#endif  // HAS_ARA_LOG

#ifdef HAS_ARA_PER
    res = ara::per::isoftkv::Initialize();
    if (!res)
        return res;
    ara::core::internal::RegisterDeinitialize(ara::per::isoftkv::Deinitialize);
#endif  // HAS_ARA_PER

#ifdef HAS_ARA_CRYPTO
    res = ara::crypto::internal::Initialize();
    if (!res)
        return res;
    ara::core::internal::RegisterDeinitialize(ara::crypto::internal::Deinitialize);
#endif  // HAS_ARA_CRYPTO

#ifdef HAS_ARA_COM
    res = ara::com::internal::Initialize();
    if (!res)
        return res;
    ara::core::internal::RegisterDeinitialize(ara::com::internal::Deinitialize);
#endif  // HAS_ARA_COM

#ifdef HAS_ARA_DIAG
    res = ara::diag::internal::Initialize();
    if (!res)
        return res;
    ara::core::internal::RegisterDeinitialize(ara::diag::internal::Deinitialize);
#endif  // HAS_ARA_DIAG

#ifdef HAS_ARA_IDSM_CLIENT
    ara::core::internal::RegisterDeinitialize(ara::idsm::internal::Deinitialize);
#endif

#ifdef HAS_ARA_FW_CLIENT
    ara::core::internal::RegisterDeinitialize(ara::fw::internal::Deinitialize);
#endif  //HAS_ARA_FW_CLIENT
    return res;
}

///
/// @brief Destroy all data structures and threads of the AUTOSAR Adaptive Runtime for Applications.
///        After this call, no interaction with the ARA is possible. This call must be made inside of main(),
///        i.e., in a place where it is guaranteed that the static initialization has completed and destruciton
///        of statically initialized data has not yet started. Calls made to ARA APIs after a call to
///        ara::core::Deinitialize() but before destruction of statically initialized data will be rejected with an
///        error or, if no error is defined, lead to undefined behavior. Calls made to ARA APIs after the
///        destruction of statically initialized data will lead to undefined behavior.
///
/// @return Result<void> A Result object that indicates whether the ARA was
///         successfully destroyed. Typical error cases to be reported here are that the user is still holding some
///         resource inside the ARA. Note that this Result is the only way for the ARA to report an error that is
///         guaranteed to be available, e.g., in case ara::log has already been deinitialized. The user is not expected
///         to be able to recover from such an error. However, the user may have a project-specific way of
///         recording errors during deinitialization without ara::log.
///
/// @uptrace{SWS_CORE_10002, 8e002917280b4adf4bc2c27c80b2597950554e03}
///
Result< void > Deinitialize() noexcept { return ara::core::internal::Deinitialize(); }

}  // namespace core
}  // namespace ara

#endif  // ARA_CORE_INITIALIZATION_H_
