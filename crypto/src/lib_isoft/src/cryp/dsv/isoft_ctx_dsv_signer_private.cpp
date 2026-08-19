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
/// @file       isoft_ctx_dsv_signer_private.cpp
/// @brief      AutoSar-Crypto Encryption/Decryption Module
/// @details
/// @date       2022-03-23
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default Encryption/Signature Storage
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_01009
/// @unit_name=PCtxDsv_SignerPrivate
/// @unit_description=Base class for signature private key context interface
/// @endcode
///
/// ================================================================

#include "ara/crypto/cryp/dsv/isoft_ctx_dsv_signer_private.h"

#include "ara/crypto/common/isoft_data_type.h"
#include "ara/crypto/cryp/cryobj/isoft_signature.h"
#include "ara/crypto/cryp/hash/isoft_ctx_hash_function.h"
#include "ara/crypto/cryp/isoft_crypto_provider.h"
#include "ara/crypto/cryp/isoft_service_signature.h"
#include "ara/crypto/cryp/select/isoft_select_rsa.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
//********************************/
/// @brief Signature private key context interface.
/// @param cryptoProvider Crypto provider.
PCtxDsv_SignerPrivate::PCtxDsv_SignerPrivate(PCryptoProvider &cryptoProvider) noexcept
    : PCtxDsv_SignerPrivate{cryptoProvider, nullptr,
                            static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kHashSha2_256)}
{
}

/// @brief Constructor with parameters.
/// @param cryptoProvider Crypto provider.
/// @param pPrivateKey Pointer to the private key object.
/// @param nHashAlgID Hash algorithm crypto primitive ID.
PCtxDsv_SignerPrivate::PCtxDsv_SignerPrivate(PCryptoProvider &cryptoProvider,
                                             PrivateKey const *const pPrivateKey,
                                             CryptoPrimitiveId::AlgId const nHashAlgID) noexcept
    : SignerPrivateCtx{}  // NOLINT
    , cryptoProvider_{cryptoProvider}
    , pPrivateKey_{pPrivateKey}
    , nHashAlgID_{nHashAlgID}
{
}
//*/ //CryptoContext Interface
/// @brief Returns the CryptoPrimitiveId instance containing the instance identifier.
/// @returns CryptoPrimitiveId instance.
CryptoPrimitiveId::Uptr PCtxDsv_SignerPrivate::GetCryptoPrimitiveId() const noexcept
{
    return {std::make_unique< PAlgId_Asymmetric_Rsa >()};
}
/// @brief Checks whether the encryption context is initialized and ready to use. It checks all required values, including: key value, IV/seed, etc.
/// @returns  @c true if the crypto context is completely initialized and ready to use, and @c false otherwise
bool PCtxDsv_SignerPrivate::IsInitialized() const noexcept { return IsKeyAvailable(); }
/// @brief Gets a reference to the Crypto Provider for this context.
/// @returns a reference to Crypto Provider instance that provides this context
// PRQA S 4623 QAC /// @qac: AUTOSAR standard interface
inline CryptoProvider &PCtxDsv_SignerPrivate::MyProvider() const noexcept { return cryptoProvider_; }
// PRQA L:QAC
//*/ //SignerPrivateCtx Interface
/// @brief Clears the encryption context.
/// @returns has value if reset sucess false otherwise
ara::core::Result< void > PCtxDsv_SignerPrivate::Reset() noexcept
{
    pPrivateKey_ = nullptr;
    return ara::core::Result< void >::FromValue();
}
/// @brief Sets (deploys) a key into the signer private algorithm context.
/// @brief SWS_CRYPT_01820
/// @param key Private key.
/// @returns has value if SetKey sucess false otherwise
ara::core::Result< void > PCtxDsv_SignerPrivate::SetKey(PrivateKey const &key) noexcept
{
    /// @error: SecurityErrorDomain::kIncompatibleObject     if the provided key object is incompatible with this
    /// symmetric key context
    /// If the provided key object is incompatible with this symmetric key context
    if (false == CheckKey(key)) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kIncompatibleObject);
    }
    /// @error: SecurityErrorDomain::kUsageViolation         if the transformation type associated with this context is
    /// prohibited by the "allowed usage" restrictions of provided key object
    /// If the "allowed usage" restrictions of the provided key object prohibit the type of transformation associated with this context
    uint32_t const nKeyAllowedUsage{key.GetAllowedUsage()};
    if (((kAllowDataEncryption & nKeyAllowedUsage) != kAllowDataEncryption)
        || ((kAllowSignature & nKeyAllowedUsage) != kAllowSignature)) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUsageViolation);
    }
    pPrivateKey_ = &key;
    return ara::core::Result< void >::FromValue();
}
/// @brief Signs the provided digest value stored in the hash function context.
/// This method must put the hash function algorithm ID of the key pair used and the COUID of the key pair into the resulting signature object!
/// The key pair here is a private key and a public key. The COUID of the public key should be placed in the signature.
/// During signature verification, the public key COUID is extracted and compared with the public key COUID used in the verification context.
ara::core::Result< Signature::Uptrc >
/// @brief Sign a provided digest value stored in the hash-function context.
///       This method must put the hash-function algorithm ID and a @a COUID of the used key-pair to the resulting
///       signature object! The user supplied @c context may be used for such algorithms as: Ed25519ctx, Ed25519ph,
///       Ed448ph. If the target algorithm doesn't support the @c context argument then the empty (default) value must
///       be supplied!
/// @brief SWS_CRYPT_02415
/// @param hashFn A finalized hash-function context that contains a digest.
/// value ready for sign
/// @param context An optional user supplied "context" (its support depends on the specific algorithm).
/// depends from concrete algorithm)
/// @returns unique smart pointer to serialized signature
/// @error: SecurityErrorDomain::kInvalidArgument   if hash-function algorithm does not comply with the signature
///     algorithm specification of this context
/// @error: SecurityErrorDomain::kInvalidInputSize  if the user supplied @c context has incorrect (or unsupported) size
/// @error: SecurityErrorDomain::kProcessingNotFinished  if the method @c hash.Finish() was not called before the call
///     of this method
/// @error: SecurityErrorDomain::kUninitializedContext   this context was not initialized by a key value
PCtxDsv_SignerPrivate::SignPreHashed(HashFunctionCtx const &hashFn, ReadOnlyMemRegion const &context) const noexcept
{
    /// @error: SecurityErrorDomain::kUninitializedContext   this context was not initialized by a key value
    if (false == IsInitialized()) {
        return ara::core::Result< Signature::Uptrc >::FromError(SecurityErrorDomain::Errc::kUninitializedContext);
    }
    CryptoPrimitiveId::AlgId const nHashAlgID{hashFn.GetCryptoPrimitiveId()->GetPrimitiveId()};
    /// @error: SecurityErrorDomain::kInvalidArgument        if hash-function algorithm does not comply with the
    /// signature algorithm specification of this context
    if ((nHashAlgID <= 0U) || (nHashAlgID != nHashAlgID_)) {
        return ara::core::Result< Signature::Uptrc >::FromError(SecurityErrorDomain::Errc::kInvalidArgument);
    }
    /// @error: SecurityErrorDomain::kProcessingNotFinished  if the method @c hash.Finish() was not called before the
    /// call of this method
    DigestService::Uptr const pDigestService{hashFn.GetDigestService()};
    if (false == pDigestService->IsFinished()) {
        return ara::core::Result< Signature::Uptrc >::FromError(SecurityErrorDomain::Errc::kProcessingNotFinished);
    }
    /// @error: SecurityErrorDomain::kInvalidInputSize       if the user supplied @c context has incorrect (or
    /// unsupported) size
    if ((context.size() + pDigestService->GetDigestSize())
        > static_cast< std::size_t >(ESelectLength::kMaxInputBuffLen)) {
        return ara::core::Result< Signature::Uptrc >::FromError(SecurityErrorDomain::Errc::kInvalidInputSize);
    }
    // Compute signature
    ara::core::Result< ara::core::Vector< ara::core::Byte > > const resultDigest{hashFn.GetDigest()};
    ReadOnlyMemRegion const memDigest{
        static_cast< uint8_t const * >(static_cast< void const * >(resultDigest.Value().data())),
        resultDigest.Value().size()};

    return SignPreHashed(nHashAlgID, memDigest);
}
/// @brief Signs a directly provided hash value or message value.
/// This method can be used to implement "multi-pass" signature algorithms that directly process messages, i.e., without "pre-hashing" (e.g., Ed25519ctx).
/// This method is also suitable for implementing traditional pre-hashed signature schemes (e.g., Ed25519ph, Ed448ph, ECDSA). If the target algorithm does not support the context parameter, a null value (default) must be provided!
ara::core::Result< ara::core::Vector< ara::core::Byte > >
/// @brief SWS_CRYPT_02416
/// @param value Data involved in the algorithm operation.
/// @param context Context involved in the algorithm operation.
/// @return actual size of the signature value stored to the output buffer
PCtxDsv_SignerPrivate::Sign(ReadOnlyMemRegion const &value, ReadOnlyMemRegion const &context) const noexcept
{
    /// SWS_CRYPT_02416
    /// @error: SecurityErrorDomain::kUninitializedContext   this context was not initialized by a key value
    if (false == IsInitialized()) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kUninitializedContext);
    }
    /// SWS_CRYPT_02416
    /// @error: SecurityErrorDomain::kInvalidInputSize       if the user supplied @c context has incorrect (or
    /// unsupported) size
    if ((context.size() + value.size()) != _GetHashResultLen()) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kInvalidInputSize);
    }
    ara::core::Result< ara::core::Vector< ara::core::Byte > > const resVec{_DoSign(value, context)};
    if (!resVec.HasValue()) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kInvalidArgument);
    }
    ara::core::Vector< ara::core::Byte > const &vecSign{resVec.Value()};

    if (vecSign.empty()) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kInvalidArgument);
    }
    return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromValue(vecSign);
}
/// @brief Signs a directly provided digest value and creates a signature object.
/// This method must put the hash function algorithm ID of the key pair used and the COUID of the key pair into the resulting signature object!
ara::core::Result< Signature::Uptrc >
/// @brief Sign a directly provided digest value and create the @c Signature object.
///       This method must put the hash-function algorithm ID and a @a COUID of the used key-pair to the resulting
///       signature object! The user supplied @c context may be used for such algorithms as: Ed25519ctx, Ed25519ph,
///       Ed448ph. If the target algorithm doesn't support the @c context argument then the empty (default) value
///       must be supplied!
/// @param hashAlgId Hash function algorithm ID.
/// @param hashValue Hash function value (resulting digest without any truncations).
/// @param context An optional user supplied "context" (its support depends on the specific algorithm).
///     depends from concrete algorithm
/// @returns   unique smart pointer to serialized signature
PCtxDsv_SignerPrivate::SignPreHashed(AlgId hashAlgId,
                                     ReadOnlyMemRegion const &hashValue,
                                     ReadOnlyMemRegion const &context) const noexcept
{
    /// @error: SecurityErrorDomain::kUninitializedContext   this context was not initialized by a key value
    if (false == IsInitialized()) {
        return ara::core::Result< Signature::Uptrc >::FromError(SecurityErrorDomain::Errc::kUninitializedContext);
    }
    /// @error: SecurityErrorDomain::kInvalidArgument        if hash-function algorithm does not comply with the
    /// signature algorithm specification of this context
    if (hashAlgId != nHashAlgID_) {
        return ara::core::Result< Signature::Uptrc >::FromError(SecurityErrorDomain::Errc::kInvalidArgument);
    }

    /// SWS_CRYPT_02416
    /// @error: SecurityErrorDomain::kInvalidInputSize       if the user supplied @c context has incorrect (or
    /// unsupported) size
    if ((context.size() + hashValue.size()) > static_cast< std::size_t >(ESelectLength::kMaxInputBuffLen)) {
        return ara::core::Result< Signature::Uptrc >::FromError(SecurityErrorDomain::Errc::kInvalidInputSize);
    }

    ara::core::Vector< ara::core::Byte > vecSign;

    /// [SWS_CRYPT_02418] Truncation of hash value
    /// Check the bit length of memDigest,
    /// · The bit length of the input hash value is greater than the bit length used for signing/verification
    /// · And according to the algorithm specified by Algid, it allows the use of a hash value with the provided bit length (here referring to the length to be truncated) and specifies the truncation.
    std::size_t const sigSize{GetSignatureSize()};
    if (hashValue.size() > sigSize) {
        /// Truncation
        ReadOnlyMemRegion const hashValueTruncation{hashValue.first(sigSize)};
        ara::core::Result< ara::core::Vector< ara::core::Byte > > resVec{_DoSign(hashValueTruncation, context)};
        if (!resVec.HasValue()) {
            return ara::core::Result< Signature::Uptrc >::FromError(SecurityErrorDomain::Errc::kInvalidArgument);
        }
        vecSign = std::move(resVec).Value();
    } else {
        ara::core::Result< ara::core::Vector< ara::core::Byte > > resVec{_DoSign(hashValue, context)};
        if (!resVec.HasValue()) {
            return ara::core::Result< Signature::Uptrc >::FromError(SecurityErrorDomain::Errc::kInvalidArgument);
        }
        vecSign = std::move(resVec).Value();
    }

    if (vecSign.empty()) {
        return ara::core::Result< Signature::Uptrc >::FromError(SecurityErrorDomain::Errc::kInvalidArgument);
    }

    // Assemble the return value
    CryptoPrimitiveId::AlgId nCryptoAlgId{GetCryptoAlgId()};
    isoft_def::PSignature::Uptr pSignature{std::make_unique< isoft_def::PSignature >(hashAlgId, nCryptoAlgId)};
    if (nCryptoAlgId == 0U) {
    }
    std::size_t const nLen{vecSign.size()};
    pSignature->SetSignatureData(hashAlgId, vecSign.data(), static_cast< uint32_t >(nLen));
    pSignature->SetDependence(pPrivateKey_->GetObjectId());  // Put the private key COUID into the signature
    return ara::core::Result< Signature::Uptrc >::FromValue(std::move(pSignature));
}
//*/ //PServiceSignature Interface
/// @brief Gets the actual bit length of the key loaded into the context. If no key is set for the context, returns 0.
/// @returns  actual length of a key (now set to the algorithm context) in bits
std::size_t PCtxDsv_SignerPrivate::GetActualKeyBitLength() const noexcept
{
    if (false == IsKeyAvailable()) {
        return 0U;
    }
    return pPrivateKey_->GetPayloadSize() * kInt_8U;  // Convert to bit length
}
/// @brief Gets the COUID of the key deployed to the context attached to this extension service. If no key is set for the context, returns an empty COUID (Nil).
/// @returns the COUID of the CryptoObject
CryptoObjectUid PCtxDsv_SignerPrivate::GetActualKeyCOUID() const noexcept
{
    if (false == IsKeyAvailable()) {
        return CryptoObjectUid{};
    }
    return pPrivateKey_->GetObjectId().mCouid;
}
/// @brief Gets the allowed usage of this context (according to the key object properties loaded into this context).
/// If the context has not been initialized with a key object, 0 must be returned (all flags reset).
/// @returns  a combination of bit-flags that specifies allowed usages of the context
AllowedUsageFlags PCtxDsv_SignerPrivate::GetAllowedUsage() const noexcept
{
    if (false == IsKeyAvailable()) {
        return kAllowPrototypedOnly;
    }
    return pPrivateKey_->GetAllowedUsage();
}
/// @brief Checks whether a key is set for this context.
/// @returns  true if a key has been set to this context false otherwise
bool PCtxDsv_SignerPrivate::IsKeyAvailable() const noexcept
{
    if (nullptr == pPrivateKey_) {
        return false;
    }
    return true;
}
/// @brief Gets the hash size required by the current signature algorithm.
/// @returns  required hash algorithm ID or @c kAlgIdAny if the signature algorithm specification does not include a
///     concrete hash function
std::size_t PCtxDsv_SignerPrivate::GetRequiredHashSize() const noexcept
{
    ara::core::Result< HashFunctionCtx::Uptr > const hashResult{MyProvider().CreateHashFunctionCtx(nHashAlgID_)};
    if (false == hashResult.HasValue()) {
        return 0U;
    }
    PCtxHashFunction *const pHashCtx{
        static_cast< PCtxHashFunction * >(static_cast< void * >(hashResult.Value().get()))};
    return static_cast< std::size_t >(pHashCtx->GetHashLength());
}
//***************/
/// @brief Performs the signature algorithm logic.
ara::core::Result< ara::core::Vector< ara::core::Byte > >
/// @param digest Digest data.
/// @param context Context involved in the algorithm operation.
/// @returns Signature data.
PCtxDsv_SignerPrivate::_DoSign(ReadOnlyMemRegion const &digest, ReadOnlyMemRegion const &context) const noexcept
{
    if (false == context.empty()) {
        internal::PAutoBuff buffInput{static_cast< uint32_t >(ESelectLength::kMaxInputBuffLen)};
        bool const ret{buffInput.SetData(digest.data(), static_cast< uint32_t >(digest.size()), false)};
        std::ignore = ret;
        bool const ret1{buffInput.AddData(context.data(), static_cast< uint32_t >(context.size()))};
        std::ignore = ret1;
        return DoEncrypto(buffInput.Data(), buffInput.size(), false);
    }
    return DoEncrypto(digest.data(), static_cast< uint32_t >(digest.size()), false);
}
//*/ //PCtxDsv_VerifierPublic Interface
/// @brief Gets the public key corresponding to the private key.
/// @returns Public key instance.
PublicKey::Uptrc PCtxDsv_SignerPrivate::GetPublicKey() const noexcept
{
    ara::core::Result< PublicKey::Uptrc > result{pPrivateKey_->GetPublicKey()};
    if (false == result.HasValue()) {
        return {nullptr};
    }
    return std::move(result).Value();
}

//********************************/
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara
