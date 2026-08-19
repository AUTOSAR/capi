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
/// @file       isoft_ctx_key_decapsulator_private_rsa.cpp
/// @brief      AutoSar-Crypto encryption/decryption module
/// @details
/// @date       2022-04-26
/// @author     Chang Zheng
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/DefaultEncryptionDecryption/KeyEncapsulation
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_01015
/// @unit_name=PCtxKeyDecapsulatorPrivateRsa
/// @unit_description=RSA-based private key decapsulation context
/// @endcode
///
/// ================================================================

#include "ara/crypto/cryp/kem/isoft_ctx_key_decapsulator_private_rsa.h"

#include "ara/crypto/common/isoft_data_type.h"
#include "ara/crypto/cryp/cryobj/isoft_crypto_primitive_id.h"
#include "ara/crypto/cryp/cryobj/isoft_secret_seed.h"
#include "ara/crypto/cryp/isoft_convert_alg_id.h"
#include "ara/crypto/cryp/isoft_crypto_provider.h"
#include "ara/crypto/cryp/isoft_service_extension.h"
#include "ara/crypto/cryp/wrap/isoft_ctx_symmetric_key_wrapper_aes_pad.h"
#include "ara/crypto/cryp/wrap/isoft_ctx_symmetric_key_wrapper_des.h"
#include "ara/crypto/openssl/isoft_openssl_encrypt.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {

///****************/// PCtxKeyDecapsulatorPrivate
/// @brief Parameterized constructor
/// @param cryptoProvider crypto provider
/// @qac Suspected unable to modify [4268]: Initialization helper can be replaced with a delegator.
PCtxKeyDecapsulatorPrivateRsa::PCtxKeyDecapsulatorPrivateRsa(PCryptoProvider &cryptoProvider) noexcept
    : PCtxKeyDecapsulatorPrivate{cryptoProvider}
{
    pCtxDecryptorPrivateRsa_ = std::make_unique< PCtxDecryptorPrivate_Rsa >(cryptoProvider);
}
/// @brief Decapsulate the key encryption key (KEK). The resulting SymmetricKey object has the following properties: session, non-exportable, Key
/// Usage:kAllowKeyImporting。
///         This method can be used to directly generate the target key without creating an intermediate SecretSeed object.
/// @brief Decapsulate Key Encryption Key (KEK).
///         Produced @c SymmetricKey object has following attributes: session, non-exportable, Key Usage: @c
///         kAllowKeyImporting. This method can be used for direct production of the target key, without creation of the
///         intermediate @c Encapsulate object.
/// @param input  an input buffer (its size should be equal @c GetEncapsulatedSize() bytes)
/// @param kdf  a context of a key derivation function, which should be used for the target KEK production
/// @param kekAlgId  an algorithm ID of the target KEK
/// @param salt  an optional salt value (if used, it should be unique for each instance of the target key)
/// @param ctxLabel  an pptional application specific "context label" (it can identify purpose of the target key nd/or communication parties)
/// @returns unique smart pointer to a symmetric key object derived from a key material decapsulated from the input block
/// @trace_id_sws={SWS_CRYPT_21412}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02102}
/// @uptrace={RS_CRYPTO_02108}
/// @uptrace={RS_CRYPTO_02115}
/// @error: SecurityErrorDomain::kUninitializedContext   if the context was not initialized by a private key value
/// @error: SecurityErrorDomain::kUnknownIdentifier      if @c kekAlgId specifies incorrect algorithm
/// @error: SecurityErrorDomain::kInvalidInputSize       if (input.size() <> this->GetEncapsulatedSize()) @threadsafety={Thread-safe}
ara::core::Result< SymmetricKey::Uptrc > PCtxKeyDecapsulatorPrivateRsa::DecapsulateKey(
    ReadOnlyMemRegion const &input,
    KeyDerivationFunctionCtx &kdf,
    AlgId kekAlgId,
    ReadOnlyMemRegion const &salt,
    ReadOnlyMemRegion const &ctxLabel) const noexcept
{
    if (!IsInitialized()) {
        return ara::core::Result< SymmetricKey::Uptrc >::FromError(SecurityErrorDomain::Errc::kUninitializedContext);
    }

    PCryptoProvider *const pCryptoProvider{dynamic_cast< PCryptoProvider * >(&MyProvider())};
    if (pCryptoProvider == nullptr) {
        return ara::core::Result< SymmetricKey::Uptrc >::FromError(SecurityErrorDomain::Errc::kInvalidArgument);
    }
    ara::core::Result< ara::core::String > resAlgName{pCryptoProvider->ConvertToAlgName(kekAlgId)};
    if (!resAlgName.HasValue()) {
        return ara::core::Result< SymmetricKey::Uptrc >::FromError(SecurityErrorDomain::Errc::kUnknownIdentifier);
    }

    if (input.size() < kInputSizeMin) {
        return ara::core::Result< SymmetricKey::Uptrc >::FromError(SecurityErrorDomain::Errc::kInvalidInputSize);
    }

    if (input.size() > kInputSizeMax) {
        return ara::core::Result< SymmetricKey::Uptrc >::FromError(SecurityErrorDomain::Errc::kInvalidInputSize);
    }
    // Convert ciphertext C to integer c, most significant byte first. Decrypt integer c using the recipient's private key (n,d) to recover integer z (see note)
    ara::core::Vector< u_char > vecRandomByteZ;
    vecRandomByteZ.resize(kInt_1024U);
    u_char *const randomByteZ{vecRandomByteZ.data()};
    size_t retlenZ{0U};

    u_char const *const data{input.data()};
    size_t const len{input.size()};
    size_t nlen{0U};
    nEncapsulatedSize_ = len;

    // Separate the encrypted key data EK into ciphertext C of length nLen bytes and encapsulated key data WK
    PKeyPrivate_Ipc_Rsa const *const pPrivateKeyIpcRsa{
        dynamic_cast< PKeyPrivate_Ipc_Rsa const * >(pCtxDecryptorPrivateRsa_->GetPrivateKey())};
    if (pPrivateKeyIpcRsa != nullptr) {  // ipc branch
        DecapsulateKey_Ipc(pPrivateKeyIpcRsa->GetSlotId(), data, len, static_cast< uint8_t * >(randomByteZ), retlenZ,
                           nlen);
    } else {
        PKeyPrivate_Rsa const *const pPrivateKeyRsa{
            dynamic_cast< PKeyPrivate_Rsa const * >(pCtxDecryptorPrivateRsa_->GetPrivateKey())};
        if (pPrivateKeyRsa == nullptr) {
            return ara::core::Result< SymmetricKey::Uptrc >::FromValue(nullptr);
        }
        nlen = static_cast< size_t >(Get_RsaKey_Size(pPrivateKeyRsa->GetRsa()));

        ara::core::Vector< u_char > vecRandomByteC;
        vecRandomByteC.resize(kInt_1024U);
        u_char *const randomByteC{vecRandomByteC.data()};

        std::ignore = memcpy(static_cast< void * >(randomByteC), data, nlen);
        std::ignore = Rsasve_recover_evp(pPrivateKeyRsa->GetRsa(), static_cast< uint8_t * >(randomByteZ), &retlenZ,
                                         static_cast< uint8_t const * >(randomByteC), nlen);
    }

    ara::core::Vector< u_char > vecWrapeddataWK;
    vecWrapeddataWK.resize(kInt_1024U);
    u_char *const wrapeddataWK{vecWrapeddataWK.data()};

    std::ignore = memcpy(static_cast< void * >(wrapeddataWK), data + nlen, len - nlen);
    // reset data
    if (kdf.Reset().HasValue() == false) {
        return ara::core::Result< SymmetricKey::Uptrc >::FromError(SecurityErrorDomain::Errc::kInvalidArgument);
    }
    // add salt
    std::ignore = kdf.AddSalt(salt);
    // set number of iterations
    if (kdf.ConfigIterations(kInt_2U) == 0U) {
        return ara::core::Result< SymmetricKey::Uptrc >::FromError(SecurityErrorDomain::Errc::kInvalidArgument);
    }

    ara::core::String const strAlgName{std::move(resAlgName).Value()};
    if (strAlgName.empty()) {
        return ara::core::Result< SymmetricKey::Uptrc >::FromError(SecurityErrorDomain::Errc::kInvalidArgument);
    }
    ReadOnlyMemRegion const kekId{T_TransBytes(strAlgName.data()), strAlgName.size()};
    if (kdf.Init(kekId, kAlgIdAny, kAllowDataEncryption | kAllowDataDecryption | kAllowKeyImporting | kAllowKdfMaterial,
                 ctxLabel)
            .HasValue()
        == false) {
        return ara::core::Result< SymmetricKey::Uptrc >::FromError(SecurityErrorDomain::Errc::kInvalidArgument);
    }
    /// kdf material AlgId
    AlgId kdfKeyMaterialAlgId{static_cast< AlgId >(EPhCtxTypeID::kUnDefine)};
    size_t const nKeyLen{kdf.GetTargetKeyBitLength()};
    bool isAesKey{false};
    CryptoPrimitiveId::AlgId const kdfAlgid{kdf.GetCryptoPrimitiveId()->GetPrimitiveId()};

    switch (kdfAlgid) {
        case static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kKdf_Aes):
        case static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kKdf_Aes128): {
            isAesKey            = true;
            kdfKeyMaterialAlgId = static_cast< AlgId >(EPhCtxTypeID::kSymmetricAesKey128);
        } break;
        case static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kKdf_Aes192): {
            isAesKey            = true;
            kdfKeyMaterialAlgId = static_cast< AlgId >(EPhCtxTypeID::kSymmetricAesKey192);
        } break;
        case static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kKdf_Aes256): {
            isAesKey            = true;
            kdfKeyMaterialAlgId = static_cast< AlgId >(EPhCtxTypeID::kSymmetricAesKey256);
        } break;
        case static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kKdf_Des): {
            kdfKeyMaterialAlgId = static_cast< AlgId >(EPhCtxTypeID::kSymmetricDesKey);
        } break;
        case static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kKdf_3Des): {
            kdfKeyMaterialAlgId = static_cast< AlgId >(EPhCtxTypeID::kSymmetric3DesKey);
        } break;

        default: {
            break;
        }
    }

    if (kdfKeyMaterialAlgId == static_cast< AlgId >(EPhCtxTypeID::kUnDefine)) {
        return ara::core::Result< SymmetricKey::Uptrc >::FromError(SecurityErrorDomain::Errc::kInvalidArgument);
    }

    if (retlenZ < nKeyLen / kInt_8U) {
        return ara::core::Result< SymmetricKey::Uptrc >::FromError(SecurityErrorDomain::Errc::kInvalidArgument);
    }

    // PRQA S 2327 QAC /// @qac: QAC documentation says lambda expression assignment using "=" is supported
    auto const funGetAesKeyId = [len, nlen]() noexcept -> AlgId {
        // PRQA L:QAC
        AlgId targetAlgId{0U};
        switch (len - nlen - kInt_8U) {
            case kInt_16U: {
                targetAlgId = static_cast< AlgId >(EPhCtxTypeID::kSymmetricAesKey128);
            } break;
            case kInt_24U: {
                targetAlgId = static_cast< AlgId >(EPhCtxTypeID::kSymmetricAesKey192);
            } break;
            case kInt_32U: {
                targetAlgId = static_cast< AlgId >(EPhCtxTypeID::kSymmetricAesKey256);
            } break;
            default: {
                targetAlgId = static_cast< AlgId >(EPhCtxTypeID::kUnDefine);
            } break;
        }
        return targetAlgId;
    };
    // PRQA S 2327 QAC /// @qac: QAC documentation says lambda expression assignment using "=" is supported
    auto const funGetDesKeyId = [len, nlen]() noexcept -> AlgId {
        AlgId targetAlgId{0U};
        switch (len - nlen - kInt_16U) {
            case kInt_8U: {
                targetAlgId = static_cast< AlgId >(EPhCtxTypeID::kSymmetricDesKey);
            } break;
            case kInt_16U: {
                targetAlgId = static_cast< AlgId >(EPhCtxTypeID::kSymmetric2DesKey);
            } break;
            case kInt_24U: {
                targetAlgId = static_cast< AlgId >(EPhCtxTypeID::kSymmetric3DesKey);
            } break;
            default: {
                targetAlgId = static_cast< AlgId >(EPhCtxTypeID::kUnDefine);
            } break;
        }
        return targetAlgId;
    };
    // PRQA L:QAC
    PCtxSymmetricKeyWrapper_Base::Uptr pSymmetricKeyWrapper{nullptr};
    AlgId targetAlgId{0U};
    if (isAesKey) {
        pSymmetricKeyWrapper = std::make_unique< PCtxSymmetricKeyWrapperAesPad >(*pCryptoProvider);
        targetAlgId          = funGetAesKeyId();
    } else {
        pSymmetricKeyWrapper = std::make_unique< PCtxSymmetricKeyWrapperDes >(*pCryptoProvider);
        targetAlgId          = funGetDesKeyId();
    }
    if (targetAlgId == static_cast< AlgId >(EPhCtxTypeID::kUnDefine)) {
        return ara::core::Result< SymmetricKey::Uptrc >::FromError(SecurityErrorDomain::Errc::kInvalidArgument);
    }

    ReadOnlyMemRegion const memKeyData{static_cast< uint8_t const * >(static_cast< void const * >(randomByteZ)),
                                       nKeyLen / kInt_8U};
    ara::core::Result< SymmetricKey::Uptrc > const resultSymmetric{PCryptoProvider::GenerateSymmetricKeyEx(
        kdfKeyMaterialAlgId, memKeyData,
        kAllowDataEncryption | kAllowDataDecryption | kAllowKeyImporting | kAllowKdfMaterial, false, true)};
    if (!resultSymmetric.HasValue()) {
        return ara::core::Result< SymmetricKey::Uptrc >::FromError(SecurityErrorDomain::Errc::kInvalidArgument);
    }
    SymmetricKey::Uptrc const &pSymmetricKeyMaterial{resultSymmetric.Value()};
    if (kdf.SetSourceKeyMaterial(*(pSymmetricKeyMaterial.get())).HasValue() == false) {
        return ara::core::Result< SymmetricKey::Uptrc >::FromError(SecurityErrorDomain::Errc::kInvalidArgument);
    }

    ara::core::Result< SymmetricKey::Uptrc > const pResult{kdf.DeriveKey()};
    if (!pResult.HasValue()) {
        return ara::core::Result< SymmetricKey::Uptrc >::FromError(SecurityErrorDomain::Errc::kInvalidArgument);
    }

    // Use the underlying key wrapping scheme with the key encryption key KEK to unwrap the encapsulated key data WK to recover key data K
    SymmetricKey const *const symmetricKey{pResult->get()};
    ara::core::Result< void > const resKey{pSymmetricKeyWrapper->SetKey(*symmetricKey, CryptoTransform::kUnwrap)};
    if (!resKey.HasValue()) {
        return ara::core::Result< SymmetricKey::Uptrc >::FromError(SecurityErrorDomain::Errc::kInvalidArgument);
    }
    ReadOnlyMemRegion const srcCrypto{static_cast< uint8_t * >(wrapeddataWK), len - nlen};
    AllowedUsageFlags const allowedUsage{kAllowDataEncryption | kAllowDataDecryption | kAllowKeyImporting};
    ara::core::Result< RestrictedUseObject::Uptrc > restrictedUseObject{
        pSymmetricKeyWrapper->UnwrapKey(srcCrypto, targetAlgId, allowedUsage)};
    if (!restrictedUseObject.HasValue()) {
        return ara::core::Result< SymmetricKey::Uptrc >::FromError(SecurityErrorDomain::Errc::kUninitializedContext);
    }
    RestrictedUseObject::Uptrc pRestrictedUseObject{std::move(restrictedUseObject).Value()};

    // downcast
    ara::core::Result< SymmetricKey::Uptrc > pResultSymmetricKey{
        RestrictedUseObject::Downcast< SymmetricKey >(std::move(pRestrictedUseObject))};
    if (!pResultSymmetricKey.HasValue()) {
        return ara::core::Result< SymmetricKey::Uptrc >::FromError(SecurityErrorDomain::Errc::kUninitializedContext);
    }

    SymmetricKey::Uptrc pSymmetricKey{std::move(pResultSymmetricKey).Value()};
    return ara::core::Result< SymmetricKey::Uptrc >::FromValue(std::move(pSymmetricKey));
}
/// @brief IPC version logic implementation of key decapsulation
/// @param slotid key slot ID
/// @param pInputData start address of input data in algorithm operation
/// @param len input data length
/// @param pOutputdata start address of output buffer
/// @param outlen output buffer length
/// @param rsalen returned RSA length
void PCtxKeyDecapsulatorPrivateRsa::DecapsulateKey_Ipc(uint32_t const slotid,
                                                       u_char const *const pInputData,
                                                       size_t const len,
                                                       u_char *const pOutputdata,
                                                       size_t &outlen,
                                                       size_t &rsalen) noexcept
{
    if (outlen > 0U) {
    }  // for qac
    if (rsalen > 0U) {
    }                                        // for qac
    keys::isoft_def::PIpcAutoPacket aswMsg;  // receive response packet
    aswMsg.ReInit();
    keys::isoft_def::PIpcClient *const ipcClient{keys::isoft_def::PIpcClient::GetInstance_Keys()};
    if (ipcClient == nullptr) {
        return;
    }
    bool const bDealIpc{ipcClient->DealIpcRequest(
        FUNC_NAME_Asymmetric(DecapsulateKey), aswMsg,
        [slotid, pInputData, len](keys::isoft_def::PIpcAutoPacket const *const pReqMsg) -> uint16_t {
            if (pReqMsg != nullptr) {
                keys::isoft_def::cryptoctx::PIpcReq_Common ipcReq;
                ipcReq.SetIpcSlotID(slotid);
                ipcReq.SetDataLen(static_cast< uint32_t >(len));
                std::ignore = pReqMsg->AddDataToIpc(&ipcReq, sizeof(ipcReq));
                std::ignore = pReqMsg->AddDataToIpc(pInputData, static_cast< uint16_t >(len));
            }
            return static_cast< uint16_t >(sizeof(keys::isoft_def::cryptoctx::PIpcReq_Common) + len);
        })};
    if (false == bDealIpc) {
        return;
    }
    keys::isoft_def::cryptoctx::PIpcAsw_DecapsulateKey *const pIpcAsw{
        (aswMsg.GetIpcBody< keys::isoft_def::cryptoctx::PIpcAsw_DecapsulateKey >())};
    if (0 != pIpcAsw->GetErrorID()) {
        return;
    }
    uint8_t const *const pdata{pIpcAsw->GetData()};
    outlen      = pIpcAsw->GetDataLen();
    rsalen      = pIpcAsw->GetRsaLen();
    std::ignore = memcpy(pOutputdata, pdata, outlen);
}
/// @brief Decapsulate key material.
/// The returned key material object should be used to derive a symmetric key. The resulting SecretSeed object has the following properties: session, non-exportable, AlgID = this KEM AlgID.
/// @param input buffer containing the encapsulated seed
/// @param allowedUsage usage scope
/// @returns  unique smart pointer to @c SecretSeed object, which keeps the key material decapsulated from the input
/// buffer
ara::core::Result< SecretSeed::Uptrc > PCtxKeyDecapsulatorPrivateRsa::DecapsulateSeed(
    ReadOnlyMemRegion const &input, SecretSeed::Usage allowedUsage) const noexcept
{
    if (!IsInitialized()) {
        return ara::core::Result< SecretSeed::Uptrc >::FromError(SecurityErrorDomain::Errc::kUninitializedContext);
    }

    if (input.size() > kInt_1024U) {
        return ara::core::Result< SecretSeed::Uptrc >::FromError(SecurityErrorDomain::Errc::kInsufficientCapacity);
    }

    nEncapsulatedSize_ = input.size();

    ara::core::Vector< u_char > vecTdata;
    vecTdata.resize(kInt_1024U);
    u_char *const tData{vecTdata.data()};

    PKeyPrivate_Rsa const *const pPrivateKeyRsa{
        static_cast< PKeyPrivate_Rsa const * >(static_cast< void const * >(pCtxDecryptorPrivateRsa_->GetPrivateKey()))};
    if (pPrivateKeyRsa == nullptr) {
        return ara::core::Result< SecretSeed::Uptrc >::FromValue(nullptr);
    }

    EVP_PKEY *const rsaKey{pPrivateKeyRsa->GetRsa()};
    size_t const nlen{static_cast< size_t >(Get_RsaKey_Size(pPrivateKeyRsa->GetRsa()))};

    size_t retlen{0U};
    std::ignore = Rsasve_recover_evp(rsaKey, static_cast< u_char * >(tData), &retlen,
                                     static_cast< u_char const * >(input.data()), nlen);
    PSecretSeed::Uptr pSecretSeed{std::make_unique< PSecretSeed >(static_cast< void const * >(tData), retlen)};
    if (retlen == 0U) {
    }
    pSecretSeed->SetSession(true);
    pSecretSeed->SetExportable(false);
    pSecretSeed->SetAllowedUsage(allowedUsage);

    return ara::core::Result< SecretSeed::Uptrc >::FromValue(std::move(pSecretSeed));
}
/// @brief Get the fixed size of the encapsulated data block.
/// @returns  size of the encapsulated data block in bytes
std::size_t PCtxKeyDecapsulatorPrivateRsa::GetEncapsulatedSize() const noexcept { return nEncapsulatedSize_; }
/// @brief Get ExtensionService instance.
/// @returns ExtensionService instance
ExtensionService::Uptr PCtxKeyDecapsulatorPrivateRsa::GetExtensionService() const noexcept
{
    return {std::make_unique< PServiceExtension< PCtxKeyDecapsulatorPrivateRsa > >(*this)};
}
/// @brief Get the key entropy (bit length) of the key (KEK) material.
/// For RSA systems, the return value corresponds to the modulus length N(-1); for DH-like systems, the return value corresponds to the modulus length q(-1).
/// @returns  entropy of the KEK material in bits
std::size_t PCtxKeyDecapsulatorPrivateRsa::GetKekEntropy() const noexcept
{
    if (pCtxDecryptorPrivateRsa_ == nullptr) {
        return 0U;
    }
    PKeyPrivate_Rsa const *const pPrivateKeyRsa{
        static_cast< PKeyPrivate_Rsa const * >(static_cast< void const * >(pCtxDecryptorPrivateRsa_->GetPrivateKey()))};
    if (pPrivateKeyRsa == nullptr) {
        return 0U;
    }
    return pPrivateKeyRsa->GetPayloadSize();
}
/// @brief Clear the encryption context.
/// @returns  has vlaue if Reset sucess false otherwise
ara::core::Result< void > PCtxKeyDecapsulatorPrivateRsa::Reset() noexcept
{
    if (pCtxDecryptorPrivateRsa_ == nullptr) {
        return ara::core::Result< void >::FromValue();
    }
    return pCtxDecryptorPrivateRsa_->Reset();
}
/// @brief Set (deploy) a key to the key decapsulator private algorithm context.
/// @param key private key
/// @returns  has vlaue if SetKey sucess false otherwise
ara::core::Result< void > PCtxKeyDecapsulatorPrivateRsa::SetKey(PrivateKey const &key) noexcept
{
    uint32_t const nKeyAllowedUsage{key.GetAllowedUsage()};
    if (((kAllowKeyImporting & nKeyAllowedUsage) != kAllowKeyImporting)
        || ((kAllowDataDecryption & nKeyAllowedUsage) != kAllowDataDecryption)) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUsageViolation);
    }
    if (pCtxDecryptorPrivateRsa_ == nullptr) {
        return ara::core::Result< void >::FromValue();
    }
    return pCtxDecryptorPrivateRsa_->SetKey(key);
}
/// @brief Get the allowed usage of this context (according to the key object properties loaded into this context).
/// If the context has not been initialized with a key object, it must return 0 (all flags reset).
/// @returns  a combination of bit-flags that specifies allowed usages of the context
AllowedUsageFlags PCtxKeyDecapsulatorPrivateRsa::GetAllowedUsage() const noexcept
{
    if (false == IsKeyAvailable()) {
        return 0U;
    }
    return pCtxDecryptorPrivateRsa_->GetAllowedUsage();
}
/// @brief Get the COUID of the key deployed to the context attached to this extension service. If no key is set for the context, returns an empty COUID (Nil).
/// @returns  the COUID of the CryptoObject
CryptoObjectUid PCtxKeyDecapsulatorPrivateRsa::GetActualKeyCOUID() const noexcept
{
    if (false == IsKeyAvailable()) {
        return CryptoObjectUid{};
    }
    return pCtxDecryptorPrivateRsa_->GetActualKeyCOUID();
}
/// @brief Get the maximum supported key length (in bits).
/// @returns  maximal supported length of the key in bits
std::size_t PCtxKeyDecapsulatorPrivateRsa::GetMaxKeyBitLength() const noexcept
{
    if (pCtxDecryptorPrivateRsa_ == nullptr) {
        return 0U;
    }
    return pCtxDecryptorPrivateRsa_->GetMaxKeyBitLength();
}
/// @brief Get the minimum supported key length (in bits).
/// @returns  minimal supported length of the key in bits
std::size_t PCtxKeyDecapsulatorPrivateRsa::GetMinKeyBitLength() const noexcept
{
    if (pCtxDecryptorPrivateRsa_ == nullptr) {
        return 0U;
    }
    return pCtxDecryptorPrivateRsa_->GetMinKeyBitLength();
}
/// @brief Verify support for a specific key length according to the context.
/// @param keyBitLength key length: in bits
/// @returns  @c true if provided value of the key length is supported by the context
bool PCtxKeyDecapsulatorPrivateRsa::IsKeyBitLengthSupported(std::size_t keyBitLength) const noexcept
{
    if (pCtxDecryptorPrivateRsa_ == nullptr) {
        return false;
    }
    return pCtxDecryptorPrivateRsa_->IsKeyBitLengthSupported(keyBitLength);
}
/// @brief Check whether a key is set for this context.
/// @returns  true if a key has been set to this context false otherwise
bool PCtxKeyDecapsulatorPrivateRsa::IsKeyAvailable() const noexcept
{
    if (pCtxDecryptorPrivateRsa_ == nullptr) {
        return false;
    }
    return pCtxDecryptorPrivateRsa_->IsKeyAvailable();
}
//***************/
/// @brief Get the actual bit length of the key loaded into the context. If no key is set for the context, returns 0.
/// @returns  actual length of a key (now set to the algorithm context) in bits
std::size_t PCtxKeyDecapsulatorPrivateRsa::GetActualKeyBitLength() const noexcept
{
    if (false == IsKeyAvailable()) {
        return 0U;
    }
    return pCtxDecryptorPrivateRsa_->GetActualKeyBitLength();
}
/// @brief Check whether the encryption context has been initialized and is usable. It checks all required values, including: key value, IV/seed, etc.
/// @returns  true if already init false otherwise
bool PCtxKeyDecapsulatorPrivateRsa::IsInitialized() const noexcept { return IsKeyAvailable(); }
/// @brief Returns a CryptoPrimitivId instance containing the instance identifier.
/// @returns CryptoPrimitiveId instance
CryptoPrimitiveId::Uptr PCtxKeyDecapsulatorPrivateRsa::GetCryptoPrimitiveId() const noexcept
{
    return {std::make_unique< PAlgId_Kem_private_rsa >()};
}
//********************************/
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara
