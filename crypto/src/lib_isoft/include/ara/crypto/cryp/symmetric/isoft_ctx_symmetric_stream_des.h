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
/// @file       isoft_ctx_symmetric_stream_des.h
/// @brief      AutoSar-Crypto encryption/decryption module
/// @details    General stream cipher context interface (covering all operation modes).
/// @date       2022-02-16
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <table>}<2022-02-16  </td>1.0.0    <td>hanjingjing      <tr>Create initial version</table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default encryption/Symmetric encryption
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_01004
/// @unit_name=PCtxSymmetricStream_Des_Base
/// @unit_description=DES-based stream symmetric encryption base class
/// @endcode
///
/// ================================================================
///
/// </table>
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_PUHUA_CTX_SYMMETRIC_STREAM_DES_H_
#define ARA_CRYPTO_CRYP_PUHUA_CTX_SYMMETRIC_STREAM_DES_H_

#include <openssl/des.h>

#include "ara/crypto/common/isoft_data_type.h"
#include "ara/crypto/cryp/symmetric/isoft_ctx_symmetric_stream.h"
#include "ara/crypto/openssl/isoft_openssl_encrypt.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
//********************************/
/// @brief General stream cipher context interface: DES base class
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00311
/// @trace_id_dd=DD_CRYPTO_01442
/// @needwork = ad
/// @endcode
class PCtxSymmetricStream_Des_Base : public PCtxSymmetricStream
{
protected:
/// @brief DES key
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00000
/// @trace_id_dd=DD_CRYPTO_01443
/// @needwork = dda
/// @endcode
// PRQA S 2028,2428 QAC /// @qac: Introduced from openssl library, cannot be modified
#if PUHUA_OPENSSL_IS_11X
    mutable DES_key_schedule desKeySchedule_{};  // NOLINT
#else
    mutable u_char desKeySchedule_[8];  // NOLINT
#endif
    // PRQA L:QAC

protected:
    /// @brief Initialization vector
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01444
    /// @needwork = dda
    /// @endcode
    // PRQA S 2028,2428 QAC /// @qac: Introduced from openssl library, cannot be modified
    DES_cblock initVector_{0U};  // NOLINT
    // PRQA L:QAC

public:
/// @brief Get the DES key.
/// @return DES_key_schedule pointer
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00000
/// @trace_id_dd=DD_CRYPTO_01446
/// @needwork = dda
/// @endcode
#if PUHUA_OPENSSL_IS_11X
    DES_key_schedule const* GetPDesKeySchedule1() const noexcept { return &desKeySchedule_; }
#else
    u_char const* GetPDesKeySchedule1() const noexcept { return desKeySchedule_; }
#endif

protected:
    /// @brief Get the length of the initialization vector.
    /// @return Length of the initialization vector
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01449
    /// @needwork = dda
    /// @endcode
    uint32_t _GetinitVectorLen() const noexcept { return static_cast< uint32_t >(sizeof(initVector_)); }

public:
    /// @brief Parameterized constructor
    /// @param cryptoProvider Crypto provider
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01450
    /// @needwork = dda
    /// @endcode
    explicit PCtxSymmetricStream_Des_Base(PCryptoProvider& cryptoProvider) noexcept;

public:  // AUTOSAR-AP interface
    /// @brief Returns the CryptoPrimitivId instance containing the instance identifier.
    /// @name   GetCryptoPrimitiveId
    /// @returns CryptoPrimitivId instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01451
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override = 0;
    /// @brief Get the BlockService instance.
    /// @name   GetBlockService
    /// @returns BlockService instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01452
    /// @needwork = dda
    /// @endcode
    BlockService::Uptr GetBlockService() const noexcept override = 0;
    /// @brief Check the bytewise property of the operation mode.
    /// @name   IsBytewiseMode
    /// @returns true if byte mode false if block mode
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01453
    /// @needwork = dda
    /// @endcode
    bool IsBytewiseMode() const noexcept override;
    /// @brief Clear the encryption context.
    /// @name   Reset
    /// @returns has value if reset sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01454
    /// @needwork = dda
    /// @endcode
    ara::core::Result< void > Reset() noexcept override;
    /// @brief Set (deploy) a key to the symmetric algorithm context.
    /// @name   SetKey
    /// @param key Symmetric key
    /// @param transform Encryption direction: encrypt or decrypt
    /// @return has value if SetKey sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01455
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
    /// @trace_id_dd=DD_CRYPTO_01456
    /// @needwork = dda
    /// @endcode
    std::size_t GetMaxKeyBitLength() const noexcept override;
    /// @brief Get the minimum supported key length (in bits).
    /// @name   GetMinKeyBitLength
    /// @returns minimal supported length of the key in bits
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01457
    /// @needwork = dda
    /// @endcode
    std::size_t GetMinKeyBitLength() const noexcept override;
    /// @brief Get the block (or internal buffer) size of the underlying algorithm.
    /// @name   GetBlockSize
    /// @returns Block (or internal buffer) size of the underlying algorithm
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01458
    /// @needwork = dda
    /// @endcode
    std::size_t GetBlockSize() const noexcept override;
    /// @brief Get the default expected size of the initialization vector (IV) or nonce.
    /// @brief Get default expected size of the Initialization Vector (IV) or nonce.
    /// @returns default expected size of IV in bytes
    /// @name   GetIvSize
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_29032}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00312
    /// @trace_id_dd=DD_CRYPTO_01459
    /// @needwork = ad
    /// @endcode
    std::size_t GetIvSize() const noexcept override;
    /// @brief Validate the validity of a specific initialization vector (IV) length.
    /// @brief Verify validity of specific Initialization Vector (IV) length.
    /// @name   IsValidIvSize
    /// @param ivSize  the length of the IV in bytes
    /// @returns @c true if provided IV length is supported by the algorithm and @c false otherwise
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_29034}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00313
    /// @trace_id_dd=DD_CRYPTO_01460
    /// @needwork = ad
    /// @endcode
    bool IsValidIvSize(std::size_t ivSize) const noexcept override;
    /// @brief Get the actual bit length of the IV loaded into the context.
    /// @brief Get actual bit-length of an IV loaded to the context.
    /// @param ivUid  optional pointer to a buffer for saving an @a COUID of a IV object now loaded to the context.
    /// If the context was initialized by a @c SecretSeed object then the output buffer @c *ivUid must be filled
    ///       by @a COUID of this loaded IV object, in other cases @c *ivUid must be filled by all zeros.
    /// @returns actual length of the IV (now set to the algorithm context) in bits
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_29035}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00314
    /// @trace_id_dd=DD_CRYPTO_01461
    /// @needwork = ad
    /// @endcode
    std::size_t GetActualIvBitLength(ara::core::Optional< CryptoObjectUid >& ivUid) const noexcept override;

protected:  // PCtxSymmetricStream interface
    /// @brief Check whether the key meets requirements.
    /// @name   CheckKey
    /// @param key Symmetric key
    /// @returns true if check key sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01462
    /// @needwork = dda
    /// @endcode
    bool CheckKey(SymmetricKey const& key) const noexcept override;
    /// @brief Perform initialization operation: using initialization vector.
    /// @name   DoInit
    /// @param iv Initialization vector
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01463
    /// @needwork = dda
    /// @endcode
    void DoInit(ReadOnlyMemRegion const& iv) noexcept override;
    /// @brief Perform initialization operation: using secret seed.
    /// @name   DoInit
    /// @param iv Initialization secret seed
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01464
    /// @needwork = dda
    /// @endcode
    void DoInit(SecretSeed const& iv) noexcept override;
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
    /// @trace_id_dd=DD_CRYPTO_01465
    /// @needwork = dda
    /// @endcode
    uint32_t DoCipherLocal(internal::PAutoBuff& buffOutput,
                           uint8_t const* pInputData,
                           uint32_t nDataLen,
                           bool alignedData = false) noexcept override = 0;
    /// @brief Initialize the initialization vector.
    /// @name   _InitVector
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01466
    /// @needwork = dda
    /// @endcode
    void _InitVector() noexcept;

