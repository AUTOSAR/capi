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
/// @file       isoft_ctx_hash_function_md5.h
/// @brief      AutoSar-Crypto Encryption Module
/// @details    Hash function interface: Md5.
/// @date       2022-01-18
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr><td>2022-01-18  <td>1.0.0    <td>hanjingjing  <td>Initial version creation
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default Encryption/Hash Function
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_01021
/// @unit_name=PCtxHashFunctionMd5
/// @unit_description=Hash Context Based on MD5
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_PUHUA_CTX_HASH_FUNCTION_MD5_H_
#define ARA_CRYPTO_CRYP_PUHUA_CTX_HASH_FUNCTION_MD5_H_

#include <openssl/md5.h>

#include "ara/crypto/cryp/hash/isoft_ctx_hash_function.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
//********************************/
/// @brief Puhua hash interface: CRC.
///         Base class for all Puhua Hash derived classes. Direct use of this class is not allowed; derived classes must be used.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00158
/// @trace_id_dd=DD_CRYPTO_00938
/// @needwork = ad
/// @endcode
class PCtxHashFunctionMd5 : public PCtxHashFunction
{
public:
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06257
    /// @needwork = dda
    /// @endcode
    using PCtxHashFunction::PCtxHashFunction;
    /// @brief Default virtual destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00939
    /// @needwork = dda
    /// @endcode
    ~PCtxHashFunctionMd5() noexcept override = default;
    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00940
    /// @needwork = dda
    /// @endcode
    PCtxHashFunctionMd5(PCtxHashFunctionMd5&& other) = delete;
    /// @brief Default move assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00941
    /// @needwork = dda
    /// @endcode
    PCtxHashFunctionMd5& operator=(PCtxHashFunctionMd5&& other) = delete;
    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00942
    /// @needwork = dda
    /// @endcode
    PCtxHashFunctionMd5& operator=(PCtxHashFunctionMd5 const& other) = delete;
    /// @brief Default copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00943
    /// @needwork = dda
    /// @endcode
    PCtxHashFunctionMd5(PCtxHashFunctionMd5 const& other) = delete;

public:
    /// @brief Returns the CryptoPrimitivId instance containing the instance identifier.
    /// @brief Return CryptoPrimitivId instance containing instance identification.
    /// @name   GetCryptoPrimitiveId
    /// @returns CryptoPrimitivId instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00944
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override;

public:
    /// @brief Gets the DigestService instance.
    /// @name   GetDigestService
    /// @returns DigestService instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00946
    /// @needwork = dda
    /// @endcode
    DigestService::Uptr GetDigestService() const noexcept override;

public:  // Interface of PCtxHashFunction: Provided for DigestService calls
    /// @brief Get hash result length: byte length
    /// @name   GetHashLength
    /// @returns Hash result length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00947
    /// @needwork = dda
    /// @endcode
    uint32_t GetHashLength() const noexcept override;
    /// @brief Gets the maximum IV length corresponding to the specific algorithm; MD5 series do not require IV, so length is set to -1.
    /// @name   GetIvMaxLength
    /// @returns Corresponding maximum IV length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00948
    /// @needwork = dda
    /// @endcode
    uint32_t GetIvMaxLength() const noexcept override;

protected:  // Interface of PCtxHashFunction
    /// @brief Get hash result
    /// @name   GetHashResult
    /// @returns Pointer to hash data result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00949
    /// @needwork = dda
    /// @endcode
    inline uint8_t const* GetHashResult() const noexcept override { return hashDataMD5_; }
    /// @brief Initialize MD5 hash context via initialization vector
    /// @param piv Pointer to initialization vector
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00950
    /// @needwork = dda
    /// @endcode
    void DoInitByIV(uint8_t const* piv) noexcept override;
    /// @brief Execute Init logic
    /// @name   DoInit
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00951
    /// @needwork = dda
    /// @endcode
    void DoInit() noexcept override;
    /// @brief Execute Update logic
    /// @name   DoUpdate
    /// @param pVoidData Starting address of data
    /// @param nDataLen Data length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00952
    /// @needwork = dda
    /// @endcode
    void DoUpdate(void const* pVoidData, uint32_t nDataLen) noexcept override;
    /// @brief Execute Finish logic
    /// @name   DoFinish
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00953
    /// @needwork = dda
    /// @endcode
    void DoFinish() noexcept override;
    /// @brief Check if the specific hash function supports IV; MD5 series do not require IV.
    /// @name   SupportIv
    /// @returns true if support iv false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00954
    /// @needwork = dda
    /// @endcode
    bool SupportIv() const noexcept override;

private:
    // MD5 Message-Digest Algorithm, a widely used cryptographic hash function producing a 128-bit (16-byte) hash value
    // to ensure information transmission integrity and consistency.
    /// @brief MD5_CTX
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00955
    /// @needwork = dda
    /// @endcode
    MD5_CTX md5CtxOpenssl_{};

private:
    /// @brief hash Data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00956
    /// @needwork = dda
    /// @endcode
    uint8_t hashDataMD5_[MD5_DIGEST_LENGTH + kPhHashTailLength]{0U};

protected:
    /// @brief GetHashDataMD5
    /// @return Pointer to MD5 data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00957
    /// @needwork = dda
    /// @endcode
    uint8_t const* _GetHashDataMD5() const noexcept { return hashDataMD5_; }
};
//********************************/
/// @brief Implementation of 16-bit Md5
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00159
/// @trace_id_dd=DD_CRYPTO_00958
/// @needwork = ad
/// @endcode
class PCtxHashFunctionMd5_16 : public PCtxHashFunctionMd5
{
public:
    /// @brief Get hash result length: byte length
    /// @name   GetHashLength
    /// @returns MD5 data length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00959
    /// @needwork = dda
    /// @endcode
    uint32_t GetHashLength() const noexcept override;

protected:  // Interface of PCtxHashFunction
    /// @brief Get hash result
    /// @name   GetHashResult
    /// @returns Pointer to MD5 data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00960
    /// @needwork = dda
    /// @endcode
    uint8_t const* GetHashResult() const noexcept override;
};
//********************************/
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_PUHUA_CTX_HASH_FUNCTION_MD5_H_
