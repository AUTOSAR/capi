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
/// @file       isoft_svr_key_slot_loader.h
/// @brief      AutoSar-Crypto key storage module
/// @details    Key slot loader on the IPC server side
/// @date       2022-08-11
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par modification log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr>2022-08-11  <td>1.0.0    <td>HAN YUXIN      <td>Create initial version
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/key manager/key loader
/// @interface_level=module
/// @trace_id_sr=SR_CRYPTO_01008
/// @unit_name=PSvrKeySlotLoader
/// @unit_description=Key loader
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_KEYS_PUHUA_SVR_KEY_SLOT_LOADER_H_
#define ARA_CRYPTO_KEYS_PUHUA_SVR_KEY_SLOT_LOADER_H_

#include <ara/core/map.h>
#include <ara/core/vector.h>

#include "ara/crypto/common/isoft_io_interface.h"
#include "ara/crypto/common/isoft_io_interface_mem.h"
#include "ara/crypto/common/isoft_thread_lock.h"
#include "ara/crypto/ipc/isoft_ipc_protocol.h"
#include "ara/crypto/keys/isoft_updates_observer.h"
#include "ara/crypto/keys/keyslot.h"
#include "ara/crypto/ksp/isoft_io_interface_kv_share.h"
#include "ara/crypto/ksp/isoft_svr_io_interface_agent.h"

