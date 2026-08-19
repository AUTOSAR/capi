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
/// @file       isoft_ctx_decryptor_private_rsa.h
/// @brief      AutoSar-Crypto encryption/decryption module
/// @details    Asymmetric encryption module. RSA private key decryption
/// @date       2022-03-02
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr><td>2022-03-02</td><td>1.0.0</td><td>hanjingjing</td><td>Create initial version</td>
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default encryption/Asymmetric encryption
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_01008
/// @unit_name=PCtxDecryptorPrivate_Rsa
/// @unit_description=Asymmetric decryption private key context interface
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_PUHUA_CTX_DECRYPTOR_PRIVATE_RSA_H_
#define ARA_CRYPTO_CRYP_PUHUA_CTX_DECRYPTOR_PRIVATE_RSA_H_

#include "ara/crypto/cryp/asymmetric/isoft_ctx_decryptor_private.h"
#include "ara/crypto/cryp/cryobj/isoft_key_private_rsa.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
//********************************/
/// @brief Asymmetric decryption private key context interface.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00459
/// @trace_id_dd=DD_CRYPTO_01851
/// @needwork = ad
/// @endcode
class PCtxDecryptorPrivate_Rsa : public PCtxDecryptorPrivate
{
public:
    /// @brief Unique pointer type alias
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00459
    /// @trace_id_dd=DD_CRYPTO_06300
    /// @needwork = dd
    /// @endcode
    using Uptr = std::unique_ptr< PCtxDecryptorPrivate_Rsa >;

public:
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06301
    /// @needwork = dda
    /// @endcode
    using PCtxDecryptorPrivate::PCtxDecryptorPrivate;
    /// @brief default virtual destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01852
    /// @needwork = dda
    /// @endcode
    ~PCtxDecryptorPrivate_Rsa() override = default;
    /// @brief default move constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01853
    /// @needwork = dda
    /// @endcode
    PCtxDecryptorPrivate_Rsa(PCtxDecryptorPrivate_Rsa&& other) = delete;
    /// @brief default copy constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01854
    /// @needwork = dda
    /// @endcode
    PCtxDecryptorPrivate_Rsa(PCtxDecryptorPrivate_Rsa const& other) = delete;
    /// @brief default move assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01855
    /// @needwork = dda
    /// @endcode
    PCtxDecryptorPrivate_Rsa& operator=(PCtxDecryptorPrivate_Rsa&& other) = delete;
    /// @brief default copy assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01856
    /// @needwork = dda
    /// @endcode
    PCtxDecryptorPrivate_Rsa& operator=(PCtxDecryptorPrivate_Rsa const& other) = delete;

public:  // AUTOSAR-AP interface
    /// @brief Returns the CryptoPrimitivId instance containing the instance identifier.
    /// @return Crypto primitive ID instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01857
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override;
    /// @brief Get the CryptoService instance.
    /// @return CryptoService instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01858
    /// @needwork = dda
    /// @endcode
    CryptoService::Uptr GetCryptoService() const noexcept override;

public:  // PServiceCrypto interface
    /// @brief Get the maximum supported key length (in bits).
    /// @return Maximum key length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01859
    /// @needwork = dda
    /// @endcode
    std::size_t GetMaxKeyBitLength() const noexcept override;
    /// @brief Get the minimum supported key length (in bits).
    /// @return Minimum key length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01860
    /// @needwork = dda
    /// @endcode
    std::size_t GetMinKeyBitLength() const noexcept override;
    /// @brief Validate support for a specific key length according to the context.
    /// @param keyBitLength Key length in bits
    /// @return true support length false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01861
    /// @needwork = dda
    /// @endcode
    bool IsKeyBitLengthSupported(std::size_t keyBitLength) const noexcept override;
    /// @brief Get the block (or internal buffer) size of the underlying algorithm.
    ///        For digest, byte stream cipher and RNG contexts, it is an informational method only used to optimize interface usage.
    /// @return Algorithm block size
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01862
    /// @needwork = dda
    /// @endcode
    std::size_t GetBlockSize() const noexcept override;

protected:
    /// @brief Check whether the key meets requirements.
    /// @param key Private key
    /// @return true if check key sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01863
    /// @needwork = dda
    /// @endcode
    bool CheckKey(PrivateKey const& key) const noexcept override;
    /// @brief Perform encryption/decryption.
    /// @param pInputData Starting address of input data for algorithm operation
    /// @param nDataLen Data length
    /// @param suppressPadding Whether to suppress padding
    /// @return Encrypted data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01864
    /// @needwork = dda
    /// @endcode
    ara::core::Result< ara::core::Vector< ara::core::Byte > > ExecuteDecryptoLogic(
        uint8_t const* pInputData, uint32_t nDataLen, bool suppressPadding) const noexcept override;
};
//********************************/
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_PUHUA_CTX_DECRYPTOR_PRIVATE_RSA_H_
