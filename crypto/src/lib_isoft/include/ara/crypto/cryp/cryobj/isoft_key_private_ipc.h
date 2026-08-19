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
/// @file       isoft_key_private_ipc.h
/// @brief      AutoSar-Crypto Encryption Module
/// @details    General asymmetric private key interface.
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
/// @module_path=/CRYPTO/Default Encryption/Asymmetric Private Key
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=PKeyPrivate_Ipc
/// @unit_description=IPC Private Key
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_PUHUA_KEY_PRIVATE_IPC_H_
#define ARA_CRYPTO_CRYP_PUHUA_KEY_PRIVATE_IPC_H_

#include "ara/crypto/cryp/cryobj/isoft_key_private.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
//********************************/
/// @brief General asymmetric private key interface.
/// @brief Generalized Asymmetric Private Key interface.
/// @code{.isoft}
/// @interface PKeyPrivate_Ipc
/// @AUTOSAR_SWS {SWS_CRYPT_22500}
/// @uptrace={RS_CRYPTO_02002}
/// @uptrace={RS_CRYPTO_02403}
/// @tracestatus={draft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01324
/// @trace_id_dd=DD_CRYPTO_03451
/// @needwork = ad
/// @endcode
class PKeyPrivate_Ipc : public PKeyPrivate_Base
{
public:
    /// @brief Constructor
    /// @name      PKeyPrivate_Ipc
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03452
    /// @needwork = dda
    /// @endcode
    PKeyPrivate_Ipc() noexcept = delete;
    /// @brief Parameterized constructor
    /// @name   PKeyPrivate_Ipc
    /// @param nSlotId Key slot ID
    /// @param nIointerfaceId IO interface ID
    /// @param nSlotName Key slot name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03453
    /// @needwork = dda
    /// @endcode
    explicit PKeyPrivate_Ipc(uint32_t const nSlotId,
                             uint32_t const nIointerfaceId,
                             ara::core::StringView const& nSlotName) noexcept;
    /// @brief Default virtual destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03454
    /// @needwork = dda
    /// @endcode
    ~PKeyPrivate_Ipc() noexcept override = default;
    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03455
    /// @needwork = dda
    /// @endcode
    PKeyPrivate_Ipc(PKeyPrivate_Ipc&& other) noexcept = delete;
    /// @brief Default copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03456
    /// @needwork = dda
    /// @endcode
    PKeyPrivate_Ipc(PKeyPrivate_Ipc const& other) noexcept = delete;
    /// @brief Default move assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03457
    /// @needwork = dda
    /// @endcode
    PKeyPrivate_Ipc& operator=(PKeyPrivate_Ipc&& other) noexcept = delete;
    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03458
    /// @needwork = dda
    /// @endcode
    PKeyPrivate_Ipc& operator=(PKeyPrivate_Ipc const& other) noexcept = delete;

public:  // CryptoObject interface
    /// @brief Returns the CryptoPrimitivId of this cryptobject.
    /// @name   GetCryptoPrimitiveId
    /// @returns
    /// @brief Returns the actual size of the object payload. In bytes.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03459
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override = 0;
    /// @brief Returns the actual size of the object payload. In bytes.
    /// @name   GetPayloadSize
    /// @returns Actual size of the payload
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03460
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
    /// @trace_id_dd=DD_CRYPTO_03461
    /// @needwork = dda
    /// @endcode
    ara::core::Result< void > Save(IOInterface& container) const noexcept override;
    /// @brief Returns the object's COIdentifier, including object type and UID.
    ///   Objects without a COUID assigned cannot be serialized/exported or saved to non-volatile storage safely. Several related objects of different types may share a COUID (e.g., private and public key), but the combination of COUID and object type must always be unique!
    /// @return Object's COIdentifier
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03462
    /// @needwork = dda
    /// @endcode
    CryptoObject::COIdentifier GetObjectId() const noexcept override;

public:  // PrivateKey interface
    /// @brief Gets the public key corresponding to this private key.
    /// @name   GetPublicKey
    /// @returns Public key instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03463
    /// @needwork = dda
    /// @endcode
    ara::core::Result< PublicKey::Uptrc > GetPublicKey() const noexcept override;

public:  // Serializable interface
    /// @brief Serialize Private itself.
    /// @name   ExportPublicly
    /// @param formatId Data format: Raw, DER, PEM, etc.
    /// @return Public key data information
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03464
    /// @needwork = dda
    /// @endcode
    ara::core::Result< ara::core::Vector< ara::core::Byte > > ExportPublicly(
        Serializable::FormatId formatId = Serializable::kFormatDefault) const noexcept override;

private:
    /// @brief Slot Id
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03465
    /// @needwork = dda
    /// @endcode
    uint32_t nSlotId_;
    /// @brief containerId
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03466
    /// @needwork = dda
    /// @endcode
    uint32_t nIointerfaceId_;
    /// @brief Slot name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03467
    /// @needwork = dda
    /// @endcode
    ara::core::StringView nSlotName_;

public:
    /// @brief Get slot ID
    /// @name   GetSlotId
    /// @returns Slot ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03468
    /// @needwork = dda
    /// @endcode
    inline virtual uint32_t GetSlotId() const noexcept { return nSlotId_; }
    /// @brief Get slot name
    /// @return Slot name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03469
    /// @needwork = dda
    /// @endcode
    inline ara::core::StringView GetSlotName() const noexcept { return nSlotName_; }
    // @brief Get interface Id
    /// @return Corresponding IOinterface ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03470
    /// @needwork = dda
    /// @endcode
    inline uint32_t GetIoinerfaceId() const noexcept { return nIointerfaceId_; }

protected:
    /// @brief Returns function name: GetPayloadSize
    /// @name   GetPayloadSizeFunName
    /// @returns Function name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03471
    /// @needwork = dda
    /// @endcode
    virtual ara::core::StringView GetPayloadSizeFunName() const noexcept = 0;
    /// @brief Returns function name: ExportPublicly
    /// @return Function name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03472
    /// @needwork = dda
    /// @endcode
    virtual ara::core::StringView ExportPubliclyFunName() const noexcept = 0;
};