namespace ara {
namespace crypto {
namespace keys {
namespace isoft_def {
//********************************/
namespace internal {
/// @brief Key slot storage name; this name cannot be changed and must be used in the ARXML
/// @return cryptod/cryptod_swc_root/rPort_kvStorage_keySlot
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_CRYPTO_02899
/// @trace_id_dd=DD_CRYPTO_05815
/// @needwork = ad
/// @endcode
inline constexpr ara::crypto::char8_t const *GetSlotStorageName() noexcept
{
    return "cryptod/cryptod_swc_root/prPort_kvStorage_keySlot";
}
}  // namespace internal
// -------------------------------------------- //
/// @brief Used to handle global IO Agent Index
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_04348
/// @trace_id_dd=DD_CRYPTO_08761
/// @needwork = ad
/// @endcode
class KeySlotIndex
{
public:
    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_08762
    /// @needwork = dda
    /// @endcode
    KeySlotIndex() noexcept = default;
    /// @brief Default destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_08763
    /// @needwork = dda
    /// @endcode
    ~KeySlotIndex() noexcept = default;
    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_08764
    /// @needwork = dda
    /// @endcode
    KeySlotIndex(KeySlotIndex &&other) = delete;
    /// @brief Default copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_08765
    /// @needwork = dda
    /// @endcode
    KeySlotIndex(KeySlotIndex const &other) = delete;
    /// @brief Default move assignment function
    /// @param other Another object instance of this class
    /// @return PSvrIoInterfaceAgent_Hsmbst&
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_08766
    /// @needwork = dda
    /// @endcode
    KeySlotIndex &operator=(KeySlotIndex &&other) = delete;
    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @return PSvrIoInterfaceAgent_Hsmbst&
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_08767
    /// @needwork = dda
    /// @endcode
    KeySlotIndex &operator=(KeySlotIndex const &other) = delete;

public:
    /// @brief Get the key slot sequence number
    /// @return Key slot sequence number
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_08768
    /// @needwork = dda
    /// @endcode
    uint32_t GetKeySlotIndex() noexcept
    {
        ara::crypto::PAutoLock const autoLock{mThreadLockIndex_};
        mKeySlotIndex_ += 1U;
        return mKeySlotIndex_;
    }

private:
    /// @brief Global key slot index
    /// @name   mKeySlotIndex_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_08769
    /// @needwork = dda
    /// @endcode
    uint32_t mKeySlotIndex_{0U};
    /// @brief Thread lock
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_08770
    /// @needwork = dda
    /// @endcode
    ara::crypto::PThreadLock mThreadLockIndex_{};
};
//********************************/
/// @brief forward declaration
class PKeys_Manager;
//********************************/
/// @brief Key slot loader on the IPC server side
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_CRYPTO_02900
/// @trace_id_dd=DD_CRYPTO_05816
/// @needwork = ad
/// @endcode
class PSvrKeySlotLoader
{
public:
    /// @brief Key slot open mode enumeration // permissions requested by the thread operating the key slot
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02901
    /// @trace_id_dd=DD_CRYPTO_05817
    /// @needwork = ad
    /// @endcode
    enum class EnumSlotOpenModal : uint32_t
    {
        kClose     = 0,  // Closed/initialized
        kReadOnly  = 1,  // Opened read-only
        kReadWrite = 2,  // Opened read-write
    };
    /// @brief Unique smart pointer of the interface.
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02900
    /// @trace_id_dd=DD_CRYPTO_06468
    /// @needwork = dd
    /// @endcode
    using Uptr = std::unique_ptr< PSvrKeySlotLoader >;
    /// @brief ara::core::Map<uint32_t, PSvrIoInterfaceAgent::Uptr> declaration
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02900
    /// @trace_id_dd=DD_CRYPTO_06469
    /// @needwork = dd
    /// @endcode
    using MAP_AgentIoInterface = ara::core::Map< uint32_t, PSvrIoInterfaceAgent::Uptr >;
    /// @brief ara::core::Map<uint64_t, EnumSlotOpenModal> declaration
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02900
    /// @trace_id_dd=DD_CRYPTO_06470
    /// @needwork = dd
    /// @endcode
    using MAP_ObserverPid = ara::core::Map< uint64_t, EnumSlotOpenModal >;
    /// @brief ara::core::Map<uint64_t, uint32_t> declaration
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02900
    /// @trace_id_dd=DD_CRYPTO_06471
    /// @needwork = dd
    /// @endcode
    using MAP_WritePid = ara::core::Map< uint64_t, uint32_t >;

public:
    /// @brief Create a new key slot loader
    /// @param keySlotManager Key slot manager
    /// @param stCertName Certificate name
    /// @return PSvrKeySlotLoader smart pointer
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02902
    /// @trace_id_dd=DD_CRYPTO_05818
    /// @needwork = ad
    /// @endcode
    static PSvrKeySlotLoader::Uptr NewKeySlotLoader(PKeys_Manager &keySlotManager,
                                                    ara::core::StringView const &stCertName) noexcept;

public:
    /// @brief Constructor
    /// @param keySlotManager Key slot manager
    /// @param stStorageName Persistent database name
    /// @param stSlotName Key slot name
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02903
    /// @trace_id_dd=DD_CRYPTO_05819
    /// @needwork = ad
    /// @endcode
    explicit PSvrKeySlotLoader(PKeys_Manager &keySlotManager,
                               ara::core::StringView const &stStorageName,
                               ara::core::StringView const &stSlotName) noexcept;
    /// @brief Default virtual destructor
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02904
    /// @trace_id_dd=DD_CRYPTO_05820
    /// @needwork = ad
    /// @endcode
    virtual ~PSvrKeySlotLoader() noexcept = default;
    /// @brief Default copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02905
    /// @trace_id_dd=DD_CRYPTO_05821
    /// @needwork = ad
    /// @endcode
    PSvrKeySlotLoader(PSvrKeySlotLoader const &other) = delete;
    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02906
    /// @trace_id_dd=DD_CRYPTO_05822
    /// @needwork = ad
    /// @endcode
    PSvrKeySlotLoader(PSvrKeySlotLoader &&other) = delete;
    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02907
    /// @trace_id_dd=DD_CRYPTO_05823
    /// @needwork = ad
    /// @endcode
    PSvrKeySlotLoader &operator=(PSvrKeySlotLoader const &other) = delete;
    /// @brief Default move assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02908
    /// @trace_id_dd=DD_CRYPTO_05824
    /// @needwork = ad
    /// @endcode
    PSvrKeySlotLoader &operator=(PSvrKeySlotLoader &&other) = delete;

private:
    /// @brief Key manager object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05825
    /// @needwork = dda
    /// @endcode
    PKeys_Manager &ipcProcessManager_;
    /// @brief Persistent storage name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05826
    /// @needwork = dda
    /// @endcode
    ara::core::String stStorageName_{""};
    /// @brief Key slot name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05827
    /// @needwork = dda
    /// @endcode
    ara::core::String stSlotName_{""};
    /// @brief KV interface wrapper
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05828
    /// @needwork = dda
    /// @endcode
    PIoInterface_KvShare::Uptr pMainIoInterface_{nullptr};
    /// @brief Key slot ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05829
    /// @needwork = dda
    /// @endcode
    uint32_t nSlotID_{0U};
    /// @brief MAP list of key slot data loaded into memory
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05830
    /// @needwork = dda
    /// @endcode
    mutable MAP_AgentIoInterface mapMemIoInterface_{};
    /// @brief List of client process IDs monitoring this key slot
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05831
    /// @needwork = dda
    /// @endcode
    mutable MAP_ObserverPid mapRegObserverPid_{};
    /// @brief MAP list of writing processes
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05832
    /// @needwork = dda
    /// @endcode
    mutable MAP_WritePid mapPidByWrite_{};
    /// @brief Unique agent ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_08785
    /// @needwork = dda
    /// @endcode
    static KeySlotIndex s_KeySlotIndex_;

    /// @brief Full parameter constructor (for qac)
    /// @param keySlotManager Key slot manager
    /// @param stStorageName Persistent database name
    /// @param stSlotName Key slot name
    /// @param pMainIoInterface IO interface pointer
    /// @param nSlotID Key slot ID
    /// @param mapMemIoInterface MAP list of key slot data loaded into memory
    /// @param mapRegObserverPid List of client process IDs monitoring this key slot
    /// @param mapPidByWrite MAP list of writing processes
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05834
    /// @needwork = dda
    /// @endcode
    PSvrKeySlotLoader(PKeys_Manager &keySlotManager,
                      ara::core::StringView const &stStorageName,
                      ara::core::StringView const &stSlotName,
                      PIoInterface_KvShare::Uptr pMainIoInterface,
                      uint32_t const nSlotID,
                      MAP_AgentIoInterface const &mapMemIoInterface,
                      MAP_ObserverPid mapRegObserverPid,
                      MAP_WritePid mapPidByWrite) noexcept;

public:
    /// @brief Get slot name
    /// @name  GetKeySlotName
    /// @returns Slot name
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02909
    /// @trace_id_dd=DD_CRYPTO_05835
    /// @needwork = ad
    /// @endcode
    ara::core::StringView GetKeySlotName() const noexcept { return T_StringView(stSlotName_); }
    /// @brief Get slot ID
    /// @name   GetSlotID
    /// @returns  SlotID
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02910
    /// @trace_id_dd=DD_CRYPTO_05836
    /// @needwork = ad
    /// @endcode
    inline uint32_t GetSlotID() const noexcept { return nSlotID_; }
    /// @brief Find the corresponding memory IO
    /// @name   FindIoInterface
    /// @param nSlotID Key slot ID
    /// @param nIoInterfaceID IO interface ID
    /// @returns PSvrIoInterfaceAgent pointer
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02911
    /// @trace_id_dd=DD_CRYPTO_05837
    /// @needwork = ad
    /// @endcode
    PSvrIoInterfaceAgent *FindIoInterface(uint32_t const nSlotID, uint32_t const nIoInterfaceID) const noexcept;

public:
    /// @brief Clear the content of this key slot
    /// @name   Clear
    /// @param nProcessID Process ID
    /// @return has vlaue if clear sucess
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02912
    /// @trace_id_dd=DD_CRYPTO_05838
    /// @needwork = ad
    /// @endcode
    ara::core::Result< void > Clear(uint64_t const nProcessId) const noexcept;
    /// @brief Check whether the slot is empty.
    /// @name   IsEmpty
    /// @returns true if empty
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02913
    /// @trace_id_dd=DD_CRYPTO_05839
    /// @needwork = ad
    /// @endcode
    ara::core::Result< bool > IsEmpty() const noexcept;
    /// @brief Get the actual attributes of the content in the key slot. If this method is called by a "User" Actor, it is always: props.exportability == false.
    /// @name   GetKeyContent
    /// @returns Actual attributes of the content in the key slot
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02914
    /// @trace_id_dd=DD_CRYPTO_05840
    /// @needwork = ad
    /// @endcode
    ara::core::Result< KeySlotContentProps > GetKeyContent() const noexcept;
    /// @brief Get the prototype attributes of the key slot.
    /// @name   GetSlotProps
    /// @returns Prototype attributes of the key slot
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02915
    /// @trace_id_dd=DD_CRYPTO_05841
    /// @needwork = ad
    /// @endcode
    ara::core::Result< KeySlotPrototypeProps > GetSlotProps() const noexcept;
    /// @brief Set the prototype attributes of the key slot.
    /// @name   SaveSlotProps
    /// @param slotProps Key slot attributes
    /// @returns true if save sucess
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02916
    /// @trace_id_dd=DD_CRYPTO_05842
    /// @needwork = ad
    /// @endcode
    ara::core::Result< bool > SaveSlotProps(KeySlotPrototypeProps const &slotProps) const noexcept;
    /// @brief Open this key slot and return an IOInterface to its content.
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02917
    /// @trace_id_dd=DD_CRYPTO_05843
    /// @needwork = ad
    /// @endcode
    ara::core::Result< uint32_t >
    /// @brief Open this key slot and return an IOInterface to its content.
    /// If the UpdatesObserver interface is registered via RegisterObserver(), then every time this slot is updated (and becomes "user" visible), the Key
    /// Storage engine (in a dedicated thread) should call the UpdatesObserver::OnUpdate() method.
    /// Monitoring of the opened key slot will continue even after the returned TrustedContainer is destroyed,
    /// because the content of the slot may be loaded into volatile memory (as a cryptobject or a CryptoContext of a crypto primitive), but after that, the TrustedContainer may be destroyed.
    /// Therefore, if you need to terminate monitoring of a key slot, you should directly call the UnsubscribeObserver(SlotNumber) method.
    /// @name   Open
    /// @param nProcessID Process ID
    /// @param subscribeForUpdates Whether to enable update subscription
    /// @param writeable Whether to open in write mode
    /// @returns
    Open(uint64_t const nProcessID, bool const subscribeForUpdates, bool writeable) const noexcept;
    /// @brief Close the read/write record of the key slot for a specific process
    /// @name   Close
    /// @param nPid Process ID
    /// @param bReadOnly Whether read-only
    /// @returns true if close sucess
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02918
    /// @trace_id_dd=DD_CRYPTO_05844
    /// @needwork = ad
    /// @endcode
    ara::core::Result< bool > Close(uint64_t const nPid, bool const bReadOnly) const noexcept;
    /// @brief Save the content of the provided source IOInterface to this key slot. The source container can represent a volatile trusted container or another KeySlot.
    ///         This method can be used to atomically update key slots within a transaction. In this case, the slot will only be updated after the corresponding CommitTransaction() call.
    /// @name   SaveCopy
    /// @param pSrcInterface Source IO interface pointer
    /// @returns true if save sucess
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02919
    /// @trace_id_dd=DD_CRYPTO_05845
    /// @needwork = ad
    /// @endcode
    ara::core::Result< bool > SaveCopy(PIoInterface const *const pSrcInterface) const noexcept;
    /// @brief Data copy check
    /// @param pSrcInterface Source IO interface pointer
    /// @return true if SaveCopyCheck sucess
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02920
    /// @trace_id_dd=DD_CRYPTO_05846
    /// @needwork = ad
    /// @endcode
    ara::core::Result< bool > SaveCopyCheck(PIoInterface const *const pSrcInterface) const noexcept;
    /// @brief Save the key passed in via IPC PIoInterface_Mem
    /// @name   SaveCopy
    /// @param nSlotID Key slot ID
    /// @param keysSrc Source key data
    /// @returns true if SaveCopy sucess
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02921
    /// @trace_id_dd=DD_CRYPTO_05847
    /// @needwork = ad
    /// @endcode
    ara::core::Result< bool > SaveCopy(uint32_t const nSlotID,
                                       keyslot::PIpcReq_SaveCopy_Mem const *const keysSrc) const noexcept;
    /// @brief Save the incoming KeyData via IPC
    /// @name   SaveKeyData
    /// @param nSlotID Key slot ID
    /// @param pData Data: starting memory address
    /// @param nLen Data length: in bytes
    /// @returns true if SaveKeyData sucess
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02922
    /// @trace_id_dd=DD_CRYPTO_05848
    /// @needwork = ad
    /// @endcode
    ara::core::Result< bool > SaveKeyData(uint32_t const nSlotID,
                                          uint8_t const *const pData,
                                          uint16_t const nLen) const noexcept;
    /// @brief Get the deletion flag
    /// @name   GetDeleteFlag
    /// @returns has value if get DeleteFlag sucess
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02923
    /// @trace_id_dd=DD_CRYPTO_05849
    /// @needwork = ad
    /// @endcode
    ara::core::Result< uint32_t > GetDeleteFlag() const noexcept;
    /// @brief Set the deletion flag
    /// @name   SetDeleteFlag
    /// @param nProcessID Process ID
    /// @returns true if set DeleteFlag sucess
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02924
    /// @trace_id_dd=DD_CRYPTO_05850
    /// @needwork = ad
    /// @endcode
    ara::core::Result< bool > SetDeleteFlag(uint64_t const nProcessID) const noexcept;
    /// @brief Reset key slot information (clear key slot content) using key slot attribute information from the configuration file
    /// @param keySlotPrototypeProps Key slot attributes
    /// @return true if reset keyslot config sucess
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02925
    /// @trace_id_dd=DD_CRYPTO_05851
    /// @needwork = ad
    /// @endcode
    ara::core::Result< bool > ResetKeySlot(KeySlotPrototypeProps const &keySlotPrototypeProps) const noexcept;

public:  // Custom function functions
    /// @brief Whether a subscription has been registered
    /// @name   IsRegObserver
    /// @param nPid Process ID
    /// @returns true has already registed observer
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02926
    /// @trace_id_dd=DD_CRYPTO_05852
    /// @needwork = ad
    /// @endcode
    bool IsRegObserver(uint64_t const nPid) const noexcept;
    /// @brief Register/subscribe an observer
    /// @name   RegObserverPid
    /// @param nPid Process ID
    /// @param accessLevel Access level
    /// @returns true if regist observer sucess
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02927
    /// @trace_id_dd=DD_CRYPTO_05853
    /// @needwork = ad
    /// @endcode
    bool RegObserverPid(uint64_t nPid, EnumSlotOpenModal accessLevel) const noexcept;
    /// @brief Unsubscribe
    /// @name   DelObserverPid
    /// @param nPid Process ID
    /// @returns true if unregist observer suecess
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02928
    /// @trace_id_dd=DD_CRYPTO_05854
    /// @needwork = ad
    /// @endcode
    bool DelObserverPid(uint64_t const nPid) const noexcept;
    /// @brief Trigger subscription
    /// @name   CallObserver
    /// @returns 0 fail 1 sucess
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02929
    /// @trace_id_dd=DD_CRYPTO_05855
    /// @needwork = ad
    /// @endcode
    int32_t CallObserver() const noexcept;
    /// @brief Check whether preparations are complete; the return value is an error code
    /// @name   IsReady
    /// @param bCheckEmpty Whether to check for an empty key slot
    /// @returns true if has already
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02930
    /// @trace_id_dd=DD_CRYPTO_05856
    /// @needwork = ad
    /// @endcode
    ara::core::Result< bool > IsReady(bool const bCheckEmpty) const noexcept;
    /// @brief Get the key information stored in the key slot
    /// @name  LoadKeyData
    /// @returns Key information stored in the key slot
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02931
    /// @trace_id_dd=DD_CRYPTO_05857
    /// @needwork = ad
    /// @endcode
    ara::core::Result< ara::core::Vector< uint8_t > > LoadKeyData() const noexcept;
    /// @brief Verify whether the process has "write" permission
    /// @name   IsEnableProcessWrite
    /// @param nProcessID Process ID
    /// @returns true if can write
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02932
    /// @trace_id_dd=DD_CRYPTO_05858
    /// @needwork = ad
    /// @endcode
    bool IsEnableProcessWrite(uint64_t const nProcessID) const noexcept;
    /// @brief Save changes
    /// @name  CommitSaveAction
    /// @returns true if CommitSaveAction sucess
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02933
    /// @trace_id_dd=DD_CRYPTO_05859
    /// @needwork = ad
    /// @endcode
    bool CommitSaveAction() const noexcept;
    /// @brief Roll back operation
    /// @name   RollbackSaveAction
    /// @returns true if RollbackSaveAction sucess
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02934
    /// @trace_id_dd=DD_CRYPTO_05860
    /// @needwork = ad
    /// @endcode
    bool RollbackSaveAction() const noexcept;
    /// @brief Get the algorithm ID stored in the key slot
    /// @name  GetPrimitiveId
    /// @returns Algorithm ID stored in the key slot
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02935
    /// @trace_id_dd=DD_CRYPTO_05861
    /// @needwork = ad
    /// @endcode
    ara::core::Result< ara::crypto::CryptoAlgId > GetPrimitiveId() const noexcept;
    /// @brief Get the usage restrictions of the algorithm stored in the key slot
    /// @return Usage restrictions of the algorithm stored in the key slot
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02936
    /// @trace_id_dd=DD_CRYPTO_05862
    /// @needwork = ad
    /// @endcode
    ara::core::Result< AllowedUsageFlags > GetAllowedUsage() const noexcept;

protected:
    /// @brief Whether there is permission to operate this object
    /// @name   IsHaveAccessRight
    /// @returns true if has access right
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05863
    /// @needwork = dda
    /// @endcode
    static bool IsHaveAccessRight() noexcept;
    /// @brief Return the PIOInterface used by itself
    /// @name   _MakeMainIoInterface
    /// @returns PIoInterface_KvShare smart pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05864
    /// @needwork = dda
    /// @endcode
    PIoInterface_KvShare::Uptr _MakeMainIoInterface() const noexcept;
    /// @brief Record the process opened in read/write mode
    /// @param nPid Process ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05865
    /// @needwork = dda
    /// @endcode
    void _AddPidByWrite(uint64_t const nPid) const noexcept;
    /// @brief Invalidate the opened iointerface
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05866
    /// @needwork = dda
    /// @endcode
    void _SetOpenIointerfaceInvalid() const noexcept;
};
//********************************/
}  // namespace  isoft_def
}  // namespace keys
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_KEYS_PUHUA_SVR_KEY_SLOT_LOADER_H_
