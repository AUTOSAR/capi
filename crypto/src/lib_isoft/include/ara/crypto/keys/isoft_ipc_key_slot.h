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
/// @file       isoft_ipc_key_slot.h
/// @brief      AutoSar-Crypto Key Storage Module
/// @details    Key slot interface for IPC client: Client key slot
/// @date       2022-08-15
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr><td>2022-08-15  <td>1.0.0    <td>hanjingjing  <td>Initial version creation
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default Key Components/Key Slot
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_05004
/// @unit_name=PIpcKeySlot
/// @unit_description=Key slot interface for IPC client
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_KEYS_PUHUA_IPC_KEY_SLOT_H_
#define ARA_CRYPTO_KEYS_PUHUA_IPC_KEY_SLOT_H_

#include "ara/crypto/ipc/isoft_ipc_client.h"
#include "ara/crypto/keys/isoft_ipc_io_interface.h"
#include "ara/crypto/keys/isoft_updates_observer.h"
#include "ara/crypto/keys/keyslot.h"

namespace ara {
namespace crypto {
namespace keys {
namespace isoft_def {
//********************************/
/// @brief Key slot interface for IPC client: Client key slot
/// @brief Key slot port-prototype interface. This class enables access to a physicl key-slot
/// @code{.isoft}
/// @trace_id_sws={SWS_CRYPT_30400}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02405}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01435
/// @trace_id_dd=DD_CRYPTO_03806
/// @needwork = ad
/// @endcode
class PIpcKeySlot : public KeySlot
{
public:
    /// @brief Key slot open mode enumeration
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01436
    /// @trace_id_dd=DD_CRYPTO_03807
    /// @needwork = ad
    /// @endcode
    enum class ESlotOpenModel : uint32_t
    {
        kClose     = 0,  // Closed/Initialized
        kReadOnly  = 1,  // Open read-only
        kReadWrite = 2,  // Open read/write
    };
    /// @brief Unique smart pointer of the interface.
    /// @brief Unique smart pointer of the interface.
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_30402}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02405}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01435
    /// @trace_id_dd=DD_CRYPTO_06347
    /// @needwork = dd
    /// @endcode
    using Uptr = std::unique_ptr< PIpcKeySlot >;

public:
    /// @brief Parameterized constructor
    /// @param ipcClient IPC client
    /// @param stSlotName Key slot name
    /// @param nIpcSlotID IPC key slot ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01437
    /// @trace_id_dd=DD_CRYPTO_03808
    /// @needwork = ad
    /// @endcode
    explicit PIpcKeySlot(PIpcClient const &ipcClient,
                         ara::core::StringView const &stSlotName,
                         uint32_t const nIpcSlotID) noexcept;
    /// @brief Default virtual destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01438
    /// @trace_id_dd=DD_CRYPTO_03809
    /// @needwork = ad
    /// @endcode
    ~PIpcKeySlot() noexcept override = default;
    /// @brief Default copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01439
    /// @trace_id_dd=DD_CRYPTO_03810
    /// @needwork = ad
    /// @endcode
    PIpcKeySlot(PIpcKeySlot const &other) = delete;
    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01440
    /// @trace_id_dd=DD_CRYPTO_03811
    /// @needwork = ad
    /// @endcode
    PIpcKeySlot(PIpcKeySlot &&other) = delete;
    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01441
    /// @trace_id_dd=DD_CRYPTO_03812
    /// @needwork = ad
    /// @endcode
    PIpcKeySlot &operator=(PIpcKeySlot const &other) = delete;
    /// @brief Default move assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01442
    /// @trace_id_dd=DD_CRYPTO_03813
    /// @needwork = ad
    /// @endcode
    PIpcKeySlot &operator=(PIpcKeySlot &&other) = delete;

public:  // KeySlot interface
    /// @brief Retrieves the CryptoProvider instance owning this KeySlot. Any key slot always has an associated default crypto provider capable of serving this key slot.
    ///         In the simplest case, all key slots can be served by a single crypto provider installed on the Adaptive Platform.
    ///         But in more complex cases, several different crypto providers may coexist in the system,
    ///         for example, if an ECU has one or several HSMs and software crypto implementations, each with its own physical key storage. In such cases, different dedicated crypto providers may serve the aforementioned HSMs and software implementations.
    /// @brief Retrieve an instance of the CryptoProvider that owns this KeySlot.
    ///         Any key slot always has an associated default Crypto Provider that can serve this key slot.
    ///         In the simplest case all key slots can be served by a single Crypto Provider installed on the Adaptive
    ///         Platform. But in a more complicated case a few different Crypto Providers may coexist in the system,
    ///         for example if ECU has one or a few HSMs and software cryptography implementation too, and each of them
    ///         has own physical key storage. In such case different dedicated Crypto Providers may serve mentioned HSMs
    ///         and the software implementation.
    /// @returns a unique_pointer to the CryptoProvider to be used with this KeySlot
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_30403}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02401}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01443
    /// @trace_id_dd=DD_CRYPTO_03814
    /// @needwork = ad
    /// @endcode
    ara::core::Result< cryp::CryptoProvider::Uptr > MyProvider() const noexcept override;
    /// @brief Clears the contents of this key-slot.
    /// This method must perform secure cleanup but cannot recover object data!
    /// This method can be used for atomic updates of key slots scoped to a certain transaction. In this case, the slot will only be updated after the corresponding CommitTransaction() call.
    /// @return has vlaue if clear sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01444
    /// @trace_id_dd=DD_CRYPTO_03815
    /// @needwork = ad
    /// @endcode
    ara::core::Result< void > Clear() noexcept override;
    /// @brief Checks if the slot is empty.
    /// @return true if is empty false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01445
    /// @trace_id_dd=DD_CRYPTO_03816
    /// @needwork = ad
    /// @endcode
    bool IsEmpty() const noexcept override;
    /// @brief Gets the actual properties of the content in the key slot. If this method is called by the "User" Actor, then always: props.exportability == false.

    /// @brief Get an actual properties of a content in the key slot.
    ///        If this method called by a "User" Actor then always: <tt>props.exportability == false</tt>.
    /// @return Key slot content property information
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kEmptyContainer     if the slot is empty
    /// @error: SecurityErrorDomain::kAccessViolation    if this method is called by an Actor, which has no any ("Owner"
    /// or "User") access rights to the key slot
    /// @trace_id_sws={SWS_CRYPT_30408}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02004}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01446
    /// @trace_id_dd=DD_CRYPTO_03817
    /// @needwork = ad
    /// @endcode
    ara::core::Result< KeySlotContentProps > GetContentProps() const noexcept override;
    /// @brief Gets the prototype properties of the key slot.
    /// @brief Get the prototyped properties of the key slot.
    /// @return Key slot property information
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_30407}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02110}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01447
    /// @trace_id_dd=DD_CRYPTO_03818
    /// @needwork = ad
    /// @endcode
    ara::core::Result< KeySlotPrototypeProps > GetPrototypedProps() const noexcept override;
    /// @brief Opens this key slot and returns an IOInterface to its contents.
    ///         If the UpdatesObserver interface is registered via RegisterObserver() call, then whenever this slot is updated (and becomes visible to "Users"),
    ///         the Key Storage engine (in a dedicated thread) should call the UpdatesObserver::OnUpdate() method.
    ///         Monitoring of the opened key slot will continue even after the returned TrustedContainer is destroyed,
    ///         because the slot contents might be loaded into volatile memory (as a cryptobject or a CryptoContext for a crypto primitive), but the TrustedContainer might be destroyed afterwards.
    ///         Therefore, if you need to terminate monitoring of the key slot, you should directly call the UnsubscribeObserver(SlotNumber) method.
    /// @brief Open this key slot and return an IOInterface to its content.
    ///         If the @c UpdatesObserver interface was provided to the call of @c RegisterObserver() then the @c
    ///         UpdatesObserver::OnUpdate() method should be called by %Key Storage engine (in a dedicated thread) every
    ///         time when this slot is updated (and become visible for "Users"). Monitoring of the opened key slot will
    ///         be continued even after destruction of the returned @c TrustedContainer, because content of the slot may
    ///         be loaded to volatile memory (as a @c CryptoObject or to a @c CryptoContext of a crypto primitive), but
    ///         the @c TrustedContainer may be destroyed after this. Therefore if you need to terminate monitoring of
    ///         the key slot then you should directly call method @c UnsubscribeObserver(SlotNumber).
    /// @param subscribeForUpdates  if this flag is @c true then the @c UpdatesObserver instance (previously
    ///            registered by a call of the method @c RegisterObserver()) will be subscribed for updates of the
    ///            opened key slot
    /// @param writeable indicates whether the key-slot shall be opened read-only (default) or with write access
    /// @returns an unique smart pointer to the IOInterface associated with the slot content
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kInvalidUsageOrder  if (true == subscribeForUpdates),
    ///                but there is no registered instance of the @c UpdatesObserver in the %Key Storage Provider
    ///                context
    /// @error: SecurityErrorDomain::kBusyResource       if the specified slot is busy because writeable == true but
    ///                 (a) the keyslot is already opened writable, and/or
    ///                 (b) the keyslot is in scope of another ongoing transaction
    /// @error: SecurityErrorDomain::kModifiedResource   if the specified slot has been modified after the KeySlot has
    /// been opened
    /// @trace_id_sws={SWS_CRYPT_30409}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02004}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01448
    /// @trace_id_dd=DD_CRYPTO_03819
    /// @needwork = ad
    /// @endcode
    ara::core::Result< IOInterface::Uptr > Open(bool subscribeForUpdates = false,
                                                bool writeable           = false) const noexcept override;
    /// @brief Saves the contents of the provided source IOInterface to this key-slot. The source container can represent a volatile trusted container or another KeySlot.
    ///         This method can be used for atomic updates of key slots scoped to a certain transaction. In this case, the slot will only be updated after the corresponding CommitTransaction() call.
    /// @brief Save the content of a provided source IOInterface to this key-slot.
    ///        The source container may represent a volatile trusted container or another KeySlot
    ///        This method may be used for atomic update of a key slot scoped to some transaction.
    ///        In such case the slot will be updated only after correspondent call of @c CommitTransaction().
    /// @param container  the source IOInterface
    /// @returns @c true if successfully saved
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kIncompatibleObject     if the source object has property "session" or if the
    ///                source IOInterface references a KeySlot from a different CryptoProvider
    /// @error: SecurityErrorDomain::kEmptyContainer         if the source IOInterface is empty
    /// @error: SecurityErrorDomain::kContentRestrictions    if the source object doesn't satisfy the slot restrictions
    /// (including version control)
    /// @error: SecurityErrorDomain::kUnreservedResource     if the target slot is not opened writeable.
    /// @trace_id_sws={SWS_CRYPT_30406}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02004}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01449
    /// @trace_id_dd=DD_CRYPTO_03820
    /// @needwork = ad
    /// @endcode
    ara::core::Result< void > SaveCopy(IOInterface const &container) noexcept override;
    /// @brief Save key passed via IPC PIoInterface_Mem
    /// @param nSlotID Key slot ID
    /// @param container IO interface
    /// @return has value if save memory sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01450
    /// @trace_id_dd=DD_CRYPTO_03821
    /// @needwork = ad
    /// @endcode
    ara::core::Result< void > SaveMemory(uint32_t const nSlotID, IOInterface const &container) noexcept;
    /// @brief Non-standard interface: Close key slot. Must be closed after modification operations are written; otherwise, this key slot cannot be modified again until the key manager restarts.
    /// @return true if close sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01451
    /// @trace_id_dd=DD_CRYPTO_03822
    /// @needwork = ad
    /// @endcode
    ara::core::Result< bool > Close() const noexcept override;
    /// @brief Get property information from configuration file.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01452
    /// @trace_id_dd=DD_CRYPTO_03823
    /// @needwork = ad
    /// @endcode
    ara::core::Result< KeySlotPrototypeProps >
    /// @brief Get the prototyped properties of the key slot.
    /// @return
    GetKeySlotPropsFromJson() const noexcept override;
    /// @brief Reset key slot
    /// @param keySlotPrototypeProps Key slot properties
    /// @return has vlaue if reset keyslot sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01453
    /// @trace_id_dd=DD_CRYPTO_03824
    /// @needwork = ad
    /// @endcode
    ara::core::Result< void > ResetKeySlot(KeySlotPrototypeProps const &keySlotPrototypeProps) noexcept override;

private:
    /// @brief IPC client
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03825
    /// @needwork = dda
    /// @endcode
    PIpcClient const &ipcClient_;
    /// @brief Key slot name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03826
    /// @needwork = dda
    /// @endcode
    ara::core::String stSlotName_;
    /// @brief KeySlot identification ID: The first packet uses KeyName, other situations use SlotID to identify the key slot
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03827
    /// @needwork = dda
    /// @endcode
    uint32_t nIpcSlotID_;

public:
    /// @brief Returns the name of the key slot
    /// @return Name of the key slot
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01454
    /// @trace_id_dd=DD_CRYPTO_03828
    /// @needwork = ad
    /// @endcode
    ara::core::StringView GetKeySlotName() const noexcept;
    /// @brief Returns the registered ID of the key slot
    /// @name  GetIpcSlotID
    /// @return Registered ID of the key slot
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01455
    /// @trace_id_dd=DD_CRYPTO_03829
    /// @needwork = ad
    /// @endcode
    inline uint32_t GetIpcSlotID() const noexcept { return nIpcSlotID_; };
    /// @brief Returns the PIOInterface used by itself
    /// @param nInterfaceID IO interface ID
    /// @return a unique_pointer to the IOInterface
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01456
    /// @trace_id_dd=DD_CRYPTO_03830
    /// @needwork = ad
    /// @endcode
    PIoInterface_Ipc::Uptr MakeMainIoInterface(uint32_t const nInterfaceID) const noexcept;
};
//********************************/
}  // namespace  isoft_def
}  // namespace keys
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_KEYS_PUHUA_IPC_KEY_SLOT_H_
