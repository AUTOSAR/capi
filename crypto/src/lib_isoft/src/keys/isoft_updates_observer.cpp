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
/// @file       isoft_updates_observer.cpp
/// @brief      AutoSar-Crypto Key Storage Module
/// @details
/// @date       2021-12-29
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default Key Component/Observer
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_05003
/// @unit_name=PUpdatesObserver
/// @unit_description=Observer
/// @endcode
///
/// ================================================================

#include "ara/crypto/keys/isoft_updates_observer.h"

#include "ara/crypto/common/isoft_log_api.h"

namespace ara {
namespace crypto {
namespace keys {
namespace isoft_def {
//********************************/
/// @brief Define an "update observer" interface.
///         If software developers want to get notifications about slot content update events, the "update observer" interface should be implemented by the consumer application.
//********************************/
/// @brief Notification method that should be called if the content of the specified slot is changed. The key storage engine should call this method in a dedicated thread.
///         The provided list may only include slots subscribed to the observation (during "User" permission opening, i.e., opened by calling the OpenAsUser() method for "reading"). Each slot number can appear only once in the provided list!
/// @brief Notification method that should be called if content of specified slots was changed.
/// Key Storage engine should call this method in a dedicated thread.
/// The provided list may include only slots subscribed for observing (during openning with the "User"
///       permissions, i.e. for "reading" via a call of the method @c OpenAsUser()).
/// Each slot number may present in the provided list only one time!
/// @param updatedSlots  List of monitored slots that were updated after opening (for reading)
/// @trace_id_sws={SWS_CRYPT_30211}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02004}
/// @threadsafety={Thread-safe}
void PUpdatesObserver::OnUpdate(TransactionScope const& updatedSlots) noexcept
{
    for (auto const& itData : updatedSlots) {
        /// Calling IPC here will cause a deadlock. For example, calling the IsEmpty interface to call the daemon-side service will cause the process to freeze. Due to IPC mechanisms, IPC cannot be called within the response of one request currently.
        ///
        std::ignore = itData;
        ara::crypto::isoft_def::LogInfo() << "PUpdatesObserver::OnUpdate";
    }
}
//********************************/
}  // namespace  isoft_def
}  // namespace keys
}  // namespace crypto
}  // namespace ara
