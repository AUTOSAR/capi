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
/// @file       isoft_key_private_rsa.h
/// @brief      AutoSar-Crypto encryption/decryption module
/// @details    General asymmetric private key interface.
/// @date       2021-12-21
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr><td>2021-12-21  </tr>1.0.0    </tr>hanjingjing      <td>Create initial version</tr>
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default encryption/Asymmetric private key
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=PKeyPrivate_Rsa
/// @unit_description=RSA private key
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_PUHUA_KEY_PRIVATE_RSA_H_
#define ARA_CRYPTO_CRYP_PUHUA_KEY_PRIVATE_RSA_H_

#include <openssl/rsa.h>

#include "ara/crypto/cryp/cryobj/isoft_key_private.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
//********************************/
/// @brief General asymmetric private key interface.
/// @brief Generalized Asymmetric Private Key interface.
/// @code{.isoft}
/// @interface PKeyPrivate_Rsa
/// @AUTOSAR_SWS {SWS_CRYPT_22500}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02002}
/// @uptrace={RS_CRYPTO_02403}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01392
/// @trace_id_dd=DD_CRYPTO_03620
/// @needwork = ad
/// @endcode
class PKeyPrivate_Rsa : public PKeyPrivate_Base
{
public:
    /// @brief Unique smart pointer for the interface.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01392
    /// @trace_id_dd=DD_CRYPTO_06342
    /// @needwork = dd
    /// @endcode
    using Uptr = std::unique_ptr< PKeyPrivate_Rsa >;
    /// @brief Unique constant smart pointer for the interface.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01392
    /// @trace_id_dd=DD_CRYPTO_06343
    /// @needwork = dd
    /// @endcode
    using Uptrc = std::unique_ptr< PKeyPrivate_Rsa const >;

public:
    /// @brief default constructor
    /// @name      PKeyPrivate_Rsa
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03621
    /// @needwork = dda
    /// @endcode
    PKeyPrivate_Rsa() noexcept = default;
    /// @brief default virtual destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03622
    /// @needwork = dda
    /// @endcode
    ~PKeyPrivate_Rsa() noexcept override;
    /// @brief default move constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03623
    /// @needwork = dda
    /// @endcode
    PKeyPrivate_Rsa(PKeyPrivate_Rsa &&other) = delete;
    /// @brief default move assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03624
    /// @needwork = dda
    /// @endcode
    PKeyPrivate_Rsa &operator=(PKeyPrivate_Rsa &&other) = delete;
    /// @brief default copy assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03625
    /// @needwork = dda
    /// @endcode
    PKeyPrivate_Rsa &operator=(PKeyPrivate_Rsa const &other) = delete;
    /// @brief default copy constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03626
    /// @needwork = dda
    /// @endcode
    PKeyPrivate_Rsa(PKeyPrivate_Rsa const &other) = delete;

public:  // CryptoObject interface
    /// @brief Returns the CryptoPrimitivId of this crypto object.
    /// @name   GetCryptoPrimitiveId
    /// @returns CryptoPrimitivId instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03627
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override;
    /// @brief Returns the actual size of the object payload. In bytes.
    /// @name   GetPayloadSize
    /// @returns Actual payload size
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_20516}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01393
    /// @trace_id_dd=DD_CRYPTO_03628
    /// @needwork = ad
    /// @endcode
    std::size_t GetPayloadSize() const noexcept override;
    /// @brief Save itself to the provided IOInterface. Crypto objects with the "session" attribute cannot be saved in KeySlot.
    /// @brief Save itself to provided IOInterface
    ///     A CryptoObject with property "session" cannot be saved in a KeySlot.
    /// @name   Save
    /// @param container IOInterface representing underlying storage
    /// @returns true if save sucess false otherwise
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_20517}
    /// @uptrace={RS_CRYPTO_02004}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @error: SecurityErrorDomain::kIncompatibleObject     if the object is "session", but the IOInterface represents
    /// a
    ///     KeySlot.
    /// @error: SecurityErrorDomain::kContentRestrictions    if the object doesn't satisfy the slot restrictions (@see keys::KeySlotPrototypeProps)
    /// @error: SecurityErrorDomain::kInsufficientCapacity   if the capacity of the target container is not enough, i.e.
    ///     if <tt>(container.Capacity() < this->StorageSize())</tt>
    /// @error: SecurityErrorDomain::kModifiedResource       if the underlying resource has been modified after  the
    ///     IOInterface has been opened, i.e., the IOInterface has been invalidated.
    /// @error: SecurityErrorDomain::kUnreservedResource     if the IOInterface is not opened writeable.
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01394
    /// @trace_id_dd=DD_CRYPTO_03629
    /// @needwork = ad
    /// @endcode
    ara::core::Result< void > Save(IOInterface &container) const noexcept override;

