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
/// @file       updates_observer.h
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
/// <td> <td>2021-12-29 <td>1.0.0 <td>hanjingjing <td>Created initial version
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default Key Components/Observer Callback
/// @interface_level=software
/// @trace_id_sr=SR_CRYPTO_05003
/// @unit_name=PUpdatesObserver
/// @unit_description=Update Observer
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_KEYS_KEY_UPDATES_OBSERVER_H_
#define ARA_CRYPTO_KEYS_KEY_UPDATES_OBSERVER_H_

#include "ara/crypto/keys/elementary_types.h"

namespace ara {
namespace crypto {
namespace keys {
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
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02786
/// @trace_id_dd=DD_CRYPTO_05644
/// @needwork = ad
/// @endcode
class UpdatesObserver
{
public:
    /// @brief Unique smart pointer for the interface.
    /// @brief Shared smart pointer of the interface.
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_30201}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02004}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_03242
    /// @trace_id_dd=DD_CRYPTO_06454
    /// @needwork = ad
    /// @endcode
    using Uptr = std::unique_ptr< UpdatesObserver >;

public:
    /// @brief Default virtual destructor
    /// @brief Destructor.
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_30210}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02004}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02787
    /// @trace_id_dd=DD_CRYPTO_05645
    /// @needwork = ad
    /// @endcode
    virtual ~UpdatesObserver() noexcept = default;
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
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02791
    /// @trace_id_dd=DD_CRYPTO_05649
    /// @needwork = ad
    /// @endcode
    virtual void OnUpdate(TransactionScope const& updatedSlots) noexcept = 0;

public:
    /// @brief Default copy assignment operator
    /// @brief Copy-assign another UpdatesObserver to this instance.
    /// @param other Another instance of this class
    /// @returns *this, containing the contents of @a other
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_30224}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02004}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02792
    /// @trace_id_dd=DD_CRYPTO_05650
    /// @needwork = ad
    /// @endcode
    UpdatesObserver& operator=(UpdatesObserver const& other) noexcept = default;

public:
    /// @brief Default move constructor
    /// @param other Another instance of this class
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02790
    /// @trace_id_dd=DD_CRYPTO_05648
    /// @needwork = ad
    /// @endcode
    UpdatesObserver(UpdatesObserver&& other) noexcept = delete;
    /// @brief Default copy constructor
    /// @param other Another instance of this class
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02789
    /// @trace_id_dd=DD_CRYPTO_05647
    /// @needwork = ad
    /// @endcode
    UpdatesObserver(UpdatesObserver const& other) noexcept = delete;
    /// @brief Default move assignment operator
    /// @brief Move-assign another UpdatesObserver to this instance.
    /// @param other Another instance of this class
    /// @returns *this, containing the contents of @a other
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_30225}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02004}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02793
    /// @trace_id_dd=DD_CRYPTO_05651
    /// @needwork = ad
    /// @endcode
    UpdatesObserver& operator=(UpdatesObserver&& other) noexcept = delete;

protected:
protected:
    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02788
    /// @trace_id_dd=DD_CRYPTO_05646
    /// @needwork = ad
    /// @endcode
    UpdatesObserver() = default;

public:
};
//********************************/
}  // namespace keys
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_KEYS_KEY_UPDATES_OBSERVER_H_
