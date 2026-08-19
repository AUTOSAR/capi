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
/// @file       isoft_ctx_symmetric_stream_aes.h
/// @brief      AutoSar-Crypto encryption and decryption module
/// @details    General stream cipher context interface (it covers all operation modes).
/// @date       2022-02-23
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
/// @module_path=/CRYPTO/Default Encryption and Decryption/Symmetric Encryption
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_01002
/// @unit_name=PCtxSymmetricStream_Aes_Base
/// @unit_description=AES-based stream symmetric encryption
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_PUHUA_CTX_SYMMETRIC_STREAM_AES_H_
#define ARA_CRYPTO_CRYP_PUHUA_CTX_SYMMETRIC_STREAM_AES_H_

#include <openssl/aes.h>

#include "ara/crypto/cryp/cryobj/isoft_key_symmetric.h"
#include "ara/crypto/cryp/symmetric/isoft_ctx_symmetric_stream.h"
#include "ara/crypto/openssl/isoft_openssl_encrypt.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
//********************************/
/// @brief Interface for symmetric block cipher context with padding: AES. // Common base class for Aes.ecb, Aes.cbc
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00323
/// @trace_id_dd=DD_CRYPTO_01515
/// @needwork = ad
/// @endcode
class PCtxSymmetricStream_Aes_Base : public PCtxSymmetricStream
{
protected:
/// @brief AES_KEY key resource
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00000
/// @trace_id_dd=DD_CRYPTO_01516
/// @needwork = dda
/// @endcode
#if PUHUA_OPENSSL_IS_11X
    mutable AES_KEY aesKey_{};  // NOLINT
#endif

private:
    /// @brief Key length in bits
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01517
    /// @needwork = dda
    /// @endcode
    uint32_t nKeyBitLength_;

protected:
    /// @brief Parameterized constructor
    /// @param cryptoProvider encryption provider
    /// @param nKeyBitLength key length in bits
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01519
    /// @needwork = dda
    /// @endcode
    explicit PCtxSymmetricStream_Aes_Base(PCryptoProvider& cryptoProvider, uint32_t const nKeyBitLength) noexcept;

public:  // CryptoContext interface
    /// @brief Return the CryptoPrimitivId instance containing the instance identifier.
    /// @name   GetCryptoPrimitiveId
    /// @returns CryptoPrimitivId instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01520
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override = 0;
    /// @brief Get the BlockService instance.
    /// @name   GetBlockService
    /// @returns BlockService instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01521
    /// @needwork = dda
    /// @endcode
    BlockService::Uptr GetBlockService() const noexcept override = 0;
    /// @brief Check the operation mode for bytewise properties.
    /// @name   IsBytewiseMode
    /// @returns true if byte mode false if block mode
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01522
    /// @needwork = dda
    /// @endcode
    bool IsBytewiseMode() const noexcept override;
    /// @brief Clear the encryption context.
    /// @name   Reset
    /// @returns  has value if reset sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01523
    /// @needwork = dda
    /// @endcode
    ara::core::Result< void > Reset() noexcept override;
    /// @brief Set (deploy) a key for the symmetric algorithm context
    /// @name  SetKey
    /// @param key symmetric key
    /// @param transform encryption direction: encrypt or decrypt
    /// @return has value if SetKey sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01524
    /// @needwork = dda
    /// @endcode
    ara::core::Result< void > SetKey(SymmetricKey const& key,
                                     CryptoTransform transform = CryptoTransform::kEncrypt) noexcept override;

public:  // PServiceStream interface
    /// @brief Get the maximum supported key length (in bits).
    /// @name   GetMaxKeyBitLength
    /// @returns maximal supported length of the key in bits
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01525
    /// @needwork = dda
    /// @endcode
    std::size_t GetMaxKeyBitLength() const noexcept override;
    /// @brief Get the minimum supported key length (in bits).
    /// @name   GetMinKeyBitLength
    /// @returns minimal supported length of the key in bits
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01526
    /// @needwork = dda
    /// @endcode
    std::size_t GetMinKeyBitLength() const noexcept override;
    /// @brief Verify support for a specific key length according to the context.
    /// @name   IsKeyBitLengthSupported
    /// @param keyBitLength key length in bits
    /// @returns @c true if provided value of the key length is supported by the context
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01527
    /// @needwork = dda
    /// @endcode
    bool IsKeyBitLengthSupported(std::size_t keyBitLength) const noexcept override;
    /// @brief Check if a key is set for this context.
    /// @name   IsKeyAvailable
    /// @returns true if a key has been set to this context false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01528
    /// @needwork = dda
    /// @endcode
    bool IsKeyAvailable() const noexcept override;
    /// @brief Get the block (or internal buffer) size of the underlying algorithm.
    /// @name   GetBlockSize
    /// @returns block (or internal buffer) size of the underlying algorithm
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01529
    /// @needwork = dda
    /// @endcode
    std::size_t GetBlockSize() const noexcept override;

protected:  // PCtxSymmetricStream interface
    /// @brief Check whether the key meets the requirements
    /// @name   CheckKey
    /// @param key symmetric key
    /// @returns true if check key sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01530
    /// @needwork = dda
    /// @endcode
    bool CheckKey(SymmetricKey const& key) const noexcept override;
    /// @brief Perform initialization operation: via initialization vector
    /// @name   DoInit
    /// @param iv initialization vector
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01531
    /// @needwork = dda
    /// @endcode
    void DoInit(ReadOnlyMemRegion const& iv) noexcept override;
    /// @brief Perform initialization operation: secret seed
    /// @name   DoInit
    /// @param iv initialization secret seed
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01532
    /// @needwork = dda
    /// @endcode
    void DoInit(SecretSeed const& iv) noexcept override;
    /// @brief Compute encryption/decryption, the result is stored in buffOutput_, the return value is the ciphertext length
    /// @name   DoCipherLocal
    /// @param buffOutput output buffer
    /// @param pInputData starting address of input data for algorithm operation
    /// @param nDataLen data length
    /// @param alignedData whether data is aligned
    /// @returns ciphertext data length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01533
    /// @needwork = dda
    /// @endcode
    uint32_t DoCipherLocal(internal::PAutoBuff& buffOutput,
                           uint8_t const* pInputData,
                           uint32_t nDataLen,
                           bool alignedData = false) noexcept override = 0;

protected:
    /// @brief Return the algorithm key length
    /// @name   _GetKeyBitLength
    /// @returns key length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01534
    /// @needwork = dda
    /// @endcode
    inline int32_t _GetKeyBitLength() const noexcept { return static_cast< int32_t >(nKeyBitLength_); }
};
//********************************/
/// @brief Interface for symmetric block cipher context with padding: Aes_IV
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00324
/// @trace_id_dd=DD_CRYPTO_01535
/// @unit_name=PCtxSymmetricStream_Aes_IV
/// @unit_description=AES-based stream symmetric encryption context base class (requires initialization vector)
/// @needwork = ad
/// @endcode
class PCtxSymmetricStream_Aes_IV : public PCtxSymmetricStream_Aes_Base
{
protected:
    /// @brief Initialization vector
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01536
    /// @needwork = dda
    /// @endcode
    uint8_t initVector_[static_cast< u_long >(PAlgId_Symmetric_AesKey::EKeyLen::kAesBlockSize)]{0U};  // NOLINT

private:
    /// @brief Secret seed UUID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01537
    /// @needwork = dda
    /// @endcode
    CryptoObjectUid secretSeedCouId_{};

protected:
    /// @brief Get the length of the initialization vector
    /// @return length of the initialization vector
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01539
    /// @needwork = dda
    /// @endcode
    uint32_t _GetinitVectorLen() const noexcept { return static_cast< uint32_t >(sizeof(initVector_)); }

public:
    /// @brief  Constructor
    /// @param  cryptoProvider
    /// @param  nKeyBitLength
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01540
    /// @needwork = dda
    /// @qac Possibly cannot modify [4268]: Initialization helper can be replaced with a delegator.
    /// @endcode
    explicit PCtxSymmetricStream_Aes_IV(PCryptoProvider& cryptoProvider, uint32_t const nKeyBitLength) noexcept
        : PCtxSymmetricStream_Aes_Base{cryptoProvider, nKeyBitLength}
    {
        _InitVector();
    }

public:  // CryptoContext interface
    /// @brief Clear the encryption context.
    /// @name   Reset
    /// @returns has value if reset sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01541
    /// @needwork = dda
    /// @endcode
    ara::core::Result< void > Reset() noexcept override;

public:  // PServiceStream interface
    /// @brief Get the actual bit length of the IV loaded into the context.
    /// @name   GetActualIvBitLength
    /// @param ivUid UID of the initialization vector (IV)
    /// @returns actual length of the IV (now set to the algorithm context) in bits
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01542
    /// @needwork = dda
    /// @endcode
    std::size_t GetActualIvBitLength(ara::core::Optional< CryptoObjectUid >& ivUid) const noexcept override;
    /// @brief Get the expected default size of the initialization vector (IV) or nonce.
    /// @name   GetIvSize
    /// @returns default expected size of IV in bytes
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01543
    /// @needwork = dda
    /// @endcode
    std::size_t GetIvSize() const noexcept override;
    /// @brief Verify the validity of a specific initialization vector (IV) length.
    /// @name   IsValidIvSize
    /// @param ivSize initialization vector length
    /// @returns  @c true if provided IV length is supported by the algorithm and @c false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01544
    /// @needwork = dda
    /// @endcode
    bool IsValidIvSize(std::size_t ivSize) const noexcept override;

protected:
    /// @brief Perform initialization operation: via initialization vector
    /// @name   DoInit
    /// @param iv initialization vector
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01545
    /// @needwork = dda
    /// @endcode
    void DoInit(ReadOnlyMemRegion const& iv) noexcept override;
    /// @brief Perform initialization operation: via secret seed
    /// @name   DoInit
    /// @param iv initialization secret seed
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01546
    /// @needwork = dda
    /// @endcode
    void DoInit(SecretSeed const& iv) noexcept override;
    /// @brief Initialize initialization vector
    /// @name   _InitVector
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01547
    /// @needwork = dda
    /// @endcode
    void _InitVector() noexcept;  // Initialize initVector_
};
//********************************/
/// @brief Interface for symmetric block cipher context with padding: Aes_CfbOfbBase
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00325
/// @trace_id_dd=DD_CRYPTO_01548
/// @unit_name=PCtxSymmetricStream_Aes_CfbOfbBase
/// @unit_description=AES-based stream symmetric encryption context base class (CFB OFB)
/// @needwork = ad
/// @endcode
class PCtxSymmetricStream_Aes_CfbOfbBase : public PCtxSymmetricStream_Aes_IV
{
public:
    /// @brief Parameterized constructor
    /// @name   PCtxSymmetricStream_Aes_CfbOfbBase
    /// @param cryptoProvider encryption provider
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01549
    /// @needwork = dda
    /// @endcode
    explicit PCtxSymmetricStream_Aes_CfbOfbBase(PCryptoProvider& cryptoProvider) noexcept;

public:
    /// @brief Check the operation mode for bytewise properties.
    /// @name   IsBytewiseMode
    /// @returns  true if byte mode false if block mode
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01550
    /// @needwork = dda
    /// @endcode
    bool IsBytewiseMode() const noexcept override;
    /// @brief Set (deploy) a key for the symmetric algorithm context.
    /// @name   SetKey
    /// @param key symmetric key
    /// @param transform encryption direction: encrypt or decrypt
    /// @return has value if SetKey sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01551
    /// @needwork = dda
    /// @endcode
    ara::core::Result< void > SetKey(SymmetricKey const& key,
                                     CryptoTransform transform = CryptoTransform::kEncrypt) noexcept override;

public:  // PServiceStream interface
    /// @brief Verify support for a specific key length according to the context.
    /// @name   IsKeyBitLengthSupported
    /// @param keyBitLength key length in bits
    /// @returns   @c true if provided value of the key length is supported by the context
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01552
    /// @needwork = dda
    /// @endcode
    bool IsKeyBitLengthSupported(std::size_t keyBitLength) const noexcept override;
};
//***************/
/// @brief Interface for symmetric block cipher context with padding: Aes_Cfb1
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00326
/// @trace_id_dd=DD_CRYPTO_01553
/// @unit_name=PCtxSymmetricStream_Aes_Cfb1
/// @unit_description=AES-CFB1 based stream symmetric encryption context
/// @needwork = ad
/// @endcode
class PCtxSymmetricStream_Aes_Cfb1 : public PCtxSymmetricStream_Aes_CfbOfbBase
{
public:
    /// @brief Use base class constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06289
    /// @needwork = dda
    /// @endcode
    using PCtxSymmetricStream_Aes_CfbOfbBase::PCtxSymmetricStream_Aes_CfbOfbBase;

public:  // AUTOSAR-AP interface
    /// @brief Get the block (or internal buffer) size of the underlying algorithm.
    /// @name   GetBlockSize
    /// @returns block (or internal buffer) size of the algorithm.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01554
    /// @needwork = dda
    /// @endcode
    std::size_t GetBlockSize() const noexcept override;
    /// @brief Return the CryptoPrimitivId instance containing the instance identifier.
    /// @name   GetCryptoPrimitiveId
    /// @returns CryptoPrimitivId instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01555
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override;
    /// @brief Get the BlockService instance.
    /// @name   GetBlockService
    /// @returns BlockService instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01556
    /// @needwork = dda
    /// @endcode
    BlockService::Uptr GetBlockService() const noexcept override;

protected:
    /// @brief Compute encryption/decryption, the result is stored in buffOutput_, the return value is the ciphertext length
    /// @name   DoCipherLocal
    /// @param buffOutput output buffer
    /// @param pInputData starting address of input data for algorithm operation
    /// @param nDataLen data length
    /// @param alignedData whether data is aligned
    /// @returns ciphertext length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01557
    /// @needwork = dda
    /// @endcode
    uint32_t DoCipherLocal(internal::PAutoBuff& buffOutput,
                           uint8_t const* pInputData,
                           uint32_t nDataLen,
                           bool alignedData = false) noexcept override;
    /// @brief Get the IPC function name corresponding to encryption/decryption
    /// @name    GetIpcFuncNameDoCipher
    /// @returns the IPC function name corresponding to encryption/decryption
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01558
    /// @needwork = dda
    /// @endcode
    ara::core::StringView GetIpcFuncNameDoCipher() const noexcept override;
};
//***************/
/// @brief Interface for symmetric block cipher context with padding: Aes_Cfb8
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00327
/// @trace_id_dd=DD_CRYPTO_01559
/// @unit_name=PCtxSymmetricStream_Aes_Cfb8
/// @unit_description=AES-CFB8 based stream symmetric encryption context
/// @needwork = ad
/// @endcode
class PCtxSymmetricStream_Aes_Cfb8 : public PCtxSymmetricStream_Aes_CfbOfbBase
{
public:
    /// @brief Use base class constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06290
    /// @needwork = dda
    /// @endcode
    using PCtxSymmetricStream_Aes_CfbOfbBase::PCtxSymmetricStream_Aes_CfbOfbBase;

public:
    /// @brief Return the CryptoPrimitivId instance containing the instance identifier.
    /// @name   GetCryptoPrimitiveId
    /// @returns CryptoPrimitivId instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01561
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override;
    /// @brief Get the BlockService instance.
    /// @name   GetBlockService
    /// @returns BlockService instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01562
    /// @needwork = dda
    /// @endcode
    BlockService::Uptr GetBlockService() const noexcept override;

protected:
    /// @brief Compute encryption/decryption, the result is stored in buffOutput_, the return value is the ciphertext length
    /// @name   DoCipherLocal
    /// @param buffOutput output buffer
    /// @param pInputData starting address of input data for algorithm operation
    /// @param nDataLen data length
    /// @param alignedData whether data is aligned
    /// @returns ciphertext length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01563
    /// @needwork = dda
    /// @endcode
    uint32_t DoCipherLocal(internal::PAutoBuff& buffOutput,
                           uint8_t const* pInputData,
                           uint32_t nDataLen,
                           bool alignedData = false) noexcept override;
    /// @brief Get the IPC function name corresponding to encryption/decryption
    /// @name   GetIpcFuncNameDoCipher
    /// @returns the IPC function name corresponding to encryption/decryption
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01564
    /// @needwork = dda
    /// @endcode
    ara::core::StringView GetIpcFuncNameDoCipher() const noexcept override;
};
//***************/
/// @brief Interface for symmetric block cipher context with padding: Aes_Cfb128
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00328
/// @trace_id_dd=DD_CRYPTO_01565
/// @unit_name=PCtxSymmetricStream_Aes_Cfb128
/// @unit_description=AES-CFB128 based stream symmetric encryption context
/// @needwork = ad
/// @endcode
class PCtxSymmetricStream_Aes_Cfb128 : public PCtxSymmetricStream_Aes_CfbOfbBase
{
public:
    /// @brief Use base class constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06291
    /// @needwork = dda
    /// @endcode
    using PCtxSymmetricStream_Aes_CfbOfbBase::PCtxSymmetricStream_Aes_CfbOfbBase;

public:  // AUTOSAR-AP interface
    /// @brief Return the CryptoPrimitivId instance containing the instance identifier.
    /// @name   GetCryptoPrimitiveId
    /// @returns CryptoPrimitivId instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01566
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override;
    /// @brief Get the BlockService instance.
    /// @name   GetBlockService
    /// @returns BlockService instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01567
    /// @needwork = dda
    /// @endcode
    BlockService::Uptr GetBlockService() const noexcept override;
    /// @brief Check the operation mode for bytewise properties.
    /// @return true if byte mode false if block mode
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01568
    /// @needwork = dda
    /// @endcode
    bool IsBytewiseMode() const noexcept override;

protected:
    /// @brief Compute encryption/decryption, the result is stored in buffOutput_, the return value is the ciphertext length
    /// @name   DoCipherLocal
    /// @param buffOutput output buffer
    /// @param pInputData starting address of input data for algorithm operation
    /// @param nDataLen data length
    /// @param alignedData whether data is aligned
    /// @returns ciphertext length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01569
    /// @needwork = dda
    /// @endcode
    uint32_t DoCipherLocal(internal::PAutoBuff& buffOutput,
                           uint8_t const* pInputData,
                           uint32_t nDataLen,
                           bool alignedData = false) noexcept override;
    /// @brief Get the IPC function name corresponding to encryption/decryption
    /// @name  GetIpcFuncNameDoCipher
    /// @returns the IPC function name corresponding to encryption/decryption
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01570
    /// @needwork = dda
    /// @endcode
    ara::core::StringView GetIpcFuncNameDoCipher() const noexcept override;
};
//***************/
/// @brief Interface for symmetric block cipher context with padding: Aes_Ofb128
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00329
/// @trace_id_dd=DD_CRYPTO_01571
/// @unit_name=PCtxSymmetricStream_Aes_Ofb128
/// @unit_description=AES-OFB128-based stream symmetric encryption context
/// @needwork = ad
/// @endcode
class PCtxSymmetricStream_Aes_Ofb128 : public PCtxSymmetricStream_Aes_CfbOfbBase
{
public:
    /// @brief Use base class construction.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06292
    /// @needwork = dda
    /// @endcode
    using PCtxSymmetricStream_Aes_CfbOfbBase::PCtxSymmetricStream_Aes_CfbOfbBase;

public:  // AUTOSAR-AP interface
    /// @brief Returns the CryptoPrimitivId instance containing the instance identifier.
    /// @name   GetCryptoPrimitiveId
    /// @returns CryptoPrimitivId instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01572
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override;
    /// @brief Get the BlockService instance.
    /// @name   GetBlockService
    /// @returns BlockService instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01573
    /// @needwork = dda
    /// @endcode
    BlockService::Uptr GetBlockService() const noexcept override;
    /// @brief Check the bytewise property of the operation mode.
    /// @return true if byte mode false if block mode
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01574
    /// @needwork = dda
    /// @endcode
    bool IsBytewiseMode() const noexcept override;

protected:
    /// @brief Get the IPC function name corresponding to encryption/decryption.
    /// @name    GetIpcFuncNameDoCipher
    /// @returns IPC function name corresponding to encryption/decryption
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01575
    /// @needwork = dda
    /// @endcode
    ara::core::StringView GetIpcFuncNameDoCipher() const noexcept override;
    /// @brief Perform encryption/decryption, the result is stored in buffOutput_, return value is the ciphertext length.
    /// @name   DoCipherLocal
    /// @param buffOutput Output buffer
    /// @param pInputData Starting address of input data for algorithm operation
    /// @param nDataLen Data length
    /// @param alignedData Whether data is aligned
    /// @returns Ciphertext length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01576
    /// @needwork = dda
    /// @endcode
    uint32_t DoCipherLocal(internal::PAutoBuff& buffOutput,
                           uint8_t const* pInputData,
                           uint32_t nDataLen,
                           bool alignedData = false) noexcept override;
};
//***************/
/// @brief Interface for symmetric block cipher contexts with padding: Aes_Ctr
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00330
/// @trace_id_dd=DD_CRYPTO_01577
/// @unit_name=PCtxSymmetricStream_Aes_Ctr
/// @unit_description=AES-CTR-based stream symmetric encryption context
/// @needwork = ad
/// @endcode
class PCtxSymmetricStream_Aes_Ctr : public PCtxSymmetricStream_Aes_IV
{
public:
    /// @brief Parameterized constructor
    /// @param cryptoProvider Crypto provider
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01578
    /// @needwork = dda
    /// @endcode
    explicit PCtxSymmetricStream_Aes_Ctr(PCryptoProvider& cryptoProvider) noexcept;

public:  // AUTOSAR-AP interface
    /// @brief Get the block (or internal buffer) size of the underlying algorithm.
    /// @name   GetBlockSize
    /// @returns Block (or internal buffer) size of the algorithm.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01579
    /// @needwork = dda
    /// @endcode
    std::size_t GetBlockSize() const noexcept override;
    /// @brief Returns the CryptoPrimitivId instance containing the instance identifier.
    /// @name   GetCryptoPrimitiveId
    /// @returns CryptoPrimitivId instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01580
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override;
    /// @brief Get the BlockService instance.
    /// @name   GetBlockService
    /// @returns BlockService instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01581
    /// @needwork = dda
    /// @endcode
    BlockService::Uptr GetBlockService() const noexcept override;
    /// @brief Set the key.
    /// @param key Symmetric key
    /// @param transform Encryption direction: encrypt or decrypt
    /// @return has value if SetKey sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01582
    /// @needwork = dda
    /// @endcode
    ara::core::Result< void > SetKey(SymmetricKey const& key, CryptoTransform transform) noexcept override;

protected:
    /// @brief Perform encryption/decryption, the result is stored in buffOutput_, return value is the ciphertext length.
    /// @name   DoCipherLocal
    /// @param buffOutput Output buffer
    /// @param pInputData Starting address of input data for algorithm operation
    /// @param nDataLen Data length
    /// @param alignedData Whether data is aligned
    /// @returns Ciphertext length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01583
    /// @needwork = dda
    /// @endcode
    uint32_t DoCipherLocal(internal::PAutoBuff& buffOutput,
                           uint8_t const* pInputData,
                           uint32_t nDataLen,
                           bool alignedData = false) noexcept override;
    /// @brief Get the IPC function name corresponding to encryption/decryption.
    /// @name    GetIpcFuncNameDoCipher
    /// @returns IPC function name corresponding to encryption/decryption
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01584
    /// @needwork = dda
    /// @endcode
    ara::core::StringView GetIpcFuncNameDoCipher() const noexcept override;
    /// @brief Check the bytewise property of the operation mode.
    /// @name   IsBytewiseMode
    /// @returns true if byte mode false if block mode
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01585
    /// @needwork = dda
    /// @endcode
    bool IsBytewiseMode() const noexcept override;
    /// @brief Check whether seeking is supported in the current mode.
    /// @name  IsSeekableMode
    /// @returns true if support seekable false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01586
    /// @needwork = dda
    /// @endcode
    bool IsSeekableMode() const noexcept override;
    /// @brief Perform Seek operation.
    /// @name   DoSeek
    /// @param offset Offset
    /// @param fromBegin Whether to start from the Begin position
    /// @returns  has value if DoSeek sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01587
    /// @needwork = dda
    /// @endcode
    ara::core::Result< void > DoSeek(std::int64_t offset, bool fromBegin = true) noexcept override;

public:
    /// @brief Get the offset position.
    /// @name   GetOffset
    /// @returns Offset position
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01588
    /// @needwork = dda
    /// @endcode
    int64_t GetOffset() const noexcept override { return offset_; }
    /// @brief Get the current offset position.
    /// @name   GetCurrentPos
    /// @returns Current offset position
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01589
    /// @needwork = dda
    /// @endcode
    uint32_t GetCurrentPos() const noexcept override { return currentPos_; }
    /// @brief Get whether to offset from the beginning.
    /// @name   GetFromBegin
    /// @returns ture from begin false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01590
    /// @needwork = dda
    /// @endcode
    bool GetFromBegin() const noexcept override { return fromBegin_; }
    /// @brief Set the current cursor position.
    /// @param currentPos Current cursor position
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01591
    /// @needwork = dda
    /// @endcode
    void SetCurrentPos(uint32_t currentPos) noexcept override { currentPos_ = currentPos; }
    /// @brief Set the offset.
    /// @param offset Offset
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01592
    /// @needwork = dda
    /// @endcode
    void SetOffset(int64_t offset) noexcept override { offset_ = offset; }
    /// @brief Set offset from the beginning position.
    /// @param fromBegin Whether to start from the Begin position
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01593
    /// @needwork = dda
    /// @endcode
    void SetFromBegin(bool fromBegin) noexcept override { fromBegin_ = fromBegin; }
    /// @brief Get the number of offset bytes from the end position.
    /// @name   GetFinishBytes
    /// @returns Number of offset bytes from the end position
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01594
    /// @needwork = dda
    /// @endcode
    uint32_t GetFinishBytes() const noexcept override { return finishByteOffset_; }
    /// @brief Set the number of offset bytes from the end position.
    /// @param finishBytes Number of offset bytes from the end position
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01595
    /// @needwork = dda
    /// @endcode
    void SetFinishBytes(uint32_t finishBytes) noexcept override { finishByteOffset_ = finishBytes; }

private:
    /// @brief Offset position
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01596
    /// @needwork = dda
    /// @endcode
    std::int64_t offset_{0};
    /// @brief Whether to offset from the beginning
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01597
    /// @needwork = dda
    /// @endcode
    bool fromBegin_{true};
    /// @brief Current position, default start position
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01598
    /// @needwork = dda
    /// @endcode
    std::uint32_t currentPos_{0U};
    /// @brief Number of offset bytes from the end position
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01599
    /// @needwork = dda
    /// @endcode
    std::uint32_t finishByteOffset_{0U};
};
//********************************/
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_PUHUA_CTX_SYMMETRIC_STREAM_AES_H_