public:  // PrivateKey interface
    /// @brief Get the public key corresponding to this private key.
    /// @name   GetPublicKey
    /// @returns Public key instance corresponding to the private key
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03630
    /// @needwork = dda
    /// @endcode
    ara::core::Result< PublicKey::Uptrc > GetPublicKey() const noexcept override;

public:  // Serializable interface
    /// @brief Serialize the Private itself.
    /// @name   ExportPublicly
    /// @param formatId Data format: Raw, DER, PEM, etc.
    /// @return Private key data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03631
    /// @needwork = dda
    /// @endcode
    ara::core::Result< ara::core::Vector< ara::core::Byte > > ExportPublicly(
        Serializable::FormatId formatId = Serializable::kFormatDefault) const noexcept override;

private:
    /// @brief Private key length: bit length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03632
    /// @needwork = dda
    /// @endcode
    uint32_t nPrivateKeyBitLength_{0U};
    /// @brief RSA structure pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03633
    /// @needwork = dda
    /// @endcode
    EVP_PKEY *pRsaPair_{nullptr};

public:
    /// @brief Return the RSA pointer.
    /// @name   GetRsa
    /// @returns Private key pointer instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03634
    /// @needwork = dda
    /// @endcode
    inline EVP_PKEY *GetRsa() const noexcept { return pRsaPair_; }
    /// @brief Find the PublicKey through the PrivateKey.
    /// @name   GetPublicKeyString
    /// @param stPrivateKey Private key
    /// @returns Public key data corresponding to the private key
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03635
    /// @needwork = dda
    /// @endcode
    ara::core::String GetPublicKeyString(ara::core::StringView const &stPrivateKey) const noexcept;
    /// @brief Bind an RSA private key.
    /// @name   AttachKey
    /// @param stPrivateKey Private key
    /// @returns true if attach sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03636
    /// @needwork = dda
    /// @endcode
    bool AttachKey(ara::core::StringView const &stPrivateKey) noexcept;
    /// @brief Randomly initialize the key.
    /// @name   RandomInitKey
    /// @param nKeyBitLength Key length in bits
    /// @returns true if init key sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03637
    /// @needwork = dda
    /// @endcode
    bool RandomInitKey(int32_t const nKeyBitLength) noexcept;

protected:
    /// @brief Delete all possibly allocated RSA resources.
    /// @name   _DeleteRsa
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03638
    /// @needwork = dda
    /// @endcode
    void _DeleteRsa() noexcept;
    /// @brief Generate the key string.
    /// @name   MakeKeyString
    /// @param pRsaKey RSA key resource
    /// @returns RSA private key data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03639
    /// @needwork = dda
    /// @endcode
    static ara::core::String MakeKeyString(BIO *const pRsaKey) noexcept;
    /// @brief Generate the public key.
    /// @name   _MakePublicKey
    /// @param pRsaPair RSA resource pointer
    /// @returns RSA public key data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03640
    /// @needwork = dda
    /// @endcode
    ara::core::String _MakePublicKey(EVP_PKEY const *const pRsaPair) const noexcept;
    /// @brief Generate the private key.
    /// @name   MakePrivateKey
    /// @param pRsaPair RSA resource pointer
    /// @returns RSA private key data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03641
    /// @needwork = dda
    /// @endcode
    static ara::core::String MakePrivateKey(EVP_PKEY *const pRsaPair) noexcept;
    /// @brief // Parse the private key.
    /// @name   ExplanPrivateKey
    /// @param stPrivateKey Private key
    /// @returns RSA pointer instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03642
    /// @needwork = dda
    /// @endcode
    static EVP_PKEY *ExplanPrivateKey(ara::core::StringView const &stPrivateKey) noexcept;

public:
    /// @brief Initialize a set of RSA keys.
    /// @name   DebugInitKey
    /// @param nKeyBitLength Key length in bits
    /// @param bPrint Whether to print logs
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03643
    /// @needwork = dda
    /// @endcode
    void DebugInitKey(int32_t const nKeyBitLength, bool const bPrint) noexcept;
    /// @brief Print the key.
    /// @name DebugPrintRsaKey
    /// @param stKey Key data: string format
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03644
    /// @needwork = dda
    /// @endcode
    static void DebugPrintRsaKey(ara::core::String const &stKey) noexcept;

protected:
    /// @brief: Save the key.
    /// @name   _SaveToKeySlot
    /// @param container IO interface
    /// @returns true if save to keyslot false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03645
    /// @needwork = dda
    /// @endcode
    bool _SaveToKeySlot(IOInterface &container) const noexcept;
};
//********************************/
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_PUHUA_KEY_PRIVATE_RSA_H_