public:
};
//***************/  //DES.CFB
/// @brief General stream cipher context interface: Des_Cfb
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00315
/// @trace_id_dd=DD_CRYPTO_01467
/// @unit_name=PCtxSymmetricStream_Des_Cfb
/// @unit_description=DES-CFB-based stream symmetric encryption
/// @needwork = ad
/// @endcode
class PCtxSymmetricStream_Des_Cfb : public PCtxSymmetricStream_Des_Base
{
public:
    /// @brief Use base class constructor.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06281
    /// @needwork = dda
    /// @endcode
    using PCtxSymmetricStream_Des_Base::PCtxSymmetricStream_Des_Base;

public:  // AUTOSAR-AP interface
    /// @brief Returns the CryptoPrimitivId instance containing the instance identifier.
    /// @name   GetCryptoPrimitiveId
    /// @returns CryptoPrimitivId instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01468
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override;
    /// @brief Get the BlockService instance.
    /// @name   GetBlockService
    /// @returns BlockService instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01469
    /// @needwork = dda
    /// @endcode
    BlockService::Uptr GetBlockService() const noexcept override;
    /// @brief Check the bytewise property of the operation mode.
    /// @name   IsBytewiseMode
    /// @returns true if byte mode false if block mode
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01453
    /// @needwork = dda
    /// @endcode
    bool IsBytewiseMode() const noexcept override;

protected:  // PCtxSymmetricStream interface
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
    /// @trace_id_dd=DD_CRYPTO_01470
    /// @needwork = dda
    /// @endcode
    uint32_t DoCipherLocal(internal::PAutoBuff& buffOutput,
                           uint8_t const* pInputData,
                           uint32_t nDataLen,
                           bool alignedData = false) noexcept override;

protected:
    /// @brief Get the IPC function name corresponding to encryption/decryption.
    /// @name    GetIpcFuncNameDoCipher
    /// @returns IPC function name corresponding to encryption/decryption
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01471
    /// @needwork = dda
    /// @endcode
    ara::core::StringView GetIpcFuncNameDoCipher() const noexcept override;
};
//***************/  //DES.Cfb64
/// @brief General stream cipher context interface: Des_Cfb64
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00316
/// @trace_id_dd=DD_CRYPTO_01472
/// @unit_name=PCtxSymmetricStream_Des_Cfb64
/// @unit_description=DES-CFB64-based stream symmetric encryption
/// @needwork = ad
/// @endcode
class PCtxSymmetricStream_Des_Cfb64 : public PCtxSymmetricStream_Des_Base
{
public:
    /// @brief Use base class constructor.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06282
    /// @needwork = dda
    /// @endcode
    using PCtxSymmetricStream_Des_Base::PCtxSymmetricStream_Des_Base;

public:  // AUTOSAR-AP interface
    /// @brief Returns the CryptoPrimitivId instance containing the instance identifier.
    /// @name   GetCryptoPrimitiveId
    /// @returns CryptoPrimitivId instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01473
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override;
    /// @brief Get the BlockService instance.
    /// @name   GetBlockService
    /// @returns BlockService instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01474
    /// @needwork = dda
    /// @endcode
    BlockService::Uptr GetBlockService() const noexcept override;

protected:  // PCtxSymmetricStream interface
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
    /// @trace_id_dd=DD_CRYPTO_01475
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
    /// @trace_id_dd=DD_CRYPTO_01476
    /// @needwork = dda
    /// @endcode
    ara::core::StringView GetIpcFuncNameDoCipher() const noexcept override;
};
//***************/  //DES.OFB
/// @brief General stream cipher context interface: Des_Ofb
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00317
/// @trace_id_dd=DD_CRYPTO_01477
/// @unit_name=PCtxSymmetricStream_Des_Ofb
/// @unit_description=DES-OFB-based stream symmetric encryption
/// @needwork = ad
/// @endcode
class PCtxSymmetricStream_Des_Ofb : public PCtxSymmetricStream_Des_Base
{
public:
    /// @brief Use base class constructor.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06283
    /// @needwork = dda
    /// @endcode
    using PCtxSymmetricStream_Des_Base::PCtxSymmetricStream_Des_Base;

public:  // AUTOSAR-AP interface
    /// @brief Returns the CryptoPrimitivId instance containing the instance identifier.
    /// @name   GetCryptoPrimitiveId
    /// @returns CryptoPrimitivId instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01478
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override;
    /// @brief Get the BlockService instance.
    /// @name   GetBlockService
    /// @returns BlockService instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01479
    /// @needwork = dda
    /// @endcode
    BlockService::Uptr GetBlockService() const noexcept override;
    /// @brief Check the bytewise property of the operation mode.
    /// @name   IsBytewiseMode
    /// @returns true if byte mode false if block mode
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01453
    /// @needwork = dda
    /// @endcode
    bool IsBytewiseMode() const noexcept override;

protected:  // PCtxSymmetricStream interface
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
    /// @trace_id_dd=DD_CRYPTO_01480
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
    /// @trace_id_dd=DD_CRYPTO_01481
    /// @needwork = dda
    /// @endcode
    ara::core::StringView GetIpcFuncNameDoCipher() const noexcept override;
};
//***************/  //DES.OFB64
/// @brief General stream cipher context interface: Des_Ofb64
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00318
/// @trace_id_dd=DD_CRYPTO_01482
/// @unit_name=PCtxSymmetricStream_Des_Ofb64
/// @unit_description=DES-OFB64-based stream symmetric encryption
/// @needwork = ad
/// @endcode
class PCtxSymmetricStream_Des_Ofb64 : public PCtxSymmetricStream_Des_Base
{
public:
    /// @brief Use base class constructor.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06284
    /// @needwork = dda
    /// @endcode
    using PCtxSymmetricStream_Des_Base::PCtxSymmetricStream_Des_Base;

public:  // AUTOSAR-AP interface
    /// @brief Returns the CryptoPrimitivId instance containing the instance identifier.
    /// @name   GetCryptoPrimitiveId
    /// @returns CryptoPrimitivId instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01483
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override;
    /// @brief Get the BlockService instance.
    /// @name   GetBlockService
    /// @returns BlockService instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01484
    /// @needwork = dda
    /// @endcode
    BlockService::Uptr GetBlockService() const noexcept override;

protected:  // PCtxSymmetricStream interface
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
    /// @trace_id_dd=DD_CRYPTO_01485
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
    /// @trace_id_dd=DD_CRYPTO_01486
    /// @needwork = dda
    /// @endcode
    ara::core::StringView GetIpcFuncNameDoCipher() const noexcept override;
};
//********************************/
/// @brief General stream cipher context interface: 3Des_Base
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00319
/// @trace_id_dd=DD_CRYPTO_01487
/// @unit_name=PCtxSymmetricStream_3Des_Base
/// @unit_description=3DES-based stream symmetric encryption base class
/// @needwork = ad
/// @endcode
class PCtxSymmetricStream_3Des_Base : public PCtxSymmetricStream_Des_Base
{
protected:
/// @brief Second symmetric key
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00000
/// @trace_id_dd=DD_CRYPTO_01488
/// @needwork = dda
/// @endcode
#if PUHUA_OPENSSL_IS_11X
    mutable DES_key_schedule desKeySchedule2_{};  // NOLINT subclass still uses it
#else
    mutable u_char desKeySchedule2_[8];  // NOLINT subclass still uses it
#endif
/// @brief Third symmetric key
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00000
/// @trace_id_dd=DD_CRYPTO_01489
/// @needwork = dda
/// @endcode
#if PUHUA_OPENSSL_IS_11X
    mutable DES_key_schedule desKeySchedule3_{};  // NOLINT subclass still uses it
#else
    mutable u_char desKeySchedule3_[8];  // NOLINT subclass still uses it
#endif
public:
    /// @brief Use base class constructor.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06539
    /// @needwork = dda
    /// @endcode
    using PCtxSymmetricStream_Des_Base::PCtxSymmetricStream_Des_Base;

public:  // AUTOSAR-AP interface
    /// @brief Returns the CryptoPrimitivId instance containing the instance identifier.
    /// @name   GetCryptoPrimitiveId
    /// @returns CryptoPrimitivId instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01492
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override = 0;
    /// @brief Get the BlockService instance.
    /// @name   GetBlockService
    /// @returns BlockService instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01493
    /// @needwork = dda
    /// @endcode
    BlockService::Uptr GetBlockService() const noexcept override = 0;
    /// @brief Clear the encryption context.
    /// @name   Reset
    /// @returns has value if reset sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01494
    /// @needwork = dda
    /// @endcode
    ara::core::Result< void > Reset() noexcept override;
    /// @brief Set (deploy) a key to the symmetric algorithm context.
    /// @name  SetKey
    /// @param key Symmetric key
    /// @param transform Encryption direction: encrypt or decrypt
    /// @return has value if SetKey sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01495
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
    /// @trace_id_dd=DD_CRYPTO_01496
    /// @needwork = dda
    /// @endcode
    std::size_t GetMaxKeyBitLength() const noexcept override;
    /// @brief Get the minimum supported key length (in bits).
    /// @name   GetMinKeyBitLength
    /// @returns minimal supported length of the key in bits
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01497
    /// @needwork = dda
    /// @endcode
    std::size_t GetMinKeyBitLength() const noexcept override;
    /// @brief Validate support for a specific key length according to the context.
    /// @name   IsKeyBitLengthSupported
    /// @param keyBitLength Key length in bits
    /// @returns @c true if provided value of the key length is supported by the context
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01498
    /// @needwork = dda
    /// @endcode
    bool IsKeyBitLengthSupported(std::size_t keyBitLength) const noexcept override;

protected:  // PCtxSymmetricStream interface
    /// @brief Check whether the key meets requirements.
    /// @name   CheckKey
    /// @param key Symmetric key
    /// @returns true if check key sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01499
    /// @needwork = dda
    /// @endcode
    bool CheckKey(SymmetricKey const& key) const noexcept override;
};

