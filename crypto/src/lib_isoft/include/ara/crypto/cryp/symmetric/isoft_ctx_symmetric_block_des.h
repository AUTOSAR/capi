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
/// @file       isoft_ctx_symmetric_block_des.h
/// @brief      AutoSar-Crypto encryption and decryption module
/// @details    Interface for symmetric block cipher context with padding: DES algorithm.
/// @date       2022-01-20
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
/// @trace_id_sr=SR_CRYPTO_01003
/// @unit_name=PCtxSymmetricBlock_Des_Base
/// @unit_description=Symmetric block encryption context based on DES
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_PUHUA_CTX_SYMMETRIC_BLOCK_DES_H_
#define ARA_CRYPTO_CRYP_PUHUA_CTX_SYMMETRIC_BLOCK_DES_H_

#include <openssl/des.h>

#include "ara/crypto/cryp/symmetric/isoft_ctx_symmetric_block.h"
#include "ara/crypto/openssl/isoft_openssl_encrypt.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
//********************************/
/// @brief Interface for symmetric block cipher context with padding: DES. // Common base class for Des.ecb, Des.cbc
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00331
/// @trace_id_dd=DD_CRYPTO_01600
/// @needwork = ad
/// @endcode
class PCtxSymmetricBlock_Des_Base : public PCtxSymmetricBlock
{
protected:
/// @brief Key used by DES
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00000
/// @trace_id_dd=DD_CRYPTO_01601
/// @needwork = dda
/// @endcode
// PRQA S 2028,2428 QAC /// @qac: introduced by openssl library file, cannot be modified
#if PUHUA_OPENSSL_IS_11X
    mutable DES_key_schedule desKeySchedule_{};  // NOLINT
#else
    mutable u_char desKeySchedule_[8];  // NOLINT
#endif
    // PRQA L:QAC

public:
/// @brief Get the key used by DES
/// @return pointer to DES_key_schedule
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00000
/// @trace_id_dd=DD_CRYPTO_01602
/// @needwork = dda
/// @endcode
#if PUHUA_OPENSSL_IS_11X
    DES_key_schedule const *GetPDesKeySchedule1() const noexcept { return &desKeySchedule_; }
#else
    u_char const *GetPDesKeySchedule1() const noexcept { return desKeySchedule_; }
#endif

public:
    /// @brief Parameterized constructor
    /// @param cryptoProvider PCryptoProvider
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_08799
    /// @needwork = dda
    /// @endcode
    explicit PCtxSymmetricBlock_Des_Base(PCryptoProvider &cryptoProvider) noexcept  // NOLINT
        : PCtxSymmetricBlock{cryptoProvider}
    {
    }
    /// @brief Default constructor
    /// @name   PCtxSymmetricBlock_Des_Base
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_08800
    /// @needwork = dda
    /// @endcode
    PCtxSymmetricBlock_Des_Base() = delete;
    /// @brief Default destructor
    /// @name   ~PCtxSymmetricBlock_Des_Base
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01603
    /// @needwork = dda
    /// @endcode
    ~PCtxSymmetricBlock_Des_Base() override = default;
    /// @brief Define copy assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01604
    /// @needwork = dda
    /// @endcode
    PCtxSymmetricBlock_Des_Base &operator=(PCtxSymmetricBlock_Des_Base const &other) const noexcept = delete;
    /// @brief Default copy constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01605
    /// @needwork = dda
    /// @endcode
    PCtxSymmetricBlock_Des_Base(PCtxSymmetricBlock_Des_Base const &other) = delete;
    /// @brief Default move constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01606
    /// @needwork = dda
    /// @endcode
    PCtxSymmetricBlock_Des_Base(PCtxSymmetricBlock_Des_Base &&other) = delete;
    /// @brief Default move assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01607
    /// @needwork = dda
    /// @endcode
    PCtxSymmetricBlock_Des_Base &operator=(PCtxSymmetricBlock_Des_Base &&other) = delete;

public:  // CryptoContext interface
    /// @brief Return the CryptoPrimitivId instance containing the instance identifier.
    /// @name   GetCryptoPrimitiveId
    /// @returns CryptoPrimitivId instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01608
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override = 0;

public:  // SymmetricBlockCipherCtx interface
    /// @brief Get the CryptoService instance.
    /// @name   GetCryptoService
    /// @returns CryptoService instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01609
    /// @needwork = dda
    /// @endcode
    CryptoService::Uptr GetCryptoService() const noexcept override = 0;
    /// @brief Clear the encryption context.
    /// @name   Reset
    /// @returns  has value if reset sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01610
    /// @needwork = dda
    /// @endcode
    ara::core::Result< void > Reset() noexcept override;
    /// @brief Set (deploy) a key for the symmetric algorithm context.
    /// @name   SetKey
    /// @param key symmetric key
    /// @param transform encryption direction: encrypt or decrypt
    /// @return has value if SetKey sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01611
    /// @needwork = dda
    /// @endcode
    ara::core::Result< void > SetKey(SymmetricKey const &key,
                                     CryptoTransform transform = CryptoTransform::kEncrypt) noexcept override;

public:  // PServiceCrypto interface
    /// @brief Get the block (or internal buffer) size of the underlying algorithm.
    ///         For digest, byte-by-byte stream cipher and RNG contexts, it is an informational method, only used to optimize interface usage.
    /// @name   GetBlockSize
    /// @returns block (or internal buffer) size of the underlying algorithm
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01612
    /// @needwork = dda
    /// @endcode
    std::size_t GetBlockSize() const noexcept override;
    /// @brief Get the maximum supported key length (in bits).
    /// @name   GetMaxKeyBitLength
    /// @returns  maximal supported length of the key in bits
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01613
    /// @needwork = dda
    /// @endcode
    std::size_t GetMaxKeyBitLength() const noexcept override;
    /// @brief Get the minimum supported key length (in bits).
    /// @name   GetMinKeyBitLength
    /// @returns minimal supported length of the key in bits
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01614
    /// @needwork = dda
    /// @endcode
    std::size_t GetMinKeyBitLength() const noexcept override;

protected:
    /// @brief Check whether the key meets the requirements
    /// @name   CheckKey
    /// @param key symmetric key
    /// @returns true if check key sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01615
    /// @needwork = dda
    /// @endcode
    bool CheckKey(SymmetricKey const &key) const noexcept override;
    /// @brief Local encryption/decryption function
    /// @name   DoSymmetric
    /// @param pOutput output
    /// @param pInput input
    /// @param nInLen data length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01616
    /// @needwork = dda
    /// @endcode
    void DoSymmetric(uint8_t *pOutput, uint8_t const *pInput, uint32_t nInLen) const noexcept override = 0;
};
//***************/
/// @brief Interface for symmetric block cipher context with padding: Des_Ecb
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00332
/// @trace_id_dd=DD_CRYPTO_01617
/// @unit_name=PCtxSymmetricBlock_Des_Ecb
/// @unit_description=Symmetric block encryption context based on DES-ECB
/// @needwork = ad
/// @endcode
class PCtxSymmetricBlock_Des_Ecb : public PCtxSymmetricBlock_Des_Base
{
public:  // CryptoContext interface
    /// @brief Parameterized constructor
    /// @param cryptoProvider PCryptoProvider
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_08801
    /// @needwork = dda
    /// @endcode
    explicit PCtxSymmetricBlock_Des_Ecb(PCryptoProvider &cryptoProvider) noexcept
        : PCtxSymmetricBlock_Des_Base{cryptoProvider} {};
    /// @brief Default constructor
    /// @name   PCtxSymmetricBlock_Des_Ecb
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_08802
    /// @needwork = dda
    /// @endcode
    PCtxSymmetricBlock_Des_Ecb() = delete;
    /// @brief Destructor
    /// @name   ~PCtxSymmetricBlock_Des_Ecb
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01618
    /// @needwork = dda
    /// @endcode
    ~PCtxSymmetricBlock_Des_Ecb() override = default;
    /// @brief Default copy constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01619
    /// @needwork = dda
    /// @endcode
    PCtxSymmetricBlock_Des_Ecb(PCtxSymmetricBlock_Des_Ecb const &other) = delete;
    /// @brief Default move constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01620
    /// @needwork = dda
    /// @endcode
    PCtxSymmetricBlock_Des_Ecb(PCtxSymmetricBlock_Des_Ecb &&other) = delete;
    /// @brief Default move assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01621
    /// @needwork = dda
    /// @endcode
    PCtxSymmetricBlock_Des_Ecb &operator=(PCtxSymmetricBlock_Des_Ecb &&other) = delete;
    /// @brief Define copy assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01622
    /// @needwork = dda
    /// @endcode
    PCtxSymmetricBlock_Des_Ecb &operator=(PCtxSymmetricBlock_Des_Ecb const &other) const noexcept = delete;

