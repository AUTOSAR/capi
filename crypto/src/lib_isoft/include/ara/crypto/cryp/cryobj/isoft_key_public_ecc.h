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
/// @file       isoft_key_public_ecc.h
/// @brief      AutoSar-Crypto encryption/decryption module
/// @details    General asymmetric public key interface.
/// @date       2022-08-02
/// @author     Chang Zheng
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr><td>2022-08-02  <tr>1.0.0    <tr>Chang Zheng      <td>Create initial version</td>
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default encryption/Asymmetric public key
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=PKeyPublic_Ecc
/// @unit_description=ECC public key
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_PUHUA_KEY_PUBLIC_ECC_H_
#define ARA_CRYPTO_CRYP_PUHUA_KEY_PUBLIC_ECC_H_

#include <openssl/ec.h>

#include "ara/crypto/cryp/cryobj/isoft_key_public.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
//********************************/
/// @brief General asymmetric public key interface.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01283
/// @trace_id_dd=DD_CRYPTO_03302
/// @needwork = ad
/// @endcode
class PKeyPublic_Ecc : public PKeyPublic_Base
{
public:
    /// @brief Unique smart pointer for the interface.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01283
    /// @trace_id_dd=DD_CRYPTO_06324
    /// @needwork = dd
    /// @endcode
    using Uptr = std::unique_ptr< PKeyPublic_Ecc >;

public:
    /// @brief Parameterized constructor
    /// @param stPublicKey Public key
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03303
    /// @needwork = dda
    /// @endcode
    explicit PKeyPublic_Ecc(ara::core::StringView const &stPublicKey) noexcept;
    /// @brief default virtual destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03304
    /// @needwork = dda
    /// @endcode
    ~PKeyPublic_Ecc() noexcept override;
    /// @brief default copy constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03305
    /// @needwork = dda
    /// @endcode
    PKeyPublic_Ecc(PKeyPublic_Ecc const &other) = delete;
    /// @brief default move constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03306
    /// @needwork = dda
    /// @endcode
    PKeyPublic_Ecc(PKeyPublic_Ecc &&other) = delete;
    /// @brief default copy assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03307
    /// @needwork = dda
    /// @endcode
    PKeyPublic_Ecc &operator=(PKeyPublic_Ecc const &other) = delete;
    /// @brief default move assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03308
    /// @needwork = dda
    /// @endcode
    PKeyPublic_Ecc &operator=(PKeyPublic_Ecc &&other) = delete;

public:  // CryptoObject interface
    /// @brief Returns the CryptoPrimitivId of this crypto object.
    /// @name   GetCryptoPrimitiveId
    /// @returns CryptoPrimitiveId instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03309
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override;
    /// @brief Returns the actual size of the object payload. In bytes.
    /// @name   GetPayloadSize
    /// @returns Actual payload size
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03310
    /// @needwork = dda
    /// @endcode
    std::size_t GetPayloadSize() const noexcept override;
    /// @brief Save itself to the provided IOInterface. Crypto objects with the "session" attribute cannot be saved in KeySlot.
    /// @name   Save
    /// @param container IO interface
    /// @returns has value if save sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03311
    /// @needwork = dda
    /// @endcode
    ara::core::Result< void > Save(IOInterface &container) const noexcept override;

public:  // Serializable interface
    /// @brief Serialize the Public itself.
    /// @name   ExportPublicly
    /// @param formatId Data format: Raw, DER, PEM, etc.
    /// @return ECC public key data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03312
    /// @needwork = dda
    /// @endcode
    ara::core::Result< ara::core::Vector< ara::core::Byte > > ExportPublicly(
        Serializable::FormatId formatId = Serializable::kFormatDefault) const noexcept override;

public:  // PublicKey interface
    /// @brief Check the correctness of the key.
    /// @name   CheckKey
    /// @param strongCheck Whether to enable strong checking
    /// @returns true if check sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03313
    /// @needwork = dda
    /// @endcode
    bool CheckKey(bool strongCheck = true) const noexcept override;
    /// @brief Calculate the hash value of the public key value. The raw public key value BLOB can be obtained through the Serializable interface.
    /// @name   HashPublicKey
    /// @param hashFunc Crypto context object for hash calculation
    /// @return ECC public key data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03314
    /// @needwork = dda
    /// @endcode
    ara::core::Result< ara::core::Vector< ara::core::Byte > > HashPublicKey(
        HashFunctionCtx &hashFunc) const noexcept override;

private:
    /// @brief EC_KEY pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03315
    /// @needwork = dda
    /// @endcode
    EVP_PKEY *pEccPair_{};
    /// @brief Bit size of the generated ECC public key
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03316
    /// @needwork = dda
    /// @endcode
    size_t nGenPublicKeyBitLen_{};

public:
    /// @brief Get the EC_KEY pointer.
    /// @name   GetEcc
    /// @returns EC_KEY pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03317
    /// @needwork = dda
    /// @endcode
    inline EVP_PKEY *GetEcc() const noexcept { return pEccPair_; };
    /// @brief
    /// @name   PointToString
    /// @param pPublicKey Public key
    /// @param len Key length
    /// @returns Point string
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03319
    /// @needwork = dda
    /// @endcode
    void PointToString(u_char *const pPublicKey, size_t &len) const noexcept;
    /// @brief Set the length of the public key.
    /// @name   SetEccKeyBitLength
    /// @param nKeyBitLength Key length in bits
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03320
    /// @needwork = dda
    /// @endcode
    void SetEccKeyBitLength(uint32_t const nKeyBitLength) noexcept;

protected:
    /// @brief Delete the ECC structure.
    /// @name   _DelEcc
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03321
    /// @needwork = dda
    /// @endcode
    void _DelEcc() noexcept;
    /// @brief Generate the public key.
    /// @param pEccPair ECC key structure pointer
    /// @param formatId Data format: Raw, DER, PEM, etc.
    /// @returns Public key data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03322
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< ara::core::Byte > _MakePublicKey(EVP_PKEY const *const pEccPair,
                                                        Serializable::FormatId const formatId) const noexcept;
    /// @brief  Save the public key to the corresponding IoInterface.
    /// @name   _SaveToKeySlot
    /// @param  container
    /// @returns true if save sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03323
    /// @needwork = dda
    /// @endcode
    bool _SaveToKeySlot(IOInterface &container) const noexcept;
};
//********************************/
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_PUHUA_KEY_PUBLIC_ECC_H_
