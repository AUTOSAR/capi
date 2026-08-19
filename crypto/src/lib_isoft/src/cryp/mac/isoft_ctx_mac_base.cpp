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
/// @file       isoft_ctx_mac_base.cpp
/// @brief      AutoSar-Crypto Encryption and Decryption Module
/// @details
/// @date       2022-04-15
/// @author     Zheng Chang
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default Encryption and Decryption/Message Authentication Code
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_01005
/// @unit_name=PCtxMac_Base
/// @unit_description=Message Authentication Code Base Class
/// @endcode
///
/// ================================================================

#include "ara/crypto/cryp/mac/isoft_ctx_mac_base.h"

#include "ara/crypto/common/isoft_common_api.h"
#include "ara/crypto/cryp/cryobj/isoft_key_symmetric.h"
#include "ara/crypto/cryp/cryobj/isoft_key_symmetric_ipc.h"
#include "ara/crypto/cryp/cryobj/isoft_secret_seed.h"
#include "ara/crypto/cryp/cryobj/isoft_signature.h"
#include "ara/crypto/cryp/isoft_crypto_provider.h"
#include "ara/crypto/ipc/isoft_ipc_auto_message.h"
#include "ara/crypto/ipc/isoft_ipc_client.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
/// @brief Constructor with parameters
/// @param cryptoProvider Crypto provider
/// @throws
PCtxMac_Base::PCtxMac_Base(PCryptoProvider &cryptoProvider) noexcept
    : PCtxMac_Base{cryptoProvider, EMacWorkState::kMacNotStarted, nullptr, CryptoTransform::kUnknown, {}}
{
}
/// @brief Constructor with parameters
/// @param cryptoProvider Crypto provider
/// @param eMacState MAC working state
/// @param pSymmetricKey Pointer to symmetric key
/// @param eCryptoTransform Encryption direction: encrypt or decrypt, etc.
/// @param secretSeedCouId Secret seed encryption material ID
PCtxMac_Base::PCtxMac_Base(PCryptoProvider &cryptoProvider,
                           EMacWorkState const eMacState,
                           SymmetricKey const *const pSymmetricKey,
                           CryptoTransform const eCryptoTransform,
                           CryptoObjectUid const &secretSeedCouId) noexcept
    : MessageAuthnCodeCtx{}  // NOLINT
    , cryptoProvider_{cryptoProvider}
    , eMacState_{eMacState}
    , pSymmetricKey_{pSymmetricKey}
    , eCryptoTransform_{eCryptoTransform}
    , secretSeedCouId_{secretSeedCouId}
{
    cmacCtx_ = openssl::isoft_def::CMAC_CTX_new();
}
/// @brief Destructor
PCtxMac_Base::~PCtxMac_Base() noexcept
{
    if (cmacCtx_ != nullptr) {
        openssl::isoft_def::CMAC_CTX_free(cmacCtx_);
        cmacCtx_ = nullptr;
    }
}
/// @brief Get a reference to the Crypto Provider for this context.
/// @return Reference to the crypto provider
// PRQA S 4623 QAC /// @qac: AUTOSAR standard interface
inline CryptoProvider &PCtxMac_Base::MyProvider() const noexcept { return cryptoProvider_; }
// PRQA L:QAC

