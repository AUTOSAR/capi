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
/// @file       isoft_ctx_mac_hash_sha.h
/// @brief      AutoSar-Crypto encryption and decryption module
/// @details    Message authentication code implemented based on hash_sha function interface.
/// @date       2022-04-15
/// @author     Zheng Chang
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
/// @module_path=/CRYPTO/Default Encryption and Decryption/Message Authentication Code
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_01007
/// @unit_name=PCtxMacHashSha1
/// @unit_description=Message authentication code based on SHA1
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_PUHUA_CTX_MAC_HASH_SHA_H_
#define ARA_CRYPTO_CRYP_PUHUA_CTX_MAC_HASH_SHA_H_

#include <openssl/hmac.h>

#include "ara/crypto/common/isoft_data_type.h"
#include "ara/crypto/cryp/mac/isoft_ctx_mac_hash.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
/// @brief Message authentication code implemented based on hash_sha function interface: SHA1
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00255
/// @trace_id_dd=DD_CRYPTO_01263
/// @needwork = ad
/// @endcode
class PCtxMacHashSha1 : public PCtxMacHash
{
public:
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06270
    /// @needwork = dda
    /// @endcode
    using PCtxMacHash::PCtxMacHash;
    /// @brief Default virtual destructor
    /// @name   ~PCtxMacHashSha1
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01264
    /// @needwork = dda
    /// @endcode
    ~PCtxMacHashSha1() noexcept override = default;
    /// @brief Default move constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01265
    /// @needwork = dda
    /// @endcode
    PCtxMacHashSha1(PCtxMacHashSha1&& other) = delete;
    /// @brief Default move assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01266
    /// @needwork = dda
    /// @endcode
    PCtxMacHashSha1& operator=(PCtxMacHashSha1&& other) = delete;
    /// @brief Default copy assignment operator
    /// @param other another object instance of this class
    /// @return  *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01267
    /// @needwork = dda
    /// @endcode
    PCtxMacHashSha1& operator=(PCtxMacHashSha1 const& other) = delete;
    /// @brief Default copy constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01268
    /// @needwork = dda
    /// @endcode
    PCtxMacHashSha1(PCtxMacHashSha1 const& other) = delete;

private:
    /// @brief MAC calculation result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01269
    /// @needwork = dda
    /// @endcode
    mutable uint8_t macData_[kInt8_20U]{kInt8_0U};

protected:
    /// @brief Initialize member variables
    /// @name  InitMacResult
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01270
    /// @needwork = dda
    /// @endcode
    void InitMacResult() noexcept override
    {
        std::ignore = memset(static_cast< uint8_t* >(macData_), 0, sizeof(macData_));
    }

public:
    /// @brief Get the length of the MAC result
    /// @name   GetMacLength
    /// @returns length of the MAC result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01271
    /// @needwork = dda
    /// @endcode
    uint32_t GetMacLength() const noexcept override;
    /// @brief Get the MAC result
    /// @name   GetMacResult
    /// @returns MAC result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01272
    /// @needwork = dda
    /// @endcode
    inline uint8_t* GetMacResult() const noexcept override { return macData_; }
    /// @brief Get the cryptographic primitive ID
    /// @name   GetCryptoPrimitiveId
    /// @returns cryptographic primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01273
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override;
    /// @brief Get the cryptographic primitive ID of the hash algorithm
    /// @return cryptographic primitive ID of the hash algorithm
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01274
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::AlgId GetHashAlgId() const noexcept override;
};

/// @brief Message authentication code implemented based on hash_sha function interface: Sha2_224
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00256
/// @trace_id_dd=DD_CRYPTO_01275
/// @unit_name=PCtxMacHashSha2_224
/// @unit_description=Message authentication code based on SHA2-224
/// @needwork = ad
/// @endcode
class PCtxMacHashSha2_224 : public PCtxMacHash
{
public:
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06271
    /// @needwork = dda
    /// @endcode
    using PCtxMacHash::PCtxMacHash;
    /// @brief Default virtual destructor
    /// @name   ~PCtxMacHashSha2_224
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01276
    /// @needwork = dda
    /// @endcode
    ~PCtxMacHashSha2_224() override = default;
    /// @brief Default move constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01277
    /// @needwork = dda
    /// @endcode
    PCtxMacHashSha2_224(PCtxMacHashSha2_224&& other) = delete;
    /// @brief Default move assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01278
    /// @needwork = dda
    /// @endcode
    PCtxMacHashSha2_224& operator=(PCtxMacHashSha2_224&& other) = delete;
    /// @brief Default copy assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01279
    /// @needwork = dda
    /// @endcode
    PCtxMacHashSha2_224& operator=(PCtxMacHashSha2_224 const& other) = delete;
    /// @brief Default copy constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01280
    /// @needwork = dda
    /// @endcode
    PCtxMacHashSha2_224(PCtxMacHashSha2_224 const& other) = delete;

private:
    /// @brief MAC calculation result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01281
    /// @needwork = dda
    /// @endcode
    mutable uint8_t macData_[kInt8_28U]{kInt8_0U};

protected:
    /// @brief Initialize member variables
    /// @name  InitMacResult
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01282
    /// @needwork = dda
    /// @endcode
    void InitMacResult() noexcept override
    {
        std::ignore = memset(static_cast< uint8_t* >(macData_), 0, sizeof(macData_));
    }

public:
    /// @brief Get the length of the MAC result
    /// @name   GetMacLength
    /// @returns length of the MAC result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01283
    /// @needwork = dda
    /// @endcode
    uint32_t GetMacLength() const noexcept override;
    /// @brief Get the MAC result
    /// @name   GetMacResult
    /// @returns starting address of the MAC result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01284
    /// @needwork = dda
    /// @endcode
    inline uint8_t* GetMacResult() const noexcept override { return macData_; }
    /// @brief Get the cryptographic primitive ID
    /// @name   GetCryptoPrimitiveId
    /// @returns cryptographic primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01285
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override;
    /// @brief Get the cryptographic primitive ID of the hash algorithm used by MAC
    /// @return cryptographic primitive ID of the hash algorithm used by MAC
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01286
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::AlgId GetHashAlgId() const noexcept override;
};

/// @brief Message authentication code implemented based on hash_sha function interface: Sha2_256
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00257
/// @trace_id_dd=DD_CRYPTO_01287
/// @unit_name=PCtxMacHashSha2_256
/// @unit_description=Message authentication code based on SHA2-256
/// @needwork = ad
/// @endcode
class PCtxMacHashSha2_256 : public PCtxMacHash
{
public:
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06272
    /// @needwork = dda
    /// @endcode
    using PCtxMacHash::PCtxMacHash;
    /// @brief Default virtual destructor
    /// @name   ~PCtxMacHashSha2_256
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01288
    /// @needwork = dda
    /// @endcode
    ~PCtxMacHashSha2_256() override = default;
    /// @brief Default move constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01289
    /// @needwork = dda
    /// @endcode
    PCtxMacHashSha2_256(PCtxMacHashSha2_256&& other) = delete;
    /// @brief Default move assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01290
    /// @needwork = dda
    /// @endcode
    PCtxMacHashSha2_256& operator=(PCtxMacHashSha2_256&& other) = delete;
    /// @brief Default copy assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01291
    /// @needwork = dda
    /// @endcode
    PCtxMacHashSha2_256& operator=(PCtxMacHashSha2_256 const& other) = delete;
    /// @brief Default copy constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01292
    /// @needwork = dda
    /// @endcode
    PCtxMacHashSha2_256(PCtxMacHashSha2_256 const& other) = delete;

private:
    /// @brief MAC calculation result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01293
    /// @needwork = dda
    /// @endcode
    mutable uint8_t macData_[kInt8_32U]{kInt8_0U};

protected:
    /// @brief Initialize member variables
    /// @name  InitMacResult
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01294
    /// @needwork = dda
    /// @endcode
    void InitMacResult() noexcept override
    {
        std::ignore = memset(static_cast< uint8_t* >(macData_), 0, sizeof(macData_));
    }

public:
    /// @brief Get the length of the MAC result
    /// @name   GetMacLength
    /// @returns length of the MAC result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01295
    /// @needwork = dda
    /// @endcode
    uint32_t GetMacLength() const noexcept override;
    /// @brief Get the MAC result
    /// @name   GetMacResult
    /// @returns starting address of the MAC result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01296
    /// @needwork = dda
    /// @endcode
    inline uint8_t* GetMacResult() const noexcept override { return macData_; }
    /// @brief Get the cryptographic primitive ID
    /// @name   GetCryptoPrimitiveId
    /// @returns cryptographic primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01297
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override;
    /// @brief Get the cryptographic primitive ID of the hash algorithm
    /// @return cryptographic primitive ID of the hash algorithm
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01298
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::AlgId GetHashAlgId() const noexcept override;
};

