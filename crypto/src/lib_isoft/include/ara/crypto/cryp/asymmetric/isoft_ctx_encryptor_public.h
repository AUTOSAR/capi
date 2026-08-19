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
/// @file       isoft_ctx_encryptor_public.h
/// @brief      AutoSar-Crypto encryption/decryption module
/// @details    Asymmetric encryption module. Public key encryption
/// @date       2022-03-01
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr><td>2022-03-01</td><td>1.0.0</td><td>hanjingjing</td><td>Create initial version</td>
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default encryption/Asymmetric encryption
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_01008
/// @unit_name=PCtxEncryptorPublic
/// @unit_description=Asymmetric encryption public key context interface base class
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_PUHUA_CTX_ENCRYPTOR_PUBLIC_H_
#define ARA_CRYPTO_CRYP_PUHUA_CTX_ENCRYPTOR_PUBLIC_H_

#include "ara/crypto/cryp/cryobj/isoft_key_public_ipc.h"
#include "ara/crypto/cryp/cryobj/public_key.h"
#include "ara/crypto/cryp/encryptor_public_ctx.h"
#include "ara/crypto/cryp/isoft_auto_buff.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
/// @brief Crypto provider
class PCryptoProvider;
//********************************/
/// @brief Asymmetric encryption public key context interface.
//********************************/
/// @brief Asymmetric encryption module. Public key encryption
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00433
/// @trace_id_dd=DD_CRYPTO_01817
/// @needwork = ad
/// @endcode
class PCtxEncryptorPublic : public EncryptorPublicCtx
{
private:
    /// @brief Crypto manager
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01818
    /// @needwork = dda
    /// @endcode
    PCryptoProvider& cryptoProvider_;
    /// @brief Public key pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01819
    /// @needwork = dda
    /// @endcode
    PublicKey const* pPublicKey_;
    /// @brief Input data buffer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01820
    /// @needwork = dda
    /// @endcode
    internal::PAutoBuff buffInput_;
    /// @brief Parameterized constructor
    /// @param cryptoProvider Crypto provider
    /// @param pPublicKey Public key
    /// @param buffInput Input buffer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01821
    /// @needwork = dda
    /// @endcode
    PCtxEncryptorPublic(PCryptoProvider& cryptoProvider,
                        PublicKey const* const pPublicKey,
                        internal::PAutoBuff const* const buffInput) noexcept;

public:
    /// @brief Parameterized constructor
    /// @param cryptoProvider Crypto provider
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00434
    /// @trace_id_dd=DD_CRYPTO_01822
    /// @needwork = ad
    /// @endcode
    explicit PCtxEncryptorPublic(PCryptoProvider& cryptoProvider) noexcept;
    /// @brief default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00435
    /// @trace_id_dd=DD_CRYPTO_01823
    /// @needwork = ad
    /// @endcode
    PCtxEncryptorPublic() = delete;
    /// @brief default virtual destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00436
    /// @trace_id_dd=DD_CRYPTO_01824
    /// @needwork = ad
    /// @endcode
    ~PCtxEncryptorPublic() noexcept override = default;
    /// @brief default move constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00437
    /// @trace_id_dd=DD_CRYPTO_01825
    /// @needwork = ad
    /// @endcode
    PCtxEncryptorPublic(PCtxEncryptorPublic&& other) = delete;
    /// @brief default copy constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00438
    /// @trace_id_dd=DD_CRYPTO_01826
    /// @needwork = ad
    /// @endcode
    PCtxEncryptorPublic(PCtxEncryptorPublic const& other) = delete;
    /// @brief default move assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00439
    /// @trace_id_dd=DD_CRYPTO_01827
    /// @needwork = ad
    /// @endcode
    PCtxEncryptorPublic& operator=(PCtxEncryptorPublic&& other) = delete;
    /// @brief default copy assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00440
    /// @trace_id_dd=DD_CRYPTO_01828
    /// @needwork = ad
    /// @endcode
    PCtxEncryptorPublic& operator=(PCtxEncryptorPublic const& other) = delete;

public:  // CryptoContext interface
    /// @brief Returns the CryptoPrimitivId instance containing the instance identifier.
    /// @name GetCryptoPrimitiveId
    /// @returns CryptoPrimitivId instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00441
    /// @trace_id_dd=DD_CRYPTO_01829
    /// @needwork = ad
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override = 0;
    /// @brief Check whether the encryption context is initialized and usable. It checks all required values, including: key value, IV/seed, etc.
    /// @name   IsInitialized
    /// @returns true if has already init false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00442
    /// @trace_id_dd=DD_CRYPTO_01830
    /// @needwork = ad
    /// @endcode
    bool IsInitialized() const noexcept override;
    /// @brief Get a reference to the Crypto Provider for this context.
    /// @name MyProvider
    /// @returns Crypto provider reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00443
    /// @trace_id_dd=DD_CRYPTO_01831
    /// @needwork = ad
    /// @endcode
    CryptoProvider& MyProvider() const noexcept override;
    /// @brief Get the public key.
    /// @name GetPublicKey
    /// @returns Public key pointer instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00444
    /// @trace_id_dd=DD_CRYPTO_01832
    /// @needwork = ad
    /// @endcode
    inline virtual PublicKey const* GetPublicKey() const noexcept { return pPublicKey_; }

public:  // EncryptorPublicCtx interface
    /// @brief Get the CryptoService instance.
    /// @name  GetCryptoService
    /// @returns CryptoService instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00445
    /// @trace_id_dd=DD_CRYPTO_01833
    /// @needwork = ad
    /// @endcode
    CryptoService::Uptr GetCryptoService() const noexcept override = 0;
    /// @brief Process an input block (encrypt/decrypt) according to the cipher configuration.
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_21012}
    /// @uptrace={RS_CRYPTO_02202}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00446
    /// @trace_id_dd=DD_CRYPTO_01834
    /// @needwork = ad
    /// @endcode
    ara::core::Result< ara::core::Vector< ara::core::Byte > >
    /// @brief Process (encrypt / decrypt) an input block according to the cryptor configuration.
    ///         Encryption with (suppressPadding == true) expects that: in.size() == GetMaxInputSize(true) && out.size()
    ///         >= GetMaxOutputSize(true). Encryption with (suppressPadding == false) expects that: in.size() <=
    ///         GetMaxInputSize(false) && in.size() > 0 && out.size() >= GetMaxOutputSize(false). Decryption expects
    ///         that: in.size() == GetMaxInputSize() && out.size() >= GetMaxOutputSize(suppressPadding). The case
    ///         (out.size() < GetMaxOutputSize()) should be used with caution, only if you are strictly certain about
    ///         the size of the output data! In case of (suppressPadding == true) the actual size of plain text should
    ///         be equal to full size of the plain data block (defined by the algorithm)!
    /// @param in  the input data block
    /// @param suppressPadding  if @c true then the method doesn't apply the padding, but the payload should fill
    ///     the whole block of the plain data
    /// @returns actual size of output data (it always <tt><= out.size()</tt>) or 0 if the input data block has
    ///     incorrect content
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kIncorrectInputSize     if the mentioned above rules about the input size is
    ///     violated
    /// @error: SecurityErrorDomain::kUninitializedContext   if the context was not initialized by a key value
    /// @endcode
    ProcessBlock(ReadOnlyMemRegion const& in, bool suppressPadding = false) const noexcept override;
    /// @brief Use base class template member function
    using EncryptorPublicCtx::ProcessBlock;
    /// @brief Clear the encryption context.
    /// @brief Clear the crypto context.
    /// @return has value if reset sucess false otherwise
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_21011}
    /// @uptrace={RS_CRYPTO_02202}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00447
    /// @trace_id_dd=DD_CRYPTO_01835
    /// @needwork = ad
    /// @endcode
    ara::core::Result< void > Reset() noexcept override;
    /// @brief Set (deploy) a key for the encryptor public algorithm context.
    /// @brief Set (deploy) a key to the encryptor public algorithm context.
    /// @param key  the source key object
    /// @returns has value if SetKey sucess false otherwise
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_21010}
    /// @uptrace={RS_CRYPTO_02001}
    /// @uptrace={RS_CRYPTO_02003}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @error: SecurityErrorDomain::kIncompatibleObject
    ///     if the provided key object is incompatible with this symmetric key context
    /// @error: SecurityErrorDomain::kUsageViolation
    ///      if the transformation type associated with this context is prohibited by the "allowed usage" restrictions
    ///      of provided key object
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00448
    /// @trace_id_dd=DD_CRYPTO_01836
    /// @needwork = ad
    /// @endcode
    ara::core::Result< void > SetKey(PublicKey const& key) noexcept override;

