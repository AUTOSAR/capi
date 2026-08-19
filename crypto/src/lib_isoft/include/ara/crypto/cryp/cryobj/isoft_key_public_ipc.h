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
/// @file       isoft_key_public_ipc.h
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
/// <tr><td>2021-12-21  <tr>1.0.0    </td>hanjingjing      </td>Create initial version</td>
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default encryption/Asymmetric public key
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=PKeyPublic_Ipc
/// @unit_description=IPC public key
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_PUHUA_KEY_PUBLIC_IPC_H_
#define ARA_CRYPTO_CRYP_PUHUA_KEY_PUBLIC_IPC_H_

#include "ara/crypto/cryp/cryobj/isoft_key_public.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
//********************************/
/// @brief General asymmetric private key interface: IPC
/// @brief Generalized Asymmetric Public Key interface.
/// @code{.isoft}
/// @interface PKeyPublic_Ipc
/// @AUTOSAR_SWS {SWS_CRYPT_22500}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02002}
/// @uptrace={RS_CRYPTO_02403}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01251
/// @trace_id_dd=DD_CRYPTO_03225
/// @needwork = ad
/// @endcode
class PKeyPublic_Ipc : public PKeyPublic_Base
{
public:
    /// @brief Constructor
    /// @name      PKeyPublic_Ipc
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03226
    /// @needwork = dda
    /// @endcode
    PKeyPublic_Ipc() noexcept = delete;
    /// @brief Parameterized constructor
    /// @name   PKeyPublic_Ipc
    /// @param nSlotId Key slot ID
    /// @param nIointerfaceId IO interface ID
    /// @param nSlotName Key slot name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03227
    /// @needwork = dda
    /// @endcode
    explicit PKeyPublic_Ipc(uint32_t const nSlotId,
                            uint32_t const nIointerfaceId,
                            ara::core::StringView const& nSlotName) noexcept;
    /// @brief default virtual destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03228
    /// @needwork = dda
    /// @endcode
    ~PKeyPublic_Ipc() noexcept override = default;
    /// @brief default move constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03229
    /// @needwork = dda
    /// @endcode
    PKeyPublic_Ipc(PKeyPublic_Ipc&& other) = delete;
    /// @brief default copy constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03230
    /// @needwork = dda
    /// @endcode
    PKeyPublic_Ipc(PKeyPublic_Ipc const& other) = delete;
    /// @brief default move assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03231
    /// @needwork = dda
    /// @endcode
    PKeyPublic_Ipc& operator=(PKeyPublic_Ipc&& other) = delete;
    /// @brief default copy assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03232
    /// @needwork = dda
    /// @endcode
    PKeyPublic_Ipc& operator=(PKeyPublic_Ipc const& other) = delete;

public:  // CryptoObject interface
    /// @brief Returns the CryptoPrimitivId of this crypto object.
    /// @name   GetCryptoPrimitiveId
    /// @returns CryptoPrimitiveId instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03233
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override = 0;
    /// @brief Returns the actual size of the object payload. In bytes.
    /// @name   GetPayloadSize
    /// @returns Actual payload size
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03234
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
    /// @trace_id_dd=DD_CRYPTO_03235
    /// @needwork = dda
    /// @endcode
    ara::core::Result< void > Save(IOInterface& container) const noexcept override;
    /// @brief Get the object ID.
    /// @return Object ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03236
    /// @needwork = dda
    /// @endcode
    CryptoObject::COIdentifier GetObjectId() const noexcept override;

public:  // Serializable interface
    /// @brief Serialize the Public itself.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03237
    /// @needwork = dda
    /// @endcode
    ara::core::Result< ara::core::Vector< ara::core::Byte > >
    /// @name   ExportPublicly
    /// @param formatId Data format: Raw, DER, PEM, etc.
    /// @return Public key data information
    ExportPublicly(Serializable::FormatId formatId = Serializable::kFormatDefault) const noexcept override;

public:  // public interface
    /// @brief Check the key.
    /// @name   CheckKey
    /// @param strongCheck Whether to enable strong checking
    /// @return ture if check key sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03238
    /// @needwork = dda
    /// @endcode
    bool CheckKey(bool strongCheck) const noexcept override;

private:
    /// @brief Key slot ID.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03239
    /// @needwork = dda
    /// @endcode
    uint32_t nSlotId_;
    /// @brief containerId
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03240
    /// @needwork = dda
    /// @endcode
    uint32_t nIointerfaceId_;
    /// @brief Slot name.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03241
    /// @needwork = dda
    /// @endcode
    ara::core::StringView nSlotName_;

public:
    /// @brief Get the key slot ID.
    /// @name   GetSlotId
    /// @returns Key slot ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03242
    /// @needwork = dda
    /// @endcode
    inline virtual uint32_t GetSlotId() const noexcept { return nSlotId_; }
    /// @brief Get the slot name.
    /// @return Slot name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03243
    /// @needwork = dda
    /// @endcode
    inline ara::core::StringView GetSlotName() const noexcept { return nSlotName_; }
    // @brief Get the interface ID.
    /// @return Iointerface ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03244
    /// @needwork = dda
    /// @endcode
    inline uint32_t GetIoinerfaceId() const noexcept { return nIointerfaceId_; }

protected:
    /// @brief Return the function name: GetPayloadSize
    /// @name   GetPayloadSizeFunName
    /// @returns Function name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03245
    /// @needwork = dda
    /// @endcode
    virtual ara::core::StringView GetPayloadSizeFunName() const noexcept = 0;
    /// @brief Return the function name: ExportPublicly
    /// @return Function name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03246
    /// @needwork = dda
    /// @endcode
    virtual ara::core::StringView ExportPubliclyFunName() const noexcept = 0;
};
/// @brief General asymmetric private key interface: IPC_ECC
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01252
/// @trace_id_dd=DD_CRYPTO_03247
/// @needwork = ad
/// @endcode
class PKeyPublic_Ipc_Ecc : public PKeyPublic_Ipc
{
public:
    /// @brief Unique smart pointer for the interface.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01252
    /// @trace_id_dd=DD_CRYPTO_06316
    /// @needwork = dd
    /// @endcode
    using Uptr = std::unique_ptr< PKeyPublic_Ipc_Ecc >;
    /// @brief Unique constant smart pointer for the interface.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01252
    /// @trace_id_dd=DD_CRYPTO_06317
    /// @needwork = dd
    /// @endcode
    using Uptrc = std::unique_ptr< PKeyPublic_Ipc_Ecc const >;

public:
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06318
    /// @needwork = dda
    /// @endcode
    using PKeyPublic_Ipc::PKeyPublic_Ipc;
    /// @brief default virtual destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03248
    /// @needwork = dda
    /// @endcode
    ~PKeyPublic_Ipc_Ecc() noexcept override = default;
    /// @brief default move constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03249
    /// @needwork = dda
    /// @endcode
    PKeyPublic_Ipc_Ecc(PKeyPublic_Ipc_Ecc&& other) = delete;
    /// @brief default copy constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03250
    /// @needwork = dda
    /// @endcode
    PKeyPublic_Ipc_Ecc(PKeyPublic_Ipc_Ecc const& other) = delete;
    /// @brief default move assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03251
    /// @needwork = dda
    /// @endcode
    PKeyPublic_Ipc_Ecc& operator=(PKeyPublic_Ipc_Ecc&& other) = delete;
    /// @brief default copy assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03252
    /// @needwork = dda
    /// @endcode
    PKeyPublic_Ipc_Ecc& operator=(PKeyPublic_Ipc_Ecc const& other) = delete;

public:  // PublicKey interface
    /// @brief Returns the CryptoPrimitivId of this crypto object.
    /// @name   GetCryptoPrimitiveId
    /// @returns CryptoPrimitivId instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03253
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override;
    /// @brief Return the function name: GetPayloadSize
    /// @name   GetPayloadSizeFunName
    /// @returns Function name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03254
    /// @needwork = dda
    /// @endcode
    ara::core::StringView GetPayloadSizeFunName() const noexcept override;
    /// @brief Return the function name: ExportPublicly
    /// @return Function name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03255
    /// @needwork = dda
    /// @endcode
    ara::core::StringView ExportPubliclyFunName() const noexcept override;
};

/// @brief General asymmetric private key interface: IPC_RSA
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01253
/// @trace_id_dd=DD_CRYPTO_03256
/// @needwork = ad
/// @endcode
class PKeyPublic_Ipc_Rsa : public PKeyPublic_Ipc
{
public:
    /// @brief Unique smart pointer for the interface.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01252
    /// @trace_id_dd=DD_CRYPTO_06319
    /// @needwork = dd
    /// @endcode
    using Uptr = std::unique_ptr< PKeyPublic_Ipc_Rsa >;
    /// @brief Unique constant smart pointer for the interface.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01252
    /// @trace_id_dd=DD_CRYPTO_06320
    /// @needwork = dd
    /// @endcode
    using Uptrc = std::unique_ptr< PKeyPublic_Ipc_Rsa const >;

public:
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06321
    /// @needwork = dda
    /// @endcode
    using PKeyPublic_Ipc::PKeyPublic_Ipc;
    /// @brief default virtual destructor
    /// @name   ~PKeyPublic_Ipc_Rsa
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03257
    /// @needwork = dda
    /// @endcode
    ~PKeyPublic_Ipc_Rsa() noexcept override = default;
    /// @brief default move constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03258
    /// @needwork = dda
    /// @endcode
    PKeyPublic_Ipc_Rsa(PKeyPublic_Ipc_Rsa&& other) = delete;
    /// @brief default copy constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03259
    /// @needwork = dda
    /// @endcode
    PKeyPublic_Ipc_Rsa(PKeyPublic_Ipc_Rsa const& other) = delete;
    /// @brief default move assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03260
    /// @needwork = dda
    /// @endcode
    PKeyPublic_Ipc_Rsa& operator=(PKeyPublic_Ipc_Rsa&& other) = delete;
    /// @brief default copy assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03261
    /// @needwork = dda
    /// @endcode
    PKeyPublic_Ipc_Rsa& operator=(PKeyPublic_Ipc_Rsa const& other) = delete;

public:  // PublicKey interface
    /// @brief Returns the CryptoPrimitivId of this crypto object.
    /// @name   GetCryptoPrimitiveId
    /// @returns CryptoPrimitiveId instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03262
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override;
    /// @brief Return the function name: GetPayloadSize
    /// @name   GetPayloadSizeFunName
    /// @returns Function name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03263
    /// @needwork = dda
    /// @endcode
    ara::core::StringView GetPayloadSizeFunName() const noexcept override;
    /// @brief Return the function name: ExportPublicly
    /// @return Function name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03264
    /// @needwork = dda
    /// @endcode
    ara::core::StringView ExportPubliclyFunName() const noexcept override;
};
//********************************/
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_PUHUA_KEY_PUBLIC_IPC_H_
