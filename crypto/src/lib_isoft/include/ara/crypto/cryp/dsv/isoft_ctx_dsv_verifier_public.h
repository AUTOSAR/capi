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
/// @file       isoft_ctx_dsv_verifier_public.h
/// @brief      AutoSar-Crypto encryption/decryption module
/// @details    Signature verification public key context interface.
/// @date       2022-03-23
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr><td>2022-03-23  <tr>1.0.0    <tr>hanjingjing      <td>Create initial version</td>
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default encryption/Signature storage
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_01009
/// @unit_name=PCtxDsv_VerifierPublic
/// @unit_description=Signature verification public key context interface base class
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_PUHUA_CTX_DSV_VERIFIER_PUBLIC_H_
#define ARA_CRYPTO_CRYP_PUHUA_CTX_DSV_VERIFIER_PUBLIC_H_

// DSV == Digital Signatures and Verifier

#include "ara/crypto/cryp/cryobj/isoft_crypto_primitive_id.h"
#include "ara/crypto/cryp/isoft_auto_buff.h"
#include "ara/crypto/cryp/verifier_public_ctx.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
/// @brief Crypto provider
class PCryptoProvider;
//********************************/
/// @brief Signature verification public key context interface.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00599
/// @trace_id_dd=DD_CRYPTO_02195
/// @needwork = ad
/// @endcode
class PCtxDsv_VerifierPublic : public VerifierPublicCtx
{
private:
    /// @brief Crypto provider object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02196
    /// @needwork = dda
    /// @endcode
    PCryptoProvider& cryptoProvider_;
    /// @brief Public key pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02197
    /// @needwork = dda
    /// @endcode
    PublicKey const* pPublicKey_;
    /// @brief Currently using default, user must set manually.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02198
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::AlgId nHashAlgID_;
    /// @brief List of signatures required by this context
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00600
    /// @trace_id_dd=DD_CRYPTO_02199
    /// @needwork = ad
    /// @endcode
    enum class ESignatureRequired : std::uint64_t
    {
        kCtxDsv_Signature = 40101U,
    };
    /// @brief Parameterized constructor
    /// @param cryptoProvider Crypto provider
    /// @param pPublicKey Public key
    /// @param nHashAlgID Hash algorithm crypto primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02200
    /// @needwork = dda
    /// @endcode
    PCtxDsv_VerifierPublic(PCryptoProvider& cryptoProvider,
                           PublicKey const* const pPublicKey,
                           CryptoPrimitiveId::AlgId const nHashAlgID) noexcept;

public:
    /// @brief Parameterized constructor
    /// @param cryptoProvider PCryptoProvider
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00601
    /// @trace_id_dd=DD_CRYPTO_02201
    /// @needwork = ad
    /// @endcode
    explicit PCtxDsv_VerifierPublic(PCryptoProvider& cryptoProvider) noexcept;
    /// @brief default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00602
    /// @trace_id_dd=DD_CRYPTO_02202
    /// @needwork = ad
    /// @endcode
    PCtxDsv_VerifierPublic() = delete;
    /// @brief default virtual destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00603
    /// @trace_id_dd=DD_CRYPTO_02203
    /// @needwork = ad
    /// @endcode
    ~PCtxDsv_VerifierPublic() noexcept override = default;
    /// @brief default move constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00604
    /// @trace_id_dd=DD_CRYPTO_02204
    /// @needwork = ad
    /// @endcode
    PCtxDsv_VerifierPublic(PCtxDsv_VerifierPublic&& other) = delete;
    /// @brief default move assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00605
    /// @trace_id_dd=DD_CRYPTO_02205
    /// @needwork = ad
    /// @endcode
    PCtxDsv_VerifierPublic& operator=(PCtxDsv_VerifierPublic&& other) = delete;
    /// @brief default copy assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00606
    /// @trace_id_dd=DD_CRYPTO_02206
    /// @needwork = ad
    /// @endcode
    PCtxDsv_VerifierPublic& operator=(PCtxDsv_VerifierPublic const& other) = delete;
    /// @brief default copy constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00607
    /// @trace_id_dd=DD_CRYPTO_02207
    /// @needwork = ad
    /// @endcode
    PCtxDsv_VerifierPublic(PCtxDsv_VerifierPublic const& other) = delete;

public:  // CryptoContext interface
    /// @brief Returns the CryptoPrimitivId instance containing the instance identifier.
    /// @name   GetCryptoPrimitiveId
    /// @returns CryptoPrimitivId instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00608
    /// @trace_id_dd=DD_CRYPTO_02208
    /// @needwork = ad
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override = 0;
    /// @brief Check whether the encryption context is initialized and usable. It checks all required values, including: key value, IV/seed, etc.
    /// @name   IsInitialized
    /// @returns @c true if the crypto context is completely initialized and ready to use, and @c false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00609
    /// @trace_id_dd=DD_CRYPTO_02209
    /// @needwork = ad
    /// @endcode
    bool IsInitialized() const noexcept override;
    /// @brief Get a reference to the Crypto Provider for this context.
    /// @name   MyProvider
    /// @returns a reference to Crypto Provider instance that provides this context
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00610
    /// @trace_id_dd=DD_CRYPTO_02210
    /// @needwork = ad
    /// @endcode
    CryptoProvider& MyProvider() const noexcept override;

public:  // VerifierPublicCtx interface
    /// @brief Extended service member class.
    /// @name   GetSignatureService
    /// @returns SignatureService instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00611
    /// @trace_id_dd=DD_CRYPTO_02211
    /// @needwork = ad
    /// @endcode
    SignatureService::Uptr GetSignatureService() const noexcept override = 0;
    /// @brief Clear the encryption context.
    /// @name   Reset
    /// @returns has value if reset sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00612
    /// @trace_id_dd=DD_CRYPTO_02212
    /// @needwork = ad
    /// @endcode
    ara::core::Result< void > Reset() noexcept override;
    /// @brief Set (deploy) a key to the verifier public algorithm context.
    /// @brief Set (deploy) a key to the verifier public algorithm context.
    /// @name  SetKey
    /// @param key  the source key object
    /// @returns has value if SetKey sucess false otherwise
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_24115}
    /// @uptrace={RS_CRYPTO_02001}
    /// @uptrace={RS_CRYPTO_02003}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @error: SecurityErrorDomain::kIncompatibleObject    if the provided key object is incompatible with this
    ///     symmetric key context
    /// @error: SecurityErrorDomain::kUsageViolation        if the transformation type associated with this context is
    ///     prohibited by the "allowed usage" restrictions of provided key object
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00613
    /// @trace_id_dd=DD_CRYPTO_02213
    /// @needwork = ad
    /// @endcode
    ara::core::Result< void > SetKey(PublicKey const& key) noexcept override;
    /// @brief Verify the signature through the digest value stored in the hash function context.
    ///         This is a pass-through interface to SWS_CRYPT_24113 for developer convenience, meaning it adds additional input checks and then calls the verify() interface from SWS_CRYPT_24113.
    /// @brief Verify signature by a digest value stored in the hash-function context.
    ///         This is a pass-through interface to SWS_CRYPT_24113 for developer convenience,
    ///         i.e. it adds additional input checks amd then calls the verify() interface from SWS_CRYPT_24113.
    /// @param hashFn  hash function to be used for hashing
    /// @param signature  the signature object for verification
    /// @param context  an optional user supplied "context" (its support depends from concrete algorithm)
    /// @returns @c true if the signature was verified successfully and @c false otherwise
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_24111}
    /// @uptrace={RS_CRYPTO_02204}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @error: SecurityErrorDomain::kProcessingNotFinished  if the method @c hashFn.Finish() was not called before this
    ///     method call
    /// @error: SecurityErrorDomain::kInvalidArgument  if the CryptoAlgId of @c hashFn differs from the CryptoAlgId of
    ///     this context
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00614
    /// @trace_id_dd=DD_CRYPTO_02214
    /// @needwork = ad
    /// @endcode
    ara::core::Result< bool > VerifyPrehashed(HashFunctionCtx const& hashFn,
                                              Signature const& signature,
                                              ReadOnlyMemRegion const& context
                                              = ReadOnlyMemRegion()) const noexcept override;
    /// @brief Verify the signature BLOB using the directly provided hash value or message value.
    ///         This method can be used to implement "multi-pass" signature algorithms that process messages directly, i.e., without requiring "pre-hashing" (e.g., Ed25519ctx).
    ///         This method is also suitable for implementing traditional pre-hashed signature schemes (e.g., Ed25519ph, Ed448ph, ECDSA).
    ///         If the target algorithm does not support the context parameter, an empty value (default) must be provided!
    ///         The user-provided context can be used for the following algorithms: Ed25519ctx, Ed25519ph, Ed448ph.
    /// @brief Verify signature BLOB by a directly provided hash or message value.
    ///         This method can be used for implementation of the "multiple passes" signature algorithms that process a
    ///         message directly, i.e. without "pre-hashing" (like Ed25519ctx). But also this method is suitable for
    ///         implementation of the traditional signature schemes with pre-hashing (like Ed25519ph, Ed448ph, ECDSA).
    ///         If the target algorithm doesn't support the @c context argument then the empty (default) value must be
    ///         supplied! The user supplied @c context may be used for such algorithms as: Ed25519ctx, Ed25519ph,
    ///         Ed448ph.
    /// @param value  the (pre-)hashed or direct message value that should be verified
    /// @param signature  the signature BLOB for the verification
    /// @param context  an optional user supplied "context" (its support depends from concrete algorithm)
    /// @returns @c true if the signature was verified successfully and @c false otherwise
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_24112}
    /// @uptrace={RS_CRYPTO_02204}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @error: SecurityErrorDomain::kUninitializedContext  if the context was not initialized by a key value
    /// @error: SecurityErrorDomain::kInvalidInputSize      if the @c context argument has unsupported size
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00615
    /// @trace_id_dd=DD_CRYPTO_02215
    /// @needwork = ad
    /// @endcode
    ara::core::Result< bool > Verify(ReadOnlyMemRegion const& value,
                                     ReadOnlyMemRegion const& signature,
                                     ReadOnlyMemRegion const& context = ReadOnlyMemRegion()) const noexcept override;
    /// @brief Verify the signature through the digest value stored in the hash function context.
    ///         This is a pass-through interface to SWS_CRYPT_24112 for developer convenience, meaning it adds additional input checks and then calls the default verify() interface.
    /// @brief Verify signature by a digest value stored in the hash-function context. This is a pass-through
    ///        interface to SWS_CRYPT_24112 for developer convenience, i.e. it adds additional input checks
    ///        amd then calls the default verify() interface.
    /// @param hashAlgId  hash function algorithm ID
    /// @param hashValue  hash function value (resulting digest without any truncations)
    /// @param signature  the signature object for the verification
    /// @param context    an optional user supplied "context" (its support depends from concrete algorithm)
    /// @returns @c true if the signature was verified successfully and @c false otherwise
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_24113}
    /// @uptrace={RS_CRYPTO_02204}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @error: SecurityErrorDomain::kIncompatibleObject    if the CryptoAlgId of this context does not match the
    /// CryptoAlgId of @c signature; or the required CryptoAlgId of the hash is not kAlgIdDefault and the required hash
    /// CryptoAlgId of this context does not match @c hashAlgId or the hash CryptoAlgId of @c signature
    /// @error: SecurityErrorDomain::kIncompatibleArguments if the provided hashAlgId is not kAlgIdDefault and the AlgId
    ///     of the provided signature object does not match the provided hashAlgId
    /// @error: SecurityErrorDomain::kBadObjectReference    if the provided signature object does not reference the
    ///     public key loaded to the context, i.e. if the COUID of the public key in the context is not equal to the
    ///     COUID referenced from the signature object.
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00616
    /// @trace_id_dd=DD_CRYPTO_02216
    /// @needwork = ad
    /// @endcode
    ara::core::Result< bool > VerifyPrehashed(CryptoAlgId hashAlgId,
                                              ReadOnlyMemRegion const& hashValue,
                                              Signature const& signature,
                                              ReadOnlyMemRegion const& context
                                              = ReadOnlyMemRegion()) const noexcept override;
    /// @brief Verify the signature through the digest value stored in the hash function context.
    ///         This is a pass-through interface to SWS_CRYPT_24112 for developer convenience, meaning it adds additional input checks and then calls the default verify() interface.
    /// @brief Verify signature by a digest value stored in the hash-function context. This is a pass-through
    ///        interface to SWS_CRYPT_24112 for developer convenience, i.e. it adds additional input checks
    ///        amd then calls the default verify() interface.
    /// @param hashFn  hash function to be used for hashing
    /// @param signature  the data BLOB to be verified
    /// @param context  an optional user supplied "context" (its support depends from concrete algorithm)
    /// @returns @c true if the signature was verified successfully and @c false otherwise
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_24114}
    /// @uptrace={RS_CRYPTO_02204}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @error: SecurityErrorDomain::kProcessingNotFinished     if the method @c hashFn.Finish() was not called before
    ///     this method call
    /// @error: SecurityErrorDomain::kInvalidArgument           if the CryptoAlgId of @c hashFn differs from the
    ///     CryptoAlgId of this context
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00617
    /// @trace_id_dd=DD_CRYPTO_02217
    /// @needwork = ad
    /// @endcode
    ara::core::Result< bool > VerifyPrehashed(HashFunctionCtx const& hashFn,
                                              ReadOnlyMemRegion const& signature,
                                              ReadOnlyMemRegion const& context
                                              = ReadOnlyMemRegion()) const noexcept override;

