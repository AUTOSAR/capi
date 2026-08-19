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
/// @file       isoft_keys_manager.h
/// @brief      AutoSar-Crypto key storage module -- key
/// @details    KeyProvider provider's IPC server-side key slot manager.
/// @date       2023-09-01
/// @author     CHE JINZHAO
/// @version    1.2.0
///
/// ================================================================
///
/// @par modification log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr><td>2023-09-019   <td>1.0.0     <td>CHE JINZHAO       <td>Create initial version
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/key manager/key slot manager
/// @interface_level=module
/// @trace_id_sr=SR_CRYPTO_01008
/// @unit_name=PKeys_Manager
/// @unit_description=Key slot manager
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_KEYS_PUHUA_KEYS_MANAGER_H_
#define ARA_CRYPTO_KEYS_PUHUA_KEYS_MANAGER_H_

#include <ara/core/map.h>
#include <ara/core/result.h>
#include <ara/core/string.h>
#include <ara/core/string_view.h>
#include <ara/core/vector.h>

#ifdef ARA_WITH_IAM
    #include <ara/iam/internal/grantquery/crypto.h>
#endif

#include <functional>

#include "ara/crypto/common/isoft_thread_lock.h"
#include "ara/crypto/ipc/isoft_ipc_auto_message.h"
#include "ara/crypto/ipc/isoft_ipc_protocol.h"
#include "ara/crypto/ipc/isoft_ipc_server.h"
#include "ara/crypto/keys/isoft_ipc_key_provider.h"
#include "ara/crypto/keys/updates_observer.h"
#include "ara/crypto/ksp/isoft_keys_process_base.h"
#include "ara/crypto/ksp/isoft_svr_key_slot_loader.h"
#include "ara/crypto/manifest/manifest_ksp_config.h"
namespace ara {
namespace crypto {
namespace keys {
namespace isoft_def {

//********************************/
/// @brief KeyProvider provider's IPC server-side key slot manager.
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_CRYPTO_02972
/// @trace_id_dd=DD_CRYPTO_05934
/// @needwork = ad
/// @endcode
class PKeys_Manager
{
public:
    /// @brief Observer map
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02972
    /// @trace_id_dd=DD_CRYPTO_06473
    /// @needwork = dd
    /// @endcode
    using MAP_UpdatesObserver = ara::core::Map< uint64_t, uint64_t >;
    /// @brief Key slot load map
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02972
    /// @trace_id_dd=DD_CRYPTO_06474
    /// @needwork = dd
    /// @endcode
    using MAP_KeySlot = ara::core::Map< uint32_t, PSvrKeySlotLoader::Uptr >;
    /// @brief Logic processing map
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02972
    /// @trace_id_dd=DD_CRYPTO_06475
    /// @needwork = dd
    /// @endcode
    using MAP_IpcProcess = ara::core::Map< ara::core::StringView, PKeysProcess_Base::Uptr >;
    /// @brief Transaction map
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02972
    /// @trace_id_dd=DD_CRYPTO_06476
    /// @needwork = dd
    /// @endcode
    using MAP_Transaction = ara::core::Map< uint64_t, ara::core::Vector< uint32_t > >;

public:
    /// @brief Constructor
    /// @name   PKeys_Manager
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02973
    /// @trace_id_dd=DD_CRYPTO_05936
    /// @needwork = ad
    /// @endcode
    PKeys_Manager() noexcept;
    /// @brief Destructor
    /// @name   ~PKeys_Manager
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02974
    /// @trace_id_dd=DD_CRYPTO_05937
    /// @needwork = ad
    /// @endcode
    virtual ~PKeys_Manager() noexcept;
    /// @brief Default copy constructor note: changed to default here, may cause errors on lower version compilers (declared to take const reference, but
    /// implicit declaration would take non-const)
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02975
    /// @trace_id_dd=DD_CRYPTO_05938
    /// @needwork = ad
    /// @endcode
    PKeys_Manager(PKeys_Manager const &other) = delete;
    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02976
    /// @trace_id_dd=DD_CRYPTO_05939
    /// @needwork = ad
    /// @endcode
    PKeys_Manager(PKeys_Manager &&other) = delete;
    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02977
    /// @trace_id_dd=DD_CRYPTO_05940
    /// @needwork = ad
    /// @endcode
    PKeys_Manager &operator=(PKeys_Manager const &other) = delete;
    /// @brief Default move assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02978
    /// @trace_id_dd=DD_CRYPTO_05941
    /// @needwork = ad
    /// @endcode
    PKeys_Manager &operator=(PKeys_Manager &&other) = delete;
    /// @brief Run the service
    /// @name   Running
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02979
    /// @trace_id_dd=DD_CRYPTO_05942
    /// @needwork = ad
    /// @endcode
    void Running() const noexcept;  // Execute
public:
    /// @brief Handling when IPC connection is established
    /// @name   OnIpcConnect
    /// @param nPid Process ID
    /// @param nSessionID Temporary session ID
    /// @returns 0 sucess fail otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02980
    /// @trace_id_dd=DD_CRYPTO_05943
    /// @needwork = ad
    /// @endcode
    int32_t OnIpcConnect(uint64_t const nPid, uint64_t const nSessionId) noexcept;
    /// @brief Handling when IPC connection is disconnected
    /// @name   OnIpcClose
    /// @param nPid Process ID
    /// @param nSessionID Temporary session ID
    /// @returns 0 sucess fail otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02981
    /// @trace_id_dd=DD_CRYPTO_05944
    /// @needwork = ad
    /// @endcode
    int32_t OnIpcClose(uint64_t const nPid, uint64_t const nSessionId) noexcept;
    /// @brief Handle IPC messages
    /// @name   ProcessIpcMsg
    /// @param pReq IPC request data
    /// @param nReqLen IPC request data length
    /// @param aswMsg Response message managed by IPC hosting
    /// @returns 0 sucess fail otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02982
    /// @trace_id_dd=DD_CRYPTO_05945
    /// @needwork = ad
    /// @endcode
    int32_t ProcessIpcMsg(uint8_t *const pReq, uint16_t const nReqLen, PIpcAutoPacket &aswMsg) noexcept;
    /// @brief Handle IPC errors
    /// @name   ProcessCmd_Error
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @param nErrorCode Error code
    /// @returns 0 sucess fail otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02983
    /// @trace_id_dd=DD_CRYPTO_05946
    /// @needwork = ad
    /// @endcode
    static int32_t ProcessCmd_Error(PIpcPac_Head const *const pReqHead,
                                    PIpcAutoPacket &aswMsg,
                                    SecurityErrorDomain::Errc const nErrorCode) noexcept;
    /// @brief Get the crypto provider object
    /// @name   GetCryptoProvider
    /// @returns Reference to the crypto provider
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02984
    /// @trace_id_dd=DD_CRYPTO_05947
    /// @needwork = ad
    /// @endcode
    cryp::CryptoProvider &GetCryptoProvider() const noexcept;

public:
    /// @brief Prepare the KeySlot, load it if it does not exist
    /// @name   PrepareKeySlotLoader
    /// @param nProcessID Process ID
    /// @param stSlotName Key slot name
    /// @return PSvrKeySlotLoader pointer
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02985
    /// @trace_id_dd=DD_CRYPTO_05948
    /// @needwork = ad
    /// @endcode
    ara::core::Result< PSvrKeySlotLoader * > PrepareKeySlotLoader(uint64_t const &nProcessID,
                                                                  ara::core::StringView const &stSlotName) noexcept;
    /// @brief Find key slot loader by name
    /// @name  FindSlotByName
    /// @param stSlotName Key slot name
    /// @return PSvrKeySlotLoader pointer
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02986
    /// @trace_id_dd=DD_CRYPTO_05949
    /// @needwork = ad
    /// @endcode
    PSvrKeySlotLoader *FindSlotByName(ara::core::StringView const &stSlotName) const noexcept;
    /// @brief Find key slot loader by ID
    /// @name  FindSlotByID
    /// @param nSlotID Key slot ID
    /// @return PSvrKeySlotLoader pointer
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02987
    /// @trace_id_dd=DD_CRYPTO_05950
    /// @needwork = ad
    /// @endcode
    PSvrKeySlotLoader *FindSlotByID(uint32_t const nSlotID) const noexcept;
    /// @brief Find an observer
    /// @name  FindUpdateObserver
    /// @param nPid Process ID
    /// @return 0 not find
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02988
    /// @trace_id_dd=DD_CRYPTO_05951
    /// @needwork = ad
    /// @endcode
    uint64_t FindUpdateObserver(uint64_t const nPid) const noexcept;
    /// @brief Trigger observer actions
    /// @name  CallObserver
    /// @param nProcessID Process ID
    /// @param nSlotID Key slot ID
    /// @param slotName Key slot name
    /// @return true if call observer sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02989
    /// @trace_id_dd=DD_CRYPTO_05952
    /// @needwork = ad
    /// @endcode
    bool CallObserver(uint64_t const nProcessID,
                      uint32_t const nSlotID,
                      ara::core::String const &slotName) const noexcept;

public:
    /// @brief Add a transaction, return the new transaction ID
    /// @name  AddNewTransaction
    /// @param nProcessID Process ID
    /// @param pSlotID Array of key slot IDs
    /// @param nSlotCount Number of key slot IDs in the array
    /// @return New transaction ID
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02990
    /// @trace_id_dd=DD_CRYPTO_05953
    /// @needwork = ad
    /// @endcode
    ara::core::Result< uint64_t > AddNewTransaction(uint32_t const nProcessID,
                                                    uint32_t const *const pSlotID,
                                                    uint32_t const nSlotCount) noexcept;
    /// @brief Update a transaction
    /// @name  CommitTransaction
    /// @param nTransactionID Transaction ID
    /// @return  0 fail 1 sucess
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02991
    /// @trace_id_dd=DD_CRYPTO_05954
    /// @needwork = ad
    /// @endcode
    ara::core::Result< uint32_t > CommitTransaction(uint64_t const nTransactionID) noexcept;
    /// @brief Roll back a transaction
    /// @name  RollbackTransaction
    /// @param nTransactionID Transaction ID
    /// @return  0 fail 1 sucess
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02992
    /// @trace_id_dd=DD_CRYPTO_05955
    /// @needwork = ad
    /// @endcode
    ara::core::Result< uint32_t > RollbackTransaction(uint64_t const nTransactionID) noexcept;
    /// @brief Register a subscription observer
    /// @name  RegisterObserver
    /// @param pReqHead IPC request packet header
    /// @param nActionPid Active process ID
    /// @param nSessionID Temporary session ID
    /// @return Active process ID
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02993
    /// @trace_id_dd=DD_CRYPTO_05956
    /// @needwork = ad
    /// @endcode
    ara::core::Result< uint32_t > RegisterObserver(PIpcPac_Head const *const pReqHead,
                                                   uint64_t const nActionPid,
                                                   uint64_t const nSessionID) noexcept;
    /// @brief Unregister a subscription observer for a key slot
    /// @name  UnsubscribeObserver
    /// @param nProcessID Process ID
    /// @param nSlotID Key slot ID
    /// @return Process ID
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02994
    /// @trace_id_dd=DD_CRYPTO_05957
    /// @needwork = ad
    /// @endcode
    ara::core::Result< uint32_t > UnsubscribeObserver(uint64_t const nProcessID, uint32_t const nSlotID) const noexcept;
    /// @brief Get the crypto provider corresponding to the slot
    /// @param stSlotName Key slot name
    /// @return Crypto provider name
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02995
    /// @trace_id_dd=DD_CRYPTO_05958
    /// @needwork = ad
    /// @endcode
    ara::core::String FindCryptoProviderBySlotName(ara::core::StringView const &stSlotName) const noexcept;
    /// @brief Get the file path where the key slot is to be saved from the configuration file
    /// @param stSlotName Key slot name
    /// @return File path where the key slot is to be saved from the configuration file
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02996
    /// @trace_id_dd=DD_CRYPTO_05959
    /// @needwork = ad
    /// @endcode
    ara::core::StringView GetSlotSavedFileName(ara::core::StringView const &stSlotName) const noexcept;
    /// @brief Find the corresponding key slot configuration in the configuration file and fill mAlgId
    /// @name  FindConfigKeySlotProps
    /// @param stSlotName Key slot name
    /// @return KeySlotPrototypeProps instance
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_02997
    /// @trace_id_dd=DD_CRYPTO_05960
    /// @needwork = ad
    /// @endcode
    keys::KeySlotPrototypeProps::Uptr FindConfigKeySlotProps(ara::core::StringView const &stSlotName) const noexcept;

private:
    /// @brief Crypto configuration file
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05961
    /// @needwork = dda
    /// @endcode
    manifest::PManifestKspConfig manifestKspConfig_{};
    /// @brief Crypto provider
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05962
    /// @needwork = dda
    /// @endcode
    cryp::CryptoProvider::Uptr pCryptoProvider_{nullptr};
    /// @brief Registered observers
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05963
    /// @needwork = dda
    /// @endcode
    MAP_UpdatesObserver mapUpdateObserver_{};
    /// @brief Key slot manager
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05964
    /// @needwork = dda
    /// @endcode
    MAP_KeySlot mapKeySlot_{};
    /// @brief IPC packet handler
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05965
    /// @needwork = dda
    /// @endcode
    MAP_IpcProcess mapIpcProcess_{};
    /// @brief Transaction management
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05966
    /// @needwork = dda
    /// @endcode
    MAP_Transaction mapTransation_{};
    /// @brief IPC service
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05967
    /// @needwork = dda
    /// @endcode
    mutable PIpcServer ipcServer_{*this};

protected:
    /// @brief Get the configuration file name
    /// @return Configuration file name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05970
    /// @needwork = dda
    /// @endcode
    static ara::core::String GetConfigFileName() noexcept;
    /// @brief Get the default CryptoProvider
    /// @return Crypto provider object instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05971
    /// @needwork = dda
    /// @endcode
    static cryp::CryptoProvider::Uptr DefCryptoProvider() noexcept;
    /// @brief Check whether there are permissions in the IAM configuration
    /// @name  _CheckIamLevel
    /// @param nProcessID Process ID
    /// @param stSlotName Key slot name
    /// @return true can access false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05972
    /// @needwork = dda
    /// @endcode
    bool _CheckIamLevel(uint64_t const nProcessID, ara::core::StringView const &stSlotName) const noexcept;
    /// @brief Find a new transaction
    /// @param nFindSlotID Key slot ID
    /// @return 0 no find  slotID otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05973
    /// @needwork = dda
    /// @endcode
    uint32_t _FindSlotInTransaction(uint32_t const nFindSlotID) const noexcept;
    /// @brief Initialize a new SlotLoader and add it to mapKeySlot_ for management
    /// @name  _InitNewSlotLoader
    /// @param stSlotName Key slot name
    /// @return PSvrKeySlotLoader pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05974
    /// @needwork = dda
    /// @endcode
    PSvrKeySlotLoader *_InitNewSlotLoader(ara::core::StringView const &stSlotName) noexcept;
    /// @brief Get the actual key slot name
    /// @param stSlotName Key slot name
    /// @return Key slot name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05975
    /// @needwork = dda
    /// @endcode
    ara::core::String _FindRealSlotName(ara::core::StringView const &stSlotName) const noexcept;
};
//********************************/
}  // namespace  isoft_def
}  // namespace keys
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_KEYS_PUHUA_KEYS_MANAGER_H_
