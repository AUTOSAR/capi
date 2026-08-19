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
/// @file       isoft_ctx_dsv_msg_recovery_public.h
/// @brief      AutoSar-Crypto encryption/decryption module
/// @details    Public key context for asymmetric recovery of short messages and their signature verification (similar to RSA).
/// @date       2022-03-28
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr><td>2022-03-28  </td>1.0.0    </tr>hanjingjing      <tr>Create initial version</tr>
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default encryption/Signature storage
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_01009
/// @unit_name=PCtxDsv_MsgRecoveryPublic
/// @unit_description=Public key context base class for asymmetric recovery of short messages and their signature verification (similar to RSA)
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_PUHUA_CTX_DSV_MSG_RECOVERY_PUBLIC_H_
#define ARA_CRYPTO_CRYP_PUHUA_CTX_DSV_MSG_RECOVERY_PUBLIC_H_

// DSV == Digital Signatures and Verifier

#include "ara/crypto/cryp/isoft_auto_buff.h"
#include "ara/crypto/cryp/msg_recovery_public_ctx.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
/// @brief Crypto provider
class PCryptoProvider;
//********************************/
/// @brief Public key context for asymmetric recovery of short messages and their signature verification (similar to RSA).
///     A restricted group of trusted subscribers can use this primitive to simultaneously provide confidentiality, authenticity, and non-repudiation of short messages, provided the public keys are properly generated and kept confidential.
///     If (0 == BlockCryptor::ProcessBlock(…)), the input message block is contaminated.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00641
/// @trace_id_dd=DD_CRYPTO_02330
/// @needwork = ad
/// @endcode
class PCtxDsv_MsgRecoveryPublic : public MsgRecoveryPublicCtx
{
private:
    /// @brief Crypto provider object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02331
    /// @needwork = dda
    /// @endcode
    PCryptoProvider& cryptoProvider_;
    /// @brief Public key pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02332
    /// @needwork = dda
    /// @endcode
    PublicKey const* pPublicKey_;
    /// @brief Parameterized constructor
    /// @param cryptoProvider Crypto provider
    /// @param pPublicKey Public key
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02333
    /// @needwork = dda
    /// @endcode
    PCtxDsv_MsgRecoveryPublic(PCryptoProvider& cryptoProvider, PublicKey const* const pPublicKey) noexcept;

public:
    /// @brief Parameterized constructor
    /// @param cryptoProvider PCryptoProvider
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00642
    /// @trace_id_dd=DD_CRYPTO_02334
    /// @needwork = ad
    /// @endcode
    explicit PCtxDsv_MsgRecoveryPublic(PCryptoProvider& cryptoProvider) noexcept;
    /// @brief default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00643
    /// @trace_id_dd=DD_CRYPTO_02335
    /// @needwork = ad
    /// @endcode
    PCtxDsv_MsgRecoveryPublic() noexcept = delete;
    /// @brief default virtual destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00644
    /// @trace_id_dd=DD_CRYPTO_02336
    /// @needwork = ad
    /// @endcode
    ~PCtxDsv_MsgRecoveryPublic() noexcept override = default;
    /// @brief default copy assignment operator
    /// @param other another object instance of this class
    /// @return  *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00645
    /// @trace_id_dd=DD_CRYPTO_02337
    /// @needwork = ad
    /// @endcode
    PCtxDsv_MsgRecoveryPublic& operator=(PCtxDsv_MsgRecoveryPublic const& other) noexcept = delete;
    /// @brief default move assignment operator
    /// @param other another object instance of this class
    /// @return  *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00646
    /// @trace_id_dd=DD_CRYPTO_02338
    /// @needwork = ad
    /// @endcode
    PCtxDsv_MsgRecoveryPublic& operator=(PCtxDsv_MsgRecoveryPublic&& other) noexcept = delete;
    /// @brief default copy constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00647
    /// @trace_id_dd=DD_CRYPTO_02339
    /// @needwork = ad
    /// @endcode
    PCtxDsv_MsgRecoveryPublic(PCtxDsv_MsgRecoveryPublic const& other) noexcept = delete;
    /// @brief default move constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00648
    /// @trace_id_dd=DD_CRYPTO_02340
    /// @needwork = ad
    /// @endcode
    PCtxDsv_MsgRecoveryPublic(PCtxDsv_MsgRecoveryPublic&& other) noexcept = delete;

public:  // CryptoContext interface
    /// @brief Returns the CryptoPrimitivId instance containing the instance identifier.
    /// @return CryptoPrimitivId instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00649
    /// @trace_id_dd=DD_CRYPTO_02341
    /// @needwork = ad
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override = 0;
    /// @brief Check whether the encryption context is initialized and usable. It checks all required values, including: key value, IV/seed, etc.
    /// @return true if has already init key
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00650
    /// @trace_id_dd=DD_CRYPTO_02342
    /// @needwork = ad
    /// @endcode
    bool IsInitialized() const noexcept override;
    /// @brief Get a reference to the Crypto Provider for this context.
    /// @return Crypto provider reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00651
    /// @trace_id_dd=DD_CRYPTO_02343
    /// @needwork = ad
    /// @endcode
    CryptoProvider& MyProvider() const noexcept override;
    /// @brief Get PublicKey
    /// @name   GetPublicKey
    /// @returns Public key pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00652
    /// @trace_id_dd=DD_CRYPTO_02344
    /// @needwork = ad
    /// @endcode
    inline virtual PublicKey const* GetPublicKey() const noexcept { return pPublicKey_; }

public:  // MsgRecoveryPublicCtx interface
    /// @brief Get the ExtensionService instance.
    /// @return ExtensionService instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00653
    /// @trace_id_dd=DD_CRYPTO_02345
    /// @needwork = ad
    /// @endcode
    ExtensionService::Uptr GetExtensionService() const noexcept override = 0;
    /// @brief Get the maximum expected size of the input data block. if (IsEncryption() == false), the value returned by this method is independent of the suppressPadding parameter and will equal the block size.
    /// false), the value returned by this method is independent of the suppressPadding parameter and equals the block size.
    /// @brief Get maximum expected size of the input data block.
    ///       if (IsEncryption() == false) then a value returned by this method is independent from
    ///       the @c suppressPadding argument and it will be equal to the block size.
    /// @param suppressPadding  if @c true then the method calculates the size for the case when the whole space of
    /// the plain data block is used for the payload only
    /// @returns maximum size of the input data block in bytes
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_22213}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00654
    /// @trace_id_dd=DD_CRYPTO_02346
    /// @needwork = ad
    /// @endcode
    std::size_t GetMaxInputSize(bool suppressPadding = false) const noexcept override;
    /// @brief Get the maximum possible size of the output data block. if (IsEncryption() == true), the value returned by this method is independent of the suppressPadding parameter and will equal the block size.
    /// true), the value returned by this method is independent of the suppressPadding parameter and equals the block size.
    /// @brief Get maximum possible size of the output data block.
    ///       If (IsEncryption() == true) then a value returned by this method is independent from the
    ///       @c suppressPadding argument and will be equal to the block size.
    /// @param suppressPadding  if @c true then the method calculates the size for the case when the whole space of
    /// the plain data block is used for the payload only
    /// @returns maximum size of the output data block in bytes
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_22214}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00655
    /// @trace_id_dd=DD_CRYPTO_02347
    /// @needwork = ad
    /// @endcode
    std::size_t GetMaxOutputSize(bool suppressPadding = false) const noexcept override;
    /// @brief Process an input block (encrypt/decrypt) according to the cipher configuration.
    /// @brief Process (encrypt / decrypt) an input block according to the cryptor configuration.
    /// Encryption with (suppressPadding == true) expects that: in.size() == GetMaxInputSize(true) && out.size()
    /// >= GetMaxOutputSize(true). Encryption with (suppressPadding == false) expects that: in.size() <=
    /// GetMaxInputSize(false) && in.size() > 0 && out.size() >= GetMaxOutputSize(false). Decryption expects that:
    /// in.size() == GetMaxInputSize() && out.size() >= GetMaxOutputSize(suppressPadding). The case (out.size() <
    /// GetMaxOutputSize()) should be used with caution, only if you are strictly certain about the size of the
    /// output data! In case of (suppressPadding == true) the actual size of plain text should be equal to full
    /// size of the plain data block (defined by the algorithm)!
    /// @param in  the input data block
    /// @returns actual size of output data (it always <= out.size()) or 0 if the input data block has incorrect content
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_22215}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02204}
    /// @error:  SecurityErrorDomain::kIncorrectInputSize  if the mentioned above rules about the input size is violated
    /// @error:  SecurityErrorDomain::kUninitializedContext  if the context was not initialized by a key value
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00656
    /// @trace_id_dd=DD_CRYPTO_02348
    /// @needwork = ad
    /// @endcode
    ara::core::Result< ara::core::Vector< ara::core::Byte > > DecodeAndVerify(
        ReadOnlyMemRegion const& in) const noexcept override;
    /// @brief Use base class template member function
    using MsgRecoveryPublicCtx::DecodeAndVerify;
    /// @brief Clear the encryption context.
    /// @brief Clear the crypto context.
    /// @return has value if reset sucess false otherwise
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_22212}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02108}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00657
    /// @trace_id_dd=DD_CRYPTO_02349
    /// @needwork = ad
    /// @endcode
    ara::core::Result< void > Reset() noexcept override;
    /// @brief Set (deploy) a key to the msg recovery public algorithm context.
    /// @brief Set (deploy) a key to the msg recovery public algorithm context.
    /// @param key  the source key object
    /// @returns has value if SetKey sucess false otherwise
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_22211}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02001}
    /// @uptrace={RS_CRYPTO_02003}
    /// @error:  SecurityErrorDomain::kIncompatibleObject  if the provided key object is incompatible with this
    /// symmetric key context
    /// @error:  SecurityErrorDomain::kUsageViolation  if the transformation type associated with this context is
    /// prohibited by the "allowed usage" restrictions of provided key object.
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00658
    /// @trace_id_dd=DD_CRYPTO_02350
    /// @needwork = ad
    /// @endcode
    ara::core::Result< void > SetKey(PublicKey const& key) noexcept override;

public:  // ExtensionService interface
    /// @brief Get the actual bit length of the key loaded into the context. If no key is set for the context, returns 0.
    /// @brief Get actual bit-length of a key loaded to the context. If no key was set to the context yet then 0 is
    /// returned.
    /// @returns actual length of a key (now set to the algorithm context) in bits
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_29045}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00659
    /// @trace_id_dd=DD_CRYPTO_02351
    /// @needwork = ad
    /// @endcode
    virtual std::size_t GetActualKeyBitLength() const noexcept;
    /// @brief Get the COUID of the key deployed to the context attached to this extended service. If no key is set for the context, returns an empty COUID (Nil).
    /// @brief Get the COUID of the key deployed to the context this extension service is attached to.
    /// If no key was set to the context yet then an empty COUID (Nil) is returned.
    /// @returns the COUID of the CryptoObject
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_29047}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00660
    /// @trace_id_dd=DD_CRYPTO_02352
    /// @needwork = ad
    /// @endcode
    virtual CryptoObjectUid GetActualKeyCOUID() const noexcept;
    /// @brief Get the allowed usage of this context (according to the key object properties loaded into this context). If the context has not been initialized with a key object, must return 0 (all flags reset).
    /// @brief Get allowed usages of this context (according to the key object attributes loaded to this context).
    /// If the context is not initialized by a key object yet then zero (all flags are reset) must be returned.
    /// @returns a combination of bit-flags that specifies allowed usages of the context
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_29046}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02008}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00661
    /// @trace_id_dd=DD_CRYPTO_02353
    /// @needwork = ad
    /// @endcode
    virtual AllowedUsageFlags GetAllowedUsage() const noexcept;
    /// @brief Get the maximum supported key length (in bits).
    /// @brief Get maximal supported key length in bits.
    /// @returns maximal supported length of the key in bits
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_29044}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00662
    /// @trace_id_dd=DD_CRYPTO_02354
    /// @needwork = ad
    /// @endcode
    virtual std::size_t GetMaxKeyBitLength() const noexcept = 0;
    /// @brief Get the minimum supported key length (in bits).
    /// @brief Get minimal supported key length in bits.
    /// @returns minimal supported length of the key in bits
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_29043}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00663
    /// @trace_id_dd=DD_CRYPTO_02355
    /// @needwork = ad
    /// @endcode
    virtual std::size_t GetMinKeyBitLength() const noexcept = 0;
    /// @brief Validate support for a specific key length according to the context.
    /// @brief Verify supportness of specific key length by the context.
    /// @param keyBitLength Key length in bits
    /// @returns @c true if provided value of the key length is supported by the context
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_29048}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00664
    /// @trace_id_dd=DD_CRYPTO_02356
    /// @needwork = ad
    /// @endcode
    virtual bool IsKeyBitLengthSupported(std::size_t keyBitLength) const noexcept = 0;
    /// @brief Check whether a key is set for this context.
    /// @brief Check if a key has been set to this context.
    /// @return true if a key has been set to this context false otherwise
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_29049}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00665
    /// @trace_id_dd=DD_CRYPTO_02357
    /// @needwork = ad
    /// @endcode
    virtual bool IsKeyAvailable() const noexcept;

protected:
    /// @brief Get the length of the encrypted block.
    /// @return Encrypted block length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02358
    /// @needwork = dda
    /// @endcode
    virtual uint32_t GetBlockSize() const noexcept = 0;
    /// @brief Check whether the key meets requirements.
    /// @param key Public key
    /// @return true if check key sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02359
    /// @needwork = dda
    /// @endcode
    virtual bool CheckKey(PublicKey const& key) const noexcept = 0;
    /// @brief Perform signature decryption logic.
    /// @param pInputData Starting address of input data for algorithm operation
    /// @param nDataLen Data length
    /// @param suppressPadding Whether to suppress padding
    /// @return Decrypted data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02360
    /// @needwork = dda
    /// @endcode
    virtual ara::core::Result< ara::core::Vector< ara::core::Byte > > DoDecrypto(
        uint8_t const* pInputData, uint32_t nDataLen, bool suppressPadding) const noexcept = 0;

protected:
public:
};
//********************************/
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_PUHUA_CTX_DSV_MSG_RECOVERY_PUBLIC_H_
