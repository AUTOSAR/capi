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
/// @file       isoft_ipc_key_provider.h
/// @brief      AutoSar-Crypto Key Storage Module
/// @details    Key storage provider interface.
/// @date       2022-01-13
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr><td>2022-01-13  <td>1.0.0    <td>hanjingjing  <td>Initial version creation
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default Key Components/Key Provider
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_05001
/// @unit_name=PIpcKeyProvider
/// @unit_description=Key Provider
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_KEYS_PUHUA_IPC_KEY_PROVIDER_H_
#define ARA_CRYPTO_KEYS_PUHUA_IPC_KEY_PROVIDER_H_

#include <ara/core/map.h>

#include <functional>

#include "ara/crypto/ipc/isoft_ipc_client.h"
#include "ara/crypto/keys/key_storage_provider.h"

namespace ara {
namespace crypto {
namespace keys {
namespace isoft_def {
//********************************/    //- @interface PIpcKeyProvider : isoft_def version key management provider
/// @brief Key storage provider interface. Any object is uniquely identified by the combination of its UUID and type.
///         HSMs/TPMs implementing the concept of "non-exportable keys" should use their own copies of externally provided crypto objects.
///         Some software crypto providers can share a single key slot if they support the same format.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01457
/// @trace_id_dd=DD_CRYPTO_03831
/// @needwork = ad
/// @endcode
class PIpcKeyProvider : public KeyStorageProvider
{
public:
    /// @brief Unique smart pointer type alias
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01457
    /// @trace_id_dd=DD_CRYPTO_06348
    /// @needwork = dd
    /// @endcode
    using Uptr = std::unique_ptr< PIpcKeyProvider >;

public:
    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01458
    /// @trace_id_dd=DD_CRYPTO_03832
    /// @needwork = ad
    /// @endcode
    PIpcKeyProvider() noexcept;
    /// @brief Default virtual destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01459
    /// @trace_id_dd=DD_CRYPTO_03833
    /// @needwork = ad
    /// @endcode
    ~PIpcKeyProvider() noexcept override;
    /// @brief Default copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01460
    /// @trace_id_dd=DD_CRYPTO_03834
    /// @needwork = ad
    /// @endcode
    PIpcKeyProvider(PIpcKeyProvider const& other) = delete;
    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01461
    /// @trace_id_dd=DD_CRYPTO_03835
    /// @needwork = ad
    /// @endcode
    PIpcKeyProvider(PIpcKeyProvider&& other) = delete;
    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01462
    /// @trace_id_dd=DD_CRYPTO_03836
    /// @needwork = ad
    /// @endcode
    PIpcKeyProvider& operator=(PIpcKeyProvider const& other) = delete;
    /// @brief Default move assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01463
    /// @trace_id_dd=DD_CRYPTO_03837
    /// @needwork = ad
    /// @endcode
    PIpcKeyProvider& operator=(PIpcKeyProvider&& other) = delete;

public:
    /// @brief Load key slot. These functions load information associated with a KeySlot into the KeySlot object.
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
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01464
    /// @trace_id_dd=DD_CRYPTO_03838
    /// @needwork = ad
    /// @endcode
    ara::core::Result< KeySlot::Uptr > LoadKeySlot(ara::core::InstanceSpecifier& iSpecify) noexcept override;

public:
    /// @brief Start a new transaction updating key slots.
    ///         To make a keyslot part of the transaction scope, the keyslot's reserveSpareSlot model parameter must be set to true.
    ///         Transactions are dedicated to simultaneously updating related key slots (in an atomic, all-or-nothing manner). All key slots that should be updated by the transaction must be opened and provided to this function.
    ///         Any changes to slots within the scope are executed by calling commit().
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
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01465
    /// @trace_id_dd=DD_CRYPTO_03839
    /// @trace_id_sr=SR_CRYPTO_05002
    /// @needwork = ad
    /// @endcode
    ara::core::Result< TransactionId > BeginTransaction(TransactionScope const& targetSlots) noexcept override;
    /// @brief Commit transaction changes to Key Storage.
    ///         During transaction execution, any changes to key slots are invisible. The commit command permanently saves all changes made during the transaction in Key Storage.
    /// @brief Commit changes of the transaction to Key Storage.
    ///         Any changes of key slots made during a transaction are invisible up to the commit execution.
    ///         The commit command permanently saves all changes made during the transaction in Key Storage
    /// @param id  an ID of a transaction that should be commited
    /// @return has value if CommitTransaction suecess false otherwise
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_30124}
    /// @uptrace={RS_CRYPTO_02004}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @error: SecurityErrorDomain::kInvalidArgument    if provided @c id is invalid, i.e. this ID is unknown or
    /// correspondent transaction already was finished (commited or rolled back)
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01466
    /// @trace_id_dd=DD_CRYPTO_03840
    /// @trace_id_sr=SR_CRYPTO_05002
    /// @needwork = ad
    /// @endcode
    ara::core::Result< void > CommitTransaction(TransactionId id) noexcept override;
    /// @brief Roll back all changes performed during a transaction in key storage.
    ///         The rollback command permanently cancels all changes made during a transaction in key storage. Rolled-back transactions are completely invisible to all applications.
    /// @brief Rollback all changes executed during the transaction in Key Storage.
    ///          The rollback command permanently cancels all changes made during the transaction in Key Storage.
    ///          A rolled back transaction is completely invisible for all applications.
    /// @param id  an ID of a transaction that should be rolled back
    /// @return has value if RollbackTransaction suecess false otherwise
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_30125}
    /// @uptrace={RS_CRYPTO_02004}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @error: SecurityErrorDomain::kInvalidArgument    if provided @c id is invalid, i.e. this ID is unknown or
    /// correspondent transaction already was finished (commited or rolled back)
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01467
    /// @trace_id_dd=DD_CRYPTO_03841
    /// @trace_id_sr=SR_CRYPTO_05002
    /// @needwork = ad
    /// @endcode
    ara::core::Result< void > RollbackTransaction(TransactionId id) noexcept override;

public:
    /// @brief Gets the pointer to the registered update observer. If no observer is registered yet, the method returns nullptr!
    /// Unique pointer pointing to the registered update observer interface (returns a copy of the internal unique pointer, i.e., the key storage provider retains ownership)
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
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01468
    /// @trace_id_dd=DD_CRYPTO_03842
    /// @trace_id_sr=SR_CRYPTO_05003
    /// @needwork = ad
    /// @endcode
    UpdatesObserver::Uptr GetRegisteredObserver() const noexcept override;
    /// @brief Consumer registers an update observer.
    ///         An application process can only register one UpdatesObserver instance, so this method always unregisters the previous observer and returns its unique pointer.
    ///         If (nullptr == observer), then the method only unregisters the previous observer! If no observer was registered, the method returns nullptr!
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
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01469
    /// @trace_id_dd=DD_CRYPTO_03843
    /// @trace_id_sr=SR_CRYPTO_05003
    /// @needwork = ad
    /// @endcode
    UpdatesObserver::Uptr RegisterObserver(UpdatesObserver::Uptr observer = {nullptr}) noexcept override;
    /// @brief Unsubscribe the update observer from monitoring changes to the specified slot.
    /// @brief Unsubscribe the Update Observer from changes monitoring of the specified slot.
    /// @param slot  number of a slot that should be unsubscribed from the updates observing
    /// @return has value if UnsubscribeObserver suecess false otherwise
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_30126}
    /// @uptrace={RS_CRYPTO_02004}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @error: SecurityErrorDomain::kInvalidArgument    if the specified slot is not monitored now (i.e. if it was not
    /// successfully opened via @c OpenAsUser() or it was already unsubscribed by this method)
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01470
    /// @trace_id_dd=DD_CRYPTO_03844
    /// @trace_id_sr=SR_CRYPTO_05003
    /// @needwork = ad
    /// @endcode
    ara::core::Result< void > UnsubscribeObserver(KeySlot& slot) noexcept override;

public:
    /// @brief Execute locally registered observer callbacks
    /// @param nSlotID Key slot ID
    /// @param slotName Key slot name
    /// @return true if do update sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01471
    /// @trace_id_dd=DD_CRYPTO_03845
    /// @needwork = ad
    /// @endcode
    bool DoUpdateObserver(uint32_t const nSlotID, ara::core::StringView const& slotName) const noexcept;
    /// @brief Get IPC client
    /// @return Pointer to IPC client
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01472
    /// @trace_id_dd=DD_CRYPTO_03846
    /// @needwork = ad
    /// @endcode
    PIpcClient* GetClient() const noexcept { return ipcClient_; }
    /// @brief Get transaction prefix
    /// @return Transaction ID prefix
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02548
    /// @trace_id_dd=DD_CRYPTO_00114
    /// @trace_id_sr=SR_CRYPTO_06005
    /// @needwork = dd
    /// @endcode
    static inline std::uint64_t GetTransactionIndexPrefix() noexcept { return kInt_0xFF00000000000000U; }

private:
    /// @brief Registered observer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03847
    /// @needwork = dda
    /// @endcode
    UpdatesObserver::Uptr pRegObserver_{nullptr};
    /// @brief IPC client pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03848
    /// @needwork = dda
    /// @endcode
    PIpcClient* ipcClient_{nullptr};

protected:
    /// @brief Register/uninstall observer callback
    /// @param bReg Whether to register
    /// @return true regist sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03850
    /// @needwork = dda
    /// @endcode
    bool _RegisterObserverIpc(bool const bReg) noexcept;
    /// @brief Copy observer callback
    /// @param pSrcObserver Source registered observer
    /// @return unique pointer to UpdatesObserver
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03851
    /// @needwork = dda
    /// @endcode
    static UpdatesObserver::Uptr CopyUpdateObserver(UpdatesObserver const* const pSrcObserver) noexcept;

public:
};
//********************************/
}  // namespace  isoft_def
}  // namespace keys
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_KEYS_PUHUA_IPC_KEY_PROVIDER_H_
