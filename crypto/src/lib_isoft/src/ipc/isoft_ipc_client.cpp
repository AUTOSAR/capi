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
/// @file       isoft_ipc_client.cpp
/// @brief      AutoSar-Crypto Key Storage Module
/// @details
/// @date       2022-08-01
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Reusable Functions/IPC Client
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_06003
/// @unit_name=PIpcClientInit
/// @unit_description=IPC Client
/// @endcode
///
/// ================================================================

#include "ara/crypto/ipc/isoft_ipc_client.h"

#include <isoft/ipccpp/utility.h>
#include <isoft/naicpp/global_evloop.h>

#include "ara/crypto/common/isoft_assert.h"
#include "ara/crypto/common/isoft_log_api.h"

namespace {
/// @brief IPC Client Initialization
std::unique_ptr< ara::crypto::keys::isoft_def::PIpcClientInit > g_InitIpcPtr{nullptr};  // NOLINT
/// @brief IPC Client: Key Manager
ara::crypto::keys::isoft_def::PIpcClient::Uptr g_InstanceKeysPtr{nullptr};  // NOLINT
/// @brief IPC Client: Certificate Manager
ara::crypto::keys::isoft_def::PIpcClient::Uptr g_InstanceX509Ptr{nullptr};  // NOLINT
/// @brief IPC Client: Hsm_nxp Key Manager
ara::crypto::keys::isoft_def::PIpcClient::Uptr g_InstanceHsmPtr{nullptr};  // NOLINT
/// @brief IPC Client: Hsm_bst Key Manager
ara::crypto::keys::isoft_def::PIpcClient::Uptr g_InstanceHsmbstPtr{nullptr};  // NOLINT
}  // namespace

