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
/// @file       isoft_key_public_rsa.h
/// @brief      AutoSar-Crypto Encryption Module
/// @details    General asymmetric public key interface.
/// @date       2022-03-02
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr><td>2022-03-02  <td>1.0.0    <td>hanjingjing  <td>Initial version creation
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default Encryption/Asymmetric Public Key
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=PKeyPublic_Rsa
/// @unit_description=RSA Public Key
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_PUHUA_KEY_PUBLIC_RSA_H_
#define ARA_CRYPTO_CRYP_PUHUA_KEY_PUBLIC_RSA_H_

#include <openssl/bio.h>

#include "ara/crypto/cryp/cryobj/isoft_key_public.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
//********************************/
/// @brief General asymmetric public key interface.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01372
/// @trace_id_dd=DD_CRYPTO_03567
/// @needwork = ad
/// @endcode
class PKeyPublic_Rsa : public PKeyPublic_Base
{
public:
    /// @brief Constant unique smart pointer of the interface.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01372
    /// @trace_id_dd=DD_CRYPTO_06340
    /// @needwork = dd
    /// @endcode
    using Uptrc = std::unique_ptr< PKeyPublic_Rsa const >;
    /// @brief Unique smart pointer of the interface.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01372
    /// @trace_id_dd=DD_CRYPTO_06341
    /// @needwork = dd
    /// @endcode
    using Uptr = std::unique_ptr< PKeyPublic_Rsa >;

public:
    /// @brief Parameterized constructor
    /// @param stPublicKey Public key
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03568
    /// @needwork = dda
    /// @endcode
    explicit PKeyPublic_Rsa(ara::core::StringView const &stPublicKey) noexcept;
    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03569
    /// @needwork = dda
    /// @endcode
    PKeyPublic_Rsa() = delete;
    /// @brief Default virtual destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03570
    /// @needwork = dda
    /// @endcode
    ~PKeyPublic_Rsa() noexcept override;
    /// @brief Default copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03571
    /// @needwork = dda
    /// @endcode
    PKeyPublic_Rsa(PKeyPublic_Rsa const &other) = delete;
    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03572
    /// @needwork = dda
    /// @endcode
    PKeyPublic_Rsa(PKeyPublic_Rsa &&other) = delete;
    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03573
    /// @needwork = dda
    /// @endcode
    PKeyPublic_Rsa &operator=(PKeyPublic_Rsa const &other) = delete;
    /// @brief Default move assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03574
    /// @needwork = dda
    /// @endcode
    PKeyPublic_Rsa &operator=(PKeyPublic_Rsa &&other) = delete;

public:  // CryptoObject interface
    /// @brief Returns the CryptoPrimitivId of this cryptobject.
    /// @name   GetCryptoPrimitiveId
    /// @returns CryptoPrimitivId instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03575
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override;
    /// @brief Returns the actual size of the object payload. In bytes.
    /// @name   GetPayloadSize
    /// @returns Actual size of the payload
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03576
    /// @needwork = dda
    /// @endcode
    std::size_t GetPayloadSize() const noexcept override;
    /// @brief Save self to the provided IOInterface. Cryptobjects with the "session" attribute cannot be saved in KeySlots.
    /// @name   Save
    /// @param container IO interface
    /// @returns has value if save sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03577
    /// @needwork = dda
    /// @endcode
    ara::core::Result< void > Save(IOInterface &container) const noexcept override;

public:  // Serializable interface:
    /// @brief Serialize Public itself.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03578
    /// @needwork = dda
    /// @endcode
    ara::core::Result< ara::core::Vector< ara::core::Byte > >
    /// @name   ExportPublicly
    /// @param formatId Data format: Raw, DER, PEM, etc.
    /// @return Public key data
    ExportPublicly(Serializable::FormatId formatId = Serializable::kFormatDefault) const noexcept override;

public:  // PublicKey interface
    /// @brief Check the validity of the key.
    /// @name   CheckKey
    /// @param strongCheck Whether to enable strong verification
    /// @returns true if check sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03579
    /// @needwork = dda
    /// @endcode
    bool CheckKey(bool strongCheck = true) const noexcept override;

private:
    /// @brief Key length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03580
    /// @needwork = dda
    /// @endcode
    uint32_t nPublicKeyBitLength_;
    /// @brief RSA resource
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03581
    /// @needwork = dda
    /// @endcode
    EVP_PKEY *pRsaPair_;
    /// @brief If slotId_ is 0, it means both public and private keys are generated locally, CheckKey returns true directly
    /// @brief Public and private keys are in the same slot
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03582
    /// @needwork = dda
    /// @endcode
    uint32_t slotId_;
    /// @brief Public and private keys are stored separately, public key's slot Id
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03583
    /// @needwork = dda
    /// @endcode
    uint32_t onlyPublicKeyId_;
    /// @brief Parameterized constructor
    /// @param nPublicKeyBitLength Public key length: in bits
    /// @param stPublicKey Public key
    /// @param pRsaPair Pointer to RSA resource
    /// @param slotId Key slot ID
    /// @param onlyPublicKeyId Public key's slot Id
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03584
    /// @needwork = dda
    /// @endcode
    PKeyPublic_Rsa(uint32_t const nPublicKeyBitLength,
                   ara::core::StringView const &stPublicKey,
                   EVP_PKEY const *const pRsaPair,
                   uint32_t const slotId,
                   uint32_t const onlyPublicKeyId) noexcept;

public:
    /// @brief Get RSA resource
    /// @return RSA pointer instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03585
    /// @needwork = dda
    /// @endcode
    inline EVP_PKEY *GetRsa() const noexcept { return pRsaPair_; }
    /// @brief: Set RSA key via memory block
    /// @name   SetRsaKey
    /// @param pData Data: starting address of memory
    /// @param nLen Data length: in bytes
    /// @returns true if set sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03586
    /// @needwork = dda
    /// @endcode
    bool SetRsaKey(uint8_t const *pData, uint32_t const nLen) noexcept;
    /// @brief Set RSA resource
    /// @name   SetRsaKey
    /// @param pRsa RSA key resource
    /// @returns true if set sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03587
    /// @needwork = dda
    /// @endcode
    bool SetRsaKey(EVP_PKEY *const pRsa) noexcept;
    /// @brief Set the length of the public key
    /// @name   SetRsaKeyBitLength
    /// @param nKeyBitLength Key length: in bits
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03588
    /// @needwork = dda
    /// @endcode
    void SetRsaKeyBitLength(uint32_t nKeyBitLength) noexcept;
    /// @brief Get key slot ID
    /// @return Key slot ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03589
    /// @needwork = dda
    /// @endcode
    uint32_t GetSlotId() const noexcept { return slotId_; }
    /// @brief Set key slot ID
    /// @param slotId Key slot ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03590
    /// @needwork = dda
    /// @endcode
    void SetKeySlotId(uint32_t const slotId) noexcept { slotId_ = slotId; }
    /// @brief Get public key's slot ID
    /// @return Public key's slot ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03591
    /// @needwork = dda
    /// @endcode
    uint32_t GetPublicKeySlotId() const noexcept { return onlyPublicKeyId_; }
    /// @brief Set public key's slot ID
    /// @param slotId Key slot ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03592
    /// @needwork = dda
    /// @endcode
    void SetPublicKeySlotId(uint32_t const slotId) noexcept { onlyPublicKeyId_ = slotId; }

protected:
    /// @brief Release RSA resource
    /// @name   _DelRsa
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03593
    /// @needwork = dda
    /// @endcode
    void _DelRsa() noexcept;
    /// @brief Generate public key
    /// @name   _MakePublicKey
    /// @param pRsaPair Pointer to RSA resource
    /// @param formatId Data format: Raw, DER, PEM, etc.
    /// @returns Public key data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03594
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< ara::core::Byte > _MakePublicKey(EVP_PKEY *const pRsaPair,
                                                        Serializable::FormatId const formatId) const noexcept;
    /// @brief: Save key
    /// @name   _SaveToKeySlot
    /// @param container IO interface
    /// @returns true if save to keyslot sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03595
    /// @needwork = dda
    /// @endcode
    bool _SaveToKeySlot(IOInterface &container) const noexcept;
};
//********************************/
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_PUHUA_KEY_PUBLIC_RSA_H_
