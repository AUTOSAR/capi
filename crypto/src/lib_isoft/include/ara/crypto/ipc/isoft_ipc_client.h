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
/// @file       isoft_ipc_client.h
/// @brief      AutoSar-Crypto Key Storage Module
/// @details    IPC client for KeyProvider provider
/// @date       2022-08-01
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr><td>2022-08-01  <td>1.0.0    <td>hanjingjing  <td>Initial version creation
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Reusable Functions/IPC Client
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_06003
/// @unit_name=PIpcClient
/// @unit_description=IPC Client
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_KEYS_PUHUA_IPC_CLIENT_H_
#define ARA_CRYPTO_KEYS_PUHUA_IPC_CLIENT_H_

#include <isoft/ipccpp/client.h>
#include <isoft/naicpp/evloop.h>

#include <functional>
#include <memory>

#include "ara/crypto/cryp/isoft_auto_buff.h"
#include "ara/crypto/ipc/isoft_ipc_auto_message.h"
#include "ara/crypto/ipc/isoft_ipc_protocol.h"
#include "ara/crypto/keys/elementary_types.h"

namespace ara {
namespace crypto {
namespace keys {
namespace isoft_def {
//********************************/
/// @brief IPC client providing KeyProvider externally
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02001
/// @trace_id_dd=DD_CRYPTO_04723
/// @needwork = ad
/// @endcode
class PIpcClientInit
{
public:
    /// @brief the constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02002
    /// @trace_id_dd=DD_CRYPTO_04724
    /// @needwork = ad
    /// @endcode
    PIpcClientInit() noexcept;
    /// @brief the destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02003
    /// @trace_id_dd=DD_CRYPTO_04725
    /// @needwork = ad
    /// @endcode
    virtual ~PIpcClientInit() noexcept;
    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02004
    /// @trace_id_dd=DD_CRYPTO_04726
    /// @needwork = ad
    /// @endcode
    PIpcClientInit(PIpcClientInit&& other) = delete;
    /// @brief Default move assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02005
    /// @trace_id_dd=DD_CRYPTO_04727
    /// @needwork = ad
    /// @endcode
    PIpcClientInit& operator=(PIpcClientInit&& other) = delete;
    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02006
    /// @trace_id_dd=DD_CRYPTO_04728
    /// @needwork = ad
    /// @endcode
    PIpcClientInit& operator=(PIpcClientInit const& other) = delete;
    /// @brief Default copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02007
    /// @trace_id_dd=DD_CRYPTO_04729
    /// @needwork = ad
    /// @endcode
    PIpcClientInit(PIpcClientInit const& other) = delete;
    /// @brief working
    /// @name   Working
    /// @returns  true if runing false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02008
    /// @trace_id_dd=DD_CRYPTO_04730
    /// @needwork = ad
    /// @endcode
    bool Working() noexcept;

protected:
    /// @brief Initialize
    /// @name   _Initialize
    /// @returns true if has init false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04731
    /// @needwork = dda
    /// @endcode
    bool _Initialize() noexcept;
    /// @brief Deinitialize
    /// @name   _Deinitialize
    /// @returns true if has deinit false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04732
    /// @needwork = dda
    /// @endcode
    bool _Deinitialize() noexcept;

private:
    /// @brief Message loop
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04733
    /// @needwork = dda
    /// @endcode
    // PRQA S 2026 QAC /// @qac: False positive
    std::shared_ptr< ::isoft::naicpp::EvLoop > pEvLoop_{};
    // PRQA L:QAC
};
//********************************/
/// @brief IPC Client
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_02009
/// @trace_id_dd=DD_CRYPTO_04734
/// @needwork = ad
/// @endcode
class PIpcClient final
{
public:
    /// @brief Unique pointer type alias
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02009
    /// @trace_id_dd=DD_CRYPTO_06384
    /// @needwork = dd
    /// @endcode
    using Uptr = std::unique_ptr< PIpcClient >;

public:
    /// @brief Build IPC data packet
    /// @param pReqMsg Pointer to the IPC packet to build, can be null
    /// @returns If pReqMsg is null, only returns the corresponding packet length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02009
    /// @trace_id_dd=DD_CRYPTO_06385
    /// @needwork = dd
    /// @endcode
    using CB_IpcReqMsg = std::function< uint16_t(PIpcAutoPacket* pReqMsg) >;
    /// @brief Initialize
    /// @name   Initialize
    /// @returns  true if init sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02010
    /// @trace_id_dd=DD_CRYPTO_04735
    /// @needwork = ad
    /// @endcode
    static bool Initialize() noexcept;
    /// @brief De-initialize
    /// @name   Deinitialize
    /// @returns true if deinit sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02011
    /// @trace_id_dd=DD_CRYPTO_04736
    /// @needwork = ad
    /// @endcode
    static bool Deinitialize() noexcept;
    /// @brief Get Keys Instance
    /// @name   GetInstance_Keys
    /// @returns Default encryption client pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02012
    /// @trace_id_dd=DD_CRYPTO_04737
    /// @needwork = ad
    /// @endcode
    static PIpcClient* GetInstance_Keys() noexcept;
    /// @brief Get X509 Instance
    /// @name   GetInstance_X509
    /// @returns Certificate client pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02013
    /// @trace_id_dd=DD_CRYPTO_04738
    /// @needwork = ad
    /// @endcode
    static PIpcClient* GetInstance_X509() noexcept;
    /// @brief Get Hsm Instance
    /// @name   GetInstance_Hsm
    /// @returns HSM NXP client pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02014
    /// @trace_id_dd=DD_CRYPTO_04739
    /// @needwork = ad
    /// @endcode
    static PIpcClient* GetInstance_Hsm() noexcept;
    /// @brief Get Hsm bst Instance
    /// @name   GetInstance_Hsmbst
    /// @returns HSM BST client pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02015
    /// @trace_id_dd=DD_CRYPTO_04740
    /// @needwork = ad
    /// @endcode
    static PIpcClient* GetInstance_Hsmbst() noexcept;

public:
    /// @brief Constructor
    /// @name PIpcClient
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_08809
    /// @needwork = dda
    /// @endcode
    PIpcClient() = delete;

public:
    /// @brief the constructor wiht parameter
    /// @name      PIpcClient
    /// @param ipcClientName IPC client name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02016
    /// @trace_id_dd=DD_CRYPTO_04741
    /// @needwork = ad
    /// @endcode
    explicit PIpcClient(ara::core::StringView const& ipcClientName) noexcept;
    /// @brief Destructor
    /// @name ~PIpcClient
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02017
    /// @trace_id_dd=DD_CRYPTO_04742
    /// @needwork = ad
    /// @endcode
    ~PIpcClient() noexcept;
    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_08810
    /// @needwork = dda
    /// @endcode
    PIpcClient(PIpcClient&& other) = delete;
    /// @brief Default move assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_08811
    /// @needwork = dda
    /// @endcode
    PIpcClient& operator=(PIpcClient&& other) = delete;
    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_08812
    /// @needwork = dda
    /// @endcode
    PIpcClient& operator=(PIpcClient const& other) = delete;
    /// @brief Default copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_08808
    /// @needwork = dda
    /// @endcode
    PIpcClient(PIpcClient const& other) = delete;

public:
    /// @brief Check if preparations are complete
    /// @name   IsWorkerReady
    /// @returns true if has work ready false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02022
    /// @trace_id_dd=DD_CRYPTO_04747
    /// @needwork = ad
    /// @endcode
    bool IsWorkerReady() const noexcept;
    /// @brief Process one IPC request: synchronous mode
    /// @param stFuncName Function name used for IPC call
    /// @param aswMsg Response message managed by IPC
    /// @param funMakeReq Callback function for building IPC request packet
    /// @param nTimeOut Timeout value
    /// @return true if deal sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02023
    /// @trace_id_dd=DD_CRYPTO_04748
    /// @needwork = ad
    /// @endcode
    bool DealIpcRequest(ara::core::StringView const& stFuncName,
                        PIpcAutoPacket& aswMsg,
                        CB_IpcReqMsg const& funMakeReq,
                        int32_t const nTimeOut = -1) const noexcept;
    /// @brief Process one IPC request: synchronous mode
    /// @param stFuncName Function name used for IPC call
    /// @param aswMsg Response message managed by IPC
    /// @param reqMsg IPC request message
    /// @param nTimeOut Timeout value
    /// @return true if deal sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02024
    /// @trace_id_dd=DD_CRYPTO_04749
    /// @needwork = ad
    /// @endcode
    bool DealIpcRequest(ara::core::StringView const& stFuncName,
                        PIpcAutoPacket& aswMsg,
                        PIpcAutoPacket const& reqMsg,
                        int32_t const nTimeOut = -1) const noexcept;
    /// @brief Process one IPC request: asynchronous mode
    /// @param stFuncName Function name used for IPC call
    /// @param funMakeReq Callback function for building IPC request packet
    /// @param handler IPC client handle
    /// @param context IPC message context information
    /// @param nTimeOut Timeout value
    /// @return true if Async send request sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02025
    /// @trace_id_dd=DD_CRYPTO_04750
    /// @needwork = ad
    /// @endcode
    bool AsyncIpcRequest(ara::core::StringView const& stFuncName,
                         CB_IpcReqMsg const& funMakeReq,
                         isoft::ipc::IPCClientHandler const& handler,
                         void* const context,
                         int32_t const nTimeOut = -1) const noexcept;
    /// @brief Register/unregister subscription: business-coupled implementation
    /// @name   DealObserverReg
    /// @param bReg Whether to register
    /// @param handler IPC client handle
    /// @param context IPC message context information
    /// @return  true if deal Observer Regist sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02026
    /// @trace_id_dd=DD_CRYPTO_04751
    /// @needwork = ad
    /// @endcode
    bool DealObserverReg(bool const bReg,
                         isoft::ipc::IPCClientHandler const& handler,
                         void* const context) const noexcept;

private:
    /// @brief IPC client object pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04752
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< ::isoft::ipc::IPCClient > pIpcClient_{};

protected:
    /// @brief Traverse KeySlot list
    /// @name   ForEachKeySlot
    /// @param targetSlots Target KeySlot vector array
    /// @param fun Callback function for processing IPC messages
    /// @return Number of traversed keyslots
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_04753
    /// @needwork = dda
    /// @endcode
    static uint32_t ForEachKeySlot(TransactionScope const& targetSlots,
                                   std::function< bool(KeySlot const* keySlot) > const& fun) noexcept;
};
//********************************/
}  // namespace  isoft_def
}  // namespace keys
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_KEYS_PUHUA_IPC_CLIENT_H_