/// @brief Compare the computed digest with the expected "signature" object.
///         The entire digest value is stored in the context until the next call to Start(), so it can be verified or extracted again. This method can be implemented "inline" after the function ara::core::memcmp() is standardized.
/// @brief Check the calculated digest against an expected "signature" object.
/// Entire digest value is kept in the context up to next call @c Start(), therefore it can be verified again or
/// extracted. This method can be implemented as "inline" after standartization of function @c ara::core::memcmp().
/// @param expected  the signature object containing an expected digest value
/// @returns @c true if value and meta-information of the provided "signature" object is identical to calculated
/// digest and current configuration of the context respectively; but @c false otherwise
/// @trace_id_sws={SWS_CRYPT_22119}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02203}
/// @uptrace={RS_CRYPTO_02204}
/// @error: SecurityErrorDomain::kProcessingNotFinished  if the digest calculation was not finished by a call of the @c
/// Finish() method
/// @error: SecurityErrorDomain::kIncompatibleObject     if the provided "signature" object was produced by another
/// crypto primitive type
/// @threadsafety={Thread-safe}
ara::core::Result< bool > PCtxMac_Base::Check(Signature const &expected) const noexcept
{
    // SWS_CRYPT_01213 Verify
    if (eMacState_ != EMacWorkState::kMacFinish) {
        return ara::core::Result< bool >::FromError(SecurityErrorDomain::Errc::kProcessingNotFinished);
    }

    if (!expected.GetDependence().HasSameSourceAs(pSymmetricKey_->GetObjectId().mCouid)) {
        return ara::core::Result< bool >::FromError(SecurityErrorDomain::Errc::kIncompatibleObject);
    }

    ara::core::Result< ara::core::Vector< ara::core::Byte > > const result{
        expected.ExportPublicly(Serializable::kFormatRawValueOnly)};

    if (false == result.HasValue()) {
        return ara::core::Result< bool >::FromValue(false);
    }

    ara::core::Vector< ara::core::Byte > const &vecCrcData{result.Value()};
    if (vecCrcData.size() != GetMacLength()) {
        return ara::core::Result< bool >::FromValue(false);
    }

    uint8_t const *const macdata{GetMacResult()};
    for (uint32_t i{0U}; i < vecCrcData.size(); ++i) {
        ara::core::Byte const byExpected{*(macdata + i)};
        if (vecCrcData[static_cast< size_t >(i)] != byExpected) {
            return ara::core::Result< bool >::FromValue(false);
        }
    }
    return ara::core::Result< bool >::FromValue(true);
}

/// @brief Set (deploy) a key for the message authentication code algorithm context.
/// @brief Set (deploy) a key to the message authn code algorithm context.
/// @name  SetKey
/// @param key  the source key object
/// @param transform  the "direction" indicator: deploy the key for direct transformation (if @c true) or for
/// reverse one (if @c false)
/// @return has value if setkey sucess false otherwise
/// @trace_id_sws={SWS_CRYPT_22118}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02001}
/// @uptrace={RS_CRYPTO_02003}
/// @error: SecurityErrorDomain::kIncompatibleObject     if the provided key object is incompatible with this symmetric
/// key context
/// @error: SecurityErrorDomain::kUsageViolation         if the transformation type associated with this context (taking
/// into account the direction specified by @c transform) is prohibited by the "allowed usage" restrictions of provided
/// key object
/// @threadsafety={Thread-safe}
ara::core::Result< void > PCtxMac_Base::SetKey(SymmetricKey const &key, CryptoTransform transform) noexcept
{
    /// SWS_CRYPT_01211
    /// @error: SecurityErrorDomain::kInvalidArgument  if the provided transformation direction is not allowed
    ///  in Message Authn Code algorithm context
    ///         If the "allowed usage" restrictions of the provided key object prohibit the transformation type associated with this context
    if ((transform != CryptoTransform::kMacGenerate) && (transform != CryptoTransform::kMacVerify)) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kInvalidArgument);
    }

    uint32_t const nKeyAllowedUsage{key.GetAllowedUsage()};
    if (transform == CryptoTransform::kMacGenerate) {
        if (((kAllowDataEncryption & nKeyAllowedUsage) != kAllowDataEncryption)
            || ((kAllowSignature & nKeyAllowedUsage) != kAllowSignature)) {
            return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUsageViolation);
        }
    } else {
        if (((kAllowDataDecryption & nKeyAllowedUsage) != kAllowDataDecryption)
            || ((kAllowVerification & nKeyAllowedUsage) != kAllowVerification)) {
            return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUsageViolation);
        }
    }

    PKeySymmetric_Ipc const *const pPKeySymmetricIpc{dynamic_cast< PKeySymmetric_Ipc const * >(&key)};
    if (pPKeySymmetricIpc != nullptr) {  // ipc
        return _SetKeyIpc(key, transform);
    }
    if (false == CheckKey(key)) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kIncompatibleObject);
    }

    pSymmetricKey_    = &key;
    eCryptoTransform_ = transform;
    return ara::core::Result< void >::FromValue();
}

