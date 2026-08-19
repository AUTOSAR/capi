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
/// @file       isoft_ctx_dsv_verifier_public.cpp
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
/// @unit_name=PCtxDsv_VerifierPublic
/// @unit_description=Signature Verification Public Key Context Interface Base Class
/// @endcode
///
/// ================================================================

#include "ara/crypto/cryp/dsv/isoft_ctx_dsv_verifier_public.h"

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
/// @brief Signature verification public key context interface.
//********************************/
/// @brief Parameterized constructor
/// @param cryptoProvider Encryption provider
/// @returns
PCtxDsv_VerifierPublic::PCtxDsv_VerifierPublic(PCryptoProvider &cryptoProvider) noexcept
    : PCtxDsv_VerifierPublic{cryptoProvider, nullptr,
                             static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kHashSha2_256)}
{
}
/// @brief Parameterized constructor
/// @param cryptoProvider Encryption provider
/// @param pPublicKey Public key
/// @param nHashAlgID Hash algorithm crypto primitive ID
PCtxDsv_VerifierPublic::PCtxDsv_VerifierPublic(PCryptoProvider &cryptoProvider,
                                               PublicKey const *const pPublicKey,
                                               CryptoPrimitiveId::AlgId const nHashAlgID) noexcept
    : VerifierPublicCtx{}  // NOLINT
    , cryptoProvider_{cryptoProvider}
    , pPublicKey_{pPublicKey}
    , nHashAlgID_{nHashAlgID}
{
}
//***************/  //CryptoContext interface
/// @brief Check if the encryption context has been initialized and is ready for use. It checks all required values, including: key values, IV/seed, etc.
/// @returns @c true if the crypto context is completely initialized and ready to use, and @c false otherwise
bool PCtxDsv_VerifierPublic::IsInitialized() const noexcept { return IsKeyAvailable(); }
/// @brief Get a reference to the Crypto Provider associated with this context.
/// @returns  a reference to Crypto Provider instance that provides this context
// PRQA S 4623 QAC /// @qac: AUTOSAR standard interface
inline CryptoProvider &PCtxDsv_VerifierPublic::MyProvider() const noexcept { return cryptoProvider_; }
// PRQA L:QAC
//***************/  //VerifierPublicCtx interface
/// @brief Clear the encryption context.
/// @returns  has value if reset sucess false otherwise
ara::core::Result< void > PCtxDsv_VerifierPublic::Reset() noexcept
{
    pPublicKey_ = nullptr;
    nHashAlgID_ = static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kHashSha2_256);
    return ara::core::Result< void >::FromValue();
}
/// @brief Set (deploy) a key to the verifier public algorithm context.
/// @brief SWS_CRYPT_01821
/// @param key Public key
/// @returns  has value if SetKey sucess false otherwise
ara::core::Result< void > PCtxDsv_VerifierPublic::SetKey(PublicKey const &key) noexcept
{
    /// @error: SecurityErrorDomain::kIncompatibleObject     if the provided key object is incompatible with this
    /// symmetric key context
    ///         If the provided key object is incompatible with this symmetric key context
    if (false == CheckKey(key)) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kIncompatibleObject);
    }
    /// @error: SecurityErrorDomain::kUsageViolation         if the transformation type associated with this context is
    /// prohibited by the "allowed usage" restrictions of provided key object
    ///         If the "Allowed Usage" restrictions of the provided key object prohibit the transformation type associated with this context
    uint32_t const nKeyAllowedUsage{key.GetAllowedUsage()};
    if (((kAllowDataDecryption & nKeyAllowedUsage) != kAllowDataDecryption)
        || ((kAllowVerification & nKeyAllowedUsage) != kAllowVerification)) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUsageViolation);
    }
    pPublicKey_ = &key;
    return ara::core::Result< void >::FromValue();
}
/// @brief Verify the signature via the digest value stored in the hash function context.
///         This is a pass-through interface to SWS_CRYPT_24113 for developer convenience, meaning it adds extra input checks and then calls the verify() interface from SWS_CRYPT_24113.
/// @brief Verify signature by a digest value stored in the hash-function context.
///         This is a pass-through interface to SWS_CRYPT_24113 for developer convenience,
///         i.e. it adds additional input checks amd then calls the verify() interface from SWS_CRYPT_24113.
/// @param hashFn  hash function to be used for hashing
/// @param signature  the signature object for verification
/// @param context  an optional user supplied "context" (its support depends from concrete algorithm)
/// @returns @c true if the signature was verified successfully and @c false otherwise
/// @error: SecurityErrorDomain::kProcessingNotFinished  if the method @c hashFn.Finish() was not called before this
///     method call
/// @error: SecurityErrorDomain::kInvalidArgument  if the CryptoAlgId of @c hashFn differs from the CryptoAlgId of this
///     context
ara::core::Result< bool > PCtxDsv_VerifierPublic::VerifyPrehashed(HashFunctionCtx const &hashFn,
                                                                  Signature const &signature,
                                                                  ReadOnlyMemRegion const &context) const noexcept
{
    /// SWS_CRYPT_02417
    /// @error: SecurityErrorDomain::kUninitializedContext   this context was not initialized by a key value
    if (false == IsInitialized()) {
        return ara::core::Result< bool >::FromError(SecurityErrorDomain::Errc::kUninitializedContext);
    }
    /// SWS_CRYPT_02417
    /// @error: SecurityErrorDomain::kProcessingNotFinished  if the method @c hash.Finish() was not called before the
    /// call of this method
    DigestService::Uptr const pDigestService{hashFn.GetDigestService()};
    if (false == pDigestService->IsFinished()) {
        return ara::core::Result< bool >::FromError(SecurityErrorDomain::Errc::kProcessingNotFinished);
    }
    /// SWS_CRYPT_02417
    /// SecurityErrorDomain::kInvalidInputSize error
    if ((context.size() + pDigestService->GetDigestSize())
        > static_cast< std::size_t >(ESelectLength::kMaxInputBuffLen)) {
        return ara::core::Result< bool >::FromError(SecurityErrorDomain::Errc::kInvalidInputSize);
    }
    /// SWS_CRYPT_02417
    /// @error:  SecurityErrorDomain::kInvalidArgument  if the CryptoAlgId of @c hashFn differs from the CryptoAlgId of
    /// this context
    CryptoPrimitiveId::AlgId const nHashAlgID{hashFn.GetCryptoPrimitiveId()->GetPrimitiveId()};
    if (nHashAlgID <= 0U) {
        return ara::core::Result< bool >::FromError(SecurityErrorDomain::Errc::kInvalidArgument);
    }

    if (pPublicKey_->GetObjectId().mCouid != signature.GetDependence()) {
        return ara::core::Result< bool >::FromError(SecurityErrorDomain::Errc::kBadObjectReference);
    }
    // Get digest information
    ara::core::Result< ara::core::Vector< ara::core::Byte > > const resultDigest{hashFn.GetDigest()};
    if (false == resultDigest.HasValue()) {
        return ara::core::Result< bool >::FromError(SecurityErrorDomain::Errc::kInvalidArgument);
    }
    ReadOnlyMemRegion const memDigest{
        static_cast< uint8_t const * >(static_cast< void const * >(resultDigest.Value().data())),
        resultDigest.Value().size()};

    return VerifyPrehashed(nHashAlgID, memDigest, signature);
}
/// @brief Verify a signature BLOB via a directly provided hash value or message value.
///         This method can be used to implement "multi-pass" signature algorithms that handle messages directly, i.e., do not require "pre-hashing" (like Ed25519ctx).
///         This method is also suitable for implementations of traditional pre-hashed signature schemes (like Ed25519ph, Ed448ph, ECDSA).
///         If the target algorithm does not support the context parameter, an empty value (default) must be provided!
///         User-provided context can be used for the following algorithms: Ed25519ctx, Ed25519ph, Ed448ph.
/// @brief Verify signature BLOB by a directly provided hash or message value.
///         This method can be used for implementation of the "multiple passes" signature algorithms that process a
///         message directly, i.e. without "pre-hashing" (like Ed25519ctx). But also this method is suitable for
///         implementation of the traditional signature schemes with pre-hashing (like Ed25519ph, Ed448ph, ECDSA). If
///         the target algorithm doesn't support the @c context argument then the empty (default) value must be
///         supplied! The user supplied @c context may be used for such algorithms as: Ed25519ctx, Ed25519ph, Ed448ph.
/// @param value   The (pre-)hashed or direct message value that should be verified
/// @param signature The signature BLOB for the verification
///            (the BLOB contains a plain sequence of the digital
///            signature components located in fixed/maximum length fields defined by the algorithm specification,
///            and each component is presented by a raw bytes sequence padded by zeroes to full length of the field;
///            e.g. in case of (EC)DSA-256 (i.e. length of the q module is 256 bits) the signature BLOB must have two
///            fixed-size fields: 32 + 32 bytes, for R and S components respectively, i.e. total BLOB size is 64 bytes)
/// @param context An optional user-supplied "context" (its support depends on the specific algorithm)
///     from concrete algorithm)
/// @returns @c true if the signature was verified successfully and @c false otherwise
/// @error: SecurityErrorDomain::kUninitializedContext  if the context was not initialized by a key value
/// @error: SecurityErrorDomain::kInvalidInputSize      if the @c context argument has unsupported size
ara::core::Result< bool > PCtxDsv_VerifierPublic::Verify(ReadOnlyMemRegion const &value,
                                                         ReadOnlyMemRegion const &signature,
                                                         ReadOnlyMemRegion const &context) const noexcept
{
    /// @error: SecurityErrorDomain::kUninitializedContext   this context was not initialized by a key value
    if (false == IsInitialized()) {
        return ara::core::Result< bool >::FromError(SecurityErrorDomain::Errc::kUninitializedContext);
    }
    /// @error:  SecurityErrorDomain::kInvalidInputSize      if the @c context argument has unsupported size
    if ((value.size() + context.size()) > signature.size()) {
        return ara::core::Result< bool >::FromError(SecurityErrorDomain::Errc::kInvalidInputSize);
    }
    bool const bCompare{_DoVerify(value, signature, context)};
    return ara::core::Result< bool >::FromValue(bCompare);
}
/// @brief Verify the signature via the digest value stored in the hash function context.
///         This is a pass-through interface to SWS_CRYPT_24112 for developer convenience, meaning it adds extra input checks and then calls the default verify() interface.
/// @brief Verify signature by a digest value stored in the hash-function context. This is a pass-through
///        interface to SWS_CRYPT_24112 for developer convenience, i.e. it adds additional input checks
///        amd then calls the default verify() interface.
/// @param hashAlgId  hash function algorithm ID
/// @param hashValue  hash function value (resulting digest without any truncations)
/// @param signature  the signature object for the verification
/// @param context    an optional user supplied "context" (its support depends from concrete algorithm)
/// @returns @c true if the signature was verified successfully and @c false otherwise
/// @error: SecurityErrorDomain::kIncompatibleObject    if the CryptoAlgId of this context does not match the
///     CryptoAlgId of @c signature; or the required CryptoAlgId of the hash is not kAlgIdDefault and the required hash
///     CryptoAlgId of this context does not match @c hashAlgId or the hash CryptoAlgId of @c signature
/// @error: SecurityErrorDomain::kIncompatibleArguments if the provided hashAlgId is not kAlgIdDefault and the AlgId of
///     the provided signature object does not match the provided hashAlgId
/// @error: SecurityErrorDomain::kBadObjectReference    if the provided signature object does not reference the public
///     key loaded to the context, i.e. if the COUID of the public key in the context is not equal to the COUID
///     referenced from the signature object.
ara::core::Result< bool > PCtxDsv_VerifierPublic::VerifyPrehashed(CryptoAlgId hashAlgId,
                                                                  ReadOnlyMemRegion const &hashValue,
                                                                  Signature const &signature,
                                                                  ReadOnlyMemRegion const &context) const noexcept
{
    /// @error: SecurityErrorDomain::kUninitializedContext   this context was not initialized by a key value
    if (false == IsInitialized()) {
        return ara::core::Result< bool >::FromError(SecurityErrorDomain::Errc::kUninitializedContext);
    }
    /// @error:  SecurityErrorDomain::kIncompatibleObject    if the CryptoAlgId of this context does not match the
    /// CryptoAlgId of @c signature;
    ///         or the required CryptoAlgId of the hash is not kAlgIdDefault and the required hash CryptoAlgId of this
    ///         context does not match @c hashAlgId or the hash CryptoAlgId of @c signature

    /// If the provided hash’ AlgId is not kAlgIdDefault and the hash’ AlgId does not match the AlgId of the provided signature object
    if (hashAlgId != kAlgIdDefault) {
        if (hashAlgId != signature.GetHashAlgId()) {
            return ara::core::Result< bool >::FromError(SecurityErrorDomain::Errc::kIncompatibleArguments);
        }
    }

    PSignature const &pSignature{dynamic_cast< PSignature const & >(signature)};
    CryptoPrimitiveId::AlgId const nSigCryId{pSignature.GetCryptoAlgId()};
    bool const isMatching{IsCryptoAlgIdMacthing(nSigCryId)};
    if (isMatching == false) {
        return ara::core::Result< bool >::FromError(SecurityErrorDomain::Errc::kIncompatibleObject);
    }

    /// SWS_CRYPT_02417
    if (hashAlgId != kAlgIdDefault) {
        if (nHashAlgID_ != hashAlgId) {
            return ara::core::Result< bool >::FromError(SecurityErrorDomain::Errc::kIncompatibleObject);
        }
        if (nHashAlgID_ != signature.GetHashAlgId()) {
            return ara::core::Result< bool >::FromError(SecurityErrorDomain::Errc::kIncompatibleObject);
        }
    }

    /// SWS_CRYPT_02417
    if (pPublicKey_->GetObjectId().mCouid != signature.GetDependence()) {
        return ara::core::Result< bool >::FromError(SecurityErrorDomain::Errc::kBadObjectReference);
    }

    // Get signature information
    ara::core::Result< ara::core::Vector< ara::core::Byte > > const resultSignature{
        signature.ExportPublicly(Serializable::kFormatRawValueOnly)};
    if (false == resultSignature.HasValue()) {
        return ara::core::Result< bool >::FromError(SecurityErrorDomain::Errc::kInvalidArgument);
    }
    ReadOnlyMemRegion const memSignature{
        static_cast< uint8_t const * >(static_cast< void const * >(resultSignature.Value().data())),
        resultSignature.Value().size()};

    std::size_t const sigSize{GetSignatureSize()};
    bool bCompare{false};
    if (hashValue.size() > sigSize) {
        ReadOnlyMemRegion const hashValueTruncation{hashValue.first(sigSize)};
        bCompare = _DoVerify(hashValueTruncation, memSignature, context);
    } else {
        bCompare = _DoVerify(hashValue, memSignature, context);
    }
    return ara::core::Result< bool >::FromValue(bCompare);
}
/// @brief Verify the signature via the digest value stored in the hash function context.
///         This is a pass-through interface to SWS_CRYPT_24112 for developer convenience, meaning it adds extra input checks and then calls the default verify() interface.
/// @brief Verify signature by a digest value stored in the hash-function context. This is a pass-through
///        interface to SWS_CRYPT_24112 for developer convenience, i.e. it adds additional input checks
///        amd then calls the default verify() interface.
/// @param hashFn  hash function to be used for hashing
/// @param signature  the data BLOB to be verified
/// @param context  an optional user supplied "context" (its support depends from concrete algorithm)
/// @returns @c true if the signature was verified successfully and @c false otherwise
/// @error: SecurityErrorDomain::kProcessingNotFinished     if the method @c hashFn.Finish() was not called before this
///     method call
/// @error: SecurityErrorDomain::kInvalidArgument           if the CryptoAlgId of @c hashFn differs from the CryptoAlgId
///     of this context
ara::core::Result< bool > PCtxDsv_VerifierPublic::VerifyPrehashed(HashFunctionCtx const &hashFn,
                                                                  ReadOnlyMemRegion const &signature,
                                                                  ReadOnlyMemRegion const &context) const noexcept
{
    /// @error: SecurityErrorDomain::kUninitializedContext   this context was not initialized by a key value
    if (false == IsInitialized()) {
        return ara::core::Result< bool >::FromError(SecurityErrorDomain::Errc::kUninitializedContext);
    }
    /// @error: SecurityErrorDomain::kProcessingNotFinished  if the method @c hash.Finish() was not called before the
    /// call of this method
    DigestService::Uptr const pDigestService{hashFn.GetDigestService()};
    if (false == pDigestService->IsFinished()) {
        return ara::core::Result< bool >::FromError(SecurityErrorDomain::Errc::kProcessingNotFinished);
    }

    /// SWS_CRYPT_02417
    /// SecurityErrorDomain::kInvalidInputSize error
    if ((context.size() + pDigestService->GetDigestSize())
        > static_cast< std::size_t >(ESelectLength::kMaxInputBuffLen)) {
        return ara::core::Result< bool >::FromError(SecurityErrorDomain::Errc::kInvalidInputSize);
    }

    /// @error:  SecurityErrorDomain::kInvalidArgument  if the CryptoAlgId of @c hashFn differs from the CryptoAlgId of this context
    CryptoPrimitiveId::AlgId const nHashAlgID{hashFn.GetCryptoPrimitiveId()->GetPrimitiveId()};
    if ((nHashAlgID <= 0U) || (nHashAlgID != nHashAlgID_)) {
        return ara::core::Result< bool >::FromError(SecurityErrorDomain::Errc::kInvalidArgument);
    }

    // Get digest information
    ara::core::Result< ara::core::Vector< ara::core::Byte > > const resultDigest{hashFn.GetDigest()};
    if (false == resultDigest.HasValue()) {
        return ara::core::Result< bool >::FromError(SecurityErrorDomain::Errc::kInvalidArgument);
    }
    ReadOnlyMemRegion const memDigest{
        static_cast< uint8_t const * >(static_cast< void const * >(resultDigest.Value().data())),
        resultDigest.Value().size()};

    /// [SWS_CRYPT_02418] Truncation of hash value
    /// Determine the bit length of memDigest,
    /// ·The bit length of the input hash value is greater than the bit length used for signing/verification
    /// ·And according to the algorithm specified by Algid, using a hash value with the provided bit length (referring to the length to be truncated here) is allowed and truncation is specified
    std::size_t const sigSize{GetSignatureSize()};
    bool bCompare{false};
    if (memDigest.size() > sigSize) {
        ReadOnlyMemRegion const memDigestTruncation{memDigest.first(sigSize)};
        bCompare = _DoVerify(memDigestTruncation, signature, context);
    } else {
        bCompare = _DoVerify(memDigest, signature, context);
    }
    return ara::core::Result< bool >::FromValue(bCompare);
}
//***************/  //PServiceSignature interface
/// @brief Get the actual bit length of the key loaded into the context. Returns 0 if no key is set for the context.
/// @returns  actual length of a key (now set to the algorithm context) in bits
std::size_t PCtxDsv_VerifierPublic::GetActualKeyBitLength() const noexcept
{
    if (false == IsKeyAvailable()) {
        return 0U;
    }
    return pPublicKey_->GetPayloadSize() * kInt_8U;  // Convert bit length
}
/// @brief Get the COUID of the key deployed to the context attached to this extended service. Returns an empty COUID (Nil) if no key is set for the context.
/// @returns the COUID of the CryptoObject
CryptoObjectUid PCtxDsv_VerifierPublic::GetActualKeyCOUID() const noexcept
{
    if (false == IsKeyAvailable()) {
        return CryptoObjectUid{};
    }
    return pPublicKey_->GetObjectId().mCouid;
}
/// @brief Get the allowed usage for this context (based on the properties of the key object loaded into this context).
/// If the context has not yet been initialized with a key object, it must return 0 (all flags reset).
/// @returns  a combination of bit-flags that specifies allowed usages of the context
AllowedUsageFlags PCtxDsv_VerifierPublic::GetAllowedUsage() const noexcept
{
    if (false == IsKeyAvailable()) {
        return kAllowPrototypedOnly;
    }
    return pPublicKey_->GetAllowedUsage();
}
/// @brief Check if a key is set for this context.
/// @returns   true if a key has been set to this context false otherwise
bool PCtxDsv_VerifierPublic::IsKeyAvailable() const noexcept
{
    if (pPublicKey_ == nullptr) {
        return false;
    }
    return true;
}
/// @brief Get the hash size required by the current signature algorithm.
/// @returns required hash size in bytes
std::size_t PCtxDsv_VerifierPublic::GetRequiredHashSize() const noexcept
{
    ara::core::Result< HashFunctionCtx::Uptr > const hashResult{MyProvider().CreateHashFunctionCtx(nHashAlgID_)};
    if (false == hashResult.HasValue()) {
        return 0U;
    }
    PCtxHashFunction *const pHashCtx{
        static_cast< PCtxHashFunction * >(static_cast< void * >(hashResult.Value().get()))};
    return static_cast< std::size_t >(pHashCtx->GetHashLength());
}
//***************/  //PCtxDsv_VerifierPublic interface
/// @brief Signature verification algorithm
/// @param digest Digest data
/// @param signature Signature data
/// @param context Context participating in algorithm operation
/// @returns  Verification data
bool PCtxDsv_VerifierPublic::_DoVerify(ReadOnlyMemRegion const &digest,
                                       ReadOnlyMemRegion const &signature,
                                       ReadOnlyMemRegion const &context) const noexcept
{
    ara::core::String const strDigest(T_String(static_cast< void const * >(digest.data()), digest.size()));
    ara::core::String const strContext(T_String(static_cast< void const * >(context.data()), context.size()));
    ara::core::String strTotal{strDigest};
    strTotal += strContext;

    return DoDecrypto(signature.data(), static_cast< uint32_t >(signature.size()), false, T_TransBytes(strTotal.data()),
                      static_cast< uint32_t >(strTotal.size()));
}
//********************************/
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara
