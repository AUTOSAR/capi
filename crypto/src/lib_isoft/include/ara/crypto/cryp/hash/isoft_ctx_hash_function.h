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
/// @file       isoft_ctx_hash_function.h
/// @brief      AutoSar-Crypto Encryption Module
/// @details    Hash function interface.
/// @date       2021-12-21
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr><td>2021-12-21  <td>1.0.0    <td>hanjingjing  <td>Initial version creation
/// <tr><td>2023-08-18  <td>0.2      <td>Che Jinzhao <td>Improve functional requirements
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default Encryption/Hash Function
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_01020
/// @unit_name=PCtxHashFunction
/// @unit_description=Hash Context Base Class
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_PUHUA_CTX_HASH_FUNCTION_H_
#define ARA_CRYPTO_CRYP_PUHUA_CTX_HASH_FUNCTION_H_

#include <openssl/evp.h>

#include "ara/crypto/common/isoft_data_type.h"
#include "ara/crypto/cryp/hash_function_ctx.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
/// @brief Encryption provider
class PCryptoProvider;
/// @brief the hash length
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00115
/// @trace_id_dd=DD_CRYPTO_00745
/// @needwork = dd
/// @endcode
int32_t const kPhHashTailLength{4};
//********************************/
/// @brief Puhua hash interface base class.
///         Base class for all Puhua Hash derived classes (implements some common interface functions for hash algorithms). Direct use of this class is not allowed; derived classes must be used.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00115
/// @trace_id_dd=DD_CRYPTO_00746
/// @needwork = ad
/// @endcode
class PCtxHashFunction : public HashFunctionCtx
{
public:
    /// @brief (GetHashAlgId()) returns EVP_MD object. By passing specific parameters via GetHashAlgId(), virtual functions can be avoided.
    /// @param algId Crypto primitive ID of the encryption algorithm
    /// @return Pointer to EVP_MD structure
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00116
    /// @trace_id_dd=DD_CRYPTO_00747
    /// @needwork = ad
    /// @endcode
    static EVP_MD const* GetEVP_MD(AlgId const algId) noexcept;

public:
    /// @brief the hash workstata
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00117
    /// @trace_id_dd=DD_CRYPTO_00748
    /// @needwork = ad
    /// @endcode
    enum class EHashWorkState : std::uint8_t
    {
        /// @brief Not started
        kHashNotStarted = 0,
        /// @brief Started
        kHashStart = 0x01,
        /// @brief Working
        kHashUpdate = 0x02,
        /// @brief Completed
        kHashFinish = 0x0F,
    };

private:
    /// @brief Encryption provider object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00749
    /// @needwork = dda
    /// @endcode
    PCryptoProvider& cryptoProvider_;
    /// @brief T
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00750
    /// @needwork = dda
    /// @endcode
    EHashWorkState eHashState_;
    /// @brief If secretSeed is used for IV initialization, record its COUID for GetActualIvBitLength calls to retrieve the COUID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00751
    /// @needwork = dda
    /// @endcode
    CryptoObjectUid secretSeedCouId_;
    /// @brief Parameterized constructor
    /// @param cryptoProvider Encryption provider
    /// @param eHashState Hash working state
    /// @param secretSeedCouId Secret seed crypto material ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00752
    /// @needwork = dda
    /// @endcode
    PCtxHashFunction(PCryptoProvider& cryptoProvider,
                     EHashWorkState const eHashState,
                     CryptoObjectUid const& secretSeedCouId) noexcept;

public:
    /// @brief Parameterized constructor
    /// @name   PCtxHashFunction
    /// @param cryptoProvider Encryption provider PCryptoProvider
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00118
    /// @trace_id_dd=DD_CRYPTO_00753
    /// @needwork = ad
    /// @endcode
    explicit PCtxHashFunction(PCryptoProvider& cryptoProvider) noexcept;
    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00119
    /// @trace_id_dd=DD_CRYPTO_00754
    /// @needwork = ad
    /// @endcode
    PCtxHashFunction() = delete;
    /// @brief Default virtual destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00120
    /// @trace_id_dd=DD_CRYPTO_00755
    /// @needwork = ad
    /// @endcode
    ~PCtxHashFunction() override;
    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00121
    /// @trace_id_dd=DD_CRYPTO_00756
    /// @needwork = ad
    /// @endcode
    PCtxHashFunction(PCtxHashFunction&& other) = delete;
    /// @brief Default move assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00122
    /// @trace_id_dd=DD_CRYPTO_00757
    /// @needwork = ad
    /// @endcode
    PCtxHashFunction& operator=(PCtxHashFunction&& other) = delete;
    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00123
    /// @trace_id_dd=DD_CRYPTO_00758
    /// @needwork = ad
    /// @endcode
    PCtxHashFunction& operator=(PCtxHashFunction const& other) = delete;
    /// @brief Default copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00124
    /// @trace_id_dd=DD_CRYPTO_00759
    /// @needwork = ad
    /// @endcode
    PCtxHashFunction(PCtxHashFunction const& other) = delete;

private:
    /// @brief EVP_MD_CTX context
    EVP_MD_CTX* evpMdCtx_;

public:
    /// @brief Initialize
    /// @param type
    /// @return 1 success, others failure
    int EVP_DigestInit_ph(const EVP_MD* type) { return EVP_DigestInit_ex(evpMdCtx_, type, nullptr); }
    /// @brief Update
    /// @param d Data to update
    /// @param cnt Length of data to update
    /// @return 1 success, others failure
    int EVP_DigestUpdate_ph(const void* d, size_t cnt) { return EVP_DigestUpdate(evpMdCtx_, d, cnt); }
    /// @brief Finalize
    /// @param md Hash result
    /// @param s Hash result length
    /// @return 1 success, others failure
    int EVP_DigestFinal_ph(u_char* md, unsigned int* s) { return EVP_DigestFinal_ex(evpMdCtx_, md, s); }

public:  // CryptoContext interface
    /// @brief Returns the CryptoPrimitivId instance containing the instance identifier.
    /// @brief Return CryptoPrimitivId instance containing instance identification.
    /// @name  GetCryptoPrimitiveId
    /// @returns CryptoPrimitivId instance
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_20411}
    /// @uptrace={RS_CRYPTO_02008}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00125
    /// @trace_id_dd=DD_CRYPTO_00760
    /// @needwork = ad
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override = 0;
    /// @brief Checks if the crypto context has been initialized and is ready for use. It checks all required values, including: key values, IV/seed, etc.
    /// @brief Check if the crypto context is already initialized and ready to use.
    ///           It checks all required values, including: key value, IV/seed, etc.
    /// @name IsInitialized
    /// @returns @c true if the crypto context is completely initialized and ready to use, and @c false otherwise
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_20412}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00126
    /// @trace_id_dd=DD_CRYPTO_00761
    /// @needwork = ad
    /// @endcode
    bool IsInitialized() const noexcept override;
    /// @brief Gets the reference to the Crypto Provider for this context.
    /// @brief Get a reference to Crypto Provider of this context.
    /// @name MyProvider
    /// @returns a reference to Crypto Provider instance that provides this context
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_20654}
    /// @uptrace={RS_CRYPTO_02401}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00127
    /// @trace_id_dd=DD_CRYPTO_00762
    /// @needwork = ad
    /// @endcode
    CryptoProvider& MyProvider() const noexcept override;

public:  // HashFunctionCtx interface: Derived classes implement themselves
    /// @brief Gets the DigestService instance.
    /// @brief Get DigestService instance.
    /// @name  GetDigestService
    /// @returns DigestService instance
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_21102}
    /// @uptrace={RS_CRYPTO_02006}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00128
    /// @trace_id_dd=DD_CRYPTO_00763
    /// @needwork = ad
    /// @endcode
    DigestService::Uptr GetDigestService() const noexcept override = 0;
    /// @brief Gets the partial digest request result.
    ///         The entire digest value is kept in the context until the next call to Start(), so any part of it can be extracted or verified again.
    /// @brief Get requested part of calculated digest.
    ///         Entire digest value is kept in the context up to next call @c Start(), therefore any its part can be
    ///         extracted again or verified. If (full_digest_size <= offset) then return_size = 0 bytes; else
    ///         return_size = min(output.size(), (full_digest_size - offset)) bytes. This method can be implemented as
    ///         "inline" after standartization of function @c ara::core::memcpy().
    /// @name  GetDigest
    /// @param offset  position of the first byte of digest that should be placed to the output buffer
    /// @returns number of digest bytes really stored to the output buffer (they are always <= @c output.size() and
    ///     denoted below as @a return_size)
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_21116}
    /// @uptrace={RS_CRYPTO_02205}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    ///         If (full_digest_size <= offset) then return_size = 0 bytes; Else return_size = min(output.size()，
    ///         (full_digest_size - offset)) bytes. This method can be implemented "inline" after the function ara::core::memcpy() is standardized.
    /// @error: SecurityErrorDomain::kProcessingNotFinished  if the digest calculation was not finished by a call of the @c Finish() method
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00129
    /// @trace_id_dd=DD_CRYPTO_00764
    /// @needwork = ad
    /// @endcode
    ara::core::Result< ara::core::Vector< ara::core::Byte > > GetDigest(std::size_t offset
                                                                        = 0U) const noexcept override;
    /// @brief Uses base class template functions
    using HashFunctionCtx::GetDigest;

public:  // HashFunctionCtx interface: Provides default implementation for Hash family
    /// @brief Initializes the context for a new data stream processing or generation (depending on the primitive) without an IV.
    /// @brief Initialize the context for a new data stream processing or generation (depending on the primitive)
    ///     without IV.
    /// @name Start
    /// @returns has vlaue if has started false otherwise
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @uptrace={RS_CRYPTO_02302}
    /// @trace_id_sws={SWS_CRYPT_21118}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @error: SecurityErrorDomain::kMissingArgument    the configured hash function expected an IV
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00130
    /// @trace_id_dd=DD_CRYPTO_00765
    /// @needwork = ad
    /// @endcode
    ara::core::Result< void > Start() noexcept override;
    /// @brief Initializes the context for a new data stream processing or generation (depending on the primitive).
    /// If the IV size exceeds the maximum supported size of the algorithm, the implementation may use only the leading bytes of the sequence.
    /// @brief Initialize the context for a new data stream processing or generation (depending on the primitive).
    ///         If IV size is greater than maximally supported by the algorithm then an implementation may use the
    ///         leading bytes only from the sequence.
    /// @name  Start
    /// @param iv  an optional Initialization Vector (IV) or "nonce" value
    /// @returns has vlaue if has started false otherwise
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_21110}
    /// @uptrace={RS_CRYPTO_02302}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @error: SecurityErrorDomain::kInvalidInputSize   if the size of provided IV is not supported (i.e. if it is not
    ///     enough for the initialization)
    /// @error: SecurityErrorDomain::kUnsupported        if the base algorithm (or its current implementation)
    ///     principally doesn't support the IV variation, but provided IV value is not empty, i.e. if <tt>(iv.empty() ==
    ///     false)</tt>
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00131
    /// @trace_id_dd=DD_CRYPTO_00766
    /// @needwork = ad
    /// @endcode
    ara::core::Result< void > Start(ReadOnlyMemRegion const& iv) noexcept override;
    /// @brief Initializes the context for a new data stream processing or generation (depending on the primitive).
    /// If the IV size exceeds the maximum supported size of the algorithm, the implementation may use only the leading bytes of the sequence.
    /// @brief Initialize the context for a new data stream processing or generation (depending on the primitive).
    ///         If IV size is greater than maximally supported by the algorithm then an implementation may use the
    ///         leading bytes only from the sequence.
    /// @name  Start
    /// @param iv  the Initialization Vector (IV) or "nonce" object
    /// @returns has vlaue if has started false otherwise
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_21111}
    /// @uptrace={RS_CRYPTO_02302}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @error: SecurityErrorDomain::kInvalidInputSize   if the size of provided IV is not supported (i.e. if it is not
    ///     enough for the initialization)
    /// @error: SecurityErrorDomain::kUnsupported        if the base algorithm (or its current implementation)
    ///     principally doesn't support the IV variation
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00132
    /// @trace_id_dd=DD_CRYPTO_00767
    /// @needwork = ad
    /// @endcode
    ara::core::Result< void > Start(SecretSeed const& iv) noexcept override;
    /// @brief Updates the digest calculation context with a new part of the message. This method is dedicated to cases where the restricted use object is part of the "message".
    /// @brief Update the digest calculation context by a new part of the message.
    ///         This method is dedicated for cases then the @c RestrictedUseObject is a part of the "message".
    /// @name  Update
    /// @param in  a part of input message that should be processed
    /// @returns has vlaue if has Updated false otherwise
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @uptrace={RS_CRYPTO_02302}
    /// @trace_id_sws={SWS_CRYPT_21112}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @error: SecurityErrorDomain::kProcessingNotStarted   if the digest calculation was not initiated by a call of
    /// the
    ///     @c Start() method
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00133
    /// @trace_id_dd=DD_CRYPTO_00768
    /// @needwork = ad
    /// @endcode
    ara::core::Result< void > Update(RestrictedUseObject const& in) noexcept override;
    /// @brief Updates the digest calculation context with a new part of the message.
    /// @brief Update the digest calculation context by a new part of the message.
    /// @name   Update
    /// @param in  a part of the input message that should be processed
    /// @returns has vlaue if has Updated false otherwise
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_21113}
    /// @uptrace={RS_CRYPTO_02302}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @error: SecurityErrorDomain::kProcessingNotStarted   if the digest calculation was not initiated by a call of
    /// the
    ///     @c Start() method
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00134
    /// @trace_id_dd=DD_CRYPTO_00769
    /// @needwork = ad
    /// @endcode
    ara::core::Result< void > Update(ReadOnlyMemRegion const& in) noexcept override;
    /// @brief Updates the digest calculation context with a new part of the message. This method facilitates handling constant tags.
    /// @brief Update the digest calculation context by a new part of the message.
    ///         This method is convenient for processing of constant tags.
    /// @name Update
    /// @param in  a byte value that is a part of input message
    /// @returns has vlaue if has Updated false otherwise
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_21114}
    /// @uptrace={RS_CRYPTO_02302}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @error: SecurityErrorDomain::kProcessingNotStarted  if the digest calculation was not initiated by a call of the
    ///     @c Start() method
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00135
    /// @trace_id_dd=DD_CRYPTO_00770
    /// @needwork = ad
    /// @endcode
    ara::core::Result< void > Update(std::uint8_t const& in) noexcept override;
    /// @brief Completes the digest calculation and optionally generates a "signature" object. Only after calling this method can the digest be signed, verified, extracted, or compared.
    /// @brief Finish the digest calculation and optionally produce the "signature" object.
    ///       Only after call of this method the digest can be signed, verified, extracted or compared.
    /// @returns unique smart pointer to created signature object, if <tt>(makeSignatureObject == true)</tt> or an empty
    /// Signature object if <tt>(makeSignatureObject == false)</tt>
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_21115}
    /// @uptrace={RS_CRYPTO_02302}
    /// @uptrace={RS_CRYPTO_02205}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @error: SecurityErrorDomain::kProcessingNotStarted  if the digest calculation was not initiated by a call of the
    ///     @c Start() method
    /// @error: SecurityErrorDomain::kInvalidUsageOrder  if the digest calculation has not started yet or not been
    ///     updated at least once
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00136
    /// @trace_id_dd=DD_CRYPTO_00771
    /// @needwork = ad
    /// @endcode
    ara::core::Result< ara::core::Vector< ara::core::Byte > > Finish() noexcept override;

public:  // PCtxHashFunction interface: Provided for DigestService calls
    /// @brief Returns the length of the Hash result
    /// @name   GetHashLength
    /// @returns Length of the Hash result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00137
    /// @trace_id_dd=DD_CRYPTO_00772
    /// @needwork = ad
    /// @endcode
    virtual uint32_t GetHashLength() const noexcept = 0;
    /// @brief Checks the current state of stream processing: whether it has started.
    /// @name   IsStarted
    /// @returns true if has started false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00138
    /// @trace_id_dd=DD_CRYPTO_00773
    /// @needwork = ad
    /// @endcode
    virtual bool IsStarted() const noexcept;
    /// @brief Checks the current state of stream processing: whether it is completed.
    /// @name   IsFinished
    /// @returns true if has finished false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00139
    /// @trace_id_dd=DD_CRYPTO_00774
    /// @needwork = ad
    /// @endcode
    virtual bool IsFinished() const noexcept;
    /// @brief Compares the calculated digest with the expected value.
    /// @name   Compare
    /// @param expected Expected value
    /// @param offset Offset
    /// @returns true if equal false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00140
    /// @trace_id_dd=DD_CRYPTO_00775
    /// @needwork = ad
    /// @endcode
    virtual bool Compare(ReadOnlyMemRegion const& expected, std::size_t offset) const noexcept;
    /// @brief Crop IV to meet algorithm-supported size; if exceeded, keep only the first l bytes.
    /// @param iv Initialization secret seed
    /// @param ivMaxLength Maximum length of initialization seed
    /// @param pTempIv Memory address to store the result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00141
    /// @trace_id_dd=DD_CRYPTO_00776
    /// @needwork = ad
    /// @endcode
    static void ClipIV(SecretSeed const& iv, uint32_t const ivMaxLength, uint8_t* const pTempIv) noexcept;
    /// @brief Get the seed COUID filling the context IV
    /// @name   GetSecretSeedCryptoObjectUid
    /// @returns Seed COUID filling the context IV
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00142
    /// @trace_id_dd=DD_CRYPTO_00777
    /// @needwork = ad
    /// @endcode
    inline CryptoObjectUid GetSecretSeedCryptoObjectUid() const noexcept { return secretSeedCouId_; }
    /// @brief Get the maximum IV length corresponding to the specific algorithm
    /// @name   GetIvMaxLength
    /// @returns Maximum corresponding IV length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00143
    /// @trace_id_dd=DD_CRYPTO_00778
    /// @needwork = ad
    /// @endcode
    virtual uint32_t GetIvMaxLength() const noexcept = 0;
    /// @brief Check if data length meets hash algorithm requirements
    /// @param value the data value
    /// @return true if the length is valild false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00144
    /// @trace_id_dd=DD_CRYPTO_00779
    /// @needwork = ad
    /// @endcode
    virtual bool CheckSize(ara::crypto::ReadOnlyMemRegion const& value) const noexcept
    {
        std::ignore = value;
        return true;
    }

protected:  // PCtxHashFunction interface
    /// @brief Returns the Hash result
    /// @name   GetHashResult
    /// @returns Hash result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00780
    /// @needwork = dda
    /// @endcode
    virtual uint8_t const* GetHashResult() const noexcept = 0;
    /// @brief Initialize
    /// @brief SWS_CRYPT_00903 Calling this function clears the calculated hash value
    /// @name   DoInitByIV
    /// @param piv Pointer to initialization vector
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00781
    /// @needwork = dda
    /// @endcode
    virtual void DoInitByIV(uint8_t const* piv) noexcept = 0;
    /// @brief Initialize, called by start()
    /// @brief SWS_CRYPT_00903 Calling this function clears the calculated hash value
    /// @name   DoInit
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00782
    /// @needwork = dda
    /// @endcode
    virtual void DoInit() noexcept = 0;
    /// @brief Update data
    /// @name   DoUpdate
    /// @param pVoidData Starting address of data
    /// @param nDataLen Data length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00783
    /// @needwork = dda
    /// @endcode
    virtual void DoUpdate(void const* pVoidData, uint32_t nDataLen) noexcept = 0;
    /// @brief Complete hash operation
    /// @name   DoFinish
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00784
    /// @needwork = dda
    /// @endcode
    virtual void DoFinish() noexcept = 0;
    /// @brief Get HashState
    /// @return Hash state
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00785
    /// @needwork = dda
    /// @endcode
    EHashWorkState _GeteHashState() const noexcept { return eHashState_; }
    /// @brief Set hash state
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00786
    /// @needwork = dda
    /// @endcode
    void _SeteHashState(EHashWorkState const& other) noexcept { eHashState_ = other; }
    /// @brief Check if the specific hash function supports IV
    /// @name   SupportIv
    /// @returns true if support iv false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00787
    /// @needwork = dda
    /// @endcode
    virtual bool SupportIv() const noexcept = 0;
};
//********************************/
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_PUHUA_CTX_HASH_FUNCTION_H_
