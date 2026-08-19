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
/// @file       isoft_ctx_hash_function_sha.h
/// @brief      AutoSar-Crypto Encryption Module
/// @details    Hash function interface: SHA.
/// @date       2022-01-19
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
/// @trace_id_sr=SR_CRYPTO_01022
/// @unit_name=PCtxHashFunctionSha
/// @unit_description=Hash Context Based on SHA
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_PUHUA_CTX_HASH_FUNCTION_SHA_H_
#define ARA_CRYPTO_CRYP_PUHUA_CTX_HASH_FUNCTION_SHA_H_

#include <openssl/sha.h>

#include "ara/crypto/cryp/hash/isoft_ctx_hash_function.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
//********************************/
/// @brief Puhua hash interface: SHA.
///         Base class for all Puhua Hash.SHA derived classes. Direct use of this class is not allowed; derived classes must be used.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00151
/// @trace_id_dd=DD_CRYPTO_00845
/// @needwork = ad
/// @endcode
class PCtxHashFunctionSha : public PCtxHashFunction
{
public:
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06251
    /// @needwork = dda
    /// @endcode
    using PCtxHashFunction::PCtxHashFunction;
    /// @brief Default virtual destructor
    /// @name   ~PCtxHashFunctionSha
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00846
    /// @needwork = dda
    /// @endcode
    ~PCtxHashFunctionSha() noexcept override = default;
    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00847
    /// @needwork = dda
    /// @endcode
    PCtxHashFunctionSha(PCtxHashFunctionSha &&other) = delete;
    /// @brief Default move assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00848
    /// @needwork = dda
    /// @endcode
    PCtxHashFunctionSha &operator=(PCtxHashFunctionSha &&other) = delete;
    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00849
    /// @needwork = dda
    /// @endcode
    PCtxHashFunctionSha &operator=(PCtxHashFunctionSha const &other) = delete;
    /// @brief Default copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00850
    /// @needwork = dda
    /// @endcode
    PCtxHashFunctionSha(PCtxHashFunctionSha const &other) = delete;

public:
    /// @brief Returns the CryptoPrimitivId instance containing the instance identifier.
    /// @name   GetCryptoPrimitiveId
    /// @returns CryptoPrimitivId instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00851
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override = 0;

public:
    /// @brief Gets the DigestService instance.
    /// @brief Get DigestService instance.
    /// @name  GetDigestService
    /// @returns DigestService instance
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_21102}
    /// @uptrace={RS_CRYPTO_02006}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00152
    /// @trace_id_dd=DD_CRYPTO_00853
    /// @needwork = ad
    /// @endcode
    DigestService::Uptr GetDigestService() const noexcept override;

public:  // Interface of PCtxHashFunction: Provided for DigestService calls
    /// @brief Returns the length of the Hash result
    /// @name   GetHashLength
    /// @returns Hash result length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00854
    /// @needwork = dda
    /// @endcode
    uint32_t GetHashLength() const noexcept override = 0;
    /// @brief Gets the maximum IV length corresponding to the specific algorithm; SHA series do not require IV, so length is set to -1.
    /// @name   GetIvMaxLength
    /// @returns Corresponding maximum IV length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00855
    /// @needwork = dda
    /// @endcode
    uint32_t GetIvMaxLength() const noexcept override = 0;

protected:  // Interface of PCtxHashFunction
    /// @brief Get hash calculation result
    /// @name   GetHashResult
    /// @returns Pointer to hash data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00856
    /// @needwork = dda
    /// @endcode
    uint8_t const *GetHashResult() const noexcept override = 0;
    /// @brief Perform initialization operation: via initialization vector
    /// @name   DoInitByIV
    /// @param piv pointer to initialization vector
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00857
    /// @needwork = dda
    /// @endcode
    void DoInitByIV(uint8_t const *piv) noexcept override = 0;
    /// @brief Perform initialization operation
    /// @name   DoInit
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00858
    /// @needwork = dda
    /// @endcode
    void DoInit() noexcept override;
    /// @brief Perform update operation
    /// @name   DoUpdate
    /// @param pVoidData starting address of data
    /// @param nDataLen data length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00859
    /// @needwork = dda
    /// @endcode
    void DoUpdate(void const *pVoidData, uint32_t nDataLen) noexcept override = 0;
    /// @brief Perform finalization operation
    /// @name   DoFinish
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00860
    /// @needwork = dda
    /// @endcode
    void DoFinish() noexcept override = 0;
    /// @brief Check whether the specific hash function supports IV; SHA series all require IV
    /// @name   SupportIv
    /// @returns true if support iv false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00861
    /// @needwork = dda
    /// @endcode
    bool SupportIv() const noexcept override;
    /// @brief Check initialization vector
    /// @param piv pointer to initialization vector
    /// @return true if support IV false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00862
    /// @needwork = dda
    /// @endcode
    static bool CheckIV(uint8_t const *const piv) noexcept;
};
//********************************/
/// @brief Puhua hash interface: SHA1.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00153
/// @trace_id_dd=DD_CRYPTO_00863
/// @needwork = ad
/// @endcode
class PCtxHashFunctionSha1 : public PCtxHashFunctionSha
{
private:
    /// @brief hash Data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00865
    /// @needwork = dda
    /// @endcode
    uint8_t hashDataSha_[SHA_DIGEST_LENGTH + kPhHashTailLength]{0U};

public:
    /// @brief Use base class constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06252
    /// @needwork = dda
    /// @endcode
    using PCtxHashFunctionSha::PCtxHashFunctionSha;
    /// @name   ~PCtxHashFunctionSha1
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00866
    /// @needwork = dda
    /// @endcode
    ~PCtxHashFunctionSha1() override = default;
    /// @brief Define copy assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00867
    /// @needwork = dda
    /// @endcode
    PCtxHashFunctionSha1 &operator=(PCtxHashFunctionSha1 const &other) const noexcept = delete;
    /// @brief Default copy constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00868
    /// @needwork = dda
    /// @endcode
    PCtxHashFunctionSha1(PCtxHashFunctionSha1 const &other) = delete;
    /// @brief Default move constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00869
    /// @needwork = dda
    /// @endcode
    PCtxHashFunctionSha1(PCtxHashFunctionSha1 &&other) = delete;
    /// @brief Default move assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00870
    /// @needwork = dda
    /// @endcode
    PCtxHashFunctionSha1 &operator=(PCtxHashFunctionSha1 &&other) = delete;
    /// @brief Return the CryptoPrimitivId instance containing the instance identifier.
    /// @name   GetCryptoPrimitiveId
    /// @returns CryptoPrimitivId instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00871
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override;

public:  // PCtxHashFunction interface: provided for DigestService to call
    /// @brief Return the length of the hash result
    /// @name   GetHashLength
    /// @returns length of the hash result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00872
    /// @needwork = dda
    /// @endcode
    uint32_t GetHashLength() const noexcept override;

protected:  // PCtxHashFunction interface
    /// @brief Get the hash calculation result
    /// @name   GetHashResult
    /// @returns pointer to the hash result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00873
    /// @needwork = dda
    /// @endcode
    inline uint8_t const *GetHashResult() const noexcept override { return hashDataSha_; }
    /// @brief Perform initialization operation: via initialization vector
    /// @name   DoInitByIV
    /// @param piv pointer to initialization vector
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00874
    /// @needwork = dda
    /// @endcode
    void DoInitByIV(uint8_t const *piv) noexcept override;
    /// @brief Perform update operation
    /// @name   DoUpdate
    /// @param pVoidData starting address of data
    /// @param nDataLen data length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00875
    /// @needwork = dda
    /// @endcode
    void DoUpdate(void const *pVoidData, uint32_t nDataLen) noexcept override;
    /// @brief Perform finalization operation
    /// @name   DoFinish
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00876
    /// @needwork = dda
    /// @endcode
    void DoFinish() noexcept override;
    /// @brief Get the maximum length of the corresponding IV according to the specific algorithm
    /// @name   GetIvMaxLength
    /// @returns maximum length of the corresponding IV
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00877
    /// @needwork = dda
    /// @endcode
    uint32_t GetIvMaxLength() const noexcept override;
};
//***************/
/// @brief Puhua hash interface: SHA2-224.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00154
/// @trace_id_dd=DD_CRYPTO_00878
/// @needwork = ad
/// @endcode
class PCtxHashFunctionSha2_224 : public PCtxHashFunctionSha
{
private:
    /// @brief hash Data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00880
    /// @needwork = dda
    /// @endcode
    uint8_t hashDataSha_[SHA224_DIGEST_LENGTH + kPhHashTailLength]{0U};

public:
    /// @brief Use base class constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06253
    /// @needwork = dda
    /// @endcode
    using PCtxHashFunctionSha::PCtxHashFunctionSha;
    /// @name   ~PCtxHashFunctionSha2_224
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00881
    /// @needwork = dda
    /// @endcode
    ~PCtxHashFunctionSha2_224() override = default;
    /// @brief Default copy constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00882
    /// @needwork = dda
    /// @endcode
    PCtxHashFunctionSha2_224(PCtxHashFunctionSha2_224 const &other) = delete;
    /// @brief Default move constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00883
    /// @needwork = dda
    /// @endcode
    PCtxHashFunctionSha2_224(PCtxHashFunctionSha2_224 &&other) = delete;
    /// @brief Default move assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00884
    /// @needwork = dda
    /// @endcode
    PCtxHashFunctionSha2_224 &operator=(PCtxHashFunctionSha2_224 &&other) = delete;
    /// @brief Define copy assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00885
    /// @needwork = dda
    /// @endcode
    PCtxHashFunctionSha2_224 &operator=(PCtxHashFunctionSha2_224 const &other) const noexcept = delete;
    /// @brief Return the CryptoPrimitivId instance containing the instance identifier.
    /// @name   GetCryptoPrimitiveId
    /// @returns CryptoPrimitivId instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00886
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override;

public:  // PCtxHashFunction interface: provided for DigestService to call
    /// @brief Return the length of the hash result
    /// @name   GetHashLength
    /// @returns length of the hash result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00887
    /// @needwork = dda
    /// @endcode
    uint32_t GetHashLength() const noexcept override;
    /// @brief Get the maximum length of the corresponding IV according to the specific algorithm
    /// @name   GetIvMaxLength
    /// @returns maximum length of the corresponding IV
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00888
    /// @needwork = dda
    /// @endcode
    uint32_t GetIvMaxLength() const noexcept override;

protected:  // PCtxHashFunction interface
    /// @brief Get the hash calculation result
    /// @name   GetHashResult
    /// @returns pointer to the hash calculation result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00889
    /// @needwork = dda
    /// @endcode
    inline uint8_t const *GetHashResult() const noexcept override { return hashDataSha_; }
    /// @brief Perform initialization operation: via initialization vector
    /// @name   DoInitByIV
    /// @param piv pointer to initialization vector
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00890
    /// @needwork = dda
    /// @endcode
    void DoInitByIV(uint8_t const *piv) noexcept override;
    /// @brief Perform update operation
    /// @name   DoUpdate
    /// @param pVoidData starting address of data
    /// @param nDataLen data length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00891
    /// @needwork = dda
    /// @endcode
    void DoUpdate(void const *pVoidData, uint32_t nDataLen) noexcept override;
    /// @brief Perform finalization operation
    /// @name   DoFinish
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00892
    /// @needwork = dda
    /// @endcode
    void DoFinish() noexcept override;
};
//***************/
/// @brief Puhua hash interface: SHA2-256.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00155
/// @trace_id_dd=DD_CRYPTO_00893
/// @needwork = ad
/// @endcode
class PCtxHashFunctionSha2_256 : public PCtxHashFunctionSha
{
private:
    /// @brief hash Data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00895
    /// @needwork = dda
    /// @endcode
    uint8_t hashDataSha_[SHA256_DIGEST_LENGTH + kPhHashTailLength]{0U};

public:
    /// @brief Use base class constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06254
    /// @needwork = dda
    /// @endcode
    using PCtxHashFunctionSha::PCtxHashFunctionSha;
    /// @name   ~PCtxHashFunctionSha2_256
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00896
    /// @needwork = dda
    /// @endcode
    ~PCtxHashFunctionSha2_256() override = default;
    /// @brief Default copy constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00897
    /// @needwork = dda
    /// @endcode
    PCtxHashFunctionSha2_256(PCtxHashFunctionSha2_256 const &other) = delete;
    /// @brief Default move constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00898
    /// @needwork = dda
    /// @endcode
    PCtxHashFunctionSha2_256(PCtxHashFunctionSha2_256 &&other) = delete;
    /// @brief Default move assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00899
    /// @needwork = dda
    /// @endcode
    PCtxHashFunctionSha2_256 &operator=(PCtxHashFunctionSha2_256 &&other) = delete;
    /// @brief Define copy assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00900
    /// @needwork = dda
    /// @endcode
    PCtxHashFunctionSha2_256 &operator=(PCtxHashFunctionSha2_256 const &other) const noexcept = delete;
    /// @brief Return the CryptoPrimitivId instance containing the instance identifier.
    /// @name   GetCryptoPrimitiveId
    /// @returns CryptoPrimitivId instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00901
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override;

public:  // PCtxHashFunction interface: provided for DigestService to call
    /// @brief Return the length of the hash result
    /// @name   GetHashLength
    /// @returns length of the hash result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00902
    /// @needwork = dda
    /// @endcode
    uint32_t GetHashLength() const noexcept override;

protected:  // PCtxHashFunction interface
    /// @brief Get the hash calculation result
    /// @name   GetHashResult
    /// @returns pointer to the hash calculation result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00903
    /// @needwork = dda
    /// @endcode
    inline uint8_t const *GetHashResult() const noexcept override { return hashDataSha_; }
    /// @brief Perform initialization operation: via initialization vector
    /// @name   DoInitByIV
    /// @param piv pointer to initialization vector
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00904
    /// @needwork = dda
    /// @endcode
    void DoInitByIV(uint8_t const *piv) noexcept override;
    /// @brief Perform update operation
    /// @name   DoUpdate
    /// @param pVoidData starting address of data
    /// @param nDataLen data length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00905
    /// @needwork = dda
    /// @endcode
    void DoUpdate(void const *pVoidData, uint32_t nDataLen) noexcept override;
    /// @brief Perform finalization operation
    /// @name   DoFinish
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00906
    /// @needwork = dda
    /// @endcode
    void DoFinish() noexcept override;
    /// @brief Get the maximum length of the corresponding IV according to the specific algorithm
    /// @name   GetIvMaxLength
    /// @returns maximum length of the corresponding IV
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00907
    /// @needwork = dda
    /// @endcode
    uint32_t GetIvMaxLength() const noexcept override;
};
//***************/
/// @brief Puhua hash interface: SHA2-384.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00156
/// @trace_id_dd=DD_CRYPTO_00908
/// @needwork = ad
/// @endcode
class PCtxHashFunctionSha2_384 : public PCtxHashFunctionSha
{
private:
    /// @brief hash Data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00910
    /// @needwork = dda
    /// @endcode
    uint8_t hashDataSha_[SHA384_DIGEST_LENGTH + kPhHashTailLength]{0U};

public:
    /// @brief Use base class constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06255
    /// @needwork = dda
    /// @endcode
    using PCtxHashFunctionSha::PCtxHashFunctionSha;
    /// @name   ~PCtxHashFunctionSha2_384
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00911
    /// @needwork = dda
    /// @endcode
    ~PCtxHashFunctionSha2_384() override = default;
    /// @brief Default copy constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00912
    /// @needwork = dda
    /// @endcode
    PCtxHashFunctionSha2_384(PCtxHashFunctionSha2_384 const &other) = delete;
    /// @brief Default move constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00913
    /// @needwork = dda
    /// @endcode
    PCtxHashFunctionSha2_384(PCtxHashFunctionSha2_384 &&other) = delete;
    /// @brief Default move assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00914
    /// @needwork = dda
    /// @endcode
    PCtxHashFunctionSha2_384 &operator=(PCtxHashFunctionSha2_384 &&other) = delete;
    /// @brief Define copy assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00915
    /// @needwork = dda
    /// @endcode
    PCtxHashFunctionSha2_384 &operator=(PCtxHashFunctionSha2_384 const &other) const noexcept = delete;
    /// @brief Return the CryptoPrimitivId instance containing the instance identifier.
    /// @name   GetCryptoPrimitiveId
    /// @returns CryptoPrimitivId instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00916
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override;

public:  // PCtxHashFunction interface: provided for DigestService to call
    /// @brief Return the length of the hash result
    /// @name   GetHashLength
    /// @returns length of the hash result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00917
    /// @needwork = dda
    /// @endcode
    uint32_t GetHashLength() const noexcept override;

protected:  // PCtxHashFunction interface
    /// @brief Get the hash calculation result
    /// @name   GetHashResult
    /// @returns pointer to the hash calculation result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00918
    /// @needwork = dda
    /// @endcode
    inline uint8_t const *GetHashResult() const noexcept override { return hashDataSha_; }
    /// @brief Perform initialization operation: via initialization vector
    /// @name   DoInitByIV
    /// @param piv pointer to initialization vector
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00919
    /// @needwork = dda
    /// @endcode
    void DoInitByIV(uint8_t const *piv) noexcept override;
    /// @brief Perform update operation
    /// @name   DoUpdate
    /// @param pVoidData starting address of data
    /// @param nDataLen data length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00920
    /// @needwork = dda
    /// @endcode
    void DoUpdate(void const *pVoidData, uint32_t nDataLen) noexcept override;
    /// @brief Perform finalization operation
    /// @name   DoFinish
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00921
    /// @needwork = dda
    /// @endcode
    void DoFinish() noexcept override;
    /// @brief Get the maximum length of the corresponding IV according to the specific algorithm
    /// @name   GetIvMaxLength
    /// @returns maximum length of the corresponding IV
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00922
    /// @needwork = dda
    /// @endcode
    uint32_t GetIvMaxLength() const noexcept override;
};
//***************/
/// @brief Puhua hash interface: SHA2-512.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00157
/// @trace_id_dd=DD_CRYPTO_00923
/// @needwork = ad
/// @endcode
class PCtxHashFunctionSha2_512 : public PCtxHashFunctionSha
{
private:
    /// @brief sha
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00924
    /// @needwork = dda
    /// @endcode
    SHA512_CTX shaCtx_{{0U}, 0U, 0U, {{0U}}, 0U, 0U};
    /// @brief hash Data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00925
    /// @needwork = dda
    /// @endcode
    uint8_t hashDataSha_[SHA512_DIGEST_LENGTH + kPhHashTailLength]{0U};

public:
    /// @brief Use base class constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06256
    /// @needwork = dda
    /// @endcode
    using PCtxHashFunctionSha::PCtxHashFunctionSha;
    /// @name   ~PCtxHashFunctionSha2_512
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00926
    /// @needwork = dda
    /// @endcode
    ~PCtxHashFunctionSha2_512() override = default;
    /// @brief Default copy constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00927
    /// @needwork = dda
    /// @endcode
    PCtxHashFunctionSha2_512(PCtxHashFunctionSha2_512 const &other) = delete;
    /// @brief Default move constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00928
    /// @needwork = dda
    /// @endcode
    PCtxHashFunctionSha2_512(PCtxHashFunctionSha2_512 &&other) = delete;
    /// @brief Default move assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00929
    /// @needwork = dda
    /// @endcode
    PCtxHashFunctionSha2_512 &operator=(PCtxHashFunctionSha2_512 &&other) = delete;
    /// @brief Define copy assignment operator
    /// @param other another object instance of this class
    /// @return  *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00930
    /// @needwork = dda
    /// @endcode
    PCtxHashFunctionSha2_512 &operator=(PCtxHashFunctionSha2_512 const &other) const noexcept = delete;
    /// @brief Return the CryptoPrimitivId instance containing the instance identifier.
    /// @name   GetCryptoPrimitiveId
    /// @returns CryptoPrimitivId instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00931
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override;

public:  // PCtxHashFunction interface: provided for DigestService to call
    /// @brief Return the length of the hash result
    /// @name   GetHashLength
    /// @returns length of the hash result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00932
    /// @needwork = dda
    /// @endcode
    uint32_t GetHashLength() const noexcept override;

protected:  // PCtxHashFunction interface
    /// @brief Get the hash calculation result
    /// @name   GetHashResult
    /// @returns pointer to the hash calculation result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00933
    /// @needwork = dda
    /// @endcode
    inline uint8_t const *GetHashResult() const noexcept override { return hashDataSha_; }
    /// @brief Perform initialization operation: via initialization vector
    /// @name   DoInitByIV
    /// @param piv pointer to initialization vector
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00934
    /// @needwork = dda
    /// @endcode
    void DoInitByIV(uint8_t const *piv) noexcept override;
    /// @brief Perform update operation
    /// @name   DoUpdate
    /// @param pVoidData starting address of data
    /// @param nDataLen data length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00935
    /// @needwork = dda
    /// @endcode
    void DoUpdate(void const *pVoidData, uint32_t nDataLen) noexcept override;
    /// @brief Perform finalization operation
    /// @name   DoFinish
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00936
    /// @needwork = dda
    /// @endcode
    void DoFinish() noexcept override;
    /// @brief Get the maximum length of the corresponding IV according to the specific algorithm
    /// @name   GetIvMaxLength
    /// @returns maximum length of the corresponding IV
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00937
    /// @needwork = dda
    /// @endcode
    uint32_t GetIvMaxLength() const noexcept override;
};
//********************************/
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_PUHUA_CTX_HASH_FUNCTION_SHA_H_
