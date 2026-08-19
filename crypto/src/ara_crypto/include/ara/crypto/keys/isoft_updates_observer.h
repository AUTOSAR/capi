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
/// @file       isoft_updates_observer.h
/// @brief      AutoSar-Crypto Key Storage Module
/// @details    "Update Observer" interface.
/// @date       2021-12-29
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <table> <td>2021-12-29 <td>1.0.0 <td>hanjingjing <td>Created initial version
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default Key Components/Observer Callback
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_05003
/// @unit_name=PUpdatesObserver
/// @unit_description=Observer
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_KEYS_PUHUA_KEY_UPDATES_OBSERVER_H_
#define ARA_CRYPTO_KEYS_PUHUA_KEY_UPDATES_OBSERVER_H_

#include "ara/crypto/keys/updates_observer.h"

namespace ara {
namespace crypto {
namespace keys {
namespace isoft_def {
//- @interface UpdatesObserver
//********************************/
/// @brief Define an "Update Observer" interface.
///         The "Update Observer" interface should be implemented by consumer applications if the software developer wants to receive notifications about slot content update events.
/// @brief Definition of an "updates observer" interface.
///         The "updates observer" interface should be implemented by a consumer application,
///         if a software developer would like to get notifications about the slots’ content update events.
/// @code{.isoft}
/// @trace_id_sws={SWS_CRYPT_30200}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02004}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02778
/// @trace_id_dd=DD_CRYPTO_05636
/// @needwork = ad
/// @endcode
// PRQA S 2659 QAC /// @qac: [2659]possibly cannot be modified
// Base class '::ara::crypto::x509::CertSignRequest' has non-deleted public copy/move special member functions.
class PUpdatesObserver : public UpdatesObserver
// PRQA L:QAC
{
public:
    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    PUpdatesObserver() = default;
    /// @brief Default virtual destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    ~PUpdatesObserver() noexcept override = default;
    /// @brief Default copy constructor
    /// @param other Another instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    PUpdatesObserver(PUpdatesObserver const& other) = delete;
    /// @brief Default copy assignment operator
    /// @param other Another instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    PUpdatesObserver& operator=(PUpdatesObserver const& other) = delete;
    /// @brief Default move constructor
    /// @param other Another instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    PUpdatesObserver(PUpdatesObserver&& other) = delete;
    /// @brief Default move assignment operator
    /// @param other Another instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    PUpdatesObserver& operator=(PUpdatesObserver&& other) = delete;

public:
    /// @brief Notification method that should be called if the content of the specified slot is changed. The key storage engine should call this method in a dedicated thread.
    ///         The provided list may only include slots subscribed for observation (opened with "User" permission, i.e., by calling the OpenAsUser() method to "read"). Each slot number can only appear once in the provided list!
    /// @brief Notification method that should be called if content of specified slots was changed.
    /// Key Storage engine should call this method in a dedicated thread.
    /// The provided list may include only slots subscribed for observing (during openning with the "User"
    ///       permissions, i.e. for "reading" via a call of the method @c OpenAsUser()).
    /// Each slot number may present in the provided list only one time!
    /// @param updatedSlots  List of monitored slots that were updated after opening (for reading)
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_30211}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02004}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02785
    /// @trace_id_dd=DD_CRYPTO_05643
    /// @needwork = ad
    /// @endcode
    void OnUpdate(TransactionScope const& updatedSlots) noexcept override;

public:
};
//********************************/
}  // namespace  isoft_def
}  // namespace keys
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_KEYS_PUHUA_KEY_UPDATES_OBSERVER_H_
