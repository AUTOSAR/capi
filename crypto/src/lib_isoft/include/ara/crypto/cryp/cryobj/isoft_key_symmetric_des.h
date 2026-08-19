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
/// @file       isoft_key_symmetric_des.h
/// @brief      AutoSar-Crypto Encryption Module
/// @details    Symmetric key interface.
/// @date       2021-12-21
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr><td>2021-12-21  <td>1.0.0    <td>hanjingjing  <td>Initial version creation
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default Encryption/Symmetric Key
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=PKeySymmetric_Des
/// @unit_description=DES Symmetric Key
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_PUHUA_KEY_SYMMETRIC_DES_H_
#define ARA_CRYPTO_CRYP_PUHUA_KEY_SYMMETRIC_DES_H_

#include "ara/crypto/cryp/cryobj/isoft_key_symmetric.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
//********************************/
/// @brief Symmetric key interface: / Key used by Des
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01319
/// @trace_id_dd=DD_CRYPTO_03407
/// @needwork = ad
/// @endcode
class PKeySymmetric_Des : public PKeySymmetric_Base
{
public:
    /// @brief Constant unique smart pointer of the interface.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01319
    /// @trace_id_dd=DD_CRYPTO_06328
    /// @needwork = dd
    /// @endcode
    using Uptrc = std::unique_ptr< PKeySymmetric_Des const >;
    /// @brief Unique smart pointer of the interface.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01319
    /// @trace_id_dd=DD_CRYPTO_06329
    /// @needwork = dd
    /// @endcode
    using Uptr = std::unique_ptr< PKeySymmetric_Des >;
    /// @brief Constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03408
    /// @needwork = dda
    /// @endcode
    PKeySymmetric_Des() noexcept;
    /// @brief Parameterized constructor
    /// @name PKeySymmetric_Des
    /// @param nKeyBitLen Key length: in bits
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03409
    /// @needwork = dda
    /// @endcode
    explicit PKeySymmetric_Des(uint32_t const nKeyBitLen) noexcept;
    /// @brief Parameterized constructor
    /// @name  PKeySymmetric_Des
    /// @param pKeyData Key data
    /// @param nKeyByteLen Key length: in bytes
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03410
    /// @needwork = dda
    /// @endcode
    explicit PKeySymmetric_Des(void const *const pKeyData, uint32_t const nKeyByteLen) noexcept;
    /// @brief Default virtual destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03411
    /// @needwork = dda
    /// @endcode
    ~PKeySymmetric_Des() noexcept override = default;
    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03412
    /// @needwork = dda
    /// @endcode
    PKeySymmetric_Des(PKeySymmetric_Des &&other) = delete;
    /// @brief Default move assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03413
    /// @needwork = dda
    /// @endcode
    PKeySymmetric_Des &operator=(PKeySymmetric_Des &&other) = delete;
    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03414
    /// @needwork = dda
    /// @endcode
    PKeySymmetric_Des &operator=(PKeySymmetric_Des const &other) = delete;
    /// @brief Default copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03415
    /// @needwork = dda
    /// @endcode
    PKeySymmetric_Des(PKeySymmetric_Des const &other) = delete;

public:  ////CryptoObject interface
    /// @brief Returns the CryptoPrimitivId of this cryptobject.
    /// @name   GetCryptoPrimitiveId
    /// @returns CryptoPrimitiveId instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03416
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override;
    /// @brief Returns the actual size of the object payload. In bytes.
    /// @name   GetPayloadSize
    /// @returns Actual size of the payload
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03417
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
    /// @trace_id_dd=DD_CRYPTO_03418
    /// @needwork = dda
    /// @endcode
    ara::core::Result< void > Save(IOInterface &container) const noexcept override;

private:
    /// @brief Key data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03419
    /// @needwork = dda
    /// @endcode
    mutable uint8_t keyData_[static_cast< int32_t >(PAlgId_Symmetric_DesKey::EKeyLen::kDesKeyMaxBitLength) / kInt_8];
    /// @brief Key length: in bits
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03420
    /// @needwork = dda
    /// @endcode
    uint32_t nKeyBitLen_;

public:
    /// @brief: Get the memory sequence of KeyData
    /// @name   GetKeySymmetric
    /// @returns Pointer to key data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03421
    /// @needwork = dda
    /// @endcode
    inline uint8_t *GetKeySymmetric() const noexcept override { return static_cast< uint8_t * >(keyData_); }
    /// @brief Randomly initialize the key
    /// @name   RandomInitKey
    /// @param nKeyBitLength Key length: in bits
    /// @returns true if init key sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03422
    /// @needwork = dda
    /// @endcode
    bool RandomInitKey(int32_t const nKeyBitLength) noexcept;
    /// @brief Initialize a Des.Key
    /// @name   DebugInitKey
    /// @param stKey Key data: string format
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03423
    /// @needwork = dda
    /// @endcode
    void DebugInitKey(ara::core::StringView const &stKey) noexcept;
};
//***************/
/// @brief Symmetric key interface: 3Des
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01320
/// @trace_id_dd=DD_CRYPTO_03424
/// @needwork = ad
/// @endcode
class PKeySymmetric_3Des : public PKeySymmetric_Des
{
public:
    /// @brief Constructor
    /// @name   PKeySymmetric_3Des
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03425
    /// @needwork = dda
    /// @endcode
    PKeySymmetric_3Des() noexcept;
    /// @brief Constructor
    /// @name   PKeySymmetric_3Des
    /// @param pKeyData Key data
    /// @param nDataLen Data length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03426
    /// @needwork = dda
    /// @endcode
    explicit PKeySymmetric_3Des(void const *const pKeyData, uint32_t const nDataLen) noexcept;

public:  ////CryptoObject interface
    /// @brief Returns the CryptoPrimitivId of this cryptobject.
    /// @name   GetCryptoPrimitiveId
    /// @returns CryptoPrimitiveId instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03427
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override;
};
//***************/
/// @brief Symmetric key interface: 2Des
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01321
/// @trace_id_dd=DD_CRYPTO_03428
/// @needwork = ad
/// @endcode
class PSymmetricKey_2Des : public PKeySymmetric_Des
{
public:
    /// @brief Constructor
    /// @name   PSymmetricKey_2Des
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03429
    /// @needwork = dda
    /// @endcode
    PSymmetricKey_2Des() noexcept;
    /// @brief Constructor
    /// @name   PSymmetricKey_2Des
    /// @param pKeyData Key data
    /// @param nDataLen Data length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03430
    /// @needwork = dda
    /// @endcode
    explicit PSymmetricKey_2Des(void const *const pKeyData, uint32_t const nDataLen) noexcept;

public:  ////CryptoObject interface
    /// @brief Returns the CryptoPrimitivId of this cryptobject.
    /// @name   GetCryptoPrimitiveId
    /// @returns CryptoPrimitiveId instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03431
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override;
};
//********************************/
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_PUHUA_KEY_SYMMETRIC_DES_H_