/// @brief Set (deploy) a key for the symmetric algorithm context. for ipc
/// @param key Symmetric key
/// @param transform Encryption direction: encrypt or decrypt
/// @return has value if setkey sucess false otherwise
ara::core::Result< void > PCtxMac_Base::_SetKeyIpc(SymmetricKey const &key, CryptoTransform const transform) noexcept
{
    if (false == CheckKey(key)) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kIncompatibleObject);
    }

    pSymmetricKey_    = &key;
    eCryptoTransform_ = transform;
    ara::core::Result< ara::core::Vector< ara::core::Byte > > const resByte{_DoIpcFun(FUNC_NAME_Mac(SetKey))};
    if (!resByte.HasValue()) {
        pSymmetricKey_    = nullptr;
        eCryptoTransform_ = ara::crypto::CryptoTransform::kUnknown;
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kLogicFault);
    }
    return ara::core::Result< void >::FromValue();
}
/// @brief Perform initialization operation via IPC
/// @param piv Pointer to initialization vector
/// @return true if _DoInitIpc sucess false otherwise
bool PCtxMac_Base::_DoInitIpc(uint8_t const *const piv) const noexcept
{
    uint32_t nPivLen{0U};
    if (piv != nullptr) {
        nPivLen = static_cast< uint32_t >(strlen(T_TransChar(piv)));
    }
    ara::core::Result< ara::core::Vector< ara::core::Byte > > const resByte{
        _DoIpcFun(FUNC_NAME_Mac(DoInit), piv, nPivLen)};
    if (!resByte.HasValue()) {
        return false;
    }
    return true;
}
/// @brief Perform finalization operation via IPC
/// @return true if _DoFinishIpc sucess false otherwise
bool PCtxMac_Base::_DoFinishIpc() const noexcept
{
    ara::core::Result< ara::core::Vector< ara::core::Byte > > resByte{_DoIpcFun(FUNC_NAME_Mac(DoFinish))};
    if (!resByte.HasValue()) {
        return false;
    }
    ara::core::Vector< ara::core::Byte > vecData{std::move(resByte).Value()};
    if (vecData.empty()) {
        return false;
    }
    std::size_t const nLen{vecData.size()};
    std::ignore = std::memcpy(static_cast< void * >(GetMacResult()), static_cast< void const * >(vecData.data()), nLen);
    return true;
}
/// @brief Perform update operation via IPC
/// @param pVoidData Starting address of data
/// @param nDataLen Data length
/// @return true if _DoUpdateIpc sucess false otherwise
bool PCtxMac_Base::_DoUpdateIpc(void const *const pVoidData, uint32_t const nDataLen) const noexcept
{
    ara::core::Result< ara::core::Vector< ara::core::Byte > > const resByte{
        _DoIpcFun(FUNC_NAME_Mac(DoUpdate), pVoidData, nDataLen)};
    if (!resByte.HasValue()) {
        return false;
    }
    return true;
}
/// @brief Perform reset operation via IPC
/// @return true if DoResetIpc sucess false otherwise
bool PCtxMac_Base::DoResetIpc() noexcept
{
    /// In IPC operations, the object is deleted after finish, so there is no need to perform reset operation
    return true;
}
/// @brief Execute function via IPC
/// @param funName Function name within the IPC data packet
/// @param pVoidData Starting address of data
/// @param nDataLen Data length
/// @return MAC execution result
ara::core::Result< ara::core::Vector< ara::core::Byte > > PCtxMac_Base::_DoIpcFun(
    ara::core::StringView const &funName, void const *const pVoidData, uint32_t const nDataLen) const noexcept
{
    PKeySymmetric_Ipc const *const pIpcKey{dynamic_cast< PKeySymmetric_Ipc const * >(pSymmetricKey_)};
    if (pIpcKey == nullptr) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kLogicFault);
    }
    keys::isoft_def::PIpcClient *const ipcClient{keys::isoft_def::PIpcClient::GetInstance_Keys()};
    if (ipcClient == nullptr) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kLogicFault);
    }

    uint32_t const nTransform{static_cast< uint32_t >(eCryptoTransform_)};
    uint32_t const nSlotId{pIpcKey->GetSlotId()};
    ara::crypto::cryp::CryptoPrimitiveId::AlgId const algid{GetCryptoPrimitiveId()->GetPrimitiveId()};
    keys::isoft_def::PIpcAutoPacket aswMsg;  // Receive response packet
    bool const bDealIpc{ipcClient->DealIpcRequest(
        funName, aswMsg,
        [nSlotId, nDataLen, pVoidData, nTransform,
         algid](keys::isoft_def::PIpcAutoPacket const *const pReqMsg) -> uint16_t {
            if (pReqMsg != nullptr) {
                keys::isoft_def::cryptoctx::PIpcReq_DoMac ipcReq;
                ipcReq.nIpcSlotID = nSlotId;
                ipcReq.nTransform = nTransform;
                ipcReq.nAlgID     = static_cast< uint32_t >(algid);
                ipcReq.nDataLen   = nDataLen;
                std::ignore       = pReqMsg->AddDataToIpc(&ipcReq, sizeof(ipcReq));
                if (pVoidData != nullptr) {
                    std::ignore = pReqMsg->AddDataToIpc(pVoidData, static_cast< uint16_t >(nDataLen));
                }
            }
            return static_cast< uint16_t >(sizeof(keys::isoft_def::cryptoctx::PIpcReq_DoMac)
                                           + static_cast< size_t >(nDataLen));
        })};
    if (false == bDealIpc) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kLogicFault);
    }
    keys::isoft_def::cryptoctx::PIpcAsw_DoMac *const pIpcAsw{
        aswMsg.GetIpcBody< keys::isoft_def::cryptoctx::PIpcAsw_DoMac >()};
    if (0 != pIpcAsw->GetErrorID()) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kLogicFault);
    }
    ara::core::Vector< ara::core::Byte > vecData;
    uint8_t *const pData{pIpcAsw->GetData()};
    uint32_t const pDataLen{pIpcAsw->GetDataLen()};
    if (pData != nullptr) {
        vecData.resize(static_cast< size_t >(pDataLen));
        void *const pDesData{static_cast< void * >(vecData.data())};
        std::ignore = memcpy(pDesData, pData, static_cast< size_t >(pDataLen));
    }
    return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromValue(vecData);
}