public:  // PServiceSignature interface
    /// @brief Get the actual bit length of the key loaded into the context. If no key is set for the context, returns 0.
    /// @brief Get actual bit-length of a key loaded to the context. If no key was set to the context yet then 0 is
    /// returned.
    /// @name GetActualKeyBitLength
    /// @returns actual length of a key (now set to the algorithm context) in bits
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_29045}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00618
    /// @trace_id_dd=DD_CRYPTO_02218
    /// @needwork = ad
    /// @endcode
    virtual std::size_t GetActualKeyBitLength() const noexcept;
    /// @brief Get the COUID of the key deployed to the context attached to this extended service. If no key is set for the context, returns an empty COUID (Nil).
    /// @brief Get the COUID of the key deployed to the context this extension service is attached to.
    /// If no key was set to the context yet then an empty COUID (Nil) is returned.
    /// @name GetActualKeyCOUID
    /// @returns the COUID of the CryptoObject
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_29047}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00619
    /// @trace_id_dd=DD_CRYPTO_02219
    /// @needwork = ad
    /// @endcode
    virtual CryptoObjectUid GetActualKeyCOUID() const noexcept;
    /// @brief Get the allowed usage of this context (according to the key object properties loaded into this context).
    /// If the context has not been initialized with a key object, must return 0 (all flags reset).
    /// @brief Get allowed usages of this context (according to the key object attributes loaded to this context).
    /// If the context is not initialized by a key object yet then zero (all flags are reset) must be returned.
    /// @name GetAllowedUsage
    /// @returns a combination of bit-flags that specifies allowed usages of the context
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_29046}
    /// @uptrace={RS_CRYPTO_02008}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00620
    /// @trace_id_dd=DD_CRYPTO_02220
    /// @needwork = ad
    /// @endcode
    virtual AllowedUsageFlags GetAllowedUsage() const noexcept;
    /// @brief Get the maximum supported key length (in bits).
    /// @brief Get maximal supported key length in bits.
    /// @name GetMaxKeyBitLength
    /// @returns maximal supported length of the key in bits
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_29044}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00621
    /// @trace_id_dd=DD_CRYPTO_02221
    /// @needwork = ad
    /// @endcode
    virtual std::size_t GetMaxKeyBitLength() const noexcept = 0;
    /// @brief Get the minimum supported key length (in bits).
    /// @brief Get minimal supported key length in bits.
    /// @name  GetMinKeyBitLength
    /// @returns minimal supported length of the key in bits
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_29043}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00622
    /// @trace_id_dd=DD_CRYPTO_02222
    /// @needwork = ad
    /// @endcode
    virtual std::size_t GetMinKeyBitLength() const noexcept = 0;
    /// @brief Validate support for a specific key length according to the context.
    /// @brief Verify supportness of specific key length by the context.
    /// @name  IsKeyBitLengthSupported
    /// @param keyBitLength Key length in bits
    /// @returns @c true if provided value of the key length is supported by the context
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_29048}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00623
    /// @trace_id_dd=DD_CRYPTO_02223
    /// @needwork = ad
    /// @endcode
    virtual bool IsKeyBitLengthSupported(std::size_t keyBitLength) const noexcept = 0;
    /// @brief Check whether a key is set for this context.
    /// @brief Check if a key has been set to this context.
    /// @name IsKeyAvailable
    /// @returns true if a key has been set to this context false otherwise
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_29049}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00624
    /// @trace_id_dd=DD_CRYPTO_02224
    /// @needwork = ad
    /// @endcode
    virtual bool IsKeyAvailable() const noexcept;
    /// @brief Get the ID of the hash algorithm required by the current signature algorithm.
    /// @brief Get an ID of hash algorithm required by current signature algorithm.
    /// @name GetRequiredHashAlgId
    /// @returns required hash algorithm ID or @c kAlgIdAny if the signature algorithm specification does not include a
    ///     concrete hash function
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_29003}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00625
    /// @trace_id_dd=DD_CRYPTO_02225
    /// @needwork = ad
    /// @endcode
    inline virtual CryptoPrimitiveId::AlgId GetRequiredHashAlgId() const noexcept { return nHashAlgID_; }
    /// @brief Get the hash size required by the current signature algorithm.
    /// @brief Get the hash size required by current signature algorithm.
    /// @name GetRequiredHashSize
    /// @returns required hash size in bytes
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_29002}
    /// @uptrace={RS_CRYPTO_02309}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00626
    /// @trace_id_dd=DD_CRYPTO_02226
    /// @needwork = ad
    /// @endcode
    virtual std::size_t GetRequiredHashSize() const noexcept;
    /// @brief Get the size of the signature value generated and required by the current algorithm.
    /// @brief Get size of the signature value produced and required by the current algorithm.
    /// @name GetSignatureSize
    /// @returns size of the signature value in bytes
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_29004}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00627
    /// @trace_id_dd=DD_CRYPTO_02227
    /// @needwork = ad
    /// @endcode
    virtual std::size_t GetSignatureSize() const noexcept = 0;