public:  // PServiceCrypto interface
    /// @brief Get the actual bit length of the key loaded into the context. If no key is set for the context, returns 0.
    /// @name  GetActualKeyBitLength
    /// @returns Actual bit length of the key set for the context
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00449
    /// @trace_id_dd=DD_CRYPTO_01837
    /// @needwork = ad
    /// @endcode
    virtual std::size_t GetActualKeyBitLength() const noexcept;
    /// @brief Get the COUID of the key deployed to the context attached to this extended service. If no key is set for the context, returns an empty COUID (Nil).
    /// @name   GetActualKeyCOUID
    /// @returns COUID of the key set for the context
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00450
    /// @trace_id_dd=DD_CRYPTO_01838
    /// @needwork = ad
    /// @endcode
    virtual CryptoObjectUid GetActualKeyCOUID() const noexcept;
    /// @brief Get the allowed usage of this context (according to the key object properties loaded into this context).
    /// If the context has not been initialized with a key object, must return 0 (all flags reset).
    /// @name   GetAllowedUsage
    /// @returns Allowed usage
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00451
    /// @trace_id_dd=DD_CRYPTO_01839
    /// @needwork = ad
    /// @endcode
    virtual AllowedUsageFlags GetAllowedUsage() const noexcept;
    /// @brief Get the maximum supported key length (in bits).
    /// @name   GetMaxKeyBitLength
    /// @returns Maximum key length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00452
    /// @trace_id_dd=DD_CRYPTO_01840
    /// @needwork = ad
    /// @endcode
    virtual std::size_t GetMaxKeyBitLength() const noexcept = 0;
    /// @brief Get the minimum supported key length (in bits).
    /// @name   GetMinKeyBitLength
    /// @returns Minimum key length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00453
    /// @trace_id_dd=DD_CRYPTO_01841
    /// @needwork = ad
    /// @endcode
    virtual std::size_t GetMinKeyBitLength() const noexcept = 0;
    /// @brief Validate support for a specific key length according to the context.
    /// @name   IsKeyBitLengthSupported
    /// @param keyBitLength Key length in bits
    /// @returns true if support false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00454
    /// @trace_id_dd=DD_CRYPTO_01842
    /// @needwork = ad
    /// @endcode
    virtual bool IsKeyBitLengthSupported(std::size_t keyBitLength) const noexcept = 0;
    /// @brief Check whether a key is set for this context.
    /// @name   IsKeyAvailable
    /// @returns ture if already set key false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00455
    /// @trace_id_dd=DD_CRYPTO_01843
    /// @needwork = ad
    /// @endcode
    virtual bool IsKeyAvailable() const noexcept;
    /// @brief Get the block (or internal buffer) size of the underlying algorithm.
    /// For digest, byte stream cipher and RNG contexts, it is an informational method only used to optimize interface usage.
    /// @name   GetBlockSize
    /// @returns Encryption algorithm block size
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00456
    /// @trace_id_dd=DD_CRYPTO_01844
    /// @needwork = ad
    /// @endcode
    virtual std::size_t GetBlockSize() const noexcept = 0;
    /// @brief Get the maximum expected size of the input data block. The suppressPadding parameter will make it equal to the block size.
    /// @name   GetMaxInputSize
    /// @param suppressPadding Whether to suppress padding
    /// @returns Maximum expected input size
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00457
    /// @trace_id_dd=DD_CRYPTO_01845
    /// @needwork = ad
    /// @endcode
    virtual std::size_t GetMaxInputSize(bool suppressPadding = false) const noexcept;
    /// @brief Get the maximum possible size of the output data block. If (IsEncryption() == true), the value returned by this method is independent of the suppressPadding parameter and will equal the block size.
    /// true)，then the value returned by this method is independent of the suppressPadding parameter and will be equal to the block size.
    /// @name   GetMaxOutputSize
    /// @param suppressPadding Whether to suppress padding
    /// @returns Maximum expected output size
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00458
    /// @trace_id_dd=DD_CRYPTO_01846
    /// @needwork = ad
    /// @endcode
    virtual std::size_t GetMaxOutputSize(bool suppressPadding = false) const noexcept;

