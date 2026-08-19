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
/// @file       isoft_ctx_key_agreement_private.cpp
/// @brief      AutoSar-Crypto Encryption/Decryption Module
/// @details
/// @date       2022-05-05
/// @author     Chang Zheng
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default Encryption/Key Exchange
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_01014
/// @unit_name=PCtxKeyAgreementPrivate
/// @unit_description=Key Exchange
/// @endcode
///
/// ================================================================

#include "ara/crypto/cryp/dh/isoft_ctx_key_agreement_private.h"

#include "ara/crypto/common/isoft_data_type.h"
#include "ara/crypto/cryp/cryobj/isoft_crypto_primitive_id.h"
#include "ara/crypto/cryp/cryobj/isoft_key_private_ipc.h"
#include "ara/crypto/cryp/cryobj/isoft_key_public_ecc.h"
#include "ara/crypto/cryp/cryobj/isoft_key_public_ipc.h"
#include "ara/crypto/cryp/cryobj/isoft_secret_seed.h"
#include "ara/crypto/cryp/isoft_crypto_provider.h"
#include "ara/crypto/cryp/isoft_service_extension.h"
#include "ara/crypto/ipc/isoft_ipc_client.h"
#include "ara/crypto/ipc/isoft_ipc_protocol.h"
#include "ara/crypto/openssl/isoft_openssl_encrypt.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
/// @brief Constructor with parameters.
/// @param cryptoProvider Crypto provider.
/// @returns
/// @throws
PCtxKeyAgreementPrivate::PCtxKeyAgreementPrivate(PCryptoProvider &cryptoProvider) noexcept
    : PCtxKeyAgreementPrivate{cryptoProvider, nullptr}
{
}
/// @brief Constructor with parameters.
/// @param cryptoProvider Crypto provider.
/// @param pPrivateKey Pointer to the private key object.
PCtxKeyAgreementPrivate::PCtxKeyAgreementPrivate(PCryptoProvider &cryptoProvider,
                                                 PrivateKey const *const pPrivateKey) noexcept
    : KeyAgreementPrivateCtx{}  // NOLINT
    , cryptoProvider_{cryptoProvider}
    , pPrivateKey_{pPrivateKey}
{
}
/// @brief Gets a reference to the Crypto Provider for this context.
/// @returns Reference to the crypto provider.
// PRQA S 4623 QAC /// @qac: AUTOSAR standard interface
inline CryptoProvider &PCtxKeyAgreementPrivate::MyProvider() const noexcept { return cryptoProvider_; }
// PRQA L:QAC