protected:
    /// @brief Check whether the key meets requirements.
    /// @name   CheckKey
    /// @param key Public key
    /// @returns true if check key sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02228
    /// @needwork = dda
    /// @endcode
    virtual bool CheckKey(PublicKey const& key) const noexcept = 0;
    /// @brief Perform signature encryption logic.
    /// @name   DoDecrypto
    /// @param pInputData Starting address of input data for algorithm operation
    /// @param nDataLen Data length
    /// @param suppressPadding Whether to suppress padding
    /// @param pDigestData Digest data
    /// @param nDigestDataLen Digest data length
    /// @returns Decrypted data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02229
    /// @needwork = dda
    /// @endcode
    virtual bool DoDecrypto(uint8_t const* pInputData,
                            uint32_t nDataLen,
                            bool suppressPadding,
                            uint8_t const* const pDigestData,
                            uint32_t nDigestDataLen) const noexcept = 0;
    /// @brief Signature verification algorithm.
    /// @name   _DoVerify
    /// @param digest Digest data
    /// @param signature Signature data
    /// @param context Context for algorithm operation
    /// @returns Signature verification data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02230
    /// @needwork = dda
    /// @endcode
    bool _DoVerify(ReadOnlyMemRegion const& digest,
                   ReadOnlyMemRegion const& signature,
                   ReadOnlyMemRegion const& context) const noexcept;
    /// @brief Set the context hash ID.
    /// @name   _SetContextAglId
    /// @param nHashAlgID Hash algorithm crypto primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02231
    /// @needwork = dda
    /// @endcode
    inline void _SetContextAglId(CryptoPrimitiveId::AlgId const nHashAlgID) noexcept { nHashAlgID_ = nHashAlgID; }

public:
    /// @brief Check equality.
    /// @name   IsCryptoAlgIdMacthing
    /// @param nSigCryId Crypto primitive ID of the encryption algorithm used in the signature algorithm
    /// @returns  true if Macthi sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00628
    /// @trace_id_dd=DD_CRYPTO_02232
    /// @needwork = ad
    /// @endcode
    virtual bool IsCryptoAlgIdMacthing(CryptoPrimitiveId::AlgId const nSigCryId) const noexcept = 0;
    /// @brief Get the public key.
    /// @name   GetPublicKey
    /// @returns Public key instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00629
    /// @trace_id_dd=DD_CRYPTO_02233
    /// @needwork = ad
    /// @endcode
    inline virtual PublicKey const* GetPublicKey() const noexcept { return pPublicKey_; }
};
//********************************/
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_PUHUA_CTX_DSV_VERIFIER_PUBLIC_H_