/// @brief General asymmetric private key interface: IPC_ECC
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01325
/// @trace_id_dd=DD_CRYPTO_03473
/// @needwork = ad
/// @endcode
class PKeyPrivate_Ipc_Ecc : public PKeyPrivate_Ipc
{
public:
    /// @brief Unique smart pointer of the interface.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01325
    /// @trace_id_dd=DD_CRYPTO_06332
    /// @needwork = dd
    /// @endcode
    using Uptr = std::unique_ptr< PKeyPrivate_Ipc_Ecc >;
    /// @brief Unique constant smart pointer of the interface.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01325
    /// @trace_id_dd=DD_CRYPTO_06333
    /// @needwork = dd
    /// @endcode
    using Uptrc = std::unique_ptr< PKeyPrivate_Ipc_Ecc const >;

public:
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06334
    /// @needwork = dda
    /// @endcode
    using PKeyPrivate_Ipc::PKeyPrivate_Ipc;
    /// @brief Default virtual destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03474
    /// @needwork = dda
    /// @endcode
    ~PKeyPrivate_Ipc_Ecc() noexcept override = default;
    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03475
    /// @needwork = dda
    /// @endcode
    PKeyPrivate_Ipc_Ecc(PKeyPrivate_Ipc_Ecc&& other) = delete;
    /// @brief Default copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03476
    /// @needwork = dda
    /// @endcode
    PKeyPrivate_Ipc_Ecc(PKeyPrivate_Ipc_Ecc const& other) = delete;
    /// @brief Default move assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03477
    /// @needwork = dda
    /// @endcode
    PKeyPrivate_Ipc_Ecc& operator=(PKeyPrivate_Ipc_Ecc&& other) = delete;
    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03478
    /// @needwork = dda
    /// @endcode
    PKeyPrivate_Ipc_Ecc& operator=(PKeyPrivate_Ipc_Ecc const& other) = delete;

public:  // PrivateKey interface
    /// @brief Gets the public key corresponding to this private key.
    /// @name   GetPublicKey
    /// @returns Public key instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03479
    /// @needwork = dda
    /// @endcode
    ara::core::Result< PublicKey::Uptrc > GetPublicKey() const noexcept override;
    /// @brief Returns the CryptoPrimitivId of this cryptobject.
    /// @name   GetCryptoPrimitiveId
    /// @returns CryptoPrimitivId instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03480
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override;
    /// @brief Returns ICP packet function name: GetPayloadSize
    /// @name   GetPayloadSizeFunName
    /// @returns Function name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03481
    /// @needwork = dda
    /// @endcode
    ara::core::StringView GetPayloadSizeFunName() const noexcept override;
    /// @brief Returns ICP packet function name: ExportPublicly
    /// @return Function name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03482
    /// @needwork = dda
    /// @endcode
    ara::core::StringView ExportPubliclyFunName() const noexcept override;
};

