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
/// @file       isoft_ctx_dsv_signer_private.h
/// @brief      AutoSar-Crypto encryption and decryption module
/// @details    Signature private key context interface.
/// @date       2022-03-23
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default Encryption and Decryption/Signature Storage
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_01009
/// @unit_name=PCtxDsv_SignerPrivate
/// @unit_description=Base class for signature private key context interface
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_PUHUA_CTX_DSV_SIGNER_PRIVATE_H_
#define ARA_CRYPTO_CRYP_PUHUA_CTX_DSV_SIGNER_PRIVATE_H_

// DSV == Digital Signatures and Verifier

#include "ara/crypto/cryp/cryobj/isoft_crypto_primitive_id.h"
#include "ara/crypto/cryp/isoft_auto_buff.h"
#include "ara/crypto/cryp/signer_private_ctx.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
/// @brief Encryption and decryption provider
class PCryptoProvider;
//********************************/
/// @brief Signature private key context interface.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00563
/// @trace_id_dd=DD_CRYPTO_02125
/// @needwork = ad
/// @endcode
class PCtxDsv_SignerPrivate : public SignerPrivateCtx
{
private:
    /// @brief Encryption and decryption provider object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02126
    /// @needwork = dda
    /// @endcode
    PCryptoProvider &cryptoProvider_;
    /// @brief Private key pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02127
    /// @needwork = dda
    /// @endcode
    PrivateKey const *pPrivateKey_;
    /// @brief Hash algorithm cryptographic primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02128
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::AlgId nHashAlgID_;

public:
    /// @brief Unique smart pointer of the interface.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00563
    /// @trace_id_dd=DD_CRYPTO_06309
    /// @needwork = dd
    /// @endcode
    using Uptr = std::unique_ptr< PCtxDsv_SignerPrivate >;

public:
    /// @brief Parameterized constructor
    /// @param cryptoProvider PCryptoProvider
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00564
    /// @trace_id_dd=DD_CRYPTO_02129
    /// @needwork = ad
    /// @endcode
    explicit PCtxDsv_SignerPrivate(PCryptoProvider &cryptoProvider) noexcept;
    /// @brief Parameterized constructor
    /// @param cryptoProvider encryption provider
    /// @param pPrivateKey pointer to private key object
    /// @param nHashAlgID hash algorithm cryptographic primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00565
    /// @trace_id_dd=DD_CRYPTO_02130
    /// @needwork = ad
    /// @endcode
    PCtxDsv_SignerPrivate(PCryptoProvider &cryptoProvider,
                          PrivateKey const *const pPrivateKey,
                          CryptoPrimitiveId::AlgId const nHashAlgID) noexcept;
    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00566
    /// @trace_id_dd=DD_CRYPTO_02131
    /// @needwork = ad
    /// @endcode
    PCtxDsv_SignerPrivate() = delete;
    /// @brief Default virtual destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00567
    /// @trace_id_dd=DD_CRYPTO_02132
    /// @needwork = ad
    /// @endcode
    ~PCtxDsv_SignerPrivate() noexcept override = default;
    /// @brief Default move constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00568
    /// @trace_id_dd=DD_CRYPTO_02133
    /// @needwork = ad
    /// @endcode
    PCtxDsv_SignerPrivate(PCtxDsv_SignerPrivate &&other) = delete;
    /// @brief Default move assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00569
    /// @trace_id_dd=DD_CRYPTO_02134
    /// @needwork = ad
    /// @endcode
    PCtxDsv_SignerPrivate &operator=(PCtxDsv_SignerPrivate &&other) = delete;
    /// @brief Default copy assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00570
    /// @trace_id_dd=DD_CRYPTO_02135
    /// @needwork = ad
    /// @endcode
    PCtxDsv_SignerPrivate &operator=(PCtxDsv_SignerPrivate const &other) = delete;
    /// @brief Default copy constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00571
    /// @trace_id_dd=DD_CRYPTO_02136
    /// @needwork = ad
    /// @endcode
    PCtxDsv_SignerPrivate(PCtxDsv_SignerPrivate const &other) = delete;

public:  // CryptoContext interface
    /// @brief Return the CryptoPrimitivId instance containing the instance identifier.
    /// @return CryptoPrimitivId instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00572
    /// @trace_id_dd=DD_CRYPTO_02137
    /// @needwork = ad
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override;
    /// @brief Check whether the encryption context has been initialized and is usable. It checks all required values, including: key value, IV/seed, etc.
    /// @brief Check if the crypto context is already initialized and ready to use.
    ///           It checks all required values, including: key value, IV/seed, etc.
    /// @returns @c true if the crypto context is completely initialized and ready to use, and @c false otherwise
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_20412}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00573
    /// @trace_id_dd=DD_CRYPTO_02138
    /// @needwork = ad
    /// @endcode
    bool IsInitialized() const noexcept override;
    /// @brief Get a reference to the Crypto Provider for this context.
    /// @brief Get a reference to Crypto Provider of this context.
    /// @returns a reference to Crypto Provider instance that provides this context
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_20654}
    /// @uptrace={RS_CRYPTO_02401}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00574
    /// @trace_id_dd=DD_CRYPTO_02139
    /// @needwork = ad
    /// @endcode
    CryptoProvider &MyProvider() const noexcept override;

public:  // SignerPrivateCtx interface
    /// @brief Get the SignatureService instance.
    /// @return SignatureService instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00575
    /// @trace_id_dd=DD_CRYPTO_02140
    /// @needwork = ad
    /// @endcode
    SignatureService::Uptr GetSignatureService() const noexcept override = 0;
    /// @brief Clear the encryption context.
    /// @return has value if reset sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00576
    /// @trace_id_dd=DD_CRYPTO_02141
    /// @needwork = ad
    /// @endcode
    ara::core::Result< void > Reset() noexcept override;
    /// @brief Set (deploy) a key for the signer private algorithm context.
    /// @brief Set (deploy) a key to the signer private algorithm context.
    /// @param key  the source key object
    /// @return has value if SetKey sucess false otherwise
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_23515}
    /// @uptrace={RS_CRYPTO_02001}
    /// @uptrace={RS_CRYPTO_02003}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @error: SecurityErrorDomain::kIncompatibleObject     if the provided key object is incompatible with this
    ///     symmetric key context
    /// @error: SecurityErrorDomain::kUsageViolation         if the transformation type associated with this context is
    ///     prohibited by    /// the "allowed usage" restrictions of provided key object
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00577
    /// @trace_id_dd=DD_CRYPTO_02142
    /// @needwork = ad
    /// @endcode
    ara::core::Result< void > SetKey(PrivateKey const &key) noexcept override;
    /// @brief Sign the provided digest value stored in the hash function context.
    /// @brief Sign a provided digest value stored in the hash-function context.
    ///       This method must put the hash-function algorithm ID and a @a COUID of the used key-pair to the resulting
    ///       signature object! The user supplied @c context may be used for such algorithms as: Ed25519ctx, Ed25519ph,
    ///       Ed448ph. If the target algorithm doesn't support the @c context argument then the empty (default) value
    ///       must be supplied!
    /// @param hashFn  a finalized hash-function context that contains a digest value ready for sign
    /// @param context  an optional user supplied "context" (its support depends from concrete algorithm)
    /// @returns unique smart pointer to serialized signature
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_23511}
    /// @uptrace={RS_CRYPTO_02204}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @error: SecurityErrorDomain::kInvalidArgument        if hash-function algorithm does not comply with the
    ///     signature algorithm specification of this context
    /// @error: SecurityErrorDomain::kInvalidInputSize       if the user supplied @c context has incorrect (or
    ///     unsupported) size
    /// @error: SecurityErrorDomain::kProcessingNotFinished  if the method @c hash.Finish() was not called before the
    ///     call of this method
    /// @error: SecurityErrorDomain::kUninitializedContext   this context was not initialized by a key value
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00578
    /// @trace_id_dd=DD_CRYPTO_02143
    /// @needwork = ad
    /// @endcode
    ara::core::Result< Signature::Uptrc > SignPreHashed(HashFunctionCtx const &hashFn,
                                                        ReadOnlyMemRegion const &context
                                                        = ReadOnlyMemRegion()) const noexcept override;
    /// @brief Sign a directly provided hash value or message value.
    /// This method can be used to implement "multi-pass" signature algorithms that process messages directly (i.e., without "pre-hashing") (e.g., Ed25519ctx).
    /// This method is also suitable for implementation of traditional pre-hashed signature schemes (e.g., Ed25519ph, Ed448ph, ECDSA). If the target algorithm does not support the context parameter, an empty value (default) must be provided!
    /// @brief Sign a directly provided hash or message value.
    ///     This method can be used for implementation of the "multiple passes" signature algorithms that process a
    ///     message directly, i.e. without "pre-hashing" (like Ed25519ctx). But also this method is suitable for
    ///     implementation of the traditional signature schemes with pre-hashing (like Ed25519ph, Ed448ph, ECDSA).
    ///     If the target algorithm doesn’t support the @c context argument then the empty (default) value must be
    ///     supplied!
    /// @param value  the (pre-)hashed or direct message value that should be signed
    /// @param context  an optional user supplied "context" (its support depends from concrete algorithm)
    /// @returns actual size of the signature value stored to the output buffer
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_23512}
    /// @uptrace={RS_CRYPTO_02204}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @error: SecurityErrorDomain::kInvalidInputSize  if size of the input @c value or @c context arguments are
    ///     incorrect / unsupported
    /// @error: SecurityErrorDomain::kUninitializedContext  if the context was not initialized by a key value
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00579
    /// @trace_id_dd=DD_CRYPTO_02144
    /// @needwork = ad
    /// @endcode
    ara::core::Result< ara::core::Vector< ara::core::Byte > > Sign(ReadOnlyMemRegion const &value,
                                                                   ReadOnlyMemRegion const &context
                                                                   = ReadOnlyMemRegion()) const noexcept override;
    /// @brief Use base class template member function
    using SignerPrivateCtx::Sign;
    /// @brief Sign a directly provided digest value and create a signature object.
    /// @brief Sign a directly provided digest value and create the @c Signature object.
    ///       This method must put the hash-function algorithm ID and a @a COUID of the used key-pair to the resulting
    ///       signature object! The user supplied @c context may be used for such algorithms as: Ed25519ctx, Ed25519ph,
    ///       Ed448ph. If the target algorithm doesn't support the @c context argument then the empty (default) value
    ///       must be supplied!
    /// @param hashAlgId  hash function algorithm ID
    /// @param hashValue  hash function value (resulting digest without any truncations)
    /// @param context  an optional user supplied "context" (its support depends from concrete algorithm)
    /// @returns unique smart pointer to serialized signature
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_23513}
    /// @uptrace={RS_CRYPTO_02204}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @error: SecurityErrorDomain::kInvalidArgument        if hash-function algorithm does not comply with the
    ///     signature algorithm specification of this context
    /// @error: SecurityErrorDomain::kInvalidInputSize       if the user supplied @c context has incorrect (or
    ///     unsupported) size
    /// @error: SecurityErrorDomain::kUninitializedContext   this context was not initialized by a key value
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00580
    /// @trace_id_dd=DD_CRYPTO_02145
    /// @needwork = ad
    /// @endcode
    ara::core::Result< Signature::Uptrc > SignPreHashed(AlgId hashAlgId,
                                                        ReadOnlyMemRegion const &hashValue,
                                                        ReadOnlyMemRegion const &context
                                                        = ReadOnlyMemRegion()) const noexcept override;

public:  // PServiceSignature interface
    /// @brief Get the actual bit length of the key loaded into the context. If no key is set for the context, return 0.
    /// @brief Get actual bit-length of a key loaded to the context. If no key was set to the context yet then 0 is
    ///     returned.
    /// @returns actual length of a key (now set to the algorithm context) in bits
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_29045}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00581
    /// @trace_id_dd=DD_CRYPTO_02146
    /// @needwork = ad
    /// @endcode
    virtual std::size_t GetActualKeyBitLength() const noexcept;
    /// @brief Get the COUID of the key deployed to the context attached to this extended service. If no key is set for the context, return an empty COUID (Nil).
    /// @brief Get the COUID of the key deployed to the context this extension service is attached to.
    ///     If no key was set to the context yet then an empty COUID (Nil) is returned.
    /// @returns the COUID of the CryptoObject
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_29047}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00582
    /// @trace_id_dd=DD_CRYPTO_02147
    /// @needwork = ad
    /// @endcode
    virtual CryptoObjectUid GetActualKeyCOUID() const noexcept;
    /// @brief Get the allowed usage of this context (according to the key object attributes loaded into this context).
    /// If the context has not been initialized with a key object, 0 must be returned (all flags reset).
    /// @brief Get allowed usages of this context (according to the key object attributes loaded to this context).
    ///     If the context is not initialized by a key object yet then zero (all flags are reset) must be returned.
    /// @returns a combination of bit-flags that specifies allowed usages of the context
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_29046}
    /// @uptrace={RS_CRYPTO_02008}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00583
    /// @trace_id_dd=DD_CRYPTO_02148
    /// @needwork = ad
    /// @endcode
    virtual AllowedUsageFlags GetAllowedUsage() const noexcept;
    /// @brief Get the maximum supported key length (in bits).
    /// @brief Get maximal supported key length in bits.
    /// @returns maximal supported length of the key in bits
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_29044}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00584
    /// @trace_id_dd=DD_CRYPTO_02149
    /// @needwork = ad
    /// @endcode
    virtual std::size_t GetMaxKeyBitLength() const noexcept = 0;
    /// @brief Get the minimum supported key length (in bits).
    /// @brief Get minimal supported key length in bits.
    /// @returns minimal supported length of the key in bits
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_29043}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00585
    /// @trace_id_dd=DD_CRYPTO_02150
    /// @needwork = ad
    /// @endcode
    virtual std::size_t GetMinKeyBitLength() const noexcept = 0;
    /// @brief Verify support for a specific key length according to the context.
    /// @brief Verify supportness of specific key length by the context.
    /// @param keyBitLength key length in bits
    /// @returns @c true if provided value of the key length is supported by the context
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_29048}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00586
    /// @trace_id_dd=DD_CRYPTO_02151
    /// @needwork = ad
    /// @endcode
    virtual bool IsKeyBitLengthSupported(std::size_t keyBitLength) const noexcept = 0;
    /// @brief Check if a key is set for this context.
    /// @brief Check if a key has been set to this context.
    /// @return true if a key has been set to this context false otherwise
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_29049}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00587
    /// @trace_id_dd=DD_CRYPTO_02152
    /// @needwork = ad
    /// @endcode
    virtual bool IsKeyAvailable() const noexcept;
    /// @brief Get the ID of the hash algorithm required by the current signature algorithm.
    /// @brief Get an ID of hash algorithm required by current signature algorithm.
    /// @returns required hash algorithm ID or @c kAlgIdAny if the signature algorithm specification does not include a
    ///     concrete hash function
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_29003}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00588
    /// @trace_id_dd=DD_CRYPTO_02153
    /// @needwork = ad
    /// @endcode
    inline virtual CryptoPrimitiveId::AlgId GetRequiredHashAlgId() const noexcept { return nHashAlgID_; }
    /// @brief Get the hash size required by the current signature algorithm.
    /// @brief Get the hash size required by current signature algorithm.
    /// @returns required hash size in bytes
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_29002}
    /// @uptrace={RS_CRYPTO_02309}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00589
    /// @trace_id_dd=DD_CRYPTO_02154
    /// @needwork = ad
    /// @endcode
    virtual std::size_t GetRequiredHashSize() const noexcept;
    /// @brief Get the size of the signature value produced and required by the current algorithm.
    /// @brief Get size of the signature value produced and required by the current algorithm.
    /// @returns size of the signature value in bytes
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_29004}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00590
    /// @trace_id_dd=DD_CRYPTO_02155
    /// @needwork = ad
    /// @endcode
    virtual std::size_t GetSignatureSize() const noexcept = 0;

protected:
    /// @brief Perform signature encryption logic
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02156
    /// @needwork = dda
    /// @endcode
    virtual ara::core::Result< ara::core::Vector< ara::core::Byte > >
    /// @param pInputData starting address of input data for algorithm operation
    /// @param nDataLen data length
    /// @param suppressPadding whether to suppress padding
    /// @return
    DoEncrypto(uint8_t const *pInputData, uint32_t nDataLen, bool suppressPadding) const noexcept = 0;
    /// @brief Perform signature algorithm logic
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02157
    /// @needwork = dda
    /// @endcode
    ara::core::Result< ara::core::Vector< ara::core::Byte > >
    /// @param digest digest data
    /// @param context context for algorithm operation
    /// @return
    _DoSign(ReadOnlyMemRegion const &digest, ReadOnlyMemRegion const &context) const noexcept;
    /// @brief Get the hash result length corresponding to the hash ID
    size_t _GetHashResultLen() const noexcept
    {
        EPhCtxTypeID hashId = static_cast< EPhCtxTypeID >(nHashAlgID_);
        size_t len{0U};
        switch (hashId) {
            case EPhCtxTypeID::kHashMd5:
                len = kInt_16U;
                break;
            case EPhCtxTypeID::kHashSha1:
                len = kInt_20U;
                break;
            case EPhCtxTypeID::kHashSha2_224:
                len = kInt_28U;
                break;
            case EPhCtxTypeID::kHashSha2_256:
                len = kInt_32U;
                break;
            case EPhCtxTypeID::kHashSha2_384:
                len = kInt_48U;
                break;
            case EPhCtxTypeID::kHashSha2_512:
                len = kInt_64U;
                break;

            default:
                break;
        }
        return len;
    }

public:
    /// @brief Check whether the key meets the requirements
    /// @param key private key
    /// @return true if check key sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00591
    /// @trace_id_dd=DD_CRYPTO_02158
    /// @needwork = ad
    /// @endcode
    virtual bool CheckKey(PrivateKey const &key) const noexcept = 0;
    /// @brief Set the Hash ID
    /// @param eAlgID hash algorithm cryptographic primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00592
    /// @trace_id_dd=DD_CRYPTO_02159
    /// @needwork = ad
    /// @endcode
    inline void SetHashAlgID(EPhCtxTypeID const eAlgID) noexcept
    {
        nHashAlgID_ = static_cast< CryptoPrimitiveId::AlgId >(eAlgID);
    }
    /// @brief Get the public key corresponding to the private key
    /// @return public key instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00593
    /// @trace_id_dd=DD_CRYPTO_02160
    /// @needwork = ad
    /// @endcode
    PublicKey::Uptrc GetPublicKey() const noexcept;
    /// @brief Set the private key
    /// @return private key instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00594
    /// @trace_id_dd=DD_CRYPTO_02161
    /// @needwork = ad
    /// @endcode
    inline PrivateKey const *GetPrivateKey() const noexcept { return pPrivateKey_; }
    /// @brief Get Crypto AlgId
    /// @return algorithm ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00595
    /// @trace_id_dd=DD_CRYPTO_02162
    /// @needwork = ad
    /// @endcode
    virtual CryptoPrimitiveId::AlgId GetCryptoAlgId() const noexcept = 0;

public:
};
//********************************/
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_PUHUA_CTX_DSV_SIGNER_PRIVATE_H_
