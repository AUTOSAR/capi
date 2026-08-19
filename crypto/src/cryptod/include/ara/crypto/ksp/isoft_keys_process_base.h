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
/// @file       isoft_keys_process_base.h
/// @brief      AutoSar-Crypto key storage module
/// @details    KeyProvider provider's IPC server side: base class for logical processing of key providers
/// @date       2022-08-11
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/key manager/key slot manager
/// @interface_level=module
/// @trace_id_sr=SR_CRYPTO_01008
/// @unit_name=PKeysProcess_Base
/// @unit_description=Base class for logical processing of server-side key providers
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_KEYS_PUHUA_KEYS_PROCESS_BASE_H_
#define ARA_CRYPTO_KEYS_PUHUA_KEYS_PROCESS_BASE_H_

#include <ara/core/map.h>
#include <ara/core/result.h>
#include <ara/core/string.h>
#include <ara/core/string_view.h>
#include <openssl/pem.h>

#include <functional>
#include <memory>

#include "ara/crypto/cryp/crypto_provider.h"
#include "ara/crypto/ipc/isoft_ipc_auto_message.h"
#include "ara/crypto/ipc/isoft_ipc_protocol.h"
#include "ara/crypto/ksp/isoft_svr_key_slot_loader.h"
#include "ara/crypto/openssl/isoft_openssl_encrypt.h"

namespace ara {
namespace crypto {
namespace keys {
namespace isoft_def {
//********************************/
/// @brief forward declaration
class PKeys_Manager;
//********************************/
/// @brief KeyProvider provider's IPC server side: base class for logical processing of key providers
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_CRYPTO_03005
/// @trace_id_dd=DD_CRYPTO_05991
/// @needwork = ad
/// @endcode
class PKeysProcess_Base
{
public:
    /// @brief PKeysProcess_Base unique pointer type
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_03005
    /// @trace_id_dd=DD_CRYPTO_06477
    /// @needwork = dd
    /// @endcode
    using Uptr = std::unique_ptr< PKeysProcess_Base >;
    /// @brief ara::core::Result<uint16_t> declaration
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_03005
    /// @trace_id_dd=DD_CRYPTO_06478
    /// @needwork = dd
    /// @endcode
    using PResultLen = ara::core::Result< uint16_t >;

public:
    /// @brief Constructor with parameters
    /// @name   PKeysProcess_Base
    /// @param lpcProcessManager LCP command processing manager
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_03006
    /// @trace_id_dd=DD_CRYPTO_05992
    /// @needwork = ad
    /// @endcode
    explicit PKeysProcess_Base(PKeys_Manager& lpcProcessManager) noexcept;
    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_03007
    /// @trace_id_dd=DD_CRYPTO_05993
    /// @needwork = ad
    /// @endcode
    virtual ~PKeysProcess_Base() noexcept = default;
    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_03008
    /// @trace_id_dd=DD_CRYPTO_05994
    /// @needwork = ad
    /// @endcode
    PKeysProcess_Base() = delete;
    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @returns *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_03009
    /// @trace_id_dd=DD_CRYPTO_05995
    /// @needwork = ad
    /// @endcode
    PKeysProcess_Base& operator=(PKeysProcess_Base const& other) = delete;
    /// @brief Default move assignment function
    /// @param other Another object instance of this class
    /// @returns *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_03010
    /// @trace_id_dd=DD_CRYPTO_05996
    /// @needwork = ad
    /// @endcode
    PKeysProcess_Base& operator=(PKeysProcess_Base&& other) = delete;
    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_03011
    /// @trace_id_dd=DD_CRYPTO_05997
    /// @needwork = ad
    /// @endcode
    PKeysProcess_Base(PKeysProcess_Base&& other) = delete;
    /// @brief Default copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_03012
    /// @trace_id_dd=DD_CRYPTO_05998
    /// @needwork = ad
    /// @endcode
    PKeysProcess_Base(PKeysProcess_Base const& other) noexcept = delete;

public:
    /// @brief IPC connection established
    /// @name   OnIpcConnect
    /// @param nPid Process ID
    /// @param nSessionID Temporary session ID
    /// @returns 0 sucess fail otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_03013
    /// @trace_id_dd=DD_CRYPTO_05999
    /// @needwork = ad
    /// @endcode
    virtual PResultLen OnIpcConnect(uint64_t const nPid, uint64_t const nSessionId) noexcept = 0;
    /// @brief IPC connection closed
    /// @name   OnIpcClose
    /// @param nPid Process ID
    /// @param nSessionID Temporary session ID
    /// @returns 0 sucess fail otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_03014
    /// @trace_id_dd=DD_CRYPTO_06000
    /// @needwork = ad
    /// @endcode
    virtual PResultLen OnIpcClose(uint64_t const nPid, uint64_t const nSessionId) noexcept = 0;
    /// @brief Handle IPC messages
    /// @name   ProcessIpcMsg
    /// @param pReq IPC request data
    /// @param nReqLen IPC request data length
    /// @param aswMsg Response message managed by IPC hosting
    /// @returns 0 sucess fail otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_03015
    /// @trace_id_dd=DD_CRYPTO_06001
    /// @needwork = ad
    /// @endcode
    virtual PResultLen ProcessIpcMsg(uint8_t* pReq, uint16_t nReqLen, PIpcAutoPacket& aswMsg) noexcept = 0;

public:
    /// @brief Get the crypto provider
    /// @name   GetCryptoProvider
    /// @returns Reference to the crypto provider
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_03016
    /// @trace_id_dd=DD_CRYPTO_06002
    /// @needwork = ad
    /// @endcode
    cryp::CryptoProvider& GetCryptoProvider() const noexcept;

protected:
    /// @brief Find key slot loader
    /// @param ipcSlotID Key slot ID
    /// @return PSvrKeySlotLoader pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_03017
    /// @trace_id_dd=DD_CRYPTO_06003
    /// @needwork = ad
    /// @endcode
    keys::isoft_def::PSvrKeySlotLoader* _FindSlotLoader(uint32_t const ipcSlotID) const noexcept;
    /// @brief Prepare a key slot loader
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_03018
    /// @trace_id_dd=DD_CRYPTO_06004
    /// @needwork = ad
    /// @endcode
    ara::core::Result< PSvrKeySlotLoader* >
    /// @brief Prepare a key slot loader
    /// @param nProcessID Process ID
    /// @param stSlotName Key slot name
    /// @return
    _PrepareKeySlotLoader(uint64_t const& nProcessID, ara::core::StringView const& stSlotName) const noexcept;
    /// @brief Add a transaction
    /// @param nProcessID Process ID
    /// @param pSlotID Array of key slot IDs
    /// @param nSlotCount Number of key slot IDs in the array
    /// @return Transaction ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_03019
    /// @trace_id_dd=DD_CRYPTO_06005
    /// @needwork = ad
    /// @endcode
    ara::core::Result< uint64_t > _AddNewTransaction(uint32_t const nProcessID,
                                                     uint32_t const* const pSlotID,
                                                     uint32_t const nSlotCount) const noexcept;
    /// @brief Commit transaction
    /// @param nTransactionID Transaction ID
    /// @return 0 fail 1 sucess
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_03020
    /// @trace_id_dd=DD_CRYPTO_06006
    /// @needwork = ad
    /// @endcode
    ara::core::Result< uint32_t > _CommitTransaction(uint64_t const nTransactionID) const noexcept;
    /// @brief Get the corresponding crypto provider by key slot name
    /// @param stSlotName Key slot name
    /// @return Corresponding crypto provider for the key slot name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_03021
    /// @trace_id_dd=DD_CRYPTO_06007
    /// @needwork = ad
    /// @endcode
    ara::core::String _FindCryptoProviderBySlotName(ara::core::StringView const& stSlotName) const noexcept;
    /// @brief Roll back transaction
    /// @param nTransactionID Transaction ID
    /// @return 0 fail 1 sucess
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_03022
    /// @trace_id_dd=DD_CRYPTO_06008
    /// @needwork = ad
    /// @endcode
    ara::core::Result< uint32_t > _RollbackTransaction(uint64_t const nTransactionID) const noexcept;
    /// @brief Check whether the current process is an observer
    /// @param nPid Process ID
    /// @return 0 not find
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_03023
    /// @trace_id_dd=DD_CRYPTO_06009
    /// @needwork = ad
    /// @endcode
    uint64_t _FindUpdateObserver(uint64_t const nPid) const noexcept;
    /// @brief Register an observer
    /// @param pReqHead IPC request packet header
    /// @param nActionPid Active process ID
    /// @param nSessionID Temporary session ID
    /// @return Active process ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_03024
    /// @trace_id_dd=DD_CRYPTO_06010
    /// @needwork = ad
    /// @endcode
    ara::core::Result< uint32_t > _RegisterObserver(PIpcPac_Head const* const pReqHead,
                                                    uint64_t const nActionPid,
                                                    uint64_t const nSessionID) const noexcept;
    /// @brief Unregister an observer
    /// @param nProcessID Process ID
    /// @param nSlotID Key slot ID
    /// @return Process ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_03025
    /// @trace_id_dd=DD_CRYPTO_06011
    /// @needwork = ad
    /// @endcode
    ara::core::Result< uint32_t > _UnsubscribeObserver(uint64_t const nProcessID,
                                                       uint32_t const nSlotID) const noexcept;
    /// @brief Find key slot attribute information from the configuration file
    /// @param stSlotName Key slot name
    /// @return Key slot attribute information from the configuration file
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_03026
    /// @trace_id_dd=DD_CRYPTO_06012
    /// @needwork = ad
    /// @endcode
    keys::KeySlotPrototypeProps::Uptr _FindConfigKeySlotProps(ara::core::StringView const& stSlotName) const noexcept;
    /// @brief Load data from the key slot
    /// @param nSlotID Key slot ID
    /// @return Data from the key slot
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_03028
    /// @trace_id_dd=DD_CRYPTO_06014
    /// @needwork = ad
    /// @endcode
    ara::core::Result< ara::core::Vector< uint8_t > > _LoadKeyData(uint32_t const nSlotId) const noexcept
    {
        keys::isoft_def::PSvrKeySlotLoader* const pFindSlotLoader{_FindSlotLoader(nSlotId)};
        if (nullptr == pFindSlotLoader) {
            // IPC parameter error;
            return ara::core::Result< ara::core::Vector< uint8_t > >::FromError(SecurityErrorDomain::Errc::kIpcFault);
        }
        return pFindSlotLoader->LoadKeyData();
    }
    /// @brief Get the AlgID of the key slot
    /// @param nSlotID Key slot ID
    /// @return AlgID of the key slot
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_03029
    /// @trace_id_dd=DD_CRYPTO_06015
    /// @needwork = ad
    /// @endcode
    ara::core::Result< ara::crypto::CryptoAlgId > _GetPrimitiveId(uint32_t const nSlotId) const noexcept
    {
        keys::isoft_def::PSvrKeySlotLoader* const pFindSlotLoader{_FindSlotLoader(nSlotId)};
        if (nullptr == pFindSlotLoader) {
            // IPC parameter error;
            return ara::core::Result< ara::crypto::CryptoAlgId >::FromError(SecurityErrorDomain::Errc::kIpcFault);
        }
        return pFindSlotLoader->GetPrimitiveId();
    }
    /// @brief Get the usage restrictions of the key slot
    /// @param nSlotID Key slot ID
    /// @return Usage restrictions of the key slot
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_03030
    /// @trace_id_dd=DD_CRYPTO_06016
    /// @needwork = ad
    /// @endcode
    ara::core::Result< AllowedUsageFlags > _GetAllowedUsage(uint32_t const nSlotId) const noexcept
    {
        keys::isoft_def::PSvrKeySlotLoader* const pFindSlotLoader{_FindSlotLoader(nSlotId)};
        if (nullptr == pFindSlotLoader) {
            // IPC parameter error;
            return ara::core::Result< AllowedUsageFlags >::FromError(SecurityErrorDomain::Errc::kIpcFault);
        }
        return pFindSlotLoader->GetAllowedUsage();
    }
    /// @brief Set key
    /// @name   _SetKey
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @returns has value if setkey sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_03031
    /// @trace_id_dd=DD_CRYPTO_06017
    /// @needwork = ad
    /// @endcode
    PResultLen _SetKey(keys::isoft_def::PIpcPac_Head const* const pReqHead,
                       keys::isoft_def::PIpcAutoPacket& aswMsg) const noexcept;

private:
    /// @brief Key manager class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06018
    /// @needwork = dda
    /// @endcode
    PKeys_Manager& lpcProcessManager_;

protected:
    /// @brief Handle IPC error information
    /// @name   ProcessCmd_Error
    /// @throws
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @param nErrorCode Error code
    /// @return 0 sucess fail otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06019
    /// @needwork = dda
    /// @endcode
    static PResultLen ProcessCmd_Error(PIpcPac_Head const* const pReqHead,
                                       PIpcAutoPacket& aswMsg,
                                       SecurityErrorDomain::Errc const nErrorCode) noexcept;
    /// @brief Handle IPC error information
    /// @name   ProcessCmd_Error
    /// @param pReqHead IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @param nErrorCode Error code
    /// @returns 0 sucess fail otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06020
    /// @needwork = dda
    /// @endcode
    static PResultLen ProcessCmd_Error(PIpcPac_Head const* const pReqHead,
                                       PIpcAutoPacket& aswMsg,
                                       int32_t const nErrorCode) noexcept;
};
//********************************/
/// @brief Logical processing template class
/// @code{.isoft}
/// @tparam T_Process
/// @interface_level=module
/// @trace_id_ad=AD_CRYPTO_03032
/// @trace_id_dd=DD_CRYPTO_06021
/// @needwork = ad
/// @endcode
template < typename T_Process_key >
class PKeysProcess_T_Base : public PKeysProcess_Base
{
public:
    /// @brief Command processing function
    /// @name  CB_ProcessCmd
    /// @param pPacReq IPC request packet header
    /// @param aswMsg Response message managed by IPC hosting
    /// @return Packet length
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_03005
    /// @trace_id_dd=DD_CRYPTO_06479
    /// @needwork = dd
    /// @endcode
    using CB_ProcessCmd = PResultLen (T_Process_key::*)(PIpcPac_Head const* pPacReq, PIpcAutoPacket& aswMsg) const;
    /// @brief Map redefinition
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_03005
    /// @trace_id_dd=DD_CRYPTO_06480
    /// @needwork = dd
    /// @endcode
    using MAP_ProcessCmd = ara::core::Map< ara::core::StringView, CB_ProcessCmd >;

public:
    /// @brief Constructor
    /// @param pObject IPC message handler object
    /// @param lpcProcessManager LCP command processing manager
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_03033
    /// @trace_id_dd=DD_CRYPTO_06022
    /// @needwork = ad
    /// @endcode
    explicit PKeysProcess_T_Base(T_Process_key* const pObject, PKeys_Manager& lpcProcessManager) noexcept
        : PKeysProcess_Base{lpcProcessManager}, pObject_{pObject} {};
    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_03034
    /// @trace_id_dd=DD_CRYPTO_06023
    /// @needwork = ad
    /// @endcode
    ~PKeysProcess_T_Base() override = default;
    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_03035
    /// @trace_id_dd=DD_CRYPTO_06024
    /// @needwork = ad
    /// @endcode
    PKeysProcess_T_Base() noexcept = delete;
    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @returns *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_03036
    /// @trace_id_dd=DD_CRYPTO_06025
    /// @needwork = ad
    /// @endcode
    PKeysProcess_T_Base& operator=(PKeysProcess_T_Base const& other) noexcept = delete;
    /// @brief Default move assignment function
    /// @param other Another object instance of this class
    /// @returns *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_03037
    /// @trace_id_dd=DD_CRYPTO_06026
    /// @needwork = ad
    /// @endcode
    PKeysProcess_T_Base& operator=(PKeysProcess_T_Base&& other) noexcept = delete;
    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_03038
    /// @trace_id_dd=DD_CRYPTO_06027
    /// @needwork = ad
    /// @endcode
    PKeysProcess_T_Base(PKeysProcess_T_Base&& other) = delete;
    /// @brief Default copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_03039
    /// @trace_id_dd=DD_CRYPTO_06028
    /// @needwork = ad
    /// @endcode
    PKeysProcess_T_Base(PKeysProcess_T_Base const& other) noexcept = delete;

public:
    /// @brief IPC connection established
    /// @name   OnIpcConnect
    /// @param nPid Process ID
    /// @param nSessionID Temporary session ID
    /// @returns 0 sucess fail otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_03040
    /// @trace_id_dd=DD_CRYPTO_06029
    /// @needwork = ad
    /// @endcode
    PResultLen OnIpcConnect(uint64_t const nPid, uint64_t const nSessionId) noexcept override
    {
        std::ignore = nPid;
        std::ignore = nSessionId;
        return PResultLen::FromValue(0);
    }
    /// @brief IPC connection closed
    /// @name   OnIpcClose
    /// @param nPid Process ID
    /// @param nSessionID Temporary session ID
    /// @returns 0 sucess fail otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_03041
    /// @trace_id_dd=DD_CRYPTO_06030
    /// @needwork = ad
    /// @endcode
    PResultLen OnIpcClose(uint64_t const nPid, uint64_t const nSessionId) noexcept override
    {
        std::ignore = nPid;
        std::ignore = nSessionId;
        return PResultLen::FromValue(0);
    }
    /// @brief Handle IPC messages
    /// @name  ProcessIpcMsg
    /// @param pReq IPC request data
    /// @param nReqLen IPC request data length
    /// @param aswMsg Response message managed by IPC hosting
    /// @return 0 sucess fail otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_03042
    /// @trace_id_dd=DD_CRYPTO_06031
    /// @needwork = ad
    /// @endcode
    PResultLen ProcessIpcMsg(uint8_t* pReq, uint16_t nReqLen, PIpcAutoPacket& aswMsg) noexcept override
    {
        std::ignore = nReqLen;
        PIpcPac_Head* const pPacHead{static_cast< PIpcPac_Head* >(static_cast< void* >(pReq))};
        ara::core::StringView const stFuncName{pPacHead->GetFuncName()};
        typename MAP_ProcessCmd::iterator const itFind{std::move(mapProcessCmd_.find(stFuncName))};
        if (itFind == mapProcessCmd_.end()) {
            return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);
        }
        CB_ProcessCmd const pFindProcess{itFind->second};
        return std::move((pObject_->*pFindProcess)(pPacHead, aswMsg));  // Pass error code
        // return ( (T_Process*)(this)->*pFindProcess)(pPacHead, aswMsg);
    }

private:
    /// @brief Processing logic map structure
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06032
    /// @needwork = dda
    /// @endcode
    MAP_ProcessCmd mapProcessCmd_{};

protected:
    /// @brief Insert into map structure
    /// @param key Process identifier
    /// @param fun Callback function for handling IPC messages
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_03043
    /// @trace_id_dd=DD_CRYPTO_06033
    /// @needwork = ad
    /// @endcode
    void _InsertMapCmd(ara::core::StringView const& key, CB_ProcessCmd const fun) noexcept
    {
        mapProcessCmd_[key] = fun;
    }

private:
    /// @brief Specific processing class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06034
    /// @needwork = dda
    /// @endcode
    T_Process_key* pObject_{nullptr};

protected:
};
//********************************/
}  // namespace  isoft_def
}  // namespace keys
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_KEYS_PUHUA_KEYS_PROCESS_BASE_H_