namespace ara {
namespace crypto {
namespace keys {
namespace isoft_def {
//********************************/
/// @brief Provide IPC service for KeyProvider externally
using ::isoft::naicpp::EvLoop;
using ::isoft::naicpp::GlobalGeneralEvLoop;
//********************************/
/// @brief Constructor
/// @qac Suspected unmodifiable [4268]: Initialization helper can be replaced with a delegator.
PIpcClientInit::PIpcClientInit() noexcept { std::ignore = _Initialize(); }
/// @brief Destructor
PIpcClientInit::~PIpcClientInit() noexcept { std::ignore = _Deinitialize(); }
/// @brief Start
/// @return true if runing false otherwise
bool PIpcClientInit::Working() noexcept
{
    if (pEvLoop_ != nullptr) {
        std::ignore = pEvLoop_->Run(true);
    }
    return true;
}
/// @brief Initialize
/// @return true if has init false otherwise
bool PIpcClientInit::_Initialize() noexcept
{
    // 2022-10-22 Initialization in ara::core::Initialize depends on whether the precompilation macro is defined
    // 2022-10-27
    // According to the internal logic of GlobalGeneralEvLoop::Initialize, this function can be called multiple times (only the first time performs initialization). So, call it again to be safe.
    std::ignore = GlobalGeneralEvLoop::Initialize(GlobalGeneralEvLoop::Mode::kInsideThread);
    std::shared_ptr< EvLoop > const evLoopPtr{GlobalGeneralEvLoop::Get()};
    if (evLoopPtr == nullptr) {
        return false;
    }
    int32_t const r{isoft::ipc::IPCInitNaiUDS(evLoopPtr)};
    if (r == -1) {
        return false;
    }
    pEvLoop_ = evLoopPtr;
    return true;
}
/// @brief Deinitialize
/// @return true if has deinit false otherwise
bool PIpcClientInit::_Deinitialize() noexcept
{
    int32_t const ret{0};
    if (-1 == isoft::ipc::IPCDeInitNaiUDS()) {
        return false;
    }
    if (pEvLoop_ != nullptr) {
        pEvLoop_.reset();
    }

    return ret == 0;
}
//********************************/
/// @brief Get the client related to encryption
/// @return Pointer to the default encryption client
PIpcClient* PIpcClient::GetInstance_Keys() noexcept
{
    if (g_InstanceKeysPtr != nullptr) {
        return g_InstanceKeysPtr.get();
    }
    if (g_InstanceKeysPtr == nullptr) {
        g_InstanceKeysPtr = std::make_unique< PIpcClient >(GetIpcName_Remote_Keys());
    }
    return g_InstanceKeysPtr.get();
}
//********************************/
/// @brief Get the client related to certificates
/// @return Pointer to the certificate client
PIpcClient* PIpcClient::GetInstance_X509() noexcept
{
    if (g_InstanceX509Ptr != nullptr) {
        return g_InstanceX509Ptr.get();
    }
    if (g_InstanceX509Ptr == nullptr) {
        g_InstanceX509Ptr = std::make_unique< PIpcClient >(GetIpcName_Remote_X509());
    }
    return g_InstanceX509Ptr.get();
}
//********************************/
/// @brief Get the client related to HSM
/// @return Pointer to the hsm nxp client
PIpcClient* PIpcClient::GetInstance_Hsm() noexcept
{
    if (g_InstanceHsmPtr != nullptr) {
        return g_InstanceHsmPtr.get();
    }
    if (g_InstanceHsmPtr == nullptr) {
        g_InstanceHsmPtr = std::make_unique< PIpcClient >(GetIpcName_Remote_Hsm());
    }
    return g_InstanceHsmPtr.get();
}
//********************************/
/// @brief Get the client related to HSM-BST
/// @return Pointer to the hsm bst client
PIpcClient* PIpcClient::GetInstance_Hsmbst() noexcept
{
    if (g_InstanceHsmbstPtr != nullptr) {
        return g_InstanceHsmbstPtr.get();
    }
    if (g_InstanceHsmbstPtr == nullptr) {
        g_InstanceHsmbstPtr = std::make_unique< PIpcClient >(GetIpcName_Remote_Hsmbst());
    }
    return g_InstanceHsmbstPtr.get();
}
//********************************/
/// @brief Initialize
/// @return true if has init false otherwise
bool PIpcClient::Initialize() noexcept
{
    // Initialization of g_pInstance is created on the first call
    if (false == g_InitIpcPtr.operator bool()) {
        g_InitIpcPtr = std::make_unique< PIpcClientInit >();
    }
    return true;
}
/// @brief Deinitialize
/// @return true if has deinit false otherwise
bool PIpcClient::Deinitialize() noexcept
{
    if (g_InstanceKeysPtr != nullptr) {
        g_InstanceKeysPtr.reset();
    }
    if (g_InstanceX509Ptr != nullptr) {
        g_InstanceX509Ptr.reset();
    }
    if (g_InitIpcPtr != nullptr) {
        g_InitIpcPtr.reset();
    }
    return true;
}
//********************************/
/// @brief Constructor with parameters
/// @param ipcClientName IPC client name
/// @qac Suspected unmodifiable [4268]: Initialization helper can be replaced with a delegator.
PIpcClient::PIpcClient(ara::core::StringView const& ipcClientName) noexcept
{
    std::ignore = PIpcClient::Initialize();
    pIpcClient_ = ::isoft::ipc::IPCClient::Create(ipcClientName.data());
    PH_ASSERT(pIpcClient_.operator bool());
    std::ignore = pIpcClient_->Start();
}
/// @brief Destructor
PIpcClient::~PIpcClient() noexcept
{
    if (pIpcClient_ != nullptr) {
        std::ignore = pIpcClient_->Stop();
        pIpcClient_->Release();
        pIpcClient_.reset();
    }
    pIpcClient_ = nullptr;
}
/// @brief Determine if preparations are complete
/// @return true if has work ready false otherwise
bool PIpcClient::IsWorkerReady() const noexcept { return pIpcClient_.operator bool(); }
/// @brief Process an IPC request once: synchronous mode
/// @param stFuncName Function name used for IPC call
/// @param aswMsg IPC-managed response message
/// @param funMakeReq Callback function for constructing the request packet
/// @param nTimeOut Timeout
/// @return true if deal sucess false otherwise
bool PIpcClient::DealIpcRequest(ara::core::StringView const& stFuncName,
                                PIpcAutoPacket& aswMsg,
                                CB_IpcReqMsg const& funMakeReq,
                                int32_t const nTimeOut) const noexcept
{
    if (false == IsWorkerReady()) {
        return false;
    }
    if (nullptr == funMakeReq) {
        return false;
    }
    uint16_t const nHeadLen{static_cast< uint16_t >(sizeof(PIpcPac_Head) + stFuncName.size())};
    uint16_t const nNeedLen{static_cast< uint16_t >(nHeadLen + funMakeReq(nullptr))};
    // Construct the request packet
    isoft::ipc::IPCPacket* const pIpcReq{pIpcClient_->MakeRequest()};
    PIpcAutoPacket reqMsg{pIpcReq, nNeedLen};
    // Configure common data items in the packet header
    std::ignore = reqMsg.InitIpcHead(stFuncName, nNeedLen);
    reqMsg.SetFirstSize(static_cast< int32_t >(nHeadLen));
    PH_ASSERT(nNeedLen > funMakeReq(&reqMsg));
    return DealIpcRequest(stFuncName, aswMsg, reqMsg, nTimeOut);
}
/// @brief Process an IPC request once: synchronous mode
/// @param stFuncName Function name used for IPC call
/// @param aswMsg IPC-managed response message
/// @param reqMsg IPC request message
/// @param nTimeOut Timeout
/// @return true if deal sucess false otherwise
bool PIpcClient::DealIpcRequest(ara::core::StringView const& stFuncName,
                                PIpcAutoPacket& aswMsg,
                                PIpcAutoPacket const& reqMsg,
                                int32_t const nTimeOut) const noexcept
{
    if (false == IsWorkerReady()) {
        return false;
    }
    // Use blocking mode
    aswMsg.ReInit();
    isoft::ipc::IPCPacket* pIpcAsw{nullptr};
    int32_t const ret{pIpcClient_->SendSync(reqMsg.GetIpcPacket(), &pIpcAsw, nTimeOut)};
    if (ret < 0) {
        ara::crypto::isoft_def::LogError() << "DealIpcRequest, RequestCmdID = [" << stFuncName.data() << "]";
        return false;
    }
    aswMsg.AttachPacket(pIpcAsw, true);
    // Process the return packet: then handle at the function call site
    return true;
}
/// @brief Process an IPC request once: asynchronous mode
/// @param stFuncName Function name used for IPC call
/// @param funMakeReq Callback function for constructing the request packet
/// @param handler IPC client handle
/// @param context IPC message context information
/// @param nTimeOut Timeout
/// @return true if Async send request sucess false otherwise
bool PIpcClient::AsyncIpcRequest(ara::core::StringView const& stFuncName,
                                 CB_IpcReqMsg const& funMakeReq,
                                 isoft::ipc::IPCClientHandler const& handler,
                                 void* const context,
                                 int32_t const nTimeOut) const noexcept
{
    if (false == IsWorkerReady()) {
        return false;
    }
    if (nullptr == funMakeReq) {
        return false;
    }
    uint16_t nNeedLen{funMakeReq(nullptr)};
    nNeedLen += static_cast< uint16_t >(sizeof(PIpcPac_Head) + stFuncName.size());
    isoft::ipc::IPCPacket* const pIpcReq{pIpcClient_->MakeRequest()};
    // 2023-01-13 Temporarily not modified: the real solution is the same as DealIpcRequest
    PIpcAutoPacket reqMsg{pIpcReq, nNeedLen};
    // Configure common data items in the packet header
    bool const ret1{reqMsg.InitIpcHead(stFuncName, nNeedLen)};
    std::ignore = ret1;
    // Construct the request packet
    PH_ASSERT(nNeedLen > funMakeReq(&reqMsg));
    // Use blocking mode
    int32_t const ret{pIpcClient_->SendAsync(pIpcReq, true, handler, nTimeOut, context)};

    if (ret < 0) {
        ara::crypto::isoft_def::LogError() << "AsyncIpcRequest, RequestCmdID = [" << reqMsg.GetFuncName() << "]";
        return false;
    }
    // Process the return packet: modify at the function call site
    return true;
}
/// @brief Register subscription
/// @param bReg Whether to register
/// @param handler IPC client handle
/// @param context IPC message context information
/// @return true if deal Observer Regist sucess false otherwise
bool PIpcClient::DealObserverReg(bool const bReg,
                                 isoft::ipc::IPCClientHandler const& handler,
                                 void* const context) const noexcept
{
    if (false == IsWorkerReady()) {
        return false;
    }
    // Subscription messages are asynchronous
    isoft::ipc::IPCPacket* const pIpcReq{pIpcClient_->MakeRequest()};
    ara::core::StringView const stFuncName{FUNC_NAME_KeyProvider(RegisterObserver)};
    uint16_t const nNeedLen{static_cast< uint16_t >(sizeof(PIpcPac_Head) + stFuncName.size()
                                                    + sizeof(keyprovider::PIpcReq_RegisterObserver))};
    PIpcAutoPacket ipcPacket{pIpcReq, nNeedLen};
    bool const ret{ipcPacket.InitIpcHead(stFuncName, nNeedLen)};
    std::ignore = ret;
    PIpcPac_Head* const pIpcHead{static_cast< PIpcPac_Head* >(static_cast< void* >(ipcPacket.data()))};
    keyprovider::PIpcReq_RegisterObserver* const pRegObserverPacket{
        T_TransPtr< uint8_t, keyprovider::PIpcReq_RegisterObserver >(T_TransBytes(pIpcHead + 1)
                                                                     + pIpcHead->nFuncNameLen)};
    if (bReg) {
        pRegObserverPacket->nActionPid = pIpcHead->nProcessID;
    } else {
        pRegObserverPacket->nActionPid = 0U;
    }
    int32_t const nReturn{static_cast< int32_t >(pIpcClient_->SendAsync(pIpcReq, true, handler, -1, context))};
    return 0 == nReturn;
}
//********************************/
/// @brief Traverse the list of KeySlots
/// @param targetSlots Vector array of target key slots
/// @param fun Callback function for processing IPC messages
/// @return
uint32_t PIpcClient::ForEachKeySlot(TransactionScope const& targetSlots,
                                    std::function< bool(KeySlot const* keySlot) > const& fun) noexcept
{
    uint32_t nReturn{0U};
    for (auto const& itData : targetSlots) {
        if (false == fun(itData)) {
            return nReturn;
        }
        nReturn += 1U;
    }
    return nReturn;
}
//********************************/
}  // namespace  isoft_def
}  // namespace keys
}  // namespace crypto
}  // namespace ara