    /// @brief Return the CryptoPrimitivId instance containing the instance identifier.
    /// @name   GetCryptoPrimitiveId
    /// @returns CryptoPrimitivId instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01623
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override;

public:  // SymmetricBlockCipherCtx interface
    /// @brief Get the CryptoService instance.
    /// @name   GetCryptoService
    /// @returns CryptoService instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01624
    /// @needwork = dda
    /// @endcode
    CryptoService::Uptr GetCryptoService() const noexcept override;

protected:
    /// @brief Local encryption/decryption function
    /// @name   DoSymmetric
    /// @param pOutput output
    /// @param pInput input
    /// @param nInLen data length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01625
    /// @needwork = dda
    /// @endcode
    void DoSymmetric(uint8_t *pOutput, uint8_t const *pInput, uint32_t nInLen) const noexcept override;
    /// @brief Get the IPC function name corresponding to encryption/decryption
    /// @name    GetIpcFuncNameDoCipher
    /// @returns the IPC function name corresponding to encryption/decryption
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01626
    /// @needwork = dda
    /// @endcode
    ara::core::StringView GetIpcFuncNameDoCipher() const noexcept override;
};
//***************/
/// @brief Interface for symmetric block cipher context with padding: Des_Cbc
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00333
/// @trace_id_dd=DD_CRYPTO_01627
/// @unit_name=PCtxSymmetricBlock_Des_Cbc
/// @unit_description=Symmetric block encryption context based on DES-CBC
/// @needwork = ad
/// @endcode
class PCtxSymmetricBlock_Des_Cbc : public PCtxSymmetricBlock_Des_Base
{
private:
    /// @brief Initialization vector
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01628
    /// @needwork = dda
    /// @endcode
    // PRQA S 2028,2428 QAC /// @qac: introduced by openssl library file, cannot be modified
    DES_cblock initVector_{0U};
    // PRQA L:QAC

public:
    /// @brief Unique smart pointer of the interface.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00333
    /// @trace_id_dd=DD_CRYPTO_06295
    /// @needwork = dd
    /// @endcode
    using Uptr = std::unique_ptr< PCtxSymmetricBlock_Des_Cbc >;
    /// @brief  Parameterized constructor
    /// @name   PCtxSymmetricBlock_Des_Cbc
    /// @param  cryptoProvider PCryptoProvider
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01629
    /// @needwork = dda
    /// @qac Possibly cannot modify [4268]: Initialization helper can be replaced with a delegator.
    /// @endcode
    explicit PCtxSymmetricBlock_Des_Cbc(PCryptoProvider &cryptoProvider) noexcept
        : PCtxSymmetricBlock_Des_Base{cryptoProvider}
    {
        _InitVector();
    }
    /// @brief   Default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    PCtxSymmetricBlock_Des_Cbc() = delete;
    /// @brief   Default destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01630
    /// @needwork = dda
    /// @endcode
    ~PCtxSymmetricBlock_Des_Cbc() override = default;
    /// @brief Default copy constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01631
    /// @needwork = dda
    /// @endcode
    PCtxSymmetricBlock_Des_Cbc(PCtxSymmetricBlock_Des_Cbc const &other) = delete;
    /// @brief Default move constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01632
    /// @needwork = dda
    /// @endcode
    PCtxSymmetricBlock_Des_Cbc(PCtxSymmetricBlock_Des_Cbc &&other) = delete;
    /// @brief Default move assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01633
    /// @needwork = dda
    /// @endcode
    PCtxSymmetricBlock_Des_Cbc &operator=(PCtxSymmetricBlock_Des_Cbc &&other) = delete;
    /// @brief Define copy assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01634
    /// @needwork = dda
    /// @endcode
    PCtxSymmetricBlock_Des_Cbc &operator=(PCtxSymmetricBlock_Des_Cbc const &other) const noexcept = delete;

public:  // CryptoContext interface
    /// @brief Return the CryptoPrimitivId instance containing the instance identifier.
    /// @name   GetCryptoPrimitiveId
    /// @returns CryptoPrimitivId instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01635
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override;

public:  // SymmetricBlockCipherCtx interface
    /// @brief Get the CryptoService instance.
    /// @name   GetCryptoService
    /// @returns CryptoService instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01636
    /// @needwork = dda
    /// @endcode
    CryptoService::Uptr GetCryptoService() const noexcept override;
    /// @brief Clear the encryption context.
    /// @name   Reset
    /// @returns has value if reset sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01637
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
    /// @trace_id_dd=DD_CRYPTO_01638
    /// @needwork = dda
    /// @endcode
    void DoSymmetric(uint8_t *pOutput, uint8_t const *pInput, uint32_t nInLen) const noexcept override;
    /// @brief Get the IPC function name: DoCipher
    /// @name    GetIpcFuncNameDoCipher
    /// @returns the IPC function name corresponding to encryption/decryption
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01639
    /// @needwork = dda
    /// @endcode
    ara::core::StringView GetIpcFuncNameDoCipher() const noexcept override;

protected:
    /// @brief Initialize initialization vector
    /// @name   _InitVector
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01640
    /// @needwork = dda
    /// @endcode
    void _InitVector() noexcept;
};
//********************************/
/// @brief Interface for symmetric block cipher context with padding: 3Des_Base
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00334
/// @trace_id_dd=DD_CRYPTO_01641
/// @unit_name=PCtxSymmetricBlock_3Des_Base
/// @unit_description=Base class for symmetric block encryption context based on 3DES
/// @needwork = ad
/// @endcode
class PCtxSymmetricBlock_3Des_Base : public PCtxSymmetricBlock_Des_Base
{
protected:
/// @brief Second key
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00000
/// @trace_id_dd=DD_CRYPTO_01642
/// @needwork = dda
/// @endcode
// PRQA S 2028,2428 QAC /// @qac: introduced by openssl library file, cannot be modified
#if PUHUA_OPENSSL_IS_11X
    mutable DES_key_schedule desKeySchedule2_{};  // NOLINT
#else
    mutable u_char desKeySchedule2_[8];  // NOLINT
#endif
// PRQA L:QAC
/// @brief Third key
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00000
/// @trace_id_dd=DD_CRYPTO_01643
/// @needwork = dda
/// @endcode
// PRQA S 2028,2428 QAC /// @qac: introduced by openssl library file, cannot be modified
#if PUHUA_OPENSSL_IS_11X
    mutable DES_key_schedule desKeySchedule3_{};  // NOLINT
#else
    mutable u_char desKeySchedule3_[8];  // NOLINT
#endif
    // PRQA L:QAC

public:
    /// @brief Parameterized constructor
    /// @param cryptoProvider PCryptoProvider
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_08807
    /// @needwork = dda
    /// @endcode
    explicit PCtxSymmetricBlock_3Des_Base(PCryptoProvider &cryptoProvider) noexcept  // NOLINT
        : PCtxSymmetricBlock_Des_Base{cryptoProvider} {};
    /// @brief Default constructor
    /// @name   PCtxSymmetricBlock_3Des_Base
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_08803
    /// @needwork = dda
    /// @endcode
    PCtxSymmetricBlock_3Des_Base() = delete;
    /// @name   ~PCtxSymmetricBlock_3Des_Base
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01644
    /// @needwork = dda
    /// @endcode
    ~PCtxSymmetricBlock_3Des_Base() override = default;
    /// @brief Default copy constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01645
    /// @needwork = dda
    /// @endcode
    PCtxSymmetricBlock_3Des_Base(PCtxSymmetricBlock_3Des_Base const &other) = delete;
    /// @brief Default move constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01646
    /// @needwork = dda
    /// @endcode
    PCtxSymmetricBlock_3Des_Base(PCtxSymmetricBlock_3Des_Base &&other) = delete;
    /// @brief Default move assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01647
    /// @needwork = dda
    /// @endcode
    PCtxSymmetricBlock_3Des_Base &operator=(PCtxSymmetricBlock_3Des_Base &&other) = delete;
    /// @brief Define copy assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01648
    /// @needwork = dda
    /// @endcode
    PCtxSymmetricBlock_3Des_Base &operator=(PCtxSymmetricBlock_3Des_Base const &other) const noexcept = delete;

public:  // AUTOSAR-AP interface
    /// @brief Clear the encryption context.
    /// @name   Reset
    /// @returns has value if reset sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01649
    /// @needwork = dda
    /// @endcode
    ara::core::Result< void > Reset() noexcept override;
    /// @brief Set (deploy) a key for the symmetric algorithm context.
    /// @name   SetKey
    /// @param key symmetric key
    /// @param transform encryption direction: encrypt or decrypt
    /// @return has value if SetKey sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01650
    /// @needwork = dda
    /// @endcode
    ara::core::Result< void > SetKey(SymmetricKey const &key,
                                     CryptoTransform transform = CryptoTransform::kEncrypt) noexcept override;

public:  // PServiceCrypto interface
    /// @brief Get the maximum supported key length (in bits).
    /// @name   GetMaxKeyBitLength
    /// @returns maximal supported length of the key in bits
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01651
    /// @needwork = dda
    /// @endcode
    std::size_t GetMaxKeyBitLength() const noexcept override;
    /// @brief Get the minimum supported key length (in bits).
    /// @name   GetMinKeyBitLength
    /// @returns minimal supported length of the key in bits
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01652
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
    /// @trace_id_dd=DD_CRYPTO_01653
    /// @needwork = dda
    /// @endcode
    bool IsKeyBitLengthSupported(std::size_t keyBitLength) const noexcept override;
    /// @brief Get the block (or internal buffer) size of the underlying algorithm.
    ///         For digest, byte-by-byte stream cipher and RNG contexts, it is an informational method, only used to optimize interface usage.
    /// @name   GetBlockSize
    /// @returns block (or internal buffer) size of the underlying algorithm
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01654
    /// @needwork = dda
    /// @endcode
    std::size_t GetBlockSize() const noexcept override;

protected:
    /// @brief Check whether the key meets the requirements
    /// @name   CheckKey
    /// @param key symmetric key
    /// @returns true if check key sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01655
    /// @needwork = dda
    /// @endcode
    bool CheckKey(SymmetricKey const &key) const noexcept override;
    /// @brief Local encryption/decryption function
    /// @name   DoSymmetric
    /// @param pOutput output
    /// @param pInput input
    /// @param nInLen data length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01656
    /// @needwork = dda
    /// @endcode
    void DoSymmetric(uint8_t *pOutput, uint8_t const *pInput, uint32_t nInLen) const noexcept override = 0;

public:
/// @brief Get the second key
/// @return pointer to DES_key_schedule
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00000
/// @trace_id_dd=DD_CRYPTO_01657
/// @needwork = dda
/// @endcode
#if PUHUA_OPENSSL_IS_11X
    DES_key_schedule const *GetPDesKeySchedule2() const noexcept { return &desKeySchedule2_; }
#else
    u_char const *GetPDesKeySchedule2() const noexcept { return desKeySchedule2_; }
#endif
/// @brief Get the third key
/// @return pointer to DES_key_schedule
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00000
/// @trace_id_dd=DD_CRYPTO_01658
/// @needwork = dda
/// @endcode
#if PUHUA_OPENSSL_IS_11X
    DES_key_schedule const *GetPDesKeySchedule3() const noexcept { return &desKeySchedule3_; }
#else
    u_char const *GetPDesKeySchedule3() const noexcept { return desKeySchedule3_; }
#endif
};
//***************/
/// @brief Interface for symmetric block cipher context with padding: 3Des_Ecb
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00335
/// @trace_id_dd=DD_CRYPTO_01659
/// @unit_name=PCtxSymmetricBlock_3Des_Ecb
/// @unit_description=Symmetric block encryption context based on 3DES-ECB
/// @needwork = ad
/// @endcode
class PCtxSymmetricBlock_3Des_Ecb : public PCtxSymmetricBlock_3Des_Base
{
public:
    /// @brief Parameterized constructor
    /// @param cryptoProvider PCryptoProvider
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_08804
    /// @needwork = dda
    /// @endcode
    explicit PCtxSymmetricBlock_3Des_Ecb(PCryptoProvider &cryptoProvider) noexcept
        : PCtxSymmetricBlock_3Des_Base{cryptoProvider} {};
    /// @brief Default constructor
    /// @name   PCtxSymmetricBlock_3Des_Ecb
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_08805
    /// @needwork = dda
    /// @endcode
    PCtxSymmetricBlock_3Des_Ecb() = delete;
    /// @brief Destructor
    /// @name   ~PCtxSymmetricBlock_3Des_Ecb
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01660
    /// @needwork = dda
    /// @endcode
    ~PCtxSymmetricBlock_3Des_Ecb() override = default;
    /// @brief Define copy assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01661
    /// @needwork = dda
    /// @endcode
    PCtxSymmetricBlock_3Des_Ecb &operator=(PCtxSymmetricBlock_3Des_Ecb const &other) const noexcept = delete;
    /// @brief Default copy constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01662
    /// @needwork = dda
    /// @endcode
    PCtxSymmetricBlock_3Des_Ecb(PCtxSymmetricBlock_3Des_Ecb const &other) = delete;
    /// @brief Default move constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01663
    /// @needwork = dda
    /// @endcode
    PCtxSymmetricBlock_3Des_Ecb(PCtxSymmetricBlock_3Des_Ecb &&other) = delete;
    /// @brief Default move assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01664
    /// @needwork = dda
    /// @endcode
    PCtxSymmetricBlock_3Des_Ecb &operator=(PCtxSymmetricBlock_3Des_Ecb &&other) = delete;

public:  // AUTOSAR-AP interface
    /// @brief Return the CryptoPrimitivId instance containing the instance identifier.
    /// @name   GetCryptoPrimitiveId
    /// @returns CryptoPrimitivId instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01665
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override;
    /// @brief Get the CryptoService instance.
    /// @name   GetCryptoService
    /// @returns CryptoService instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01666
    /// @needwork = dda
    /// @endcode
    CryptoService::Uptr GetCryptoService() const noexcept override;

protected:
    /// @brief Local encryption/decryption function
    /// @name   DoSymmetric
    /// @param pOutput output
    /// @param pInput input
    /// @param nInLen data length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01667
    /// @needwork = dda
    /// @endcode
    void DoSymmetric(uint8_t *pOutput, uint8_t const *pInput, uint32_t nInLen) const noexcept override;
    /// @brief Get the IPC function name corresponding to encryption/decryption
    /// @name    GetIpcFuncNameDoCipher
    /// @returns the IPC function name corresponding to encryption/decryption
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01668
    /// @needwork = dda
    /// @endcode
    ara::core::StringView GetIpcFuncNameDoCipher() const noexcept override;
};
//***************/
/// @brief Interface for symmetric block cipher context with padding: 3Des_Cbc
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00336
/// @trace_id_dd=DD_CRYPTO_01669
/// @unit_name=PCtxSymmetricBlock_3Des_Cbc
/// @unit_description=Symmetric block encryption context based on 3DES-CBC
/// @needwork = ad
/// @endcode
class PCtxSymmetricBlock_3Des_Cbc : public PCtxSymmetricBlock_3Des_Base
{
private:
    /// @brief Initialization vector
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01670
    /// @needwork = dda
    /// @endcode
    // PRQA S 2028,2428 QAC /// @qac: introduced by openssl library file, cannot be modified
    DES_cblock initVector_{0U};
    // PRQA L:QAC

public:
    /// @brief PCtxSymmetricBlock_3Des_Cbc smart pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06298
    /// @needwork = dda
    /// @endcode
    using Uptr = std::unique_ptr< PCtxSymmetricBlock_3Des_Cbc >;
    /// @brief  Parameterized constructor
    /// @param  cryptoProvider PCryptoProvider
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01671
    /// @needwork = dda
    /// @qac Possibly cannot modify [4268]: Initialization helper can be replaced with a delegator.
    /// @endcode
    explicit PCtxSymmetricBlock_3Des_Cbc(PCryptoProvider &cryptoProvider) noexcept
        : PCtxSymmetricBlock_3Des_Base{cryptoProvider}
    {
        _InitVector();
    }
    /// @brief Constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01672
    /// @needwork = dda
    /// @endcode
    PCtxSymmetricBlock_3Des_Cbc() = delete;
    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01672
    /// @needwork = dda
    /// @endcode
    ~PCtxSymmetricBlock_3Des_Cbc() override = default;
    /// @brief Define copy assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01673
    /// @needwork = dda
    /// @endcode
    PCtxSymmetricBlock_3Des_Cbc &operator=(PCtxSymmetricBlock_3Des_Cbc const &other) const noexcept = delete;
    /// @brief Default copy constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01674
    /// @needwork = dda
    /// @endcode
    PCtxSymmetricBlock_3Des_Cbc(PCtxSymmetricBlock_3Des_Cbc const &other) = delete;
    /// @brief Default move constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01675
    /// @needwork = dda
    /// @endcode
    PCtxSymmetricBlock_3Des_Cbc(PCtxSymmetricBlock_3Des_Cbc &&other) = delete;
    /// @brief Default move assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01676
    /// @needwork = dda
    /// @endcode
    PCtxSymmetricBlock_3Des_Cbc &operator=(PCtxSymmetricBlock_3Des_Cbc &&other) = delete;

public:  // AUTOSAR-AP interface
    /// @brief Return the CryptoPrimitivId instance containing the instance identifier.
    /// @name   GetCryptoPrimitiveId
    /// @returns CryptoPrimitivId instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01677
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override;
    /// @brief Get the CryptoService instance.
    /// @name   GetCryptoService
    /// @returns CryptoService instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01678
    /// @needwork = dda
    /// @endcode
    CryptoService::Uptr GetCryptoService() const noexcept override;
    /// @brief Clear the encryption context.
    /// @name   Reset
    /// @returns has value if reset sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01679
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
    /// @trace_id_dd=DD_CRYPTO_01680
    /// @needwork = dda
    /// @endcode
    void DoSymmetric(uint8_t *pOutput, uint8_t const *pInput, uint32_t nInLen) const noexcept override;
    /// @brief Get the IPC function name corresponding to encryption/decryption
    /// @name    GetIpcFuncNameDoCipher
    /// @returns the IPC function name corresponding to encryption/decryption
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01681
    /// @needwork = dda
    /// @endcode
    ara::core::StringView GetIpcFuncNameDoCipher() const noexcept override;

protected:
    /// @brief Initialize initialization vector
    /// @name   _InitVector
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01682
    /// @needwork = dda
    /// @endcode
    void _InitVector() noexcept;
};
//********************************/
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_PUHUA_CTX_SYMMETRIC_BLOCK_DES_H_
