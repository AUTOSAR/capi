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
/// @file       isoft_ctx_encryptor_public_rsa.h
/// @brief      AutoSar-Crypto encryption/decryption module
/// @details    Asymmetric encryption module. RSA public key encryption
/// @date       2022-03-02
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <td><td>2022-03-02</td><td>1.0.0</td><td>hanjingjing</td><td>Create initial version</td>
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default encryption/Asymmetric encryption
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_01008
/// @unit_name=PCtxEncryptorPublic_Rsa
/// @unit_description=Asymmetric encryption public key context interface
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_PUHUA_CTX_ENCRYPTOR_PUBLIC_RSA_H_
#define ARA_CRYPTO_CRYP_PUHUA_CTX_ENCRYPTOR_PUBLIC_RSA_H_

#include "ara/crypto/cryp/asymmetric/isoft_ctx_encryptor_public.h"
#include "ara/crypto/cryp/cryobj/isoft_key_public_rsa.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
//********************************/
/// @brief Asymmetric encryption public key context interface: RSA.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00460
/// @trace_id_dd=DD_CRYPTO_01865
/// @needwork = ad
/// @endcode
class PCtxEncryptorPublic_Rsa : public PCtxEncryptorPublic
{
public:
    /// @brief Unique smart pointer type alias
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00460
    /// @trace_id_dd=DD_CRYPTO_06302
    /// @needwork = dd
    /// @endcode
    using Uptr = std::unique_ptr< PCtxEncryptorPublic_Rsa >;

public:
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06303
    /// @needwork = dda
    /// @endcode
    using PCtxEncryptorPublic::PCtxEncryptorPublic;
    /// @brief default virtual destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01866
    /// @needwork = dda
    /// @endcode
    ~PCtxEncryptorPublic_Rsa() noexcept override = default;
    /// @brief default move constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01867
    /// @needwork = dda
    /// @endcode
    PCtxEncryptorPublic_Rsa(PCtxEncryptorPublic_Rsa&& other) = delete;
    /// @brief default copy constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01868
    /// @needwork = dda
    /// @endcode
    PCtxEncryptorPublic_Rsa(PCtxEncryptorPublic_Rsa const& other) = delete;
    /// @brief default move assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01869
    /// @needwork = dda
    /// @endcode
    PCtxEncryptorPublic_Rsa& operator=(PCtxEncryptorPublic_Rsa&& other) = delete;
    /// @brief default copy assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01870
    /// @needwork = dda
    /// @endcode
    PCtxEncryptorPublic_Rsa& operator=(PCtxEncryptorPublic_Rsa const& other) = delete;

public:  // AUTOSAR-AP interface
    /// @brief Returns the CryptoPrimitivId instance containing the instance identifier.
    /// @name   GetCryptoPrimitiveId
    /// @returns CryptoPrimitivId instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01871
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override;
    /// @brief Get the CryptoService instance.
    /// @name   GetCryptoService
    /// @returns CryptoService instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01872
    /// @needwork = dda
    /// @endcode
    CryptoService::Uptr GetCryptoService() const noexcept override;

public:  // PServiceCrypto interface
    /// @brief Get the maximum supported key length (in bits).
    /// @name   GetMaxKeyBitLength
    /// @returns Maximum key length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01873
    /// @needwork = dda
    /// @endcode
    std::size_t GetMaxKeyBitLength() const noexcept override;
    /// @brief Get the minimum supported key length (in bits).
    /// @name   GetMinKeyBitLength
    /// @returns Minimum key length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01874
    /// @needwork = dda
    /// @endcode
    std::size_t GetMinKeyBitLength() const noexcept override;
    /// @brief Validate support for a specific key length according to the context.
    /// @name   IsKeyBitLengthSupported
    /// @param keyBitLength Key length in bits
    /// @returns true if support false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01875
    /// @needwork = dda
    /// @endcode
    bool IsKeyBitLengthSupported(std::size_t keyBitLength) const noexcept override;
    /// @brief Get the block (or internal buffer) size of the underlying algorithm.
    /// For digest, byte stream cipher and RNG contexts, it is an informational method only used to optimize interface usage.
    /// @name   GetBlockSize
    /// @returns Block size
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01876
    /// @needwork = dda
    /// @endcode
    std::size_t GetBlockSize() const noexcept override;

protected:
    /// @brief Check whether the key meets requirements.
    /// @name   CheckKey
    /// @param key Public key
    /// @returns true if check sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01877
    /// @needwork = dda
    /// @endcode
    bool CheckKey(PublicKey const& key) const noexcept override;
    /// @brief Perform encryption/decryption.
    /// @name   ExecuteEncryptoLogic
    /// @param pInputData Starting address of input data for algorithm operation
    /// @param nDataLen Data length
    /// @param suppressPadding Whether to suppress padding
    /// @returns Encrypted data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01878
    /// @needwork = dda
    /// @endcode
    ara::core::Result< ara::core::Vector< ara::core::Byte > > ExecuteEncryptoLogic(
        uint8_t const* pInputData, uint32_t nDataLen, bool suppressPadding) const noexcept override;
    /// @brief Get key modulus length: different padding schemes in RSA correspond to different values.
    /// @name   GetModulusSize
    /// @param suppressPadding Whether to suppress padding
    /// @returns Key modulus length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01879
    /// @needwork = dda
    /// @endcode
    uint32_t GetModulusSize(bool suppressPadding) const noexcept override;
};
//********************************/
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_PUHUA_CTX_ENCRYPTOR_PUBLIC_RSA_H_
