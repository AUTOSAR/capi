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
/// @file       isoft_ctx_symmetric_key_wrapper_des.cpp
/// @brief      AutoSar-Crypto Encryption and Decryption Module
/// @details
/// @date       2023-07-04
/// @author     Zheng Chang
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default Encryption and Decryption/Key Wrapping
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_01019
/// @unit_name=PCtxSymmetricKeyWrapperDes
/// @unit_description=DES-based Key Wrapping and Unwrapping Context
/// @endcode
///
/// ================================================================

#include "ara/crypto/cryp/wrap/isoft_ctx_symmetric_key_wrapper_des.h"

#include "ara/crypto/cryp/cryobj/isoft_key_symmetric_ipc.h"
#include "ara/crypto/cryp/cryobj/isoft_secret_seed.h"
#include "ara/crypto/cryp/cryobj/isoft_secret_seed_ipc.h"
#include "ara/crypto/cryp/isoft_service_extension.h"
#include "ara/crypto/openssl/isoft_openssl_encrypt.h"

namespace {
/// @brief Check the key
/// @param key Key
/// @return
bool L_CheckKey(ara::crypto::cryp::SymmetricKey const &key) noexcept
{
    bool pass{false};
    ara::crypto::cryp::isoft_def::PAlgId_Symmetric_DesKey cryptoKey;
    if (true == cryptoKey.IsMinePrimitiveId(key.GetCryptoPrimitiveId()->GetPrimitiveId())) {
        pass = true;
        return pass;
    }
    ara::crypto::cryp::isoft_def::PAlgId_Symmetric_2DesKey crypto2desKey;
    if (true == crypto2desKey.IsMinePrimitiveId(key.GetCryptoPrimitiveId()->GetPrimitiveId())) {
        pass = true;
        return pass;
    }
    ara::crypto::cryp::isoft_def::PAlgId_Symmetric_3DesKey crypto3desKey;
    if (true == crypto3desKey.IsMinePrimitiveId(key.GetCryptoPrimitiveId()->GetPrimitiveId())) {
        pass = true;
        return pass;
    }

    return pass;
}
}  // namespace

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
//***************/
/// @brief Constructor with parameters
/// @param cryptoProvider Crypto provider
/// @qac Suspected unmodifiable [4268]: Initialization helper can be replaced with a delegator.
PCtxSymmetricKeyWrapperDes::PCtxSymmetricKeyWrapperDes(PCryptoProvider &cryptoProvider) noexcept
    : PCtxSymmetricKeyWrapper_Base{cryptoProvider}
    , pSymmetricBlockDes_{std::make_unique< PCtxSymmetricBlock_3Des_Cbc >(cryptoProvider)}
{
}
/// @brief Calculate the size (in bytes) of the wrapped key from the raw key length (in bits). This method is useful for implementations different from RFC3394 / RFC5649.
/// @brief Calculate size of the wrapped key in bytes from original key length in bits.
///       This method can be useful for some implementations different from RFC3394 / RFC5649.
/// @param keyLength  original key length in bits
/// @returns size of the wrapped key in bytes
/// @trace_id_sws={SWS_CRYPT_24013}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02201}
/// @threadsafety={Thread-safe}
std::size_t PCtxSymmetricKeyWrapperDes::CalculateWrappedKeySize(std::size_t keyLength) const noexcept
{
    return keyLength + kInt_16U * kInt_8U;
}
/// @brief Get the maximum length of the target key supported by the implementation. This method is useful for implementations different from RFC3394 / RFC5649.
/// @brief Get maximum length of the target key supported by the implementation.
///       This method can be useful for some implementations different from RFC3394 / RFC5649.
/// @returns maximum length of the target key in bits
/// @trace_id_sws={SWS_CRYPT_24012}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02201}
/// @threadsafety={Thread-safe}
std::size_t PCtxSymmetricKeyWrapperDes::GetMaxTarGetKeyLength() const noexcept { return GetMaxKeyBitLength(); }
/// @brief Set (deploy) a key for the symmetric algorithm context.
/// @param key Symmetric key
/// @param transform Encryption direction: encrypt or decrypt
/// @returns has value if setkey sucess false otherwise
ara::core::Result< void > PCtxSymmetricKeyWrapperDes::SetKey(SymmetricKey const &key,
                                                             CryptoTransform transform) noexcept
{
    // SetKey shall return a kIncompatibleObject error, if the provided key object is incompatible with this symmetric key context
    // Legacy issue, needs testing
    if (false == L_CheckKey(key)) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kIncompatibleObject);
    }

    /// @error: SetKey shall return a kInvalidArgument error, if the provided transformation
    /// direction is not allowed in Symmetric Key wrapper algorithm context.
    if ((transform != CryptoTransform::kWrap) && (transform != CryptoTransform::kUnwrap)) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kInvalidArgument);
    }

    if (pSymmetricBlockDes_ == nullptr) {
        return ara::core::Result< void >::FromValue();
    }
    uint32_t const nKeyAllowedUsage{key.GetAllowedUsage()};

    if (transform == CryptoTransform::kWrap) {
        transform = CryptoTransform::kEncrypt;
        if ((kAllowKeyExporting & nKeyAllowedUsage) != kAllowKeyExporting) {
            return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUsageViolation);
        }
    } else if (transform == CryptoTransform::kUnwrap) {
        transform = CryptoTransform::kDecrypt;
        if ((kAllowKeyImporting & nKeyAllowedUsage) != kAllowKeyImporting) {
            return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUsageViolation);
        }
    } else {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUsageViolation);
    }
    return pSymmetricBlockDes_->SetKey(key, transform);
}

/// @brief: Clear the encryption context.
/// @returns  has vlaue if Reset sucess false otherwise
ara::core::Result< void > PCtxSymmetricKeyWrapperDes::Reset() noexcept
{
    if (pSymmetricBlockDes_ == nullptr) {
        return ara::core::Result< void >::FromValue();
    }
    return pSymmetricBlockDes_->Reset();
}

/// @brief Get the allowed usage of this context (according to the key object properties loaded into this context).
/// If the context has not been initialized with a key object, it must return 0 (all flags reset).
/// @returns  a combination of bit-flags that specifies allowed usages of the context
AllowedUsageFlags PCtxSymmetricKeyWrapperDes::GetAllowedUsage() const noexcept
{
    if (false == IsKeyAvailable()) {
        return 0U;
    }
    return pSymmetricBlockDes_->GetAllowedUsage();
}

/// @brief Get the COUID of the key deployed to the context attached to this extended service. If no key is set for the context, return an empty COUID (Nil).
/// @returns the COUID of the CryptoObject
CryptoObjectUid PCtxSymmetricKeyWrapperDes::GetActualKeyCOUID() const noexcept
{
    if (false == IsKeyAvailable()) {
        return CryptoObjectUid{};
    }
    return pSymmetricBlockDes_->GetActualKeyCOUID();
}
/// @brief Get the maximum supported key length (in bits).
/// @returns maximal supported length of the key in bits
std::size_t PCtxSymmetricKeyWrapperDes::GetMaxKeyBitLength() const noexcept
{
    if (false == IsKeyAvailable()) {
        return 0U;
    }
    return pSymmetricBlockDes_->GetMaxKeyBitLength();
}
/// @brief Get the minimum supported key length (in bits).
/// @returns  minimal supported length of the key in bits
std::size_t PCtxSymmetricKeyWrapperDes::GetMinKeyBitLength() const noexcept
{
    if (false == IsKeyAvailable()) {
        return 0U;
    }
    return pSymmetricBlockDes_->GetMinKeyBitLength();
}
/// @brief Verify support for a specific key length according to the context.
/// @param keyBitLength Key length: in bits
/// @returns  @c true if provided value of the key length is supported by the context
bool PCtxSymmetricKeyWrapperDes::IsKeyBitLengthSupported(std::size_t keyBitLength) const noexcept
{
    if (false == IsKeyAvailable()) {
        return false;
    }
    return pSymmetricBlockDes_->IsKeyBitLengthSupported(keyBitLength);
}
/// @brief Check whether a key is set for this context.
/// @returns  true if a key has been set to this context false otherwise
bool PCtxSymmetricKeyWrapperDes::IsKeyAvailable() const noexcept
{
    if (pSymmetricBlockDes_ == nullptr) {
        return false;
    }
    return pSymmetricBlockDes_->IsKeyAvailable();
}
//***************/
/// @brief Get the actual bit length of the key loaded into the context. If no key is set for the context, return 0.
/// @returns  actual length of a key (now set to the algorithm context) in bits
std::size_t PCtxSymmetricKeyWrapperDes::GetActualKeyBitLength() const noexcept
{
    if (false == IsKeyAvailable()) {
        return 0U;
    }
    return pSymmetricBlockDes_->GetActualKeyBitLength();
}
/// @brief Perform a "key unwrap" operation on the provided BLOB and generate a SecretSeed object.
/// @param stFuncName Function name used for IPC call
/// @param pInputData Starting address of input data in algorithm operation
/// @param nDataLen Data length
/// @param nKeySlotId Key slot ID
/// @return Wrapping or unwrapping result
ara::core::Result< ara::core::Vector< ara::core::Byte > > PCtxSymmetricKeyWrapperDes::_DoWrapAndUnWrapIpc(
    ara::core::StringView const &stFuncName,
    uint8_t const *const pInputData,
    uint32_t const nDataLen,
    uint32_t const nKeySlotId) const noexcept
{
    keys::isoft_def::PIpcAutoPacket aswMsg;  // Receive response packet
    keys::isoft_def::PIpcClient *const ipcClient{keys::isoft_def::PIpcClient::GetInstance_Keys()};
    if (ipcClient == nullptr) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kLogicFault);
    }
    bool const bDealIpc{ipcClient->DealIpcRequest(
        stFuncName, aswMsg,
        [this, pInputData, nDataLen, nKeySlotId](keys::isoft_def::PIpcAutoPacket const *const pReqMsg) -> uint16_t {
            if (pReqMsg != nullptr) {
                keys::isoft_def::cryptoctx::PIpcReq_Wrap ipcReq;
                ipcReq.SetIpcSlotID(pSymmetricBlockDes_->GetSlotId());
                if (pSymmetricBlockDes_->GetTransformation().HasValue()) {
                    ara::crypto::CryptoTransform const nTransform{pSymmetricBlockDes_->GetTransformation().Value()};
                    uint32_t intTransform{0U};
                    if (nTransform == ara::crypto::CryptoTransform::kEncrypt) {
                        intTransform = static_cast< uint32_t >(ara::crypto::CryptoTransform::kWrap);
                    } else if (nTransform == ara::crypto::CryptoTransform::kDecrypt) {
                        intTransform = static_cast< uint32_t >(ara::crypto::CryptoTransform::kUnwrap);
                    } else {
                    }
                    ipcReq.SetTransform(intTransform);
                }
                ipcReq.SetKeySlotId(nKeySlotId);
                ipcReq.SetDataLen(nDataLen);
                ipcReq.SetBlocksize(static_cast< uint32_t >(pSymmetricBlockDes_->GetBlockSize()));
                std::ignore = pReqMsg->AddDataToIpc(&ipcReq, sizeof(ipcReq));
                if (ipcReq.GetDataLen() > 0U) {
                    std::ignore = pReqMsg->AddDataToIpc(pInputData, static_cast< uint16_t >(nDataLen));
                }
            }
            return static_cast< uint16_t >(sizeof(keys::isoft_def::cryptoctx::PIpcReq_Wrap) + nDataLen);
        })};
    if (false == bDealIpc) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kLogicFault);
    }
    keys::isoft_def::cryptoctx::PIpcAsw_common *const pIpcAsw{
        static_cast< keys::isoft_def::cryptoctx::PIpcAsw_common * >(static_cast< void * >(aswMsg.GetIpcBody()))};
    if (0 != pIpcAsw->GetErrorID()) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kLogicFault);
    }
    uint8_t const *const pdata{pIpcAsw->GetData()};
    uint32_t const len{pIpcAsw->GetDataLen()};

    ara::core::Vector< ara::core::Byte > resultByte;
    try {
        resultByte.reserve(static_cast< std::size_t >(len));
    } catch (std::bad_alloc &) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kInsufficientCapacity);
    }
    for (uint32_t i{0U}; i < len; ++i) {
        ara::core::Byte const byData{*(pdata + i)};
        resultByte.push_back(byData);
    }
    return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromValue(resultByte);
}
/// @brief Perform a "key unwrap" operation on the provided BLOB and generate a SecretSeed object.
///         If the implementation is based on the AES block cipher and applied to AES key material, this method should comply with RFC3394 or RFC5649. The created SecretSeed object has the following properties: session and non-exportable (because it is imported without meta information).
/// @param wrappedSeed Wrapped seed
/// @return Unwrapped data
ara::core::Result< ara::core::Vector< ara::core::Byte > > PCtxSymmetricKeyWrapperDes::DoUnwrap(
    ReadOnlyMemRegion const &wrappedSeed) const noexcept
{
    uint8_t const *const pData{wrappedSeed.data()};
    size_t const nSize{wrappedSeed.size()};
    if ((pData == nullptr) || (nSize == 0U)) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kLogicFault);
    }
    if (_GetSlotId() != 0U) {
        return _DoWrapAndUnWrapIpc(FUNC_NAME_Wrap(DoUnwrapDesKey), pData, static_cast< uint32_t >(nSize));
    }
    return DoUnWrap(pData, static_cast< uint32_t >(nSize));
}
/// @brief Perform a "key wrapping" operation on the provided key material.
///         If an implementation is based on the AES block cipher and applied to AES keys, this method should comply with RFC3394 or RFC5649. The method CalculateWrappedKeySize() can be used to calculate the size of the required output buffer.
/// @param key Key material
/// @return Wrapped data
ara::core::Result< ara::core::Vector< ara::core::Byte > > PCtxSymmetricKeyWrapperDes::DoWrapKeyMaterial(
    RestrictedUseObject const &key) const noexcept
{
    uint8_t const *pData{nullptr};
    uint32_t nSlotId{0U};

    PKeySymmetric_Ipc const *const pKeySymmetricIpc{dynamic_cast< PKeySymmetric_Ipc const * >(&key)};
    if (pKeySymmetricIpc != nullptr) {
        nSlotId = pKeySymmetricIpc->GetSlotId();
    } else {
        PKeySymmetric_Base const *const pKeySymmetricBase{dynamic_cast< PKeySymmetric_Base const * >(&key)};
        if (pKeySymmetricBase != nullptr) {
            pData = pKeySymmetricBase->GetKeySymmetric();
        }
    }

    if ((nSlotId == 0U) && (pData == nullptr)) {
        PSecretSeed_Ipc const *const pSecretSeedIpc{dynamic_cast< PSecretSeed_Ipc const * >(&key)};
        if (pSecretSeedIpc != nullptr) {
            nSlotId = pSecretSeedIpc->GetSlotId();
        } else {
            PSecretSeed const *const pSecretSeed{dynamic_cast< PSecretSeed const * >(&key)};
            if (pSecretSeed != nullptr) {
                pData = pSecretSeed->GetSecretSeedData();
            }
        }
    }
    size_t const nSize{key.GetPayloadSize()};

    if (((pData == nullptr) || (nSize == 0U)) && (nSlotId == 0U)) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kLogicFault);
    }

    if (_GetSlotId() != 0U) {  /// ipc
        if (nSlotId != 0U) {
            /// Use IPC to encrypt IPC
            return _DoWrapAndUnWrapIpc(FUNC_NAME_Wrap(DoWrapDesKeyMaterial), nullptr, 0U, nSlotId);
        }
        /// Use IPC to encrypt non-IPC
        return _DoWrapAndUnWrapIpc(FUNC_NAME_Wrap(DoWrapDesKeyMaterial),
                                   static_cast< uint8_t const * >(static_cast< void const * >(pData)),
                                   static_cast< uint32_t >(nSize));
    }
    if (nSlotId != 0U) {
        SymmetricKey const *const pSymmetricKey{_GetSymmetricKey()};
        PKeySymmetric_Base const *const pDesKey{dynamic_cast< PKeySymmetric_Base const * >(pSymmetricKey)};
        if (pDesKey == nullptr) {
            return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
                SecurityErrorDomain::Errc::kLogicFault);
        }
        uint8_t const *const pKeyData{pDesKey->GetKeySymmetric()};
        size_t const nKeySize{pDesKey->GetPayloadSize()};
        /// Use non-IPC to encrypt IPC
        return _DoWrapAndUnWrapIpc(FUNC_NAME_Wrap(DoWrapDesKeyMaterial), pKeyData, static_cast< uint32_t >(nKeySize),
                                   nSlotId);
    }
    return DoWrap(pData, static_cast< uint32_t >(nSize));
}

/// @brief Get an ExtensionService instance.
/// @returns ExtensionService instance
ExtensionService::Uptr PCtxSymmetricKeyWrapperDes::GetExtensionService() const noexcept
{
    return {std::make_unique< PServiceExtension< PCtxSymmetricKeyWrapperDes > >(*this)};
}

/// @brief Return a CryptoPrimitivId instance containing the instance identifier.
/// @returns CryptoPrimitiveId instance
CryptoPrimitiveId::Uptr PCtxSymmetricKeyWrapperDes::GetCryptoPrimitiveId() const noexcept
{
    CryptoPrimitiveId::Uptr pReturn{std::make_unique< PAlgId_WrapDes >()};
    return pReturn;
}

/// @brief Get the expected granularity (block size) of the target key.
///         If the class implements RFC3394 (KW without padding), this method should return 8 (i.e., 8 bytes = 64 bits).
///         If the class implements RFC5649 (KW with padding), this method should return 1 (i.e., 1 octet = 8 bits).
/// @returns  size of the block in bytes
std::size_t PCtxSymmetricKeyWrapperDes::GetTargetKeyGranularity() const noexcept
{
    return static_cast< std::size_t >(kTargetKeyGranularity);
}
/// @brief Execute wrapping logic
/// @param pInputData Starting address of input data in algorithm operation
/// @param nDataLen Data length
/// @return Wrapped data
ara::core::Result< ara::core::Vector< ara::core::Byte > > PCtxSymmetricKeyWrapperDes::DoWrap(
    uint8_t const *pInputData, uint32_t nDataLen) const noexcept
{
    void *pDesKey1{const_cast< void * >(_GetDesKeySchedule1())};
    if (pDesKey1 == nullptr) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kLogicFault);
    }
    void *pDesKey2{const_cast< void * >(_GetDesKeySchedule2())};
    if (pDesKey2 == nullptr) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kLogicFault);
    }
    void *pDesKey3{const_cast< void * >(_GetDesKeySchedule3())};
    if (pDesKey3 == nullptr) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kLogicFault);
    }

    ara::core::Vector< u_char > vecCiphertext;
    vecCiphertext.resize(kInt_1024U);
    u_char *const ciphertext{vecCiphertext.data()};
    std::ignore = memset(static_cast< void * >(ciphertext), 0, kInt_1024U);

    int32_t const len{DES_wrap_key(
        pDesKey1, pDesKey2, pDesKey3, static_cast< u_char * >(static_cast< void * >(ciphertext)),
        static_cast< u_char const * >(static_cast< void const * >(pInputData)), static_cast< size_t >(nDataLen))};
    if (len <= 0) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kLogicFault);
    }
    ara::core::Vector< ara::core::Byte > resultByte;
    try {
        resultByte.reserve(static_cast< std::size_t >(len));
    } catch (std::bad_alloc &) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kInsufficientCapacity);
    }
    for (int32_t i{0}; i < len; ++i) {
        ara::core::Byte const byData{*(ciphertext + i)};
        resultByte.push_back(byData);
    }
    return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromValue(resultByte);
}
/// @brief Execute local unwrapping logic
/// @param pInputData Starting address of input data in algorithm operation
/// @param nDataLen Data length
/// @return Unwrapped data
ara::core::Result< ara::core::Vector< ara::core::Byte > > PCtxSymmetricKeyWrapperDes::DoUnWrap(
    uint8_t const *pInputData, uint32_t nDataLen) const noexcept
{
    void *const pDesKey1{const_cast< void * >(_GetDesKeySchedule1())};
    if (pDesKey1 == nullptr) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kLogicFault);
    }
    void *const pDesKey2{const_cast< void * >(_GetDesKeySchedule2())};
    if (pDesKey2 == nullptr) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kLogicFault);
    }
    void *const pDesKey3{const_cast< void * >(_GetDesKeySchedule3())};
    if (pDesKey3 == nullptr) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kLogicFault);
    }

    ara::core::Vector< u_char > vecPlaintext;
    vecPlaintext.resize(kInt_1024U);
    u_char *const plaintext{vecPlaintext.data()};
    std::ignore = memset(static_cast< void * >(plaintext), 0, kInt_1024U);

    int32_t const len{DES_unwrap_key(pDesKey1, pDesKey2, pDesKey3, static_cast< uint8_t * >(plaintext),
                                     static_cast< u_char const * >(static_cast< void const * >(pInputData)),
                                     static_cast< size_t >(nDataLen))};
    /// The case where -1 is returned is likely due to the wrapping and unwrapping keys being different
    if (len <= 0) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kLogicFault);
    }
    ara::core::Vector< ara::core::Byte > resultByte;
    try {
        resultByte.reserve(static_cast< std::size_t >(len));
    } catch (std::bad_alloc &) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kInsufficientCapacity);
    }
    for (int32_t i{0}; i < len; ++i) {
        ara::core::Byte const byData{*(plaintext + i)};
        resultByte.push_back(byData);
    }
    return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromValue(resultByte);
}
//***************/
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara
