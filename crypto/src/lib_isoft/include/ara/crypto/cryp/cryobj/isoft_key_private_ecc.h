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
/// @file       isoft_key_private_ecc.h
/// @brief      AutoSar-Crypto Encryption Module
/// @details    General asymmetric private key interface.
/// @date       2021-08-02
/// @author     Chang Zheng
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr><td>2021-08-02  <td>1.0.0    <td>Chang Zheng  <td>Initial version creation
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default Encryption/Asymmetric Private Key
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=PKeyPrivate_Ecc
/// @unit_description=ECC Private Key
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_PUHUA_KEY_PRIVATE_ECC_H_
#define ARA_CRYPTO_CRYP_PUHUA_KEY_PRIVATE_ECC_H_

#include "ara/crypto/common/isoft_data_type.h"
#include "ara/crypto/cryp/cryobj/isoft_key_private.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
// Select an elliptic curve
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01317
/// @trace_id_dd=DD_CRYPTO_03382
/// @needwork = dd
/// @endcode
size_t const kCurvesIndex{10U};
//********************************/
/// @brief General asymmetric private key interface.
/// @brief Generalized Asymmetric Private Key interface.
/// @code{.isoft}
/// @interface PKeyPrivate_Ecc
/// @AUTOSAR_SWS {SWS_CRYPT_22500}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02002}
/// @uptrace={RS_CRYPTO_02403}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01317
/// @trace_id_dd=DD_CRYPTO_03383
/// @needwork = ad
/// @endcode
class PKeyPrivate_Ecc : public PKeyPrivate_Base
{
public:
    /// @brief Unique smart pointer of the interface.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01317
    /// @trace_id_dd=DD_CRYPTO_06327
    /// @needwork = dd
    /// @endcode
    using Uptr = std::unique_ptr< PKeyPrivate_Ecc >;

public:
    /// @brief Parameterized constructor
    /// @param nGenKeyBitLength Key length: in bits
    /// @param nEccId ECC NID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03384
    /// @needwork = dda
    /// @endcode
    explicit PKeyPrivate_Ecc(std::size_t const nGenKeyBitLength = kInt_128U) noexcept;
    /// @brief Default virtual destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03385
    /// @needwork = dda
    /// @endcode
    ~PKeyPrivate_Ecc() noexcept override;
    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03386
    /// @needwork = dda
    /// @endcode
    PKeyPrivate_Ecc(PKeyPrivate_Ecc &&other) = delete;
    /// @brief Default move assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03387
    /// @needwork = dda
    /// @endcode
    PKeyPrivate_Ecc &operator=(PKeyPrivate_Ecc &&other) = delete;
    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03388
    /// @needwork = dda
    /// @endcode
    PKeyPrivate_Ecc &operator=(PKeyPrivate_Ecc const &other) = delete;
    /// @brief Default copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03389
    /// @needwork = dda
    /// @endcode
    PKeyPrivate_Ecc(PKeyPrivate_Ecc const &other) = delete;

public:  // CryptoObject interface
    /// @brief Returns the CryptoPrimitivId of this cryptobject.
    /// @return CryptoPrimitivId instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03390
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override;
    /// @brief Returns the actual size of the object payload. In bytes.
    /// @return Actual size of the payload
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03391
    /// @needwork = dda
    /// @endcode
    std::size_t GetPayloadSize() const noexcept override;
    /// @brief Save self to the provided IOInterface. Cryptobjects with the "session" attribute cannot be saved in KeySlots.
    /// @brief Save itself to provided IOInterface
    /// A CryptoObject with property "session" cannot be saved in a KeySlot.
    /// @param container IOInterface representing underlying storage
    /// @return has value if save sucess false otherwise
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_20517}
    /// @uptrace={RS_CRYPTO_02004}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @error: SecurityErrorDomain::kIncompatibleObject     if the object is "session", but the IOInterface represents
    ///     a KeySlot.
    /// @error: SecurityErrorDomain::kContentRestrictions    if the object doesn't satisfy the slot restrictions (@see keys::KeySlotPrototypeProps)
    /// @error: SecurityErrorDomain::kInsufficientCapacity   if the capacity of the target container is not enough, i.e.
    /// if <tt>(container.Capacity() < this->StorageSize())</tt>
    /// @error: SecurityErrorDomain::kModifiedResource       if the underlying resource has been modified after  the
    /// IOInterface has been opened, i.e., the IOInterface has been invalidated.
    /// @error: SecurityErrorDomain::kUnreservedResource     if the IOInterface is not opened writeable.
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01318
    /// @trace_id_dd=DD_CRYPTO_03392
    /// @needwork = ad
    /// @endcode
    ara::core::Result< void > Save(IOInterface &container) const noexcept override;

public:  // PrivateKey interface
    /// @brief Gets the public key corresponding to this private key.
    /// @return Public key instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03393
    /// @needwork = dda
    /// @endcode
    ara::core::Result< PublicKey::Uptrc > GetPublicKey() const noexcept override;

public:  // Serializable interface
    /// @brief Serialize Private itself.
    /// @param formatId Data format: Raw, DER, PEM, etc.
    /// @return Private key data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03394
    /// @needwork = dda
    /// @endcode
    ara::core::Result< ara::core::Vector< ara::core::Byte > > ExportPublicly(
        Serializable::FormatId formatId = Serializable::kFormatDefault) const noexcept override;

private:
    /// @brief EC_KEY resource pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03395
    /// @needwork = dda
    /// @endcode
    EVP_PKEY *pEccPair_{nullptr};
    /// @brief ECC NID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03396
    /// @needwork = dda
    /// @endcode
    uint32_t eccNid_{0U};
    /// @brief Bit size of the private key generated by ECC
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03397
    /// @needwork = dda
    /// @endcode
    size_t nGenPrivateKeyBitLen_{0U};

public:
    /// @brief Gets the big number inside the private key
    /// @return Big number inside the private key
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03398
    /// @needwork = dda
    /// @endcode
    BIGNUM const *GetPrivateBigNum() const noexcept;
    /// @brief Get NID
    /// @name  GetNid
    /// @returns  NID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03399
    /// @needwork = dda
    /// @endcode
    inline uint32_t GetNid() const noexcept { return eccNid_; }
    /// @brief Returns Rsa pointer
    /// @name  GetEcc
    /// @returns EC_KEY pointer instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03400
    /// @needwork = dda
    /// @endcode
    inline EVP_PKEY *GetEcc() const noexcept { return pEccPair_; }
    /// @brief Bind an Rsa private key
    /// @param stPrivateKey Private key
    /// @return true if attach sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03401
    /// @needwork = dda
    /// @endcode
    bool AttachKey(ara::core::StringView const &stPrivateKey) noexcept;

protected:
    /// @brief Delete any allocated Rsa resources that may exist
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03402
    /// @needwork = dda
    /// @endcode
    void _DeleteEcc() noexcept;
    /// @brief Generate public key
    /// @param pEccPair Pointer to Ecc key structure
    /// @return ECC public key string
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03403
    /// @needwork = dda
    /// @endcode
    static ara::core::String MakePublicKey(EVP_PKEY *const pEccPair) noexcept;
    /// @brief Generate Key string
    /// @param pEccKey ECC key resource
    /// @return ECC private key string
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03404
    /// @needwork = dda
    /// @endcode
    static ara::core::String MakeKeyString(BIO *const pEccKey) noexcept;
    /// @brief Save IO interface content to key slot
    /// @param container IO interface
    /// @return true if save sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03405
    /// @needwork = dda
    /// @endcode
    bool _SaveToKeySlot(IOInterface &container) const noexcept;
    /// @brief
    /// @param stPrivateKey
    /// @return Private key object instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03406
    /// @needwork = dda
    /// @endcode
    static EVP_PKEY *ExplanPrivateKey(ara::core::StringView const &stPrivateKey) noexcept;
};
//********************************/
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_PUHUA_KEY_PRIVATE_ECC_H_
