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
/// @file       isoft_ctx_dsv_sig_encode_private.h
/// @brief      AutoSar-Crypto encryption and decryption module
/// @details    Private key context for asymmetric signature calculation and short message encoding (similar to RSA).
/// @date       2022-03-24
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
/// @unit_name=PCtxDsv_SigEncodePrivate
/// @unit_description=Base class for private key context for asymmetric signature calculation and short message encoding (similar to RSA)
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_PUHUA_CTX_DSV_SIG_ENCODE_PRIVATE_H_
#define ARA_CRYPTO_CRYP_PUHUA_CTX_DSV_SIG_ENCODE_PRIVATE_H_

// DSV == Digital Signatures and Verifier

#include "ara/crypto/cryp/isoft_auto_buff.h"
#include "ara/crypto/cryp/sig_encode_private_ctx.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
/// @brief Encryption and decryption provider
class PCryptoProvider;
//********************************/
/// @brief Private key context for asymmetric signature calculation and short message encoding (similar to RSA).
///     A restricted group of trusted subscribers can use this primitive to simultaneously provide confidentiality, authenticity, and non-repudiation of short messages, provided the public key is properly generated and kept confidential.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00680
/// @trace_id_dd=DD_CRYPTO_02514
/// @needwork = ad
/// @endcode
class PCtxDsv_SigEncodePrivate : public SigEncodePrivateCtx
{
private:
    /// @brief Encryption and decryption provider object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02515
    /// @needwork = dda
    /// @endcode
    PCryptoProvider& cryptoProvider_;
    /// @brief Private key pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02516
    /// @needwork = dda
    /// @endcode
    PrivateKey const* pPrivateKey_;
    /// @brief Parameterized constructor
    /// @param cryptoProvider encryption provider
    /// @param pPrivateKey pointer to private key object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02517
    /// @needwork = dda
    /// @endcode
    PCtxDsv_SigEncodePrivate(PCryptoProvider& cryptoProvider, PrivateKey const* const pPrivateKey) noexcept;

public:
    /// @brief Parameterized constructor
    /// @param cryptoProvider the PCryptoProvider
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00681
    /// @trace_id_dd=DD_CRYPTO_02518
    /// @needwork = ad
    /// @endcode
    explicit PCtxDsv_SigEncodePrivate(PCryptoProvider& cryptoProvider) noexcept;
    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00682
    /// @trace_id_dd=DD_CRYPTO_02519
    /// @needwork = ad
    /// @endcode
    PCtxDsv_SigEncodePrivate() = delete;
    /// @brief Default virtual destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00683
    /// @trace_id_dd=DD_CRYPTO_02520
    /// @needwork = ad
    /// @endcode
    ~PCtxDsv_SigEncodePrivate() noexcept override = default;
    /// @brief Default move constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00684
    /// @trace_id_dd=DD_CRYPTO_02521
    /// @needwork = ad
    /// @endcode
    PCtxDsv_SigEncodePrivate(PCtxDsv_SigEncodePrivate&& other) = delete;
    /// @brief Default move assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00685
    /// @trace_id_dd=DD_CRYPTO_02522
    /// @needwork = ad
    /// @endcode
    PCtxDsv_SigEncodePrivate& operator=(PCtxDsv_SigEncodePrivate&& other) = delete;
    /// @brief Default copy assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00686
    /// @trace_id_dd=DD_CRYPTO_02523
    /// @needwork = ad
    /// @endcode
    PCtxDsv_SigEncodePrivate& operator=(PCtxDsv_SigEncodePrivate const& other) = delete;
    /// @brief Default copy constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00687
    /// @trace_id_dd=DD_CRYPTO_02524
    /// @needwork = ad
    /// @endcode
    PCtxDsv_SigEncodePrivate(PCtxDsv_SigEncodePrivate const& other) = delete;

public:  // CryptoContext interface
    /// @brief Return the CryptoPrimitivId instance containing the instance identifier.
    /// @return CryptoPrimitivId instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00688
    /// @trace_id_dd=DD_CRYPTO_02525
    /// @needwork = ad
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override = 0;
    /// @brief Check whether the encryption context has been initialized and is usable. It checks all required values, including: key value, IV/seed, etc.
    /// @return true if has already inited false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00689
    /// @trace_id_dd=DD_CRYPTO_02526
    /// @needwork = ad
    /// @endcode
    bool IsInitialized() const noexcept override;
    /// @brief Get a reference to the Crypto Provider for this context.
    /// @return reference to the encryption provider
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00690
    /// @trace_id_dd=DD_CRYPTO_02527
    /// @needwork = ad
    /// @endcode
    CryptoProvider& MyProvider() const noexcept override;
    /// @brief Get the private key
    /// @return private key pointer object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00691
    /// @trace_id_dd=DD_CRYPTO_02528
    /// @needwork = ad
    /// @endcode
    inline virtual PrivateKey const* GetPrivateKey() const noexcept { return pPrivateKey_; }

public:  // SigEncodePrivateCtx interface
    /// @brief Extension service member class.
    /// @return ExtensionService instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00692
    /// @trace_id_dd=DD_CRYPTO_02529
    /// @needwork = ad
    /// @endcode
    ExtensionService::Uptr GetExtensionService() const noexcept override = 0;
    /// @brief Get the maximum expected size of the input data block.
    ///     If (IsEncryption() == false), the value returned by this method is independent of the suppressPadding parameter, and will be equal to the block size.
    /// @brief Get maximum expected size of the input data block.
    ///       If (IsEncryption() == false) then a value returned by this method is independent from
    ///       the @c suppressPadding argument and it will be equal to the block size.
    /// @param suppressPadding  if @c true then the method calculates the size for the case when the whole space of
    ///     the plain data block is used for the payload only
    /// @returns maximum size of the input data block in bytes
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_23213}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00693
    /// @trace_id_dd=DD_CRYPTO_02530
    /// @needwork = ad
    /// @endcode
    std::size_t GetMaxInputSize(bool suppressPadding = false) const noexcept override;
    /// @brief Get the maximum possible size of the output data block.
    ///     If (IsEncryption() == true), the value returned by this method is independent of the suppressPadding parameter, and will be equal to the block size.
    /// @brief Get maximum possible size of the output data block.
    ///       If (IsEncryption() == true) then a value returned by this method is independent from the
    ///       @c suppressPadding argument and will be equal to the block size.
    /// @param suppressPadding  if @c true then the method calculates the size for the case when the whole space of
    ///     the plain data block is used for the payload only
    /// @returns maximum size of the output data block in bytes
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_23214}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00694
    /// @trace_id_dd=DD_CRYPTO_02531
    /// @needwork = ad
    /// @endcode
    std::size_t GetMaxOutputSize(bool suppressPadding = false) const noexcept override;
    /// @brief Process (encrypt/decrypt) an input block according to the encryptor configuration.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00695
    /// @trace_id_dd=DD_CRYPTO_02532
    /// @needwork = ad
    /// @endcode
    ara::core::Result< ara::core::Vector< ara::core::Byte > >
    /// @brief Process (encrypt / decrypt) an input block according to the cryptor configuration.
    /// Encryption with (suppressPadding == true)  expects that: in.size() == GetMaxInputSize(true) && out.size()
    /// >= GetMaxOutputSize(true). Encryption with (suppressPadding == false) expects that: in.size() <=
    /// GetMaxInputSize(false) && in.size() > 0 && out.size() >= GetMaxOutputSize(false). Decryption expects that:
    /// in.size() == GetMaxInputSize() && out.size() >= GetMaxOutputSize(suppressPadding). The case (out.size() <
    /// GetMaxOutputSize()) should be used with caution, only if you are strictly certain about the size of the
    /// output data! In case of (suppressPadding == true) the actual size of plain text should be equal to full
    /// size of the plain data block (defined by the algorithm)!
    /// @param in  the input data block
    /// @returns actual size of output data (it always <= out.size()) or 0 if the input data block has incorrect content
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_23215}
    /// @uptrace={RS_CRYPTO_02202}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @error: SecurityErrorDomain::kIncorrectInputSize  if the mentioned above rules about the input size is violated
    /// @error: SecurityErrorDomain::kUninitializedContext  if the context was not initialized by a key value
    /// @endcode
    SignAndEncode(ReadOnlyMemRegion const& in) const noexcept override;
    /// @brief Use base class template member function
    using SigEncodePrivateCtx::SignAndEncode;
    /// @brief Clear the encryption context.
    /// @brief Clear the crypto context.
    /// @return has value if reset sucess false otherwise
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_23212}
    /// @uptrace={RS_CRYPTO_02108}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00696
    /// @trace_id_dd=DD_CRYPTO_02533
    /// @needwork = ad
    /// @endcode
    ara::core::Result< void > Reset() noexcept override;
    /// @brief Set (deploy) a key to the sig encoding private algorithm context.
    /// @brief Set (deploy) a key to the sig encode private algorithm context.
    /// @param key  the source key object
    /// @return has value if SetKey sucess false otherwise
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_23211}
    /// @uptrace={RS_CRYPTO_02001}
    /// @uptrace={RS_CRYPTO_02003}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @error: SecurityErrorDomain::kIncompatibleObject  if the provided key object is incompatible with this symmetric
    ///     key context
    /// @error: SecurityErrorDomain::kUsageViolation  if the transformation type associated with this context is
    ///     prohibited by the "allowed usage" restrictions of provided key object
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00697
    /// @trace_id_dd=DD_CRYPTO_02534
    /// @needwork = ad
    /// @endcode
    ara::core::Result< void > SetKey(PrivateKey const& key) noexcept override;

public:  // ExtensionService interface
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
    /// @trace_id_ad=AD_CRYPTO_00698
    /// @trace_id_dd=DD_CRYPTO_02535
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
    /// @trace_id_ad=AD_CRYPTO_00699
    /// @trace_id_dd=DD_CRYPTO_02536
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
    /// @trace_id_ad=AD_CRYPTO_00700
    /// @trace_id_dd=DD_CRYPTO_02537
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
    /// @trace_id_ad=AD_CRYPTO_00701
    /// @trace_id_dd=DD_CRYPTO_02538
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
    /// @trace_id_ad=AD_CRYPTO_00702
    /// @trace_id_dd=DD_CRYPTO_02539
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
    /// @trace_id_ad=AD_CRYPTO_00703
    /// @trace_id_dd=DD_CRYPTO_02540
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
    /// @trace_id_ad=AD_CRYPTO_00704
    /// @trace_id_dd=DD_CRYPTO_02541
    /// @needwork = ad
    /// @endcode
    virtual bool IsKeyAvailable() const noexcept;

protected:
    /// @brief Get the length of the encrypted block
    /// @return length of the encrypted block
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02542
    /// @needwork = dda
    /// @endcode
    virtual uint32_t GetBlockSize() const noexcept = 0;
    /// @brief Check whether the key meets the requirements
    /// @param key private key
    /// @return true if check key sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02543
    /// @needwork = dda
    /// @endcode
    virtual bool CheckKey(PrivateKey const& key) const noexcept = 0;
    /// @brief Perform signature encryption logic
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02544
    /// @needwork = dda
    /// @endcode
    virtual ara::core::Result< ara::core::Vector< ara::core::Byte > >
    /// @param pInputData starting address of input data for algorithm operation
    /// @param nDataLen data length
    /// @param suppressPadding whether to suppress padding
    /// @return
    DoEncrypto(uint8_t const* pInputData, uint32_t nDataLen, bool suppressPadding) const noexcept = 0;

protected:
public:
};
//********************************/
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_PUHUA_CTX_DSV_SIG_ENCODE_PRIVATE_H_
