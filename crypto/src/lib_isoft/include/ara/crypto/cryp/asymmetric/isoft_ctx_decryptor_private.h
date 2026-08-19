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
/// @file       isoft_ctx_decryptor_private.h
/// @brief      AutoSar-Crypto encryption/decryption module
/// @details    Asymmetric encryption module. Private key decryption
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
/// @unit_name=PCtxDecryptorPrivate
/// @unit_description=Asymmetric decryption private key context interface base class
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_PUHUA_CTX_DECRYPTOR_PRIVATE_H_
#define ARA_CRYPTO_CRYP_PUHUA_CTX_DECRYPTOR_PRIVATE_H_

#include "ara/crypto/cryp/cryobj/isoft_key_private_ipc.h"
#include "ara/crypto/cryp/cryobj/private_key.h"
#include "ara/crypto/cryp/decryptor_private_ctx.h"
#include "ara/crypto/cryp/isoft_auto_buff.h"
#include "ara/crypto/ipc/isoft_ipc_client.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
/// @brief Crypto provider
class PCryptoProvider;
//********************************/
/// @brief Asymmetric decryption private key context interface.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00461
/// @trace_id_dd=DD_CRYPTO_01880
/// @needwork = ad
/// @endcode
class PCtxDecryptorPrivate : public DecryptorPrivateCtx
{
private:
    /// @brief Crypto manager
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01881
    /// @needwork = dda
    /// @endcode
    PCryptoProvider& cryptoProvider_;
    /// @brief Private key pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01882
    /// @needwork = dda
    /// @endcode
    PrivateKey const* pPrivateKey_;
    /// @brief Input data buffer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01883
    /// @needwork = dda
    /// @endcode
    internal::PAutoBuff buffInput_;
    /// @brief Parameterized constructor
    /// @param cryptoProvider Crypto provider
    /// @param pPrivateKey Private key object pointer
    /// @param buffInput Input buffer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01884
    /// @needwork = dda
    /// @endcode
    PCtxDecryptorPrivate(PCryptoProvider& cryptoProvider,
                         PrivateKey const* const pPrivateKey,
                         internal::PAutoBuff const* const buffInput) noexcept;

public:
    /// @brief Constructor
    /// @name   PCtxDecryptorPrivate
    /// @param cryptoProvider the PCryptoProvider
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00462
    /// @trace_id_dd=DD_CRYPTO_01885
    /// @needwork = ad
    /// @endcode
    explicit PCtxDecryptorPrivate(PCryptoProvider& cryptoProvider) noexcept;
    /// @brief default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00463
    /// @trace_id_dd=DD_CRYPTO_01886
    /// @needwork = ad
    /// @endcode
    PCtxDecryptorPrivate() = delete;
    /// @brief default virtual destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00464
    /// @trace_id_dd=DD_CRYPTO_01887
    /// @needwork = ad
    /// @endcode
    ~PCtxDecryptorPrivate() noexcept override = default;
    /// @brief default move constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00465
    /// @trace_id_dd=DD_CRYPTO_01888
    /// @needwork = ad
    /// @endcode
    PCtxDecryptorPrivate(PCtxDecryptorPrivate&& other) = delete;
    /// @brief default copy constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00466
    /// @trace_id_dd=DD_CRYPTO_01889
    /// @needwork = ad
    /// @endcode
    PCtxDecryptorPrivate(PCtxDecryptorPrivate const& other) = delete;
    /// @brief default move assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00467
    /// @trace_id_dd=DD_CRYPTO_01890
    /// @needwork = ad
    /// @endcode
    PCtxDecryptorPrivate& operator=(PCtxDecryptorPrivate&& other) = delete;
    /// @brief default copy assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00468
    /// @trace_id_dd=DD_CRYPTO_01891
    /// @needwork = ad
    /// @endcode
    PCtxDecryptorPrivate& operator=(PCtxDecryptorPrivate const& other) = delete;

public:  // CryptoContext interface
    /// @brief Returns the CryptoPrimitivId instance containing the instance identifier.
    /// @name   GetCryptoPrimitiveId
    /// @returns CryptoPrimitivId instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00469
    /// @trace_id_dd=DD_CRYPTO_01892
    /// @needwork = ad
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override = 0;
    /// @brief Check whether the encryption context is initialized and usable. It checks all required values, including: key value, IV/seed, etc.
    /// @name   IsInitialized
    /// @returns true if has already init false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00470
    /// @trace_id_dd=DD_CRYPTO_01893
    /// @needwork = ad
    /// @endcode
    bool IsInitialized() const noexcept override;
    /// @brief Get a reference to the Crypto Provider for this context.
    /// @name  MyProvider
    /// @returns Crypto provider instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00471
    /// @trace_id_dd=DD_CRYPTO_01894
    /// @needwork = ad
    /// @endcode
    CryptoProvider& MyProvider() const noexcept override;
    /// @brief Get PrivateKey
    /// @name   GetPrivateKey
    /// @returns Private key instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00472
    /// @trace_id_dd=DD_CRYPTO_01895
    /// @needwork = ad
    /// @endcode
    inline virtual PrivateKey const* GetPrivateKey() const noexcept { return pPrivateKey_; }

public:  // DecryptorPrivateCtx interface
    /// @brief Get the CryptoService instance.
    /// @brief Get CryptoService instance.
    /// @name  GetCryptoService
    /// @returns CryptoService instance
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_20802}
    /// @uptrace={RS_CRYPTO_02006}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00473
    /// @trace_id_dd=DD_CRYPTO_01896
    /// @needwork = ad
    /// @endcode
    CryptoService::Uptr GetCryptoService() const noexcept override = 0;
    /// @brief Process an input block (encrypt/decrypt) according to the cipher configuration.
    /// @brief Process (encrypt / decrypt) an input block according to the cryptor configuration.
    ///         Encryption with (suppressPadding == true) expects that: in.size() == GetMaxInputSize(true) && out.size()
    ///         >= GetMaxOutputSize(true). Encryption with (suppressPadding == false) expects that: in.size() <=
    ///         GetMaxInputSize(false) && in.size() > 0 && out.size() >= GetMaxOutputSize(false). Decryption expects
    ///         that: in.size() == GetMaxInputSize() && out.size() >= GetMaxOutputSize(suppressPadding). The case
    ///         (out.size() < GetMaxOutputSize()) should be used with caution, only if you are strictly certain about
    ///         the size of the output data! In case of (suppress Padding == true) the actual size of plain text should
    ///         be equal to full size of the plain data block (defined by the algorithm)!
    /// @param in  the input data block
    /// @param suppressPadding  if @c true then the method doesn't apply the padding, but the payload should fill
    ///     the whole block of the plain data
    /// @returns actual size of output data (it always <tt><= out.size()</tt>) or 0 if the input data block has
    ///     incorrect content
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_20812}
    /// @uptrace={RS_CRYPTO_02202}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @error: SecurityErrorDomain::kIncorrectInputSize     if the mentioned above rules about the input size is violated
    /// @error: SecurityErrorDomain::kInsufficientCapacity   if the @c out.size() is not enough to store the transformation result
    /// @error: SecurityErrorDomain::kUninitializedContext   if the context was not initialized by a key value
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00474
    /// @trace_id_dd=DD_CRYPTO_01897
    /// @needwork = ad
    /// @endcode
    // PRQA S 2024 QAC /// @qac: false positive
    // PRQA L:QAC
    ara::core::Result< ara::core::Vector< ara::core::Byte > > ProcessBlock(ReadOnlyMemRegion const& in,
                                                                           bool suppressPadding
                                                                           = false) const noexcept override;
    /// @brief Use base class template member function
    using DecryptorPrivateCtx::ProcessBlock;
    /// @brief Clear the encryption context.
    /// @brief Clear the crypto context.
    /// @name  Reset
    /// @returns has value if reset sucess false otherwise
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_20811}
    /// @uptrace={RS_CRYPTO_02202}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00475
    /// @trace_id_dd=DD_CRYPTO_01898
    /// @needwork = ad
    /// @endcode
    ara::core::Result< void > Reset() noexcept override;
    /// @brief Set (deploy) a key for the decryptor private algorithm context.
    /// @brief Set (deploy) a key to the decryptor private algorithm context.
    /// @name  SetKey
    /// @param key  the source key object
    /// @returns has value if SetKey sucess false otherwise
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_20810}
    /// @uptrace={RS_CRYPTO_02001}
    /// @uptrace={RS_CRYPTO_02003}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @error: SecurityErrorDomain::kIncompatibleObject  if the provided key object is incompatible with this symmetric
    ///     key context
    /// @error: SecurityErrorDomain::kUsageViolation  if the transformation type associated with this context is
    ///     prohibited by the "allowed usage" restrictions of provided key object
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00476
    /// @trace_id_dd=DD_CRYPTO_01899
    /// @needwork = ad
    /// @endcode
    ara::core::Result< void > SetKey(PrivateKey const& key) noexcept override;

public:  // PServiceCrypto interface
    /// @brief Get the actual bit length of the key loaded into the context. If no key is set for the context, returns 0.
    /// @name  GetActualKeyBitLength
    /// @returns Length of the key set for the context
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00477
    /// @trace_id_dd=DD_CRYPTO_01900
    /// @needwork = ad
    /// @endcode
    virtual std::size_t GetActualKeyBitLength() const noexcept;
    /// @brief Get the COUID of the key deployed to the context attached to this extended service. If no key is set for the context, returns an empty COUID (Nil).
    /// @name  GetActualKeyCOUID
    /// @returns COUID of the key set for the context
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00478
    /// @trace_id_dd=DD_CRYPTO_01901
    /// @needwork = ad
    /// @endcode
    virtual CryptoObjectUid GetActualKeyCOUID() const noexcept;
    /// @brief Get the allowed usage of this context (according to the key object properties loaded into this context).
    ///        If the context has not been initialized with a key object, must return 0 (all flags reset).
    /// @name  GetAllowedUsage
    /// @returns Key object allowed usage
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00479
    /// @trace_id_dd=DD_CRYPTO_01902
    /// @needwork = ad
    /// @endcode
    virtual AllowedUsageFlags GetAllowedUsage() const noexcept;
    /// @brief Get the maximum supported key length (in bits).
    /// @name GetMaxKeyBitLength
    /// @returns Maximum supported key length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00480
    /// @trace_id_dd=DD_CRYPTO_01903
    /// @needwork = ad
    /// @endcode
    virtual std::size_t GetMaxKeyBitLength() const noexcept = 0;
    /// @brief Get the minimum supported key length (in bits).
    /// @name GetMinKeyBitLength
    /// @returns Minimum supported key length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00481
    /// @trace_id_dd=DD_CRYPTO_01904
    /// @needwork = ad
    /// @endcode
    virtual std::size_t GetMinKeyBitLength() const noexcept = 0;
    /// @brief Validate support for a specific key length according to the context.
    /// @name   IsKeyBitLengthSupported
    /// @param keyBitLength Key length in bits
    /// @returns true if key support false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00482
    /// @trace_id_dd=DD_CRYPTO_01905
    /// @needwork = ad
    /// @endcode
    virtual bool IsKeyBitLengthSupported(std::size_t keyBitLength) const noexcept;
    /// @brief Check whether a key is set for this context.
    /// @name   IsKeyAvailable
    /// @returns true if has already set key false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00483
    /// @trace_id_dd=DD_CRYPTO_01906
    /// @needwork = ad
    /// @endcode
    virtual bool IsKeyAvailable() const noexcept;
    /// @brief Get the block (or internal buffer) size of the underlying algorithm. For digest, byte stream cipher and RNG contexts, it is an informational method only used to optimize interface usage.
    /// @name GetBlockSize
    /// @returns Algorithm block size
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00484
    /// @trace_id_dd=DD_CRYPTO_01907
    /// @needwork = ad
    /// @endcode
    virtual std::size_t GetBlockSize() const noexcept = 0;
    /// @brief Get the maximum expected size of the input data block. The suppressPadding parameter will make it equal to the block size.
    /// @name   GetMaxInputSize
    /// @param suppressPadding Whether to suppress padding
    /// @returns Maximum expected size
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00485
    /// @trace_id_dd=DD_CRYPTO_01908
    /// @needwork = ad
    /// @endcode
    virtual std::size_t GetMaxInputSize(bool suppressPadding = false) const noexcept;
    /// @brief Get the maximum possible size of the output data block. If (IsEncryption() == true), the value returned by this method is independent of the suppressPadding parameter and will equal the block size.
    /// ==true), then the value returned by this method is independent of the suppressPadding parameter and will be equal to the block size.
    /// @name   GetMaxOutputSize
    /// @param suppressPadding Whether to suppress padding
    /// @returns Maximum possible output block size
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00486
    /// @trace_id_dd=DD_CRYPTO_01909
    /// @needwork = ad
    /// @endcode
    virtual std::size_t GetMaxOutputSize(bool suppressPadding = false) const noexcept;

protected:
    /// @brief Check whether the key meets requirements.
    /// @name   CheckKey
    /// @param key Private key
    /// @returns true if check sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01910
    /// @needwork = dda
    /// @endcode
    virtual bool CheckKey(PrivateKey const& key) const noexcept = 0;
    /// @brief Perform encryption/decryption.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01911
    /// @needwork = dda
    /// @endcode
    virtual ara::core::Result< ara::core::Vector< ara::core::Byte > >
    /// @name   ExecuteDecryptoLogic
    /// @param pInputData Starting address of input data for algorithm operation
    /// @param nDataLen Data length
    /// @param suppressPadding Whether to suppress padding
    /// @returns
    ExecuteDecryptoLogic(uint8_t const* pInputData, uint32_t nDataLen, bool suppressPadding) const noexcept = 0;
};
//********************************/
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_PUHUA_CTX_DECRYPTOR_PRIVATE_H_
