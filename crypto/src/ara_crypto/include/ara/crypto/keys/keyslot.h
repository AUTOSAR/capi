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
/// @file       keyslot.h
/// @brief      AutoSar-Crypto Key Storage Module
/// @details    Key slot port-prototype interface.
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
/// @module_path=/CRYPTO/Default Key Components/Key Slot
/// @interface_level=software
/// @trace_id_sr=SR_CRYPTO_05004
/// @unit_name=PIpcKeySlot
/// @unit_description=Key Slot Base Class
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_KEYS_KEYSLOT_PORT_PROTOTYPE_H_
#define ARA_CRYPTO_KEYS_KEYSLOT_PORT_PROTOTYPE_H_

#include "ara/core/result.h"
#include "ara/crypto/common/io_interface.h"
#include "ara/crypto/cryp/crypto_provider.h"
#include "ara/crypto/keys/key_slot_content_props.h"
#include "ara/crypto/keys/key_slot_prototype_props.h"

namespace ara {
namespace crypto {
namespace keys {
//- @interface KeySlot
//********************************/
/// @brief Key slot port-prototype interface. This class allows access to physical key slots.
/// @brief Key slot port-prototype interface. This class enables access to a physicl key-slot
/// @trace_id_sws={SWS_CRYPT_30400}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02405}
//********************************/
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02794
/// @trace_id_dd=DD_CRYPTO_05652
/// @needwork = ad
/// @endcode
class KeySlot
{
public:
    /// @brief Unique smart pointer for the interface.
    /// @brief Unique smart pointer of the interface.
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_30402}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02405}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_03322
    /// @trace_id_dd=DD_CRYPTO_06644
    /// @needwork = ad
    /// @endcode
    using Uptr = std::unique_ptr< KeySlot >;

public:
    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02795
    /// @trace_id_dd=DD_CRYPTO_05653
    /// @needwork = ad
    /// @endcode
    KeySlot() = default;
    /// @brief Default virtual destructor
    /// @brief Destructor.
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_30401}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02405}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02796
    /// @trace_id_dd=DD_CRYPTO_05654
    /// @needwork = ad
    /// @endcode
    virtual ~KeySlot() noexcept = default;

public:
    /// @brief Default copy assignment operator
    /// @brief Copy-assign another KeySlot to this instance.
    /// @param other Another instance of this class
    /// @returns *this, containing the contents of @a other
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_30220}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02004}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05655
    /// @needwork = dda
    /// @endcode
    KeySlot& operator=(KeySlot const& other) = delete;
    /// @brief Default move assignment operator
    /// @brief Move-assign another KeySlot to this instance.
    /// @param other Another instance of this class
    /// @returns *this, containing the contents of @a other
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_30221}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02004}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05656
    /// @needwork = dda
    /// @endcode
    KeySlot& operator=(KeySlot&& other) = delete;

public:
    /// @brief Default copy constructor
    /// @param other Another instance of this class
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05657
    /// @needwork = dda
    /// @endcode
    KeySlot(KeySlot const& other) = delete;
    /// @brief Default move constructor
    /// @param other Another instance of this class
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05658
    /// @needwork = dda
    /// @endcode
    KeySlot(KeySlot&& other) = delete;

public:
    /// @brief Clear the content of this key-slot.
    ///         This method must perform secure cleanup, but cannot restore object data!
    ///         This method can be used for atomic update of key slots scoped within a transaction. In this case, the slot is only updated after the corresponding CommitTransaction() call.
    /// @brief Clear the content of this key-slot.
    ///         This method must perform a secure cleanup without the ability to restore the object data!
    ///         This method may be used for atomic update of a key slot scoped to some transaction.
    ///         In such case the the slot will be updated only after correspondent call of @c CommitTransaction().
    /// @return has value if Clear sucess false otherwise
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_30405}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02009}
    /// @error: SecurityErrorDomain::kUnreservedResource     if the target slot is not opened writeable.
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02797
    /// @trace_id_dd=DD_CRYPTO_05659
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< void > Clear() noexcept = 0;
    /// @brief Get the actual attributes of the content in the key slot. If this method is called by the "User" Actor, it always holds: props.exportability == false.
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_30408}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02004}
    /// @error: SecurityErrorDomain::kEmptyContainer     if the slot is empty
    /// @error: SecurityErrorDomain::kAccessViolation    if this method is called by an Actor, which has no any ("Owner"
    /// or "User") access rights to the key slot
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02798
    /// @trace_id_dd=DD_CRYPTO_05660
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< KeySlotContentProps >
    /// @brief Get an actual properties of a content in the key slot.
    ///        If this method called by a "User" Actor then always: <tt>props.exportability == false</tt>.
    /// @return Key slot content attribute information
    GetContentProps() const noexcept = 0;
    /// @brief Retrieve the instance of the CryptoProvider that owns this KeySlot. Any key slot always has an associated default crypto provider that can service this key slot.
    ///         In the simplest case, all key slots can be serviced by a single crypto provider installed on the Adaptive Platform.
    ///         However, in more complex cases, there might be several different crypto providers coexisting in the system,
    ///         for example, if the ECU has one or several HSMs and software crypto implementations, each with its own physical key storage. In such cases, different dedicated crypto providers might service the aforementioned HSMs and software implementations.
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_30403}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02401}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02799
    /// @trace_id_dd=DD_CRYPTO_05661
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< cryp::CryptoProvider::Uptr >
    /// @brief Retrieve an instance of the CryptoProvider that owns this KeySlot.
    ///        Any key slot always has an associated default Crypto Provider that can serve this key slot.
    ///       In the simplest case all key slots can be served by a single Crypto Provider installed on the Adaptive
    ///       Platform. But in a more complicated case a few different Crypto Providers may coexist in the system,
    ///       for example if ECU has one or a few HSMs and software cryptography implementation too, and each of them
    ///       has own physical key storage. In such case different dedicated Crypto Providers may serve mentioned HSMs
    ///       and the software implementation.
    /// @returns a unique_pointer to the CryptoProvider to be used with this KeySlot
    MyProvider() const noexcept = 0;
    /// @brief Get the prototype attributes of the key slot.
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_30407}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02110}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02800
    /// @trace_id_dd=DD_CRYPTO_05662
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< KeySlotPrototypeProps >
    /// @brief Get the prototyped properties of the key slot.
    /// @return Key slot attribute information
    GetPrototypedProps() const noexcept = 0;
    /// @brief Check if the slot is empty.
    /// @brief Check the slot for emptiness.
    /// @returns @c true if the slot is empty or @c false otherwise
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_30404}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02004}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02801
    /// @trace_id_dd=DD_CRYPTO_05663
    /// @needwork = ad
    /// @endcode
    virtual bool IsEmpty() const noexcept = 0;
    /// @brief Open this key slot and return an IOInterface to its content.
    ///         If an UpdatesObserver interface has been registered via the RegisterObserver() call, then each time this slot is updated (and becomes "user"-visible), the Key
    ///         Storage engine (in a dedicated thread) should call the UpdatesObserver::OnUpdate() method.
    ///         Even after the returned TrustedContainer is destroyed, monitoring of the opened key slot will continue,
    ///         because the slot's content may be loaded into volatile memory (as a crypto object or a CryptoContext of a crypto primitive), but after that, the TrustedContainer may be destroyed.
    ///         Therefore, if you need to terminate monitoring of a key slot, you should directly call the UnsubscribeObserver(SlotNumber) method.
    ///         If the @c UpdatesObserver interface was provided to the call of @c RegisterObserver() then the @c
    ///         UpdatesObserver::OnUpdate() method should be called by %Key Storage engine (in a dedicated thread) every
    ///         time when this slot is updated (and become visible for "Users"). Monitoring of the opened key slot will
    ///         be continued even after destruction of the returned @c TrustedContainer, because content of the slot may
    ///         be loaded to volatile memory (as a @c CryptoObject or to a @c CryptoContext of a crypto primitive), but
    ///         the @c TrustedContainer may be destroyed after this. Therefore if you need to terminate monitoring of
    ///         the key slot then you should directly call method @c UnsubscribeObserver(SlotNumber).
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
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02802
    /// @trace_id_dd=DD_CRYPTO_05664
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< IOInterface::Uptr >
    /// @brief Open this key slot and return an IOInterface to its content.
    /// @param subscribeForUpdates  if this flag is @c true then the @c UpdatesObserver instance previously
    /// registered by a call of the method @c RegisterObserver() will be subscribed for updates of the opened key slot
    /// @param writeable indicates whether the key-slot shall be opened read-only (default) or with write access
    /// @return an unique smart pointer to the IOInterface associated with the slot content
    Open(bool subscribeForUpdates = false, bool writeable = false) const noexcept = 0;
    /// @brief Save the content of the provided source IOInterface to this key-slot. The source container can represent a volatile trusted container or another KeySlot.
    ///         This method can be used for atomic update of key slots scoped within a transaction. In this case, the slot is only updated after the corresponding CommitTransaction() call.
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
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02803
    /// @trace_id_dd=DD_CRYPTO_05665
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< void > SaveCopy(IOInterface const& container) noexcept = 0;
    /// @brief Non-standard interface: Close the key slot. After completing write operations, the key slot must be closed, otherwise it cannot be modified again before the key manager restarts.
    /// @return has value if Close sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02804
    /// @trace_id_dd=DD_CRYPTO_05666
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< bool > Close() const noexcept = 0;
    /// @brief Get attribute information from the configuration file.
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_30407}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02110}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02805
    /// @trace_id_dd=DD_CRYPTO_05667
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< KeySlotPrototypeProps >
    /// @brief Get the prototyped properties of the key slot.
    /// @return Key slot attribute information
    GetKeySlotPropsFromJson() const noexcept = 0;
    /// @brief Reset key slot attribute information
    /// @param keySlotPrototypeProps Key slot attributes
    /// @return has value if ResetKeySlot sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02806
    /// @trace_id_dd=DD_CRYPTO_05668
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< void > ResetKeySlot(KeySlotPrototypeProps const& keySlotPrototypeProps) noexcept = 0;
};
//********************************/
}  // namespace keys
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_KEYS_KEYSLOT_PORT_PROTOTYPE_H_