//***************/  //3DES.CFB1
/// @brief General stream cipher context interface: 3Des_Cfb1
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00320
/// @trace_id_dd=DD_CRYPTO_01500
/// @unit_name=PCtxSymmetricStream_3Des_Cfb1
/// @unit_description=3DES-CFB1-based stream symmetric encryption
/// @needwork = ad
/// @endcode
class PCtxSymmetricStream_3Des_Cfb1 : public PCtxSymmetricStream_3Des_Base
{
public:
    /// @brief Use base class constructor.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06285
    /// @needwork = dda
    /// @endcode
    using PCtxSymmetricStream_3Des_Base::PCtxSymmetricStream_3Des_Base;

public:  // AUTOSAR-AP interface
    /// @brief Returns the CryptoPrimitivId instance containing the instance identifier.
    /// @name   GetCryptoPrimitiveId
    /// @returns CryptoPrimitivId instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01501
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override;
    /// @brief Get the BlockService instance.
    /// @name   GetBlockService
    /// @returns BlockService instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01502
    /// @needwork = dda
    /// @endcode
    BlockService::Uptr GetBlockService() const noexcept override;
    /// @brief Check the bytewise property of the operation mode.
    /// @name   IsBytewiseMode
    /// @returns true if byte mode false if block mode
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01453
    /// @needwork = dda
    /// @endcode
    bool IsBytewiseMode() const noexcept override;

protected:  // PCtxSymmetricStream interface
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
    /// @trace_id_dd=DD_CRYPTO_01503
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
    /// @trace_id_dd=DD_CRYPTO_01504
    /// @needwork = dda
    /// @endcode
    ara::core::StringView GetIpcFuncNameDoCipher() const noexcept override;
};
//***************/  //DES.CFB64
/// @brief General stream cipher context interface: 3Des_Cfb64
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00321
/// @trace_id_dd=DD_CRYPTO_01505
/// @unit_name=PCtxSymmetricStream_3Des_Cfb64
/// @unit_description=3DES-CFB64-based stream symmetric encryption
/// @needwork = ad
/// @endcode
class PCtxSymmetricStream_3Des_Cfb64 : public PCtxSymmetricStream_3Des_Base
{
public:
    /// @brief Use base class constructor.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06286
    /// @needwork = dda
    /// @endcode
    using PCtxSymmetricStream_3Des_Base::PCtxSymmetricStream_3Des_Base;

public:  // AUTOSAR-AP interface
    /// @brief Returns the CryptoPrimitivId instance containing the instance identifier.
    /// @name   GetCryptoPrimitiveId
    /// @returns CryptoPrimitivId instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01506
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override;
    /// @brief Get the BlockService instance.
    /// @name   GetBlockService
    /// @returns BlockService instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01507
    /// @needwork = dda
    /// @endcode
    BlockService::Uptr GetBlockService() const noexcept override;

protected:  // PCtxSymmetricStream interface
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
    /// @trace_id_dd=DD_CRYPTO_01508
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
    /// @trace_id_dd=DD_CRYPTO_01509
    /// @needwork = dda
    /// @endcode
    ara::core::StringView GetIpcFuncNameDoCipher() const noexcept override;
};
//***************/  //DES.OFB64
/// @brief General stream cipher context interface: 3Des_Ofb64
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00322
/// @trace_id_dd=DD_CRYPTO_01510
/// @unit_name=PCtxSymmetricStream_3Des_Ofb64
/// @unit_description=3DES-OFB64-based stream symmetric encryption
/// @needwork = ad
/// @endcode
class PCtxSymmetricStream_3Des_Ofb64 : public PCtxSymmetricStream_3Des_Base
{
public:
    /// @brief Use base class constructor.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06287
    /// @needwork = dda
    /// @endcode
    using PCtxSymmetricStream_3Des_Base::PCtxSymmetricStream_3Des_Base;

public:  // AUTOSAR-AP interface
    /// @brief Returns the CryptoPrimitivId instance containing the instance identifier.
    /// @name   GetCryptoPrimitiveId
    /// @returns CryptoPrimitivId instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01511
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override;
    /// @brief Get the BlockService instance.
    /// @name   GetBlockService
    /// @returns BlockService instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01512
    /// @needwork = dda
    /// @endcode
    BlockService::Uptr GetBlockService() const noexcept override;

protected:  // PCtxSymmetricStream interface
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
    /// @trace_id_dd=DD_CRYPTO_01513
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
    /// @trace_id_dd=DD_CRYPTO_01514
    /// @needwork = dda
    /// @endcode
    ara::core::StringView GetIpcFuncNameDoCipher() const noexcept override;
};
//********************************/
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_PUHUA_CTX_SYMMETRIC_STREAM_DES_H_