protected:
    /// @brief Check whether the key meets requirements.
    /// @name   CheckKey
    /// @param key Public key
    /// @returns ture if check sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01847
    /// @needwork = dda
    /// @endcode
    virtual bool CheckKey(PublicKey const& key) const noexcept = 0;
    /// @brief Perform encryption/decryption.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01848
    /// @needwork = dda
    /// @endcode
    virtual ara::core::Result< ara::core::Vector< ara::core::Byte > >
    /// @name   ExecuteEncryptoLogic
    /// @param pInputData Starting address of input data for algorithm operation
    /// @param nDataLen Data length
    /// @param suppressPadding Whether to suppress padding
    /// @returns
    ExecuteEncryptoLogic(uint8_t const* pInputData, uint32_t nDataLen, bool suppressPadding) const noexcept = 0;
    /// @brief Get key modulus length: different padding schemes in RSA correspond to different values.
    /// @name   GetModulusSize
    /// @param suppressPadding Whether to suppress padding
    /// @returns Key modulus size
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01849
    /// @needwork = dda
    /// @endcode
    virtual uint32_t GetModulusSize(bool suppressPadding) const noexcept = 0;

protected:
    /// @brief Check input parameters, returns the actual required space for output (0 indicates check failure).
    /// @name   _CheckSize
    /// @param in Input data buffer
    /// @param suppressPadding Whether to suppress padding
    /// @returns Actual required space
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01850
    /// @needwork = dda
    /// @endcode
    uint32_t _CheckSize(ReadOnlyMemRegion const& in, bool const suppressPadding) const noexcept;
};
//********************************/
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_PUHUA_CTX_ENCRYPTOR_PUBLIC_H_