/// @brief Message authentication code implemented based on hash_sha function interface: Sha2_384
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00258
/// @trace_id_dd=DD_CRYPTO_01299
/// @unit_name=PCtxMacHashSha2_384
/// @unit_description=Message authentication code based on SHA2-384
/// @needwork = ad
/// @endcode
class PCtxMacHashSha2_384 : public PCtxMacHash
{
public:
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06273
    /// @needwork = dda
    /// @endcode
    using PCtxMacHash::PCtxMacHash;
    /// @code{.isoft}
    ///@brief : Default virtual destructor
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01300
    /// @needwork = dda
    /// @endcode
    ~PCtxMacHashSha2_384() override = default;
    /// @brief Default move constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01301
    /// @needwork = dda
    /// @endcode
    PCtxMacHashSha2_384(PCtxMacHashSha2_384&& other) = delete;
    /// @brief Default move assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01302
    /// @needwork = dda
    /// @endcode
    PCtxMacHashSha2_384& operator=(PCtxMacHashSha2_384&& other) = delete;
    /// @brief Default copy assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01303
    /// @needwork = dda
    /// @endcode
    PCtxMacHashSha2_384& operator=(PCtxMacHashSha2_384 const& other) = delete;
    /// @brief Default copy constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01304
    /// @needwork = dda
    /// @endcode
    PCtxMacHashSha2_384(PCtxMacHashSha2_384 const& other) = delete;

private:
    /// @brief MAC calculation result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01305
    /// @needwork = dda
    /// @endcode
    mutable uint8_t macData_[kInt8_48U]{kInt8_0U};

protected:
    /// @brief Initialize member variables
    /// @name  InitMacResult
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01306
    /// @needwork = dda
    /// @endcode
    void InitMacResult() noexcept override
    {
        std::ignore = memset(static_cast< uint8_t* >(macData_), 0, sizeof(macData_));
    }

public:
    /// @brief Get the length of the MAC result
    /// @name   GetMacLength
    /// @returns length of the MAC result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01307
    /// @needwork = dda
    /// @endcode
    uint32_t GetMacLength() const noexcept override;
    /// @brief Get the MAC result
    /// @name   GetMacResult
    /// @returns starting address of the MAC result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01308
    /// @needwork = dda
    /// @endcode
    inline uint8_t* GetMacResult() const noexcept override { return macData_; }
    /// @brief Get the cryptographic primitive ID
    /// @name   GetCryptoPrimitiveId
    /// @returns cryptographic primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01309
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override;
    /// @brief Get the cryptographic primitive ID of the hash algorithm
    /// @return cryptographic primitive ID of the hash algorithm
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01310
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::AlgId GetHashAlgId() const noexcept override;
};

/// @brief Message authentication code implemented based on hash_sha function interface: Sha2_512
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00259
/// @trace_id_dd=DD_CRYPTO_01311
/// @unit_name=PCtxMacHashSha2_512
/// @unit_description=Message authentication code based on SHA2-512
/// @needwork = ad
/// @endcode
class PCtxMacHashSha2_512 : public PCtxMacHash
{
public:
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06274
    /// @needwork = dda
    /// @endcode
    using PCtxMacHash::PCtxMacHash;
    /// @brief Default virtual destructor
    /// @name   ~PCtxMacHashSha2_512
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01312
    /// @needwork = dda
    /// @endcode
    ~PCtxMacHashSha2_512() override = default;
    /// @brief Default move constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01313
    /// @needwork = dda
    /// @endcode
    PCtxMacHashSha2_512(PCtxMacHashSha2_512&& other) = delete;
    /// @brief Default move assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01314
    /// @needwork = dda
    /// @endcode
    PCtxMacHashSha2_512& operator=(PCtxMacHashSha2_512&& other) = delete;
    /// @brief Default copy assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01315
    /// @needwork = dda
    /// @endcode
    PCtxMacHashSha2_512& operator=(PCtxMacHashSha2_512 const& other) = delete;
    /// @brief Default copy constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01316
    /// @needwork = dda
    /// @endcode
    PCtxMacHashSha2_512(PCtxMacHashSha2_512 const& other) = delete;

private:
    /// @brief MAC calculation result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01317
    /// @needwork = dda
    /// @endcode
    mutable uint8_t macData_[kInt8_64U]{kInt8_0U};

protected:
    /// @brief Initialize member variables
    /// @name  InitMacResult
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01318
    /// @needwork = dda
    /// @endcode
    void InitMacResult() noexcept override
    {
        std::ignore = memset(static_cast< uint8_t* >(macData_), 0, sizeof(macData_));
    }

public:
    /// @brief Get the length of the MAC result
    /// @name   GetMacLength
    /// @returns length of the MAC result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01319
    /// @needwork = dda
    /// @endcode
    uint32_t GetMacLength() const noexcept override;
    /// @brief Get the MAC result
    /// @name   GetMacResult
    /// @returns starting address of the MAC result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01320
    /// @needwork = dda
    /// @endcode
    inline uint8_t* GetMacResult() const noexcept override { return macData_; }
    /// @brief Get the cryptographic primitive ID
    /// @name   GetCryptoPrimitiveId
    /// @returns cryptographic primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01321
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override;
    /// @brief Get the cryptographic primitive ID of the hash algorithm
    /// @return cryptographic primitive ID of the hash algorithm
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01322
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::AlgId GetHashAlgId() const noexcept override;
};
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_PUHUA_CTX_MAC_HASH_SHA_H_