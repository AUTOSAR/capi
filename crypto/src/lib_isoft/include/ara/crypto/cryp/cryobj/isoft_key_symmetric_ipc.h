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
/// @file       isoft_key_symmetric_ipc.h
/// @brief      AutoSar-Crypto Encryption Module
/// @details    Symmetric key interface.
/// @date       2021-08-22
/// @author     Chang Zheng
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr><td>2021-08-22  <td>1.0.0    <td>Chang Zheng  <td>Initial version creation
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default Encryption/Symmetric Key
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=PKeySymmetric_Ipc
/// @unit_description=IPC Symmetric Key
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_PUHUA_KEY_SYMMETRIC_IPC_H_
#define ARA_CRYPTO_CRYP_PUHUA_KEY_SYMMETRIC_IPC_H_

#include "ara/crypto/cryp/cryobj/isoft_key_symmetric.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
//********************************/
/// @brief Symmetric key interface. // Key used by Aes
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01322
/// @trace_id_dd=DD_CRYPTO_03432
/// @needwork = ad
/// @endcode
class PKeySymmetric_Ipc : public PKeySymmetric_Base
{
public:
    /// @brief Constant unique smart pointer of the interface.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01322
    /// @trace_id_dd=DD_CRYPTO_06330
    /// @needwork = dd
    /// @endcode
    using Uptrc = std::unique_ptr< PKeySymmetric_Ipc const >;
    /// @brief Unique smart pointer of the interface.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01322
    /// @trace_id_dd=DD_CRYPTO_06331
    /// @needwork = dd
    /// @endcode
    using Uptr = std::unique_ptr< PKeySymmetric_Ipc >;
    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03433
    /// @needwork = dda
    /// @endcode
    PKeySymmetric_Ipc() noexcept = delete;
    /// @brief Parameterized constructor
    /// @name PKeySymmetric_Ipc
    /// @param nSlotId Key slot ID
    /// @param nIointerfaceId IO interface ID
    /// @param nSlotName Key slot name
    /// @param nAlgId Encryption primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03434
    /// @needwork = dda
    /// @endcode
    explicit PKeySymmetric_Ipc(uint32_t const nSlotId,
                               uint32_t const nIointerfaceId,
                               ara::core::StringView const& nSlotName,
                               CryptoAlgId const nAlgId) noexcept;
    /// @brief Default virtual destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03435
    /// @needwork = dda
    /// @endcode
    ~PKeySymmetric_Ipc() noexcept override = default;
    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03436
    /// @needwork = dda
    /// @endcode
    PKeySymmetric_Ipc(PKeySymmetric_Ipc&& other) = delete;
    /// @brief Default move assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03437
    /// @needwork = dda
    /// @endcode
    PKeySymmetric_Ipc& operator=(PKeySymmetric_Ipc&& other) = delete;
    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03438
    /// @needwork = dda
    /// @endcode
    PKeySymmetric_Ipc& operator=(PKeySymmetric_Ipc const& other) = delete;
    /// @brief Default copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03439
    /// @needwork = dda
    /// @endcode
    PKeySymmetric_Ipc(PKeySymmetric_Ipc const& other) = delete;

public:  ////CryptoObject interface
    /// @brief Returns the CryptoPrimitivId of this cryptobject.
    /// @name   GetCryptoPrimitiveId
    /// @returns CryptoPrimitiveId instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03440
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override;
    /// @brief Returns the actual size of the object payload. In bytes.
    /// @name   GetPayloadSize
    /// @returns Actual size of the payload
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03441
    /// @needwork = dda
    /// @endcode
    std::size_t GetPayloadSize() const noexcept override;
    /// @brief Returns the object's COIdentifier, including object type and UID.
    ///         Objects without a COUID assigned cannot be serialized/exported or saved to non-volatile storage safely. Several related objects of different types may share a COUID (e.g., private and public key), but the combination of COUID and object type must always be unique!
    /// @brief Return the object's COIdentifier, which includes the object's type and UID. An object that
    ///         has no assigned @a COUID cannot be (securely) serialized / exported or saved to a non-volatile storage.
    ///         An object should not have a @a COUID if it is session and non-exportable simultaneously A few related
    ///         objects of different types can share a single @a COUID (e.g. private and public keys), but a combination
    ///         of @a COUID and object type must be unique always!
    /// @returns the object's COIdentifier including the object's type and COUID (or an empty COUID, if this object is
    ///     not identifiable).
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_20514}
    /// @uptrace={RS_CRYPTO_02005}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01323
    /// @trace_id_dd=DD_CRYPTO_03442
    /// @needwork = ad
    /// @endcode
    COIdentifier GetObjectId() const noexcept override;
    /// @brief Save self to the provided IOInterface. Cryptobjects with the "session" attribute cannot be saved in KeySlots.
    /// @name   Save
    /// @param container IO interface
    /// @returns has value if save sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03443
    /// @needwork = dda
    /// @endcode
    ara::core::Result< void > Save(IOInterface& container) const noexcept override;

public:  // RestrictedUseObject interface
    /// @brief Set key slot ID
    /// @name   GetSlotId
    /// @returns Key slot ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03444
    /// @needwork = dda
    /// @endcode
    inline uint32_t GetSlotId() const noexcept { return nSlotId_; }
    /// @brief Get slot name
    /// @return Slot name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03445
    /// @needwork = dda
    /// @endcode
    inline ara::core::StringView GetSlotName() const noexcept { return nSlotName_; }
    // @brief Get interface Id
    /// @return Iointerface ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03446
    /// @needwork = dda
    /// @endcode
    inline uint32_t GetIoinerfaceId() const noexcept { return nIointerfaceId_; }

private:
    /// @brief Key slot ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03447
    /// @needwork = dda
    /// @endcode
    uint32_t nSlotId_;
    /// @brief containerId
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03448
    /// @needwork = dda
    /// @endcode
    uint32_t nIointerfaceId_;
    /// @brief Slot name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03449
    /// @needwork = dda
    /// @endcode
    ara::core::StringView nSlotName_;
    /// @brief Corresponding algorithm ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03450
    /// @needwork = dda
    /// @endcode
    CryptoAlgId nAlgId_;
};

//********************************/
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_PUHUA_KEY_SYMMETRIC_IPC_H_