/// @brief Perform initialization operation
/// @name  _DoInit
/// @param piv Pointer to initialization vector
/// @returns  true if _DoInit sucess false otherwise
bool PCtxMac_Base::_DoInit(uint8_t const *const piv) noexcept
{
    PKeySymmetric_Ipc const *const pPKeySymmetricIpc{dynamic_cast< PKeySymmetric_Ipc const * >(GetKey())};
    if (pPKeySymmetricIpc != nullptr) {
        return _DoInitIpc(piv);
    }
    return DoInitLocal(piv);
}
/// @brief Perform update operation
/// @name  DoUpdate
/// @param pVoidData Starting address of data
/// @param nDataLen Data length
/// @returns  true if DoUpdate sucess false otherwise
bool PCtxMac_Base::DoUpdate(void const *pVoidData, uint32_t nDataLen) noexcept
{
    PKeySymmetric_Ipc const *const pPKeySymmetricIpc{dynamic_cast< PKeySymmetric_Ipc const * >(GetKey())};
    if (pPKeySymmetricIpc != nullptr) {
        return _DoUpdateIpc(pVoidData, nDataLen);
    }
    return DoUpdateLocal(pVoidData, nDataLen);
}
/// @brief Perform finalization operation via IPC
/// @name  DoFinish
/// @returns  true if DoFinish sucess false otherwise
bool PCtxMac_Base::DoFinish() noexcept
{
    PKeySymmetric_Ipc const *const pPKeySymmetricIpc{dynamic_cast< PKeySymmetric_Ipc const * >(GetKey())};
    if (pPKeySymmetricIpc != nullptr) {
        return _DoFinishIpc();
    }
    return DoFinishLocal();
}
/// @brief Perform Reset logic
/// @return true if DoReset sucess false otherwise
bool PCtxMac_Base::DoReset() noexcept
{
    PKeySymmetric_Ipc const *const pPKeySymmetricIpc{dynamic_cast< PKeySymmetric_Ipc const * >(GetKey())};
    if (pPKeySymmetricIpc != nullptr) {
        return DoResetIpc();
    }
    return DoResetLocal();
}
/// @brief SWS_CRYPT_01207、SWS_CRYPT_22115
/// @param makeSignatureObject Whether to generate a signature object
/// @return unique smart pointer to created signature object, if(makeSignatureObject == true) or nullptr if (makeSignatureObject == false)
ara::core::Result< Signature::Uptrc > PCtxMac_Base::Finish(bool makeSignatureObject) noexcept
{
    // SWS_CRYPT_01207、SWS_CRYPT_22115:
    // If Start has not been successfully called before, Finish will return a SecurityErrorDomain::kProcessingNotStarted error.
    if (eMacState_ == EMacWorkState::kMacNotStarted) {
        return ara::core::Result< Signature::Uptrc >::FromError(SecurityErrorDomain::Errc::kProcessingNotStarted);
    }

    // SWS_CRYPT_01207:
    // If Update has not been successfully called after the last call to Start, Finish will return a SecurityErrorDomain::kInvalidUsageOrder error
    // 2311: SWS_CRYPT_01207:
    // If Update has not been successfully called after the last call to Start, Finish will return a SecurityErrorDomain::kUsageViolation error
    if (eMacState_ == EMacWorkState::kMacStart) {
        return ara::core::Result< Signature::Uptrc >::FromError(SecurityErrorDomain::Errc::kUsageViolation);
    }

    // First call _DoFinish()
    std::ignore = DoFinish();

    // Then make the judgment:
    // SWS_CRYPT_22115:
    // If the buffered digest belongs to a MAC/HMAC/AE/AEAD context initialized by a key without kAllowSignature permission,
    // but (makeSignatureObject == true), then return SecurityErrorDomain::kUsageViolation error
    // 1 Use length to determine whether the digest belongs to a MAC/HMAC/AE/AEAD context
    uint32_t const nKeyAllowedUsage{pSymmetricKey_->GetAllowedUsage()};
    if ((kAllowSignature & nKeyAllowedUsage)
        != kAllowSignature) {  // If not equal, it means no kAllowSignature permission
        if (makeSignatureObject == true) {
            return ara::core::Result< Signature::Uptrc >::FromError(SecurityErrorDomain::Errc::kUsageViolation);
        }
    }

    PSignature::Uptr pSignature{nullptr};
    eMacState_ = EMacWorkState::kMacFinish;
    if (makeSignatureObject) {
        return MakeSignature();
    }
    return ara::core::Result< Signature::Uptrc >::FromValue(std::move(pSignature));
}
/// @brief Complete the digest calculation and optionally generate a "signature" object. Only after calling this method can the digest be signed, verified, extracted, or compared!
///         If the signature object is generated by a keyed MAC/HMAC/AE/AEAD algorithm, the dependent COUID of the "signature" should be set to the COUID of the symmetric key used.
/// @brief Finish the digest calculation and optionally produce the "signature" object.
///         Only after call of this method the digest can be signed, verified, extracted or compared!
///         If the signature object produced by a keyed MAC/HMAC/AE/AEAD algorithm then the dependence COUID of the
///         "signature" should be set to COUID of used symmetric key.
/// @return ara::core::Result< void >
ara::core::Result< void > PCtxMac_Base::Finish() noexcept
{
    ara::core::Result< Signature::Uptrc > const resSig{Finish(false)};
    if (resSig.HasValue() == false) {
        return ara::core::Result< void >::FromError(resSig.Error());
    }
    return ara::core::Result< void >::FromValue();
}
/// @brief SWS_CRYPT_01210
/// @param offset Offset
/// @return number of digest bytes really stored to the output buffer (they are always <= @c output.size() and
/// denoted below as @a return_size)
ara::core::Result< ara::core::Vector< ara::core::Byte > > PCtxMac_Base::GetDigest(std::size_t offset) const noexcept
{
    /// SWS_CRYPT_01219、SWS_CRYPT_01220、SWS_CRYPT_22116: if the digest calculation was not finished by a call of the
    /// Finish() method -> SecurityErrorDomain::kProcessingNotFinished
    if (eMacState_ != EMacWorkState::kMacFinish) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kProcessingNotFinished);
    }

    /// @error: SecurityErrorDomain::kUsageViolation  if the buffered digest belongs to a MAC/HMAC/AE/AEAD context
    /// initialized by a key without @c kAllowSignature permission
    /// If the buffered digest belongs to a MAC/HMAC/AE/AEAD context initialized by a key without kAllowSignature permission,
    /// return SecurityErrorDomain::kUsageViolation error
    // if() -> return(kUsageViolation)
    uint32_t const nKeyAllowedUsage{pSymmetricKey_->GetAllowedUsage()};
    if ((kAllowSignature & nKeyAllowedUsage)
        != kAllowSignature) {  // If not equal, it means no kAllowSignature permission
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kUsageViolation);
    }

    ara::core::Vector< ara::core::Byte > resultByte;
    //
    uint8_t const *const pData{GetMacResult()};
    uint32_t const nDataLen{GetMacLength()};
    for (uint32_t i{static_cast< uint32_t >(offset)}; i < nDataLen; ++i) {
        ara::core::Byte const byData{*(pData + i)};
        resultByte.push_back(byData);
    }
    return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromValue(resultByte);
}