/// @brief General asymmetric private key interface: IPC_RSA
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01326
/// @trace_id_dd=DD_CRYPTO_03483
/// @needwork = ad
/// @endcode
class PKeyPrivate_Ipc_Rsa : public PKeyPrivate_Ipc
{
public:
    /// @brief Unique smart pointer of the interface.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01325
    /// @trace_id_dd=DD_CRYPTO_06335
    /// @needwork = dd
    /// @endcode
    using Uptr = std::unique_ptr< PKeyPrivate_Ipc_Rsa >;
    /// @brief Unique constant smart pointer of the interface.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01325
    /// @trace_id_dd=DD_CRYPTO_06336
    /// @needwork = dd
    /// @endcode
    using Uptrc = std::unique_ptr< PKeyPrivate_Ipc_Rsa const >;

public:
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06337
    /// @needwork = dda
    /// @endcode
    using PKeyPrivate_Ipc::PKeyPrivate_Ipc;
    /// @brief Default virtual destructor
    /// @name   ~PKeyPrivate_Ipc_Rsa
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03484
    /// @needwork = dda
    /// @endcode
    ~PKeyPrivate_Ipc_Rsa() noexcept override = default;
    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03485
    /// @needwork = dda
    /// @endcode
    PKeyPrivate_Ipc_Rsa(PKeyPrivate_Ipc_Rsa&& other) = delete;
    /// @brief Default copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03486
    /// @needwork = dda
    /// @endcode
    PKeyPrivate_Ipc_Rsa(PKeyPrivate_Ipc_Rsa const& other) = delete;
    /// @brief Default move assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03487
    /// @needwork = dda
    /// @endcode
    PKeyPrivate_Ipc_Rsa& operator=(PKeyPrivate_Ipc_Rsa&& other) = delete;
    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03488
    /// @needwork = dda
    /// @endcode
    PKeyPrivate_Ipc_Rsa& operator=(PKeyPrivate_Ipc_Rsa const& other) = delete;

public:  // PrivateKey interface
    /// @brief Gets the public key corresponding to this private key.
    /// @name   GetPublicKey
    /// @returns Public key instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03489
    /// @needwork = dda
    /// @endcode
    ara::core::Result< PublicKey::Uptrc > GetPublicKey() const noexcept override;
    /// @brief Returns the CryptoPrimitivId of this cryptobject.
    /// @name   GetCryptoPrimitiveId
    /// @returns CryptoPrimitivId instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03490
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override;
    /// @brief Returns ICP packet function name: GetPayloadSize
    /// @name   GetPayloadSizeFunName
    /// @returns Function name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03491
    /// @needwork = dda
    /// @endcode
    ara::core::StringView GetPayloadSizeFunName() const noexcept override;
    /// @brief Returns ICP packet function name: ExportPublicly
    /// @return Function name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03492
    /// @needwork = dda
    /// @endcode
    ara::core::StringView ExportPubliclyFunName() const noexcept override;
};
//********************************/
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_PUHUA_KEY_PRIVATE_IPC_H_