/// @brief Generates a common symmetric key by performing a key agreement algorithm between this private key and the other party's public key.
/// The resulting SymmetricKey object has the following properties: session, non-exportable. This method can be used to directly generate the target key without creating an intermediate SecretSeed object.
/// @param otherSideKey Another object instance of this class, SideKey.
/// @param kdf KDF algorithm context object.
/// @param targetAlgId Target algorithm crypto primitive ID.
/// @param allowedUsage Usage scope.
/// @param salt Salt data.
/// @param ctxLabel Context label.
/// @return a unique pointer to @c SecretSeed object, which contains the key material produced by the Key-Agreement
/// algorithm
ara::core::Result< SymmetricKey::Uptrc > PCtxKeyAgreementPrivate::AgreeKey(
    PublicKey const &otherSideKey,
    KeyDerivationFunctionCtx &kdf,
    AlgId targetAlgId,
    AllowedUsageFlags allowedUsage,
    ReadOnlyMemRegion const &salt,
    ReadOnlyMemRegion const &ctxLabel) const noexcept
{
    /// @error: SecurityErrorDomain::kUninitializedContext   if the context was not initialized by a key value
    if (!IsInitialized()) {
        return ara::core::Result< SymmetricKey::Uptrc >::FromError(SecurityErrorDomain::Errc::kUninitializedContext);
    }

    size_t len{kInt_256U};
    ara::core::Vector< uint8_t > vecSharedkey;
    vecSharedkey.resize(kInt_256U);
    uint8_t *const sharedkey{vecSharedkey.data()};

    if (!_computeKey(otherSideKey, static_cast< uint8_t * >(sharedkey), len)) {
        return ara::core::Result< SymmetricKey::Uptrc >::FromError(SecurityErrorDomain::Errc::kIncompatibleObject);
    }

    AlgId kdfKeyMaterialAlgId{static_cast< AlgId >(isoft_def::EPhCtxTypeID::kUnDefine)};
    size_t const nKeyLen{kdf.GetTargetKeyBitLength()};

    CryptoPrimitiveId::AlgId const kdfAlgid{kdf.GetCryptoPrimitiveId()->GetPrimitiveId()};

    switch (kdfAlgid) {
        case static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kKdf_Aes):
        case static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kKdf_Aes128): {
            kdfKeyMaterialAlgId = static_cast< AlgId >(EPhCtxTypeID::kSymmetricAesKey128);
        } break;
        case static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kKdf_Aes192): {
            kdfKeyMaterialAlgId = static_cast< AlgId >(EPhCtxTypeID::kSymmetricAesKey192);
        } break;
        case static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kKdf_Aes256): {
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

    if (len < nKeyLen / kInt_8U) {
        return ara::core::Result< SymmetricKey::Uptrc >::FromError(SecurityErrorDomain::Errc::kInvalidArgument);
    }

    ReadOnlyMemRegion const memKeyData{static_cast< uint8_t const * >(static_cast< void const * >(sharedkey)),
                                       nKeyLen / kInt_8U};

    ara::core::Result< SymmetricKey::Uptrc > const resultSymmetric{
        PCryptoProvider::GenerateSymmetricKeyEx(kdfKeyMaterialAlgId, memKeyData, allowedUsage, false, true)};
    if (resultSymmetric.HasValue() == false) {
        return ara::core::Result< SymmetricKey::Uptrc >::FromError(SecurityErrorDomain::Errc::kInvalidArgument);
    }

    // Reset data
    if (kdf.Reset().HasValue() == false) {
        return ara::core::Result< SymmetricKey::Uptrc >::FromError(SecurityErrorDomain::Errc::kInvalidArgument);
    }
    // Add salt
    std::ignore = kdf.AddSalt(salt);

    // Set iteration count
    if (kdf.ConfigIterations(kInt_2U) == 0U) {
        return ara::core::Result< SymmetricKey::Uptrc >::FromError(SecurityErrorDomain::Errc::kInvalidArgument);
    }
    SymmetricKey::Uptrc const &pKeysrc{resultSymmetric.Value()};

    ara::core::Result< ara::core::String > const resAlgid{MyProvider().ConvertToAlgName(targetAlgId)};
    if (resAlgid.HasValue() == false) {
        return ara::core::Result< SymmetricKey::Uptrc >::FromError(SecurityErrorDomain::Errc::kInvalidArgument);
    }
    // Initialize KDF configuration
    if (kdf.Init(ReadOnlyMemRegion(T_TransBytes(resAlgid.Value().data()), resAlgid.Value().size()), kAlgIdAny,
                 pKeysrc->GetAllowedUsage() | kAllowKdfMaterial, ctxLabel)
            .HasValue()
        == false) {
        return ara::core::Result< SymmetricKey::Uptrc >::FromError(SecurityErrorDomain::Errc::kInvalidArgument);
    }

    if (kdf.SetSourceKeyMaterial(*(pKeysrc.get())).HasValue() == false) {
        return ara::core::Result< SymmetricKey::Uptrc >::FromError(SecurityErrorDomain::Errc::kInvalidArgument);
    }

    ara::core::Result< SymmetricKey::Uptrc > pResultKek{kdf.DeriveKey()};
    if (pResultKek.HasValue() == false) {
        return ara::core::Result< SymmetricKey::Uptrc >::FromError(SecurityErrorDomain::Errc::kInvalidArgument);
    }
    return pResultKek;
}
/// @brief Core function to generate a key.
/// @param otherSideKey Another object instance of this class, SideKey.
/// @param sharedKey Shared key.
/// @param len Key length.
/// @returns  true if compute share key sucess false otherwise
bool PCtxKeyAgreementPrivate::_computeKey(PublicKey const &otherSideKey,
                                          u_char *const sharedKey,
                                          size_t &len) const noexcept
{
    if (len > 0U) {
    }  // for qac
    if (sharedKey == nullptr) {
        return false;
    }

    PKeyPrivate_Ipc_Ecc const *const pKeyPrivateIpcEcc{dynamic_cast< PKeyPrivate_Ipc_Ecc const * >(pPrivateKey_)};
    if (pKeyPrivateIpcEcc != nullptr) {
        return _computeKeyIpc(otherSideKey, sharedKey, len);
    }
    // Set private key
    if (pPrivateKey_ == nullptr) {
        return false;
    }
    PKeyPrivate_Ecc const *const pPrivateKeyEcc{dynamic_cast< PKeyPrivate_Ecc const * >(pPrivateKey_)};
    if (pPrivateKeyEcc == nullptr) {
        return false;
    }
    // The other party's public key
    PKeyPublic_Ecc const *const pKeyPublicKeyEcc{dynamic_cast< PKeyPublic_Ecc const * >(&otherSideKey)};
    if (pKeyPublicKeyEcc == nullptr) {
        return false;
    }
    /* Generate a shared key for one party */
    int32_t ret{Ecdh_key_exchange(sharedKey, &len, pKeyPublicKeyEcc->GetEcc(), pPrivateKeyEcc->GetEcc())};
    return ret == 1;
}

/// @brief Generates keys via IPC.
/// @param otherSideKey Another object instance of this class, SideKey.
/// @param sharedKey Shared key.
/// @param len Key length.
/// @returns  true if compute share key sucess false otherwise
bool PCtxKeyAgreementPrivate::_computeKeyIpc(PublicKey const &otherSideKey,
                                             u_char *const sharedKey,
                                             size_t &len) const noexcept
{
    if (otherSideKey.IsSession()) {
    }  // for qac
    if (len > 0U) {
    }                                        // for qac
    keys::isoft_def::PIpcAutoPacket aswMsg;  // Receive response packet
    keys::isoft_def::PIpcClient *const ipcClient{keys::isoft_def::PIpcClient::GetInstance_Keys()};
    if (ipcClient == nullptr) {
        return false;
    }
    PKeyPrivate_Ipc_Ecc const *const pPrivateKeyIpcEcc{dynamic_cast< PKeyPrivate_Ipc_Ecc const * >(pPrivateKey_)};
    if (pPrivateKeyIpcEcc == nullptr) {
        return false;
    }
    uint32_t const nSlotId{pPrivateKeyIpcEcc->GetSlotId()};
    uint32_t nPubSlotId{0U};

    ara::core::Vector< u_char > vecPubKey;
    vecPubKey.resize(kInt_1024U);
    u_char *const pubKey{vecPubKey.data()};

    size_t pubLen{kInt_1024U};
    PKeyPublic_Ecc const *const pKeyPublicEcc{dynamic_cast< PKeyPublic_Ecc const * >(&otherSideKey)};
    if (pKeyPublicEcc != nullptr) {
        pKeyPublicEcc->PointToString(static_cast< uint8_t * >(pubKey), pubLen);
        if (pubLen == 0U) {
            return false;
        }
    } else {
        PKeyPublic_Ipc const *const pKeyPublicIpc{dynamic_cast< PKeyPublic_Ipc const * >(&otherSideKey)};
        nPubSlotId = pKeyPublicIpc->GetSlotId();
        if (nPubSlotId == 0U) {
            return false;
        }
    }

    bool const bDealIpc{ipcClient->DealIpcRequest(
        FUNC_NAME_Asymmetric(Compute_key), aswMsg,
        [nSlotId, &pubKey, pubLen, nPubSlotId](keys::isoft_def::PIpcAutoPacket const *const pReqMsg) -> uint16_t {
            if (pReqMsg != nullptr) {
                keys::isoft_def::cryptoctx::PIpcReq_AgreeKey ipcReq;
                ipcReq.SetIpcSlotID(nSlotId);
                if (nPubSlotId != 0U) {
                    ipcReq.SetIpcPubSlotID(nPubSlotId);
                } else {
                    ipcReq.SetDataLen(static_cast< uint32_t >(pubLen));
                    std::ignore = pReqMsg->AddDataToIpc(&ipcReq, sizeof(ipcReq));
                    std::ignore
                        = pReqMsg->AddDataToIpc(static_cast< void const * >(pubKey), static_cast< uint16_t >(pubLen));
                }
            }
            return static_cast< uint16_t >(sizeof(keys::isoft_def::cryptoctx::PIpcReq_AgreeKey) + pubLen);
        })};
    if (false == bDealIpc) {
        return false;
    }
    keys::isoft_def::cryptoctx::PIpcAsw_common *const pIpcAsw{
        aswMsg.GetIpcBody< keys::isoft_def::cryptoctx::PIpcAsw_common >()};
    if (0 != pIpcAsw->GetErrorID()) {
        return false;
    }
    uint8_t const *const pdata{pIpcAsw->GetData()};
    len         = pIpcAsw->GetDataLen();
    std::ignore = memcpy(sharedKey, pdata, len);
    return true;
}
/// @brief Generates a common secret seed by performing a key agreement algorithm between this private key and the other party's public key.
/// The resulting SecretSeed object has the following properties: session, non-exportable, AlgID.
/// @param otherSideKey Another object instance of this class, SideKey.
/// @param allowedUsage Usage scope.
/// @returns  unique pointer to @c SecretSeed object, which contains the key material produced by the Key-Agreement
/// algorithm
ara::core::Result< SecretSeed::Uptrc > PCtxKeyAgreementPrivate::AgreeSeed(PublicKey const &otherSideKey,
                                                                          SecretSeed::Usage allowedUsage) const noexcept
{
    /// @error: SecurityErrorDomain::kUninitializedContext   if the context was not initialized by a key value
    if (!IsInitialized()) {
        return ara::core::Result< SecretSeed::Uptrc >::FromError(SecurityErrorDomain::Errc::kUninitializedContext);
    }

    size_t len{kInt_256U};
    ara::core::Vector< uint8_t > vecSharedkey;
    vecSharedkey.resize(kInt_256U);
    uint8_t *const sharedkey{vecSharedkey.data()};
    if (!_computeKey(otherSideKey, static_cast< uint8_t * >(sharedkey), len)) {
        return ara::core::Result< SecretSeed::Uptrc >::FromError(SecurityErrorDomain::Errc::kIncompatibleObject);
    }
    PSecretSeed::Uptr pSecretSeed{std::make_unique< PSecretSeed >(static_cast< void const * >(sharedkey), len)};
    if (len == 0U) {
    }
    pSecretSeed->SetSession(true);
    pSecretSeed->SetExportable(false);
    pSecretSeed->SetAllowedUsage(allowedUsage);

    return ara::core::Result< SecretSeed::Uptrc >::FromValue(std::move(pSecretSeed));
}
/// @brief Gets the ExtensionService instance.
/// @brief Get ExtensionService instance.
/// @returns ExtensionService instance.
/// @trace_id_sws={SWS_CRYPT_21302}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02006}
ExtensionService::Uptr PCtxKeyAgreementPrivate::GetExtensionService() const noexcept
{
    return {std::make_unique< PServiceExtension< PCtxKeyAgreementPrivate > >(*this)};
}
/// @brief Clears the encryption context.
/// @brief Clear the crypto context.
/// @returns  has value if reset sucess false otherwise
/// @trace_id_sws={SWS_CRYPT_21314}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02108}
/// @threadsafety={Thread-safe}
ara::core::Result< void > PCtxKeyAgreementPrivate::Reset() noexcept
{
    pPrivateKey_ = nullptr;
    return ara::core::Result< void >::FromValue();
}
/// @brief Checks whether the key matches.
/// @param key Private key.
/// @returns  true if check key sucess false otherwiseool
bool PCtxKeyAgreementPrivate::CheckKey(PrivateKey const &key) noexcept
{
    AlgId const nAlgID{key.GetCryptoPrimitiveId()->GetPrimitiveId()};
    PAlgId_Asymmetric_EccKey const keyEcc;
    if (!keyEcc.IsMinePrimitiveId(nAlgID)) {
        return false;
    }
    return true;
}
/// @brief Sets (deploys) a key into the key agreement private algorithm context.
/// @param key Private key.
/// @returns has value if SetKey sucess false otherwise
ara::core::Result< void > PCtxKeyAgreementPrivate::SetKey(PrivateKey const &key) noexcept
{
    if (!CheckKey(key)) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kIncompatibleObject);
    }
    uint32_t const nKeyAllowedUsage{key.GetAllowedUsage()};
    if ((kAllowKeyAgreement & nKeyAllowedUsage) != kAllowKeyAgreement) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUsageViolation);
    }
    pPrivateKey_ = &key;
    return ara::core::Result< void >::FromValue();
}