/// @brief Reset the result
/// @return has vlaue if Reset sucess false otherwise
ara::core::Result< void > PCtxMac_Base::Reset() noexcept
{
    if (!DoReset()) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kLogicFault);
    }
    eMacState_ = EMacWorkState::kMacNotStarted;
    return ara::core::Result< void >::FromValue();
}

/// @brief Initialize the context for a new data stream processing or generation (depending on the primitive).
/// @param iv Initialization secret seed
/// @return has vlaue if Start sucess false otherwise
/// @trace_id_sws={SWS_CRYPT_22110}
/// @tracestatus={draft}
ara::core::Result< void > PCtxMac_Base::Start(ReadOnlyMemRegion const &iv) noexcept
{
    // SWS_CRYPT_01201: First, determine if the context has been used before, if so, reset
    if (eMacState_ == EMacWorkState::kMacFinish) {
        std::ignore
            = Reset();  // Reset sets kMacFinish to kMacNotStarted, Start sets kMacNotStarted to kMacStart, forming a cycle
    }

    if (SupportIv() == false) {
        if (iv.empty() == false) {
            return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUnsupported);
        }
    } else {
        // error2 If the size of the provided IV is not supported (i.e., if it is insufficient for initialization), return SecurityErrorDomain::kInvalidInputSize error;
        // Check the size of iv, if(IV.size < MAX) -> SecurityErrorDomain::kInvalidInputSize
        // Using iv.empty == false to judge may miss this case: IV is supported but iv is empty
        if (iv.size() < GetIvMaxLength()) {
            return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kInvalidInputSize);
        }
    }

    // error3 If the context has not been initialized by deploying a key, return SecurityErrorDomain::kUninitializedContext error;
    // if(pSymmetricKey_ == nullptr) -> ecurityErrorDomain::kUninitializedContext
    if (pSymmetricKey_ == nullptr) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUninitializedContext);
    }

    uint8_t const *piv{nullptr};
    if (iv.empty() == false) {  // Either _SupportIv() == true works
        // SWS_CRYPT_01202: If the iv size exceeds the max length, only use the first max length bytes
        ReadOnlyMemRegion const tempIv{iv.first(static_cast< size_t >(GetIvMaxLength()))};
        // Consider the case where iv size exceeds max length
        piv = static_cast< uint8_t const * >(tempIv.data());
    }

    // Reset the result, internally calls CMAC_Init for initialization
    if (_DoInit(piv) == false) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kInitFailed);
    }

    eMacState_       = EMacWorkState::kMacStart;
    secretSeedCouId_ = CryptoObjectUid();
    return ara::core::Result< void >::FromValue();
}

/// @brief Initialize the context for a new data stream processing or generation (depending on the primitive).
/// @param iv SWS_CRYPT_01203: SecretSeed can also serve as IV
/// @return has vlaue if Start sucess false otherwise
/// @trace_id_sws={SWS_CRYPT_22111}
/// @tracestatus={draft}
ara::core::Result< void > PCtxMac_Base::Start(SecretSeed const &iv) noexcept
{
    // SWS_CRYPT_01201: First, determine if the context has been used before, if so, reset // To be tested
    if (eMacState_ == EMacWorkState::kMacFinish) {
        std::ignore
            = Reset();  // Reset sets kMacFinish to kMacNotStarted, Start sets kMacNotStarted to kMacStart, forming a cycle
    }

    // error3 If the base algorithm (or its current implementation) does not fundamentally support IV variants, return SecurityErrorDomain::kUnsupported error;
    if (SupportIv()
        == false) {  // Different from the error in SWS_CRYPT_22110, this means algorithms that do not support IV cannot call this Start()!
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUnsupported);
    }

    // error1 If the context has not been initialized by deploying a key, return SecurityErrorDomain::kUninitializedContext error;
    if (pSymmetricKey_ == nullptr) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUninitializedContext);
    }

    // error2 If the size of the provided IV is not supported (i.e., if it is insufficient for initialization), return SecurityErrorDomain::kInvalidInputSize error;
    // Check the size of iv, if(IV.size < MAX) -> SecurityErrorDomain::kInvalidInputSize
    uint32_t const ivMaxLength{GetIvMaxLength()};
    if (iv.GetPayloadSize() < ivMaxLength) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kInvalidInputSize);
    }

    // error4 If the "allowed usage" restrictions of the provided Secret Seed object prohibit this transformation type, return SecurityErrorDomain::kUsageViolation error
    // if this transformation type is prohibited by the
    // "allowed usage" restrictions of the provided SecretSeed object
    AllowedUsageFlags const nKeyAllowedUsage{iv.GetAllowedUsage()};  // To be checked
    if (eCryptoTransform_ == CryptoTransform::kMacGenerate) {
        if (((kAllowDataEncryption & nKeyAllowedUsage) != kAllowDataEncryption)
            || ((kAllowSignature & nKeyAllowedUsage) != kAllowSignature)) {
            return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUsageViolation);
        }
    } else {
        if (((kAllowDataDecryption & nKeyAllowedUsage) != kAllowDataDecryption)
            || ((kAllowVerification & nKeyAllowedUsage) != kAllowVerification)) {
            return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUsageViolation);
        }
    }

    PSecretSeed const *const pSecretSeed{dynamic_cast< PSecretSeed const * >(&iv)};
    if (pSecretSeed == nullptr) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kInvalidInputSize);
    }

    // SWS_CRYPT_01202: If the iv size exceeds the max length, only use the first max length bytes
    uint8_t const *const pSecretSeedData{pSecretSeed->GetSecretSeedData()};
    uint8_t *const pTempIv{new uint8_t[static_cast< size_t >(ivMaxLength)]};

    std::ignore = memcpy(pTempIv, pSecretSeedData, static_cast< size_t >(ivMaxLength));

    uint8_t const *const piv{pTempIv};

    // Reset the result
    if (!_DoInit(piv)) {
        delete[](pTempIv);
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kInitFailed);
    }

    delete[](pTempIv);
    eMacState_       = EMacWorkState::kMacStart;
    secretSeedCouId_ = iv.GetObjectId().mCouid;
    return ara::core::Result< void >::FromValue();
}

/// @brief Update the digest calculation context with a new part of the message. This method is dedicated to the case where RestrictedUseObject is part of the "message".
/// @param in Input data buffer
/// @return has vlaue if Update sucess false otherwise
/// @trace_id_sws={SWS_CRYPT_22112}
/// @tracestatus={draft}
ara::core::Result< void > PCtxMac_Base::Update(RestrictedUseObject const &in) noexcept
{
    // SWS_CRYPT_00909: If Start has not been called before, Update will return a SecurityErrorDomain::kProcessingNotStarted error.
    if ((eMacState_ != EMacWorkState::kMacStart) && (eMacState_ != EMacWorkState::kMacUpdate)) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kProcessingNotStarted);
    }

    uint8_t const *pData{nullptr};
    if (in.GetObjectId().mCOType == CryptoObjectType::kSymmetricKey) {
        PKeySymmetric_Base const *const pKeySymmetricBase{dynamic_cast< PKeySymmetric_Base const * >(&in)};
        if (pKeySymmetricBase == nullptr) {
            return ara::core::Result< void >::FromValue();
        }
        pData = pKeySymmetricBase->GetKeySymmetric();
    } else {
        PSecretSeed const *const pSecretSeed{dynamic_cast< PSecretSeed const * >(&in)};
        if (pSecretSeed == nullptr) {
            return ara::core::Result< void >::FromValue();
        }
        pData = pSecretSeed->GetSecretSeedData();
    }

    if (!DoUpdate(pData, static_cast< uint32_t >(in.GetPayloadSize()))) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUpdateFailed);
    }

    eMacState_ = EMacWorkState::kMacUpdate;
    return ara::core::Result< void >::FromValue();
}

/// @brief Update the digest calculation context with a new part of the message.
/// @param in Input data buffer
/// @return has vlaue if Update sucess false otherwise
/// @trace_id_sws={SWS_CRYPT_22113}
/// @tracestatus={draft}
ara::core::Result< void > PCtxMac_Base::Update(ReadOnlyMemRegion const &in) noexcept
{
    // SWS_CRYPT_00909: If Start has not been called before, Update will return a SecurityErrorDomain::kProcessingNotStarted error.
    // SWS_CRYPT_01205: If Start has not been called before, Update will return a SecurityErrorDomain::kProcessingNotStarted error.
    if ((eMacState_ != EMacWorkState::kMacStart) && (eMacState_ != EMacWorkState::kMacUpdate)) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kProcessingNotStarted);
    }

    // Reset the result
    if (!DoUpdate(in.data(), static_cast< uint32_t >(in.size()))) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUpdateFailed);
    }

    eMacState_ = EMacWorkState::kMacUpdate;
    return ara::core::Result< void >::FromValue();
}

/// @brief Update the digest calculation context with a new part of the message. This method facilitates handling of constant tags.
/// @param in Input data buffer
/// @return has vlaue if Update sucess false otherwise
/// @trace_id_sws={SWS_CRYPT_22114}
/// @tracestatus={draft}
ara::core::Result< void > PCtxMac_Base::Update(std::uint8_t const &in) noexcept
{
    // SWS_CRYPT_00909: If Start has not been called before, Update will return a SecurityErrorDomain::kProcessingNotStarted error.
    if ((eMacState_ != EMacWorkState::kMacStart) && (eMacState_ != EMacWorkState::kMacUpdate)) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kProcessingNotStarted);
    }
    if (!DoUpdate(&in, 1U)) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUpdateFailed);
    }

    eMacState_ = EMacWorkState::kMacUpdate;
    return ara::core::Result< void >::FromValue();
}
/// @brief Create a signature object for this MAC. The dependent COUID of this object should be set to the symmetric key used to generate the MAC.
/// @brief Create a Signature object for this MAC.The dependence COUID of this object shall be set to the
///        symmetric key used to generate the MAC.
/// @return   unique smart pointer to serialized signature
ara::core::Result< Signature::Uptrc > PCtxMac_Base::MakeSignature() noexcept
{
    if (eMacState_ != EMacWorkState::kMacFinish) {
        return ara::core::Result< Signature::Uptrc >::FromError(SecurityErrorDomain::Errc::kProcessingNotFinished);
    }
    CryptoPrimitiveId::AlgId nAlgID{GetHashAlgId()};
    PSignature::Uptr pSignature{nullptr};

    // SWS_CRYPT_01208, SWS_CRYPT_01209
    // If generated by MAC, HMAC, AE, etc., the dependent COUID of the signature object should be set to the COUID of the symmetric key used, and the hash algorithm ID field of the signature is set to Unknown
    pSignature = std::make_unique< PSignature >(nAlgID);
    if (nAlgID == 0U) {
    }
    pSignature->SetSignatureData(static_cast< CryptoAlgId >(EPhCtxTypeID::kUnDefine), GetMacResult(), GetMacLength());
    pSignature->SetDependence(pSymmetricKey_->GetObjectId());

    return ara::core::Result< Signature::Uptrc >::FromValue(std::move(pSignature));
}

/// @brief Check whether the encryption context has been initialized and is ready for use. It checks all required values, including: key value, IV/seed, etc.
/// @return true if already init false otherwise
bool PCtxMac_Base::IsInitialized() const noexcept { return IsStarted() && (pSymmetricKey_ != nullptr); }

/// @brief Check the current state of stream processing: whether it has been started.
/// @return true if already started false otherwise
bool PCtxMac_Base::IsStarted() const noexcept
{
    return (EMacWorkState::kMacStart == eMacState_) || (EMacWorkState::kMacUpdate == eMacState_);
}

/// @brief Check the current state of stream processing: whether it has been completed.
/// @return true if already Finished false otherwise
bool PCtxMac_Base::IsFinished() const noexcept { return eMacState_ == EMacWorkState::kMacFinish; }

/// @brief Compare the calculated digest with the expected value.
/// @param expected Expected value
/// @param offset Offset
/// @return true if equal false otherwise
bool PCtxMac_Base::Compare(ReadOnlyMemRegion const &expected, std::size_t offset) const noexcept
{
    /// SWS_CRYPT_01219 Reading the MAC before Finalize and comparing it with another MAC will throw a distinct error
    if (eMacState_ != EMacWorkState::kMacFinish) {
        return false;
    }
    ara::core::Result< ara::core::Vector< ara::core::Byte > > const autoResult{GetDigest(offset)};
    if (false == autoResult.HasValue()) {
        return false;
    }
    ara::core::Vector< ara::core::Byte > const &vecCrcData{autoResult.Value()};
    if (vecCrcData.size() != expected.size()) {
        return false;
    }
    for (uint32_t i{0U}; i < vecCrcData.size(); ++i) {
        ara::core::Byte const byExpected{expected[static_cast< size_t >(i)]};
        if (vecCrcData[static_cast< size_t >(i)] != byExpected) {
            return false;
        }
    }
    return true;
}

}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara