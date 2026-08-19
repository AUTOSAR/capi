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
/// @file       key_storage_provider.h
/// @brief      AutoSar-Crypto Key Storage Module
/// @details    Key storage provider interface.
/// @date       2021-12-29
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr> <td>2021-12-29 <td>1.0.0 <td>hanjingjing <td>Created initial version
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default Key Components/Key Provider
/// @interface_level=software
/// @trace_id_sr=SR_CRYPTO_05001
/// @unit_name=PIpcKeyProvider
/// @unit_description=Key Provider Base Class
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_KEYS_KEY_STORAGE_PROVIDER_H_
#define ARA_CRYPTO_KEYS_KEY_STORAGE_PROVIDER_H_

#include "ara/core/instance_specifier.h"
#include "ara/core/result.h"
#include "ara/crypto/common/io_interface.h"
#include "ara/crypto/cryp/signer_private_ctx.h"
#include "ara/crypto/keys/elementary_types.h"
#include "ara/crypto/keys/key_slot_content_props.h"
#include "ara/crypto/keys/key_slot_prototype_props.h"
#include "ara/crypto/keys/keyslot.h"
#include "ara/crypto/keys/updates_observer.h"

namespace ara {
namespace crypto {
namespace keys {
//- @interface KeyStorageProvider
//********************************/
/// @brief Key storage provider interface. Any object is uniquely identified by the combination of its UUID and type.
/// @brief Key Storage Provider interface. Any object is uniquely identified by the combination of its UUID and type.
///         HSMs/TPMs implementing the concept of "non-extractable keys" should use own copies of externally supplied
///         crypto objects. A few software Crypto Providers can share single key slot if they support same format.
/// @code{.isoft}
/// @export_level=/Crypto
/// @trace_id_sws={SWS_CRYPT_30100}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02109}
/// @uptrace={RS_CRYPTO_02305}
/// @uptrace={RS_CRYPTO_02401}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02764
/// @trace_id_dd=DD_CRYPTO_05622
/// @needwork = ad
/// @endcode
class KeyStorageProvider
{
public:
    /// @brief Unique smart pointer for the interface.
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_30101}
    /// @uptrace={RS_CRYPTO_02004}
    /// @tracestatus={draft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_03330
    /// @trace_id_dd=DD_CRYPTO_06661
    /// @needwork = ad
    /// @endcode
    using Uptr = std::unique_ptr< KeyStorageProvider >;

public:
    /// @brief Default copy assignment operator
    /// @brief Copy-assign another KeyStorageProvider to this instance.
    /// @param other Another instance of this class
    /// @returns *this, containing the contents of @a other
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_30222}
    /// @uptrace={RS_CRYPTO_02004}
    /// @tracestatus={draft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02765
    /// @trace_id_dd=DD_CRYPTO_05623
    /// @needwork = ad
    /// @endcode
    KeyStorageProvider& operator=(KeyStorageProvider const& other) = delete;
    /// @brief Default move assignment operator
    /// @brief Move-assign another KeyStorageProvider to this instance.
    /// @param other Another instance of this class
    /// @returns *this, containing the contents of @a other
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_30223}
    /// @uptrace={RS_CRYPTO_02004}
    /// @tracestatus={draft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02766
    /// @trace_id_dd=DD_CRYPTO_05624
    /// @needwork = ad
    /// @endcode
    KeyStorageProvider& operator=(KeyStorageProvider&& other) = delete;
    /// @brief Default copy constructor
    /// @param other Another instance of this class
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02767
    /// @trace_id_dd=DD_CRYPTO_05625
    /// @needwork = ad
    /// @endcode
    KeyStorageProvider(KeyStorageProvider const& other) = delete;
    /// @brief Default move constructor
    /// @param other Another instance of this class
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02768
    /// @trace_id_dd=DD_CRYPTO_05626
    /// @needwork = ad
    /// @endcode
    KeyStorageProvider(KeyStorageProvider&& other) = delete;
    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02769
    /// @trace_id_dd=DD_CRYPTO_05627
    /// @needwork = ad
    /// @endcode
    KeyStorageProvider() = default;
    /// @brief Default virtual destructor
    /// @brief Destructor.
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_30110}
    /// @uptrace={RS_CRYPTO_02004}
    /// @tracestatus={draft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02770
    /// @trace_id_dd=DD_CRYPTO_05628
    /// @needwork = ad
    /// @endcode
    virtual ~KeyStorageProvider() noexcept = default;

public:
    /// @brief Load a key slot. These functions load information associated with a KeySlot into a KeySlot object.
    /// @brief Load a key slot. The functions loads the information associated with a KeySlot into a KeySlot object.
    /// @param iSpecify  the target key-slot instance specifier
    /// @returns an unique smart pointer to allocated key slot
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_30115}
    /// @uptrace={RS_CRYPTO_02004}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @error: SecurityErrorDomain::kUnreservedResource  if the InstanceSpecifier is incorrect
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02771
    /// @trace_id_dd=DD_CRYPTO_05629
    /// @needwork = ad
    /// @endcode
    // PRQA S 2024 QAC /// @qac: False positive
    virtual ara::core::Result< KeySlot::Uptr > LoadKeySlot(ara::core::InstanceSpecifier& iSpecify) noexcept = 0;
    // PRQA L:QAC
    /// @brief Start a new transaction for updating key slots.
    ///        To make a key slot part of a transaction scope, the reserveSpareSlot model parameter of the key slot must be set to true.
    ///        Transactions are dedicated to updating related key slots simultaneously (in an atomic, all-or-nothing way). All key slots that should be updated by the transaction must be opened and provided to this function.
    ///        Any changes to slots within the scope are executed by calling commit().
    /// @brief Begin new transaction for key slots update.
    ///         In order for a keyslot to be part of a transaction scope, the reserveSpareSlot model parameter of the
    ///         keyslot has to be set to true. A transaction is dedicated for updating related key slots simultaneously
    ///         (in an atomic, all-or-nothing, way). All key slots that should be updated by the transaction have to be
    ///         opened and provided to this function. Any changes to the slots in scope are executed by calling
    ///         commit().
    /// @param targetSlots  a list of KeySlots that should be updated during this transaction.
    /// @returns a unique ID assigned to this transaction
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_30123}
    /// @uptrace={RS_CRYPTO_02004}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @error: SecurityErrorDomain::kUnreservedResource     if @c targetSlots list has a slot that has not been
    ///        configured with the reserveSpareSlot parameter in the manifest
    /// @error: SecurityErrorDomain::kBusyResource           if @c targetSlots list has key slots that are already
    ///        involved to another pending transaction or opened in writing mode
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02772
    /// @trace_id_dd=DD_CRYPTO_05630
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< TransactionId > BeginTransaction(TransactionScope const& targetSlots) noexcept = 0;
    /// @brief Commit the changes of the transaction to the Key Storage.
    ///        During transaction execution, any changes to key slots are invisible. The commit command permanently saves all changes made during the transaction in the Key Storage.
    /// @brief Commit changes of the transaction to Key Storage.
    ///         Any changes of key slots made during a transaction are invisible up to the commit execution.
    ///         The commit command permanently saves all changes made during the transaction in Key Storage
    /// @param id  an ID of a transaction that should be commited
    /// @return has value if CommitTransaction sucess false otherwise
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_30124}
    /// @uptrace={RS_CRYPTO_02004}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @error: SecurityErrorDomain::kInvalidArgument    if provided @c id is invalid, i.e. this ID is unknown or
    /// correspondent transaction already was finished (commited or rolled back)
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02773
    /// @trace_id_dd=DD_CRYPTO_05631
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< void > CommitTransaction(TransactionId id) noexcept = 0;
    /// @brief Roll back all changes made during the transaction in the key storage.
    ///        The rollback command permanently cancels all changes made during the transaction in the key storage. Rolling back a transaction is completely invisible to all applications.
    /// @brief Rollback all changes executed during the transaction in Key Storage.
    ///          The rollback command permanently cancels all changes made during the transaction in Key Storage.
    ///          A rolled back transaction is completely invisible for all applications.
    /// @param id  an ID of a transaction that should be rolled back
    /// @return has value if RollbackTransaction sucess false otherwise
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_30125}
    /// @uptrace={RS_CRYPTO_02004}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @error: SecurityErrorDomain::kInvalidArgument    if provided @c id is invalid, i.e. this ID is unknown or
    /// correspondent transaction already was finished (commited or rolled back)
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02774
    /// @trace_id_dd=DD_CRYPTO_05632
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< void > RollbackTransaction(TransactionId id) noexcept = 0;
    /// @brief Get the pointer of the registered update observer. If no observer has been registered yet, this method will return nullptr!
    /// @brief Get pointer of registered Updates Observer. The method returns @c nullptr if no observers have been
    /// registered yet!
    /// @returns    unique pointer to the registered Updates Observer interface (copy of an internal unique pointer is
    /// returned, i.e. the %Key Storage provider continues to keep the ownership)
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_30131}
    /// @uptrace={RS_CRYPTO_02401}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02775
    /// @trace_id_dd=DD_CRYPTO_05633
    /// @needwork = ad
    /// @endcode
    virtual UpdatesObserver::Uptr GetRegisteredObserver() const noexcept = 0;
    /// @brief Register a consumer update observer.
    ///        An application process can only register one UpdatesObserver instance, so this method always unregisters the previous observer and returns its unique pointer.
    ///        If (nullptr == observer), this method only unregisters the previous observer! If no observer has been registered yet, this method will return nullptr!
    /// @brief Register consumer Updates Observer.
    ///         Only one instance of the @c UpdatesObserver may be registered by an application process,
    ///         therefore this method always unregister previous observer and return its unique pointer.
    ///         If (nullptr == observer) then the method only unregister the previous observer! The method returns @c
    ///         nullptr if no observers have been registered yet!
    /// @param observer  optional pointer to a client-supplied @c UpdatesObserver instance that should be registered
    /// inside %Key Storage implementation and called every time, when an opened for usage/loading key slot is updated
    /// externally (by its "Owner" application)
    /// @returns unique pointer to previously registered Updates Observer interface (the pointer ownership is "moved
    ///           out" to the caller code)
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_30130}
    /// @uptrace={RS_CRYPTO_02401}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02776
    /// @trace_id_dd=DD_CRYPTO_05634
    /// @needwork = ad
    /// @endcode
    virtual UpdatesObserver::Uptr RegisterObserver(UpdatesObserver::Uptr observer = {nullptr}) noexcept = 0;
    /// @brief Unsubscribe the update observer from monitoring changes on specified slots.
    /// @brief Unsubscribe the Update Observer from changes monitoring of the specified slot.
    /// @param slot  number of a slot that should be unsubscribed from the updates observing
    /// @return has value if UnsubscribeObserver sucess false otherwise
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_30126}
    /// @uptrace={RS_CRYPTO_02004}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @error: SecurityErrorDomain::kInvalidArgument    if the specified slot is not monitored now (i.e. if it was not
    /// successfully opened via @c OpenAsUser() or it was already unsubscribed by this method)
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02777
    /// @trace_id_dd=DD_CRYPTO_05635
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< void > UnsubscribeObserver(KeySlot& slot) noexcept = 0;

public:
};
//********************************/
}  // namespace keys
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_KEYS_KEY_STORAGE_PROVIDER_H_
