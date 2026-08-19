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
/// @file       isoft_ctx_symmetric_block_aes.h
/// @brief      AutoSar-Crypto Encryption Module
/// @details    Interface for symmetric block cipher context with padding: AES algorithm.
/// @date       2022-02-15
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr><td>2022-02-15  <td>1.0.0    <td>hanjingjing  <td>Initial version creation
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default Encryption/Symmetric Encryption
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_01001
/// @unit_name=PCtxSymmetricBlock_Aes_Base
/// @unit_description=AES-based Symmetric Block Encryption Context Base Class
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_PUHUA_CTX_SYMMETRIC_BLOCK_AES_H_
#define ARA_CRYPTO_CRYP_PUHUA_CTX_SYMMETRIC_BLOCK_AES_H_

#include <openssl/aes.h>

#include "ara/crypto/cryp/cryobj/isoft_key_symmetric_aes.h"
#include "ara/crypto/cryp/symmetric/isoft_ctx_symmetric_block.h"
#include "ara/crypto/openssl/isoft_openssl_encrypt.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
//********************************/
/// @brief Interface for symmetric block cipher context with padding: AES.
//********************************/ // Common base class for Aes.ecb, Aes.cbc
/// @brief Interface for symmetric block cipher context with padding: AES base class
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00308
/// @trace_id_dd=DD_CRYPTO_01412
/// @needwork = ad
/// @endcode
class PCtxSymmetricBlock_Aes_Base : public PCtxSymmetricBlock
{
protected:
/// @brief AES key
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00000
/// @trace_id_dd=DD_CRYPTO_01413
/// @needwork = dda
/// @endcode
#if PUHUA_OPENSSL_IS_11X
    mutable AES_KEY aesKey_;  // NOLINT
#endif

private:
    /// @brief Key byte length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_08662
    /// @needwork = dda
    /// @endcode
    uint32_t nKeyBitLength_;

protected:
    /// @brief Return key byte length
    /// @return Key byte length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_08663
    /// @needwork = dda
    /// @endcode
    uint32_t _GetKeyBitLength() const noexcept { return nKeyBitLength_; }

public:
    /// @brief Parameterized constructor
    /// @name  PCtxSymmetricBlock_Aes_Base
    /// @param cryptoProvider Encryption provider
    /// @param nKeyBitLength Key byte length (Note: original comment says byte length, parameter name suggests bits)
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01415
    /// @needwork = dda
    /// @endcode
    explicit PCtxSymmetricBlock_Aes_Base(PCryptoProvider &cryptoProvider,
                                         uint32_t const nKeyBitLength = static_cast< uint32_t >(
                                             PAlgId_Symmetric_AesKey::EKeyLen::kKeyBitLength_Any)) noexcept;

public:  // AUTOSAR-AP interface
    /// @brief Returns the CryptoPrimitivId instance containing the instance identifier.
    /// @name   GetCryptoPrimitiveId
    /// @returns CryptoPrimitivId instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01416
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override = 0;
    /// @brief Gets the CryptoService instance.
    /// @name   GetCryptoService
    /// @returns CryptoService instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01417
    /// @needwork = dda
    /// @endcode
    CryptoService::Uptr GetCryptoService() const noexcept override = 0;
    /// @brief Clears the crypto context.
    /// @name   Reset
    /// @returns has value if reset sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01418
    /// @needwork = dda
    /// @endcode
    ara::core::Result< void > Reset() noexcept override;
    /// @brief Sets (deploys) a key for the symmetric algorithm context.
    /// @param key Symmetric key
    /// @param transform Encryption direction: encrypt or decrypt
    /// @return has value if SetKey sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01419
    /// @needwork = dda
    /// @endcode
    ara::core::Result< void > SetKey(SymmetricKey const &key,
                                     CryptoTransform transform = CryptoTransform::kEncrypt) noexcept override;

public:  // PServiceCrypto interface
    /// @brief Gets the maximum supported key length in bits.
    /// @name   GetMaxKeyBitLength
    /// @returns maximal supported length of the key in bits
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01420
    /// @needwork = dda
    /// @endcode
    std::size_t GetMaxKeyBitLength() const noexcept override;
    /// @brief Gets the minimum supported key length in bits.
    /// @name   GetMinKeyBitLength
    /// @returns minimal supported length of the key in bits
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01421
    /// @needwork = dda
    /// @endcode
    std::size_t GetMinKeyBitLength() const noexcept override;
    /// @brief Verifies support for a specific key length based on the context.
    /// @name   IsKeyBitLengthSupported
    /// @param keyBitLength Key length: in bits
    /// @returns @c true if provided value of the key length is supported by the context
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01422
    /// @needwork = dda
    /// @endcode
    bool IsKeyBitLengthSupported(std::size_t keyBitLength) const noexcept override;
    /// @brief Checks if a key is set for this context.
    /// @name   IsKeyAvailable
    /// @returns true if a key has been set to this context false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01423
    /// @needwork = dda
    /// @endcode
    bool IsKeyAvailable() const noexcept override;
    /// @brief Gets the block (or internal buffer) size of the underlying algorithm.
    ///         For digest, byte-by-byte stream ciphers, and RNG contexts, it is an informational method used only for optimizing interface usage.
    /// @name   GetBlockSize
    /// @returns Block (or internal buffer) size of the underlying algorithm
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01424
    /// @needwork = dda
    /// @endcode
    std::size_t GetBlockSize() const noexcept override;
    /// @brief Get AES_KEY
    /// @name   GetAesKey
    /// @returns Pointer to AES_KEY
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01425
    /// @needwork = dda
    /// @qac Suppress warning [4024]: This function returns a non-const handle to private or protected member data.
    /// @endcode
    // AES_KEY const &GetAesKey() const noexcept { return aesKey_; }

protected:
    /// @brief Check if Key meets requirements
    /// @name   CheckKey
    /// @param key Symmetric key
    /// @returns true if check key sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01426
    /// @needwork = dda
    /// @endcode
    bool CheckKey(SymmetricKey const &key) const noexcept override;
    /// @brief Local encryption/decryption functional function
    /// @name   DoSymmetric
    /// @param pOutput Output
    /// @param pInput Input
    /// @param nInLen Data length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01427
    /// @needwork = dda
    /// @endcode
    void DoSymmetric(uint8_t *pOutput, uint8_t const *pInput, uint32_t nInLen) const noexcept override = 0;
};
//********************************/
/// @brief Interface for symmetric block cipher context with padding: AES_ECB
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00309
/// @trace_id_dd=DD_CRYPTO_01428
/// @unit_name=PCtxSymmetricBlock_Aes_Ecb
/// @unit_description=AES-ECB-based Symmetric Block Encryption Context
/// @needwork = ad
/// @endcode
class PCtxSymmetricBlock_Aes_Ecb : public PCtxSymmetricBlock_Aes_Base
{
public:
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06279
    /// @needwork = dda
    /// @endcode
    using PCtxSymmetricBlock_Aes_Base::PCtxSymmetricBlock_Aes_Base;

public:
    /// @brief PCtxSymmetricBlock_Aes_Base smart pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00309
    /// @trace_id_dd=DD_CRYPTO_06280
    /// @needwork = dd
    /// @endcode
    using Uptr = std::unique_ptr< PCtxSymmetricBlock_Aes_Ecb >;

public:  // AUTOSAR-AP interface
    /// @brief Returns the CryptoPrimitivId instance containing the instance identifier.
    /// @name   GetCryptoPrimitiveId
    /// @returns CryptoPrimitivId instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01429
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override;
    /// @brief Gets the CryptoService instance.
    /// @name   GetCryptoService
    /// @returns CryptoService instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01430
    /// @needwork = dda
    /// @endcode
    CryptoService::Uptr GetCryptoService() const noexcept override;

protected:
    /// @brief Local encryption/decryption functional function
    /// @name   DoSymmetric
    /// @param pOutput Output
    /// @param pInput Input
    /// @param nInLen Data length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01431
    /// @needwork = dda
    /// @endcode
    void DoSymmetric(uint8_t *pOutput, uint8_t const *pInput, uint32_t nInLen) const noexcept override;
    /// @brief Get IPC function name for encryption/decryption
    /// @name    GetIpcFuncNameDoCipher
    /// @returns IPC function name for encryption/decryption
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01432
    /// @needwork = dda
    /// @endcode
    ara::core::StringView GetIpcFuncNameDoCipher() const noexcept override;
};
//********************************/
/// @brief Interface for symmetric block cipher context with padding: AES_CBC
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00310
/// @trace_id_dd=DD_CRYPTO_01433
/// @unit_name=PCtxSymmetricBlock_Aes_Cbc
/// @unit_description=AES-CBC-based Symmetric Block Encryption Context
/// @needwork = ad
/// @endcode
class PCtxSymmetricBlock_Aes_Cbc : public PCtxSymmetricBlock_Aes_Base
{
private:
    /// @brief Initialization vector
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01434
    /// @needwork = dda
    /// @endcode
    uint8_t initVector_[static_cast< u_long >(PAlgId_Symmetric_AesKey::EKeyLen::kAesBlockSize)]{0U};

public:
    /// @brief Constructor
    /// @param cryptoProvider Encryption provider
    /// @param nKeyBitLength Key length: in bits
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01435
    /// @needwork = dda
    /// @qac Suppress warning [4268]: Initialization helper can be replaced with a delegator.
    /// @endcode
    explicit PCtxSymmetricBlock_Aes_Cbc(PCryptoProvider &cryptoProvider,
                                        uint32_t const nKeyBitLength = static_cast< uint32_t >(
                                            PAlgId_Symmetric_AesKey::EKeyLen::kKeyBitLength_Any)) noexcept
        : PCtxSymmetricBlock_Aes_Base{cryptoProvider, nKeyBitLength}
    {
        _InitVector();
    }
    /// @brief Constructor
    /// @name PCtxSymmetricBlock_Aes_Cbc
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_08793
    /// @needwork = dda
    /// @endcode
    PCtxSymmetricBlock_Aes_Cbc() = delete;
    /// @brief the destructor
    /// @name Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_08794
    /// @needwork = dda
    /// @endcode
    ~PCtxSymmetricBlock_Aes_Cbc() noexcept override = default;
    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_08795
    /// @needwork = dda
    /// @endcode
    PCtxSymmetricBlock_Aes_Cbc(PCtxSymmetricBlock_Aes_Cbc &&other) = delete;
    /// @brief Default move assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_08796
    /// @needwork = dda
    /// @endcode
    PCtxSymmetricBlock_Aes_Cbc &operator=(PCtxSymmetricBlock_Aes_Cbc &&other) = delete;
    /// @brief Default copy assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_08797
    /// @needwork = dda
    /// @endcode
    PCtxSymmetricBlock_Aes_Cbc &operator=(PCtxSymmetricBlock_Aes_Cbc const &other) = delete;
    /// @brief Default copy constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_08798
    /// @needwork = dda
    /// @endcode
    PCtxSymmetricBlock_Aes_Cbc(PCtxSymmetricBlock_Aes_Cbc const &other) = delete;

public:  // AUTOSAR-AP interface
    /// @brief Return the CryptoPrimitivId instance containing the instance identifier.
    /// @name   GetCryptoPrimitiveId
    /// @returns CryptoPrimitivId instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01436
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override;
    /// @brief Get the CryptoService instance.
    /// @name   GetCryptoService
    /// @returns CryptoService instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01437
    /// @needwork = dda
    /// @endcode
    CryptoService::Uptr GetCryptoService() const noexcept override;
    /// @brief Clear the encryption context.
    /// @name   Reset
    /// @returns has value if reset sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01438
    /// @needwork = dda
    /// @endcode
    ara::core::Result< void > Reset() noexcept override;

protected:
    /// @brief Local encryption/decryption function
    /// @name   DoSymmetric
    /// @param pOutput output
    /// @param pInput input
    /// @param nInLen data length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01439
    /// @needwork = dda
    /// @endcode
    void DoSymmetric(uint8_t *pOutput, uint8_t const *pInput, uint32_t nInLen) const noexcept override;
    /// @brief Get the IPC function name corresponding to encryption/decryption
    /// @name    GetIpcFuncNameDoCipher
    /// @returns the IPC function name corresponding to encryption/decryption
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01440
    /// @needwork = dda
    /// @endcode
    ara::core::StringView GetIpcFuncNameDoCipher() const noexcept override;

protected:
    /// @brief Initialize initialization vector
    /// @name   _InitVector
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01441
    /// @needwork = dda
    /// @endcode
    void _InitVector() noexcept;  // Initialize initVector_
};
//********************************/
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_PUHUA_CTX_SYMMETRIC_BLOCK_AES_H_