/// @brief Gets the allowed usage of this context (according to the key object properties loaded into this context).
/// If the context has not been initialized with a key object, 0 must be returned (all flags reset).
/// @returns  a combination of bit-flags that specifies allowed usages of the context
AllowedUsageFlags PCtxKeyAgreementPrivate::GetAllowedUsage() const noexcept
{
    if (false == IsKeyAvailable()) {
        return 0U;
    }
    return pPrivateKey_->GetAllowedUsage();
}

/// @brief Gets the COUID of the key deployed to the context attached to this extension service. If no key is set for the context, returns an empty COUID (Nil).
/// @returns  the COUID of the CryptoObject
CryptoObjectUid PCtxKeyAgreementPrivate::GetActualKeyCOUID() const noexcept
{
    if (false == IsKeyAvailable()) {
        return CryptoObjectUid{};
    }
    return pPrivateKey_->GetObjectId().mCouid;
}

/// @brief Gets the maximum supported key length (in bits).
/// @returns  maximal supported length of the key in bits
std::size_t PCtxKeyAgreementPrivate::GetMaxKeyBitLength() const noexcept
{
    return static_cast< std::size_t >(PAlgId_Asymmetric_EccKey::EKeyLen::kEccKeyMaxBitLength);
}

/// @brief Gets the minimum supported key length (in bits).
/// @returns minimal supported length of the key in bits
std::size_t PCtxKeyAgreementPrivate::GetMinKeyBitLength() const noexcept
{
    return static_cast< std::size_t >(PAlgId_Asymmetric_EccKey::EKeyLen::kEccKeyMinBitLength);
}
/// @brief Validates support for a specific key length based on the context.
/// @param keyBitLength Key length: in bits.
/// @returns  @c true if provided value of the key length is supported by the context
bool PCtxKeyAgreementPrivate::IsKeyBitLengthSupported(std::size_t keyBitLength) const noexcept
{
    if ((keyBitLength < GetMinKeyBitLength()) || (keyBitLength > GetMaxKeyBitLength())) {
        return false;
    }
    return true;
}
/// @brief Checks whether a key is set for this context.
/// @returns true if a key has been set to this context false otherwise
bool PCtxKeyAgreementPrivate::IsKeyAvailable() const noexcept { return pPrivateKey_ != nullptr; }

//***************/
/// @brief Gets the actual bit length of the key loaded into the context. If no key is set for the context, returns 0.
/// @returns  actual length of a key (now set to the algorithm context) in bits
std::size_t PCtxKeyAgreementPrivate::GetActualKeyBitLength() const noexcept
{
    if (false == IsKeyAvailable()) {
        return 0U;
    }
    return pPrivateKey_->GetPayloadSize() * kInt_8U;
}

/// @brief Checks whether the encryption context is initialized and ready to use. It checks all required values, including: key value, IV/seed, etc.
/// @returns  true if has already init key
bool PCtxKeyAgreementPrivate::IsInitialized() const noexcept { return IsKeyAvailable(); }

/// @brief Returns the CryptoPrimitiveId instance containing the instance identifier.
/// @returns CryptoPrimitiveId instance.
CryptoPrimitiveId::Uptr PCtxKeyAgreementPrivate::GetCryptoPrimitiveId() const noexcept
{
    return {std::make_unique< PAlgId_Key_Agreement_Private >()};
}

}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara
