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
/// @file       isoft_ctx_kdf_base.cpp
/// @brief      AutoSar-Crypto encryption/decryption module
/// @details
/// @date       2022-03-15
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/DefaultEncryptionDecryption/KeyDerivation
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_01014
/// @unit_name=PCtxKdf_Base
/// @unit_description=Key derivation context base class
/// @endcode
///
/// ================================================================

#include "ara/crypto/cryp/kdf/isoft_ctx_kdf_base.h"

#include <openssl/evp.h>

#include "ara/crypto/common/isoft_assert.h"
#include "ara/crypto/cryp/cryobj/isoft_key_symmetric.h"
#include "ara/crypto/cryp/cryobj/isoft_secret_seed.h"
#include "ara/crypto/cryp/isoft_crypto_provider.h"
#include "ara/crypto/cryp/isoft_service_extension.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
/// @brief Maximum number of iterations
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00192
/// @trace_id_dd=DD_CRYPTO_06617
/// @needwork = dd
/// @endcode
uint32_t const kIterationMax{10U};
/// @brief Minimum number of iterations
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00192
/// @trace_id_dd=DD_CRYPTO_06618
/// @needwork = dd
/// @endcode
uint32_t const kIterationMin{2U};
//********************************/
/// @brief Key derivation function interface: OpenSSL encapsulation.
//********************************/
/// @brief Parameterized constructor
/// @param cryptoProvider crypto provider
/// @returns
/// @throws
PCtxKdf_Base::PCtxKdf_Base(PCryptoProvider &cryptoProvider) noexcept
    : PCtxKdf_Base{cryptoProvider, nullptr, kAlgIdDefault, kAllowPrototypedOnly, {}, false, 0U, nullptr, kAlgIdDefault}
{
}
/// @brief Parameterized constructor
/// @param cryptoProvider crypto provider
/// @param pSrcKeyMaterial source key material data
/// @param nTargetAlgId target algorithm crypto primitive ID
/// @param nTarGetAllowedUsage target allowed usage
/// @param ctxLabel context label
/// @param bInit whether to initialize
/// @param nIterations number of iterations
/// @param buffSalt salt buffer data
/// @param targetKeyId target key ID
PCtxKdf_Base::PCtxKdf_Base(PCryptoProvider &cryptoProvider,
                           RestrictedUseObject const *const pSrcKeyMaterial,
                           AlgId const nTargetAlgId,
                           AllowedUsageFlags const nTarGetAllowedUsage,
                           ReadOnlyMemRegion const &ctxLabel,
                           bool const bInit,
                           uint32_t const nIterations,
                           internal::PAutoBuff const *const buffSalt,
                           AlgId const targetKeyId) noexcept
    : KeyDerivationFunctionCtx{}  // NOLINT
    , cryptoProvider_{cryptoProvider}
    , pSrcKeyMaterial_{pSrcKeyMaterial}
    , nTargetAlgId_{nTargetAlgId}
    , nTargetAllowedUsage_{nTarGetAllowedUsage}
    , ctxLabel_{ctxLabel}
    , bInit_{bInit}
    , nIterations_{nIterations}
    , buffSalt_{kInt_256U}
    , targetKeyId_{targetKeyId}
{
    std::ignore = pSrcKeyMaterial;
    std::ignore = buffSalt;
}

//***************/  //CryptoContext interface
/// @brief Get the reference to the Crypto Provider for this context.
/// @name  MyProvider
/// @returns reference to crypto provider
// PRQA S 4623 QAC /// @qac: AUTOSAR standard interface
inline CryptoProvider &PCtxKdf_Base::MyProvider() const noexcept { return cryptoProvider_; }
// PRQA L:QAC
//***************/  //KeyDerivationFunctionCtx interface
/// @brief Add an application filler value stored in (non-secret) ReadOnlyMemRegion. If (GetFillerSize() == 0), this method call will be ignored.
/// @brief Add an application filler value stored in a (non-secret) ReadOnlyMemRegion. If (GetFillerSize() == 0),
/// then this method call will be ignored.
///			Add a secret application filler value stored in a SecretSeed object. If (GetFillerSize() == 0),
/// then this method call will be ignored. 			Add a salt value stored in a (non-secret)
/// ReadOnlyMemRegion.
/// @name  AddSalt
/// @param salt  a salt value (if used, it should be unique for each instance of the target key)
/// @returns  ara::core::Result<void>
/// @trace_id_sws={SWS_CRYPT_21510}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02102}
/// @uptrace={RS_CRYPTO_02107}
/// @uptrace={RS_CRYPTO_02108}
/// @uptrace={RS_CRYPTO_02111}
/// @error: SecurityErrorDomain::kInvalidInputSize  if size of the appFiller is incorrect, i.e. if (appFiller.size() <
/// @threadsafety={Thread-safe}
ara::core::Result< void > PCtxKdf_Base::AddSalt(ReadOnlyMemRegion const &salt) noexcept
{
    /// @error: SecurityErrorDomain::kInvalidInputSize  if size of the appFiller is incorrect, i.e. if (appFiller.size()
    /// < GetFillerSize());
    if (salt.size() < GetKeyIdSize()) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kInvalidInputSize);
    }
    if (false == salt.empty()) {
        bool const ret{buffSalt_.SetData(salt.data(), static_cast< uint32_t >(salt.size()))};
        std::ignore = ret;
    }

    return ara::core::Result< void >::FromValue();
}
/// @brief Add a secret salt value stored in a SecretSeed object.
/// @brief Add a secret salt value stored in a SecretSeed object.
/// @name  AddSecretSalt
/// @param salt  a salt value (if used, it should be unique for each instance of the target key)
/// @return has vlaue if AddSecretSalt sucess false otherwise
/// @trace_id_sws={SWS_CRYPT_21513}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02102}
/// @uptrace={RS_CRYPTO_02107}
/// @uptrace={RS_CRYPTO_02108}
/// @uptrace={RS_CRYPTO_02111}
/// @threadsafety={Thread-safe}
ara::core::Result< void > PCtxKdf_Base::AddSecretSalt(SecretSeed const &salt) noexcept
{
    PSecretSeed const *const pSecretSeed{dynamic_cast< PSecretSeed const * >(&salt)};
    if (pSecretSeed == nullptr) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kIncompatibleObject);
    }
    uint8_t const *const pData{pSecretSeed->GetSecretSeedData()};
    std::size_t const len{pSecretSeed->GetPayloadSize()};
    std::ignore = buffSalt_.AddData(pData, static_cast< uint32_t >(len));
    return ara::core::Result< void >::FromValue();
}
/// @brief Configure the number of iterations applied by default. The implementation may impose minimum and/or maximum limits on the number of iterations.
/// @name  ConfigIterations
/// @param iterations number of iterations
/// @returns  actual number of the iterations configured in the context now (after this method call)
std::uint32_t PCtxKdf_Base::ConfigIterations(std::uint32_t iterations) noexcept
{
    if (iterations == 0U) {
        nIterations_ = kIterationMin;
    } else {
        if ((iterations > kIterationMax) || (iterations < kIterationMin)) {
            nIterations_ = kIterationMin;
        } else {
            nIterations_ = iterations;
        }
    }
    return nIterations_;
}
/// @brief Derive a symmetric key from the provided key material and the provided context configuration.
/// @brief Derive a symmetric key from the provided key material and provided context configuration.
/// @name  DeriveKey
/// @param isSession  the "session" (or "temporary") attribute for the target key (if @c true)
/// @param isExportable  the exportability attribute for the target key (if @c true)
/// @returns unique smart pointer to the created instance of derived symmetric key
/// @trace_id_sws={SWS_CRYPT_21515}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02102}
/// @uptrace={RS_CRYPTO_02107}
/// @uptrace={RS_CRYPTO_02108}
/// @uptrace={RS_CRYPTO_02111}
/// @uptrace={RS_CRYPTO_02115}
/// @error: SecurityErrorDomain::kUninitializedContext	if the context was not sufficiently initialized
/// @threadsafety={Thread-safe}
ara::core::Result< SymmetricKey::Uptrc > PCtxKdf_Base::DeriveKey(bool isSession, bool isExportable) const noexcept
{
    if (false == IsInitialized()) {
        return ara::core::Result< SymmetricKey::Uptrc >::FromError(SecurityErrorDomain::Errc::kUninitializedContext);
    }

    if (false == IsKeyAvailable()) {
        return ara::core::Result< SymmetricKey::Uptrc >::FromError(SecurityErrorDomain::Errc::kUninitializedContext);
    }
    internal::PAutoBuff buffKeyData;
    AlgId const nHashId{static_cast< AlgId >(EPhCtxTypeID::kHashMd5)};
    uint32_t const nKeyLen{static_cast< uint32_t >(GetKeyIdSize())};

    uint8_t const *pData{nullptr};
    PKeySymmetric_Base const *const pKeySymmetricBase{dynamic_cast< PKeySymmetric_Base const * >(pSrcKeyMaterial_)};
    if (pKeySymmetricBase != nullptr) {
        pData = pKeySymmetricBase->GetKeySymmetric();
    } else {
        PSecretSeed const *const pSecretSeed{dynamic_cast< PSecretSeed const * >(pSrcKeyMaterial_)};
        if (pSecretSeed != nullptr) {
            pData = pSecretSeed->GetSecretSeedData();
        }
    }

    if (pData == nullptr) {
        return ara::core::Result< SymmetricKey::Uptrc >::FromError(SecurityErrorDomain::Errc::kUninitializedContext);
    }
    std::size_t const len{pSrcKeyMaterial_->GetPayloadSize()};
    bool const ret{_Derive_Key(buffKeyData, pData, static_cast< uint32_t >(len), buffSalt_.Data(), buffSalt_.size(),
                               nHashId, nKeyLen, nIterations_)};
    if (ret == false) {
        return ara::core::Result< SymmetricKey::Uptrc >::FromError(SecurityErrorDomain::Errc::kUninitializedContext);
    }

    ReadOnlyMemRegion const memKeyData{buffKeyData.Data(), static_cast< size_t >(buffKeyData.size())};
    // targetKeyId_
    if (static_cast< uint64_t >(EPhCtxTypeID::kUnDefine) == targetKeyId_) {
        return ara::core::Result< SymmetricKey::Uptrc >::FromError(SecurityErrorDomain::Errc::kUninitializedContext);
    }

    /// This is where the targetKeyId passed in during init is used, not targetAlgId
    ara::core::Result< SymmetricKey::Uptrc > resultSymmetric{PCryptoProvider::GenerateSymmetricKeyEx(
        targetKeyId_, memKeyData, GetTarGetAllowedUsage(), isSession, isExportable)};
    if (!resultSymmetric.HasValue()) {
        return ara::core::Result< SymmetricKey::Uptrc >::FromError(SecurityErrorDomain::Errc::kUninitializedContext);
    }

    SymmetricKey::Uptrc pSymmetricKey{std::move(resultSymmetric).Value()};
    return ara::core::Result< SymmetricKey::Uptrc >::FromValue(std::move(pSymmetricKey));
}
/// @brief Derive a "subordinate" key material (secret seed) from the provided "master" key material and the provided context configuration.
/// @brief Derive a "slave" key material (secret seed) from the provided "master" key material and provided context
/// configuration.
/// @name  DeriveSeed
/// @param isSession  the "session" (or "temporary") attribute for the target key (if @c true)
/// @param isExportable  the exportability attribute for the target key (if @c true)
/// @returns unique smart pointer to the created @c SecretSeed object
/// @error: SecurityErrorDomain::kUninitializedContext	if the context was not sufficiently initialized
/// @trace_id_sws={SWS_CRYPT_21516}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02007}
/// @threadsafety={Thread-safe}
ara::core::Result< SecretSeed::Uptrc > PCtxKdf_Base::DeriveSeed(bool isSession, bool isExportable) const noexcept
{
    if (false == IsInitialized()) {
        return ara::core::Result< SecretSeed::Uptrc >::FromError(SecurityErrorDomain::Errc::kUninitializedContext);
    }
    if (false == IsKeyAvailable()) {
        return ara::core::Result< SecretSeed::Uptrc >::FromError(SecurityErrorDomain::Errc::kUninitializedContext);
    }

    internal::PAutoBuff buffKeyData;
    AlgId const nHashId{static_cast< AlgId >(EPhCtxTypeID::kHashMd5)};
    uint32_t const nKeyLen{static_cast< uint32_t >(GetKeyIdSize())};

    uint8_t const *pData{nullptr};
    PKeySymmetric_Base const *const pKeySymmetricBase{dynamic_cast< PKeySymmetric_Base const * >(pSrcKeyMaterial_)};
    if (pKeySymmetricBase != nullptr) {
        pData = pKeySymmetricBase->GetKeySymmetric();
    } else {
        PSecretSeed const *const pSecretSeed{dynamic_cast< PSecretSeed const * >(pSrcKeyMaterial_)};
        if (pSecretSeed != nullptr) {
            pData = pSecretSeed->GetSecretSeedData();
        }
    }

    if (pData == nullptr) {
        return ara::core::Result< SecretSeed::Uptrc >::FromError(SecurityErrorDomain::Errc::kUninitializedContext);
    }
    std::size_t const len{pSrcKeyMaterial_->GetPayloadSize()};
    std::ignore = _Derive_Key(buffKeyData, pData, static_cast< uint32_t >(len), buffSalt_.Data(), buffSalt_.size(),
                              nHashId, nKeyLen, nIterations_);

    ReadOnlyMemRegion const memKeyData{buffKeyData.Data(), static_cast< size_t >(buffKeyData.size())};

    PSecretSeed::Uptrc pcSecretSeed{nullptr};
    PSecretSeed::Uptr pSecretSeed{
        std::make_unique< PSecretSeed >(static_cast< void const * >(memKeyData.data()), memKeyData.size())};
    if (memKeyData.empty()) {
        std::ignore = pSecretSeed->RandomInitKey(static_cast< int32_t >(len) * kInt_8);
    }
    pSecretSeed->SetAllowedUsage(GetTarGetAllowedUsage());
    pSecretSeed->SetExportable(isExportable);
    pSecretSeed->SetSession(isSession);
    pcSecretSeed = std::move(pSecretSeed);

    return ara::core::Result< SecretSeed::Uptrc >::FromValue(std::move(pSecretSeed));
}
/// @brief Clear the encryption context.
/// @name  Reset
/// @returns  has vlaue if Reset sucess false otherwise
ara::core::Result< void > PCtxKdf_Base::Reset() noexcept
{
    pSrcKeyMaterial_     = nullptr;
    nIterations_         = 0U;
    targetKeyId_         = kAlgIdDefault;
    nTargetAlgId_        = kAlgIdDefault;
    nTargetAllowedUsage_ = kAllowPrototypedOnly;
    ctxLabel_            = ReadOnlyMemRegion();
    bInit_               = false;
    return ara::core::Result< void >::FromValue();
}
/// @brief Get the symmetric algorithm ID of the target (subordinate) key. If the context has not been configured by calling the Init() method, kAlgIdUndefined should be returned.
/// @name  GetTargetAlgId
/// @returns  the symmetric algorithm ID of the target key, configured by the last call of the @c Init() method
CryptoContext::AlgId PCtxKdf_Base::GetTargetAlgId() const noexcept
{
    if (IsInitialized()) {
        return nTargetAlgId_;
    }
    return kAlgIdUndefined;
}
/// @brief Get the allowed usage of the target (subordinate) key.
/// @returns allowed key usage bit-flags of target keys
AllowedUsageFlags PCtxKdf_Base::GetTarGetAllowedUsage() const noexcept
{
    if (!bInit_) {
        if (pSrcKeyMaterial_ != nullptr) {
            return pSrcKeyMaterial_->GetAllowedUsage();
        }
        return kAllowKdfMaterialAnyUsage;
    }
    return nTargetAllowedUsage_;
}
/// @brief Initialize this context by setting at least the target key ID.
/// @param targetKeyId  ID of the target key
/// @param targetAlgId  the identifier of the target symmetric crypto algorithm
/// @param allowedUsage bit-flags that define a list of allowed transformations' types in which the target key may be
/// used
/// @param ctxLabel  an optional application specific "context label" (this can identify the purpose of the target key
/// and/or communication parties)
/// @return has vlaue if Init sucess false otherwise
ara::core::Result< void > PCtxKdf_Base::Init(ReadOnlyMemRegion const &targetKeyId,
                                             AlgId targetAlgId,
                                             AllowedUsageFlags allowedUsage,
                                             ReadOnlyMemRegion const &ctxLabel) noexcept
{
    /// @error: SecurityErrorDomain::kIncompatibleArguments	if @c targetAlgId specifies a cryptographic algorithm
    /// different from a symmetric one with key length equal to @c GetTargetKeyBitLength();
    if (targetAlgId != kAlgIdAny) {
        bool isEqual{false};
        ara::core::Result< ara::crypto::cryp::SymmetricBlockCipherCtx::Uptr > resultBlockctx{
            cryptoProvider_.CreateSymmetricBlockCipherCtx(targetAlgId)};
        if (false == resultBlockctx.HasValue()) {
            ara::core::Result< ara::crypto::cryp::StreamCipherCtx::Uptr > resultStreamtx{
                cryptoProvider_.CreateStreamCipherCtx(targetAlgId)};
            if (resultStreamtx.HasValue() == false) {
                return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kInvalidArgument);
            }
            ara::crypto::cryp::StreamCipherCtx::Uptr const pStreamCipherCtx{std::move(resultStreamtx).Value()};
            isEqual = pStreamCipherCtx->GetBlockService()->IsKeyBitLengthSupported(GetTargetKeyBitLength());
        } else {
            ara::crypto::cryp::SymmetricBlockCipherCtx::Uptr const pSymmetricBlockCipherCtx{
                std::move(resultBlockctx).Value()};
            isEqual = pSymmetricBlockCipherCtx->GetCryptoService()->IsKeyBitLengthSupported(GetTargetKeyBitLength());
        }
        if (!isEqual) {
            return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kIncompatibleArguments);
        }
    }

    CryptoProvider::AlgId const nTargetKeyAlgId{
        MyProvider().ConvertToAlgId({T_TransChar(targetKeyId.data()), targetKeyId.size()})};
    ara::core::Result< SymmetricKey::Uptrc > resultSymmetric{cryptoProvider_.GenerateSymmetricKey(nTargetKeyAlgId, 0U)};
    if (false == resultSymmetric.HasValue()) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kInvalidArgument);
    }
    SymmetricKey::Uptrc const pSymmetricKey{std::move(std::move(resultSymmetric).Value())};
    /// If the key length of the encryption algorithm specified by @c targetAlgId is not equal to the symmetric algorithm key length returned by @c GetTargetKeyBitLength();
    if (nullptr == pSymmetricKey) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kIncompatibleArguments);
    }
    if ((pSymmetricKey->GetPayloadSize() * kInt_8U) != GetTargetKeyBitLength()) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kIncompatibleArguments);
    }

    if ((allowedUsage & kAllowKdfMaterial) != kAllowKdfMaterial) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUsageViolation);
    }
    /// @error: SecurityErrorDomain::kUsageViolation			if @c allowedUsage specifies more usages of the
    /// derived key-material than the source key-material,
    ///     i.e. usage of the derived key-material may not be expanded beyond what the source key-material allows
    if (pSrcKeyMaterial_ != nullptr) {
        /// If @c
        /// The usage of the derived key material specified by allowedUsage is greater than the usage of the source key material, i.e., the usage of the derived key material must not exceed the range allowed by the source key material.
        AllowedUsageFlags const nSrcAllowedUsageFlags{pSrcKeyMaterial_->GetAllowedUsage()};
        if ((nSrcAllowedUsageFlags & allowedUsage) != allowedUsage) {
            return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUsageViolation);
        }
    }
    targetKeyId_         = nTargetKeyAlgId;
    nTargetAlgId_        = targetAlgId;
    nTargetAllowedUsage_ = allowedUsage;
    ctxLabel_            = ctxLabel;

    bInit_ = true;
    return ara::core::Result< void >::FromValue();
}
/// @brief Deploy key-material to the key-derivation algorithm context.
/// @name  SetSourceKeyMaterial
/// @param sourceKM source key material
/// @returns  has vlaue if SetSourceKeyMaterial sucess false otherwise
ara::core::Result< void > PCtxKdf_Base::SetSourceKeyMaterial(RestrictedUseObject const &sourceKM) noexcept
{
    /// @error: SecurityErrorDomain::kIncompatibleObject		if the provided key object is incompatible with
    /// this symmetric key context
    if (false
        == ChecRestrictedUseObject(
            sourceKM))  // If the provided key object is incompatible with this symmetric key context
    {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kIncompatibleObject);
    }
    /// @error: SecurityErrorDomain::kUsageViolation			if deriving a key is prohibited by the "allowed
    /// usage" restrictions of the provided source key-material
    RestrictedUseObject::Usage const sourceUsage{sourceKM.GetAllowedUsage()};
    // If the "allowed usage" restriction of the provided source key material prohibits key derivation
    if ((sourceUsage & kAllowKdfMaterial) != kAllowKdfMaterial) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUsageViolation);
    }

    /// @error:  SecurityErrorDomain::kUsageViolatio -- if the allowed usage flags of the provided ara::crypto::cryp::RestrictedUseObject
    /// are more restrictive than the allowed usage flags previously set by ara::crypto::cryp::KeyDerivationFunctionCtx::Init
    /// If the AllowedUsage of sourceKM is more restrictive than the AllowedUsage set by the Init() function, return kUsageViolation
    if ((sourceUsage & nTargetAllowedUsage_) != nTargetAllowedUsage_) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUsageViolation);
    }
    /// @error: SecurityErrorDomain::kBruteForceRisk			if key length of the @c sourceKm is below of an
    /// internally defined limitation
    if (false
        == CheckSourceKeyLen(static_cast< uint32_t >(
            sourceKM.GetPayloadSize())))  // If the key length of @c sourceKM is below the internally defined limit
    {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kBruteForceRisk);
    }
    pSrcKeyMaterial_ = &sourceKM;
    return ara::core::Result< void >::FromValue();
}
//********************************/  //PServiceExtension interface
/// @brief Get the actual bit length of the key loaded into the context. If no key is set for the context, returns 0.
/// @name  GetActualKeyBitLength
/// @returns  actual length of a key (now set to the algorithm context) in bits
std::size_t PCtxKdf_Base::GetActualKeyBitLength() const noexcept
{
    if (IsKeyAvailable()) {
        return pSrcKeyMaterial_->GetPayloadSize() * kInt_8U;
    }
    return 0U;
}
/// @brief Get the COUID of the key deployed to the context attached to this extension service. If no key is set for the context, returns an empty COUID (Nil).
/// @name  GetActualKeyCOUID
/// @returns  the COUID of the CryptoObject
CryptoObjectUid PCtxKdf_Base::GetActualKeyCOUID() const noexcept
{
    if (IsKeyAvailable()) {
        return pSrcKeyMaterial_->GetObjectId().mCouid;
    }
    return CryptoObjectUid{};
}
/// @brief Get the allowed usage of this context (according to the key object properties loaded into this context).
/// If the context has not been initialized with a key object, it must return 0 (all flags reset).
/// @name  GetAllowedUsage
/// @returns  a combination of bit-flags that specifies allowed usages of the context
AllowedUsageFlags PCtxKdf_Base::GetAllowedUsage() const noexcept
{
    if (IsKeyAvailable()) {
        return pSrcKeyMaterial_->GetAllowedUsage();
    }
    return kAllowPrototypedOnly;
}
/// @brief Get the maximum supported key length (in bits).
/// @name  GetMaxKeyBitLength
/// @returns  maximal supported length of the key in bits
std::size_t PCtxKdf_Base::GetMaxKeyBitLength() const noexcept
{
    return static_cast< std::size_t >(PAlgId_Symmetric_DesKey::EKeyLen::kDesKeyMaxBitLength);
}
/// @brief Get the minimum supported key length (in bits).
/// @name  GetMinKeyBitLength
/// @returns  minimal supported length of the key in bits
std::size_t PCtxKdf_Base::GetMinKeyBitLength() const noexcept
{
    return static_cast< std::size_t >(PAlgId_Symmetric_DesKey::EKeyLen::kDesKeyMinBitLength);
}
/// @brief Verify support for a specific key length according to the context.
/// @name  IsKeyBitLengthSupported
/// @param keyBitLength key length: in bits
/// @returns @c true if provided value of the key length is supported by the context
bool PCtxKdf_Base::IsKeyBitLengthSupported(std::size_t keyBitLength) const noexcept
{
    return (keyBitLength >= GetMinKeyBitLength()) && (keyBitLength <= GetMaxKeyBitLength());
}
/// @brief Check whether a key is set for this context.
/// @name  IsKeyAvailable
/// @returns   true if a key has been set to this context false otherwise
bool PCtxKdf_Base::IsKeyAvailable() const noexcept
{
    if (pSrcKeyMaterial_ == nullptr) {
        return false;
    }
    return true;
}
//***************/
/// @brief Function to export key
/// @name  _Derive_Key
/// @param buffReturn buffer for return result
/// @param pSrcData source data
/// @param nSrcLen source data length
/// @param pSalt salt data
/// @param nSaltLen salt length
/// @param nHashId hash algorithm crypto primitive ID
/// @param nKeyLen key length
/// @param nIterations number of iterations
/// @return ture if derive key sucess false otherwise
bool PCtxKdf_Base::_Derive_Key(internal::PAutoBuff &buffReturn,
                               uint8_t const *const pSrcData,
                               uint32_t const nSrcLen,
                               uint8_t const *const pSalt,
                               uint32_t nSaltLen,
                               AlgId const nHashId,
                               uint32_t const nKeyLen,
                               uint32_t nIterations) const noexcept
{
    if (buffReturn.size() > 0U) {
    }  // for qac
    PH_ASSERT(nKeyLen > 0U);
    PH_ASSERT((pSrcData != nullptr) && (nSrcLen > 0U));
    if ((pSalt != nullptr) && (nSaltLen > 0U)) {
    } else {
        nSaltLen = 0U;
    }
    // allocate output buffer
    internal::PAutoBuff buffKeyData{nKeyLen};
    // determine number of iterations
    uint32_t nIterationIndex{0U};
    if (nIterations <= 0U) {
        nIterations = 1U;
    }
    while (nIterationIndex < nIterations) {
        bool const res{
            _getBuffKeyData(nIterationIndex, buffKeyData, pSrcData, nSrcLen, nSaltLen, pSalt, nKeyLen, nHashId)};
        if (res == false) {
            return false;
        }

        nIterationIndex += 1U;
    }
    buffReturn = std::move(buffKeyData);
    return true;
}
/// @brief Encapsulate part of the code inside the function to reduce the complexity of the _Derive_Key function
/// @param nIterationIndex number of iterations for key derivation
/// @param buffKeyData key data
/// @param pSrcData source data used for derivation
/// @param nSrcLen length of source data used for derivation
/// @param nSaltLen length of salt
/// @param pSalt salt value
/// @param nKeyLen key length
/// @param nHashId hash algorithm id
/// @return whether buffKeyData was successfully obtained
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_
/// @trace_id_dd=DD_CRYPTO_
/// @trace_id_sr=SR_CRYPTO_
/// @unit_name=
/// @needwork = dd
/// @endcode
bool PCtxKdf_Base::_getBuffKeyData(uint32_t const nIterationIndex,
                                   internal::PAutoBuff &buffKeyData,
                                   uint8_t const *const pSrcData,
                                   uint32_t const nSrcLen,
                                   uint32_t const nSaltLen,
                                   uint8_t const *const pSalt,
                                   uint32_t const nKeyLen,
                                   AlgId const nHashId) const noexcept
{
    ara::core::Result< HashFunctionCtx::Uptr > const resultHashCreate{MyProvider().CreateHashFunctionCtx(nHashId)};
    PH_ASSERT(resultHashCreate.HasValue());
    HashFunctionCtx::Uptr const &pCtxHash{resultHashCreate.Value()};

    uint8_t const *const pWorkSrcData{(0U == nIterationIndex) ? pSrcData : buffKeyData.Data()};
    uint32_t const nWorkSrcLen{(0U == nIterationIndex) ? nSrcLen : buffKeyData.size()};
    // assemble input buffer: original content + salt + counter
    uint32_t const nSrcBuffLen{nWorkSrcLen + nSaltLen};
    internal::PAutoBuff buffSrcData{nSrcBuffLen + 4U};
    bool const ret{buffSrcData.SetData(pWorkSrcData, nWorkSrcLen, false)};
    std::ignore = ret;
    if (nSaltLen > 0U) {
        bool const ret1{buffSrcData.AddData(pSalt, nSaltLen)};
        if (!ret1) {
            return false;
        }
    }
    buffSrcData.SetDataSize(buffSrcData.size() + kInt_4U);
    uint32_t nPieceCount{0U};
    while (true) {
        if (buffKeyData.size() >= nKeyLen) {
            break;
        }
        // add fragment information
        uint8_t *const pBuffSrcData{buffSrcData.Data(0U)};
        uint32_t resTmp{0U};
        resTmp                                  = (nPieceCount >> kInt_24U) & kInt_0xFFU;
        *(pBuffSrcData + nSrcBuffLen + kInt_0U) = static_cast< uint8_t >(resTmp);
        resTmp                                  = (nPieceCount >> kInt_16U) & kInt_0xFFU;
        *(pBuffSrcData + nSrcBuffLen + kInt_1U) = static_cast< uint8_t >(resTmp);
        resTmp                                  = (nPieceCount >> kInt_8U) & kInt_0xFFU;
        *(pBuffSrcData + nSrcBuffLen + kInt_2U) = static_cast< uint8_t >(resTmp);
        resTmp                                  = (nPieceCount)&kInt_0xFFU;
        *(pBuffSrcData + nSrcBuffLen + kInt_3U) = static_cast< uint8_t >(resTmp);
        std::ignore                             = pCtxHash->Start();
        std::ignore
            = pCtxHash->Update(ReadOnlyMemRegion(buffSrcData.Data(), static_cast< std::size_t >(buffSrcData.size())));
        ara::core::Result< ara::core::Vector< ara::core::Byte > > const resultHash{pCtxHash->Finish()};
        if (!resultHash.HasValue()) {
            return false;
        }
        ara::core::Vector< ara::core::Byte > const &vecHash{resultHash.Value()};
        uint32_t nCopyLen{static_cast< uint32_t >(resultHash.Value().size())};
        if (nCopyLen > nKeyLen) {
            nCopyLen = nKeyLen;
        }
        /// If the current length of buffKeyData plus the copy length is greater than the allocated space of buffKeyData, the following addData will fail, so truncate the data
        if (buffKeyData.size() + nCopyLen > buffKeyData.GetBuffLen()) {
            nCopyLen = buffKeyData.GetBuffLen() - buffKeyData.size();
        }
        // save result
        bool const ret1{
            buffKeyData.AddData(static_cast< uint8_t const * >(static_cast< void const * >(vecHash.data())), nCopyLen)};
        if (!ret1) {
            return false;
        }

        nPieceCount += 1U;
    }

    return true;
}
//********************************/
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara
