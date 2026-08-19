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
/// @file       isoft_ipc_io_interface.h
/// @brief      AutoSar-Crypto encryption/decryption common module
/// @details    Formal interface of IOInterface for saving and loading security objects: IPC implementation version.
/// @date       2022-04-12
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr>}<2022-04-12  </td>1.0.0    </td>hanjingjing      <td>Create initial version</td>
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default key component/IO Interface
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=PIoInterface_Ipc
/// @unit_description=IPC version of IO interface
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_KEYS_PUHUA_IO_INTERFACE_IPC_H_
#define ARA_CRYPTO_KEYS_PUHUA_IO_INTERFACE_IPC_H_

#include <ara/core/result.h>
#include <ara/core/string.h>

#include "ara/crypto/common/io_interface.h"
#include "ara/crypto/cryp/isoft_auto_buff.h"
#include "ara/crypto/ipc/isoft_ipc_client.h"

namespace ara {
namespace crypto {
namespace keys {
namespace isoft_def {
//********************************/
/// @brief Formal interface of IOInterface for saving and loading security objects: IPC implementation version.
///         Actual saving and loading should be implemented through internal methods known to the trusted pair of crypto provider and storage provider.
///         Each object should be uniquely identified by its type and crypto object unique identifier (COUID). This interface assumes that objects in the container are compressed, i.e., have a minimal optimized size.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01473
/// @trace_id_dd=DD_CRYPTO_03852
/// @needwork = ad
/// @endcode
class PIoInterface_Ipc : public ara::crypto::IOInterface
{
public:
    /// @brief Unique smart pointer for the interface.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01473
    /// @trace_id_dd=DD_CRYPTO_06349
    /// @needwork = dd
    /// @endcode
    using Uptr = std::unique_ptr< PIoInterface_Ipc >;

public:
    /// @brief Parameterized constructor
    /// @name   PIoInterface_Ipc
    /// @param ipcClient IPC client
    /// @param slotName Key slot name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01474
    /// @trace_id_dd=DD_CRYPTO_03853
    /// @needwork = ad
    /// @endcode
    explicit PIoInterface_Ipc(PIpcClient const& ipcClient, ara::core::StringView const& slotName) noexcept;
    /// @brief default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01475
    /// @trace_id_dd=DD_CRYPTO_03854
    /// @needwork = ad
    /// @endcode
    PIoInterface_Ipc() = delete;
    /// @brief default virtual destructor
    /// @name   ~PIoInterface_Ipc
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01476
    /// @trace_id_dd=DD_CRYPTO_03855
    /// @needwork = ad
    /// @endcode
    ~PIoInterface_Ipc() noexcept override = default;
    /// @brief default copy constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01477
    /// @trace_id_dd=DD_CRYPTO_03856
    /// @needwork = ad
    /// @endcode
    PIoInterface_Ipc(PIoInterface_Ipc const& other) = delete;
    /// @brief default move constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01478
    /// @trace_id_dd=DD_CRYPTO_03857
    /// @needwork = ad
    /// @endcode
    PIoInterface_Ipc(PIoInterface_Ipc&& other) = delete;
    /// @brief default copy assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01479
    /// @trace_id_dd=DD_CRYPTO_03858
    /// @needwork = ad
    /// @endcode
    PIoInterface_Ipc& operator=(PIoInterface_Ipc const& other) = delete;
    /// @brief default move assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01480
    /// @trace_id_dd=DD_CRYPTO_03859
    /// @needwork = ad
    /// @endcode
    PIoInterface_Ipc& operator=(PIoInterface_Ipc&& other) = delete;

public:  // IOInterface interface
    /// @brief Return the actual allowed key/seed usage flags defined by the key slot prototype and the current content of the container for this "Actor".
    ///         Volatile containers do not have any prototype restrictions, but can define restrictions for the current instance of the object at runtime.
    ///         The value returned by this method is the bitwise AND of the runtime-defined common usage flags and the UserPermissions prototype usage flags defined for the current "Actor".
    ///         This method is particularly useful for empty permanent prototype containers.
    /// @name GetAllowedUsage
    /// @returns Key/seed usage flags
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01481
    /// @trace_id_dd=DD_CRYPTO_03860
    /// @needwork = ad
    /// @endcode
    AllowedUsageFlags GetAllowedUsage() const noexcept override;
    /// @brief Return the capacity of the underlying resource. In bytes.
    /// @name GetCapacity
    /// @returns Capacity of the underlying resource
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01482
    /// @trace_id_dd=DD_CRYPTO_03861
    /// @needwork = ad
    /// @endcode
    std::size_t GetCapacity() const noexcept override;
    /// @brief Return the cryptobjecttype of the object referenced by this IOInterface.
    /// @name GetCryptoObjectType
    /// @returns cryptobjecttype of the referenced object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01483
    /// @trace_id_dd=DD_CRYPTO_03862
    /// @needwork = ad
    /// @endcode
    CryptoObjectType GetCryptoObjectType() const noexcept override;
    /// @brief Return the COUID of the object stored in the IOInterface.
    ///         If the container is empty, this method returns cryptobjecttype::KUndefined. Unambiguous identification of a crypto object requires two components: cryptobjectuid and cryptobjecttype.
    /// @name GetObjectId
    /// @returns COUID of the object in the IOInterface
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01484
    /// @trace_id_dd=DD_CRYPTO_03863
    /// @needwork = ad
    /// @endcode
    CryptoObjectUid GetObjectId() const noexcept override;
    /// @brief Returns the size of the object payload stored in the IOInterface's underlying buffer.
    ///         If the container is empty, the method returns 0. The return value does not account for the object's meta-information attributes,
    ///         but their size is fixed and common for all crypto objects regardless of their actual type.
    /// @name GetPayloadSize
    /// @returns Size of the object payload in the underlying buffer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01485
    /// @trace_id_dd=DD_CRYPTO_03864
    /// @needwork = ad
    /// @endcode
    std::size_t GetPayloadSize() const noexcept override;
    /// @brief Gets the vendor-specific ID of the primitive.
    /// @name GetPrimitiveId
    /// @returns Vendor-specific ID of the primitive
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01486
    /// @trace_id_dd=DD_CRYPTO_03865
    /// @needwork = ad
    /// @endcode
    CryptoAlgId GetPrimitiveId() const noexcept override;
    /// @brief Returns the content type restriction of this interface (identifying what type of data this container can hold).
    /// If KeySlotPrototypeProps::mAllowContentTypeChange == TRUE, kUndefined should be returned.
    /// If the container has a type restriction different from @c CryptoObjectType::kUndefined, only objects of said type can be saved to this container.
    /// Volatile containers have no content type restrictions.
    /// CryptoObjectType::kUndefined identifies no restriction.
    /// @name GetTypeRestriction
    /// @returns Content type restriction of the interface
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01487
    /// @trace_id_dd=DD_CRYPTO_03866
    /// @needwork = ad
    /// @endcode
    CryptoObjectType GetTypeRestriction() const noexcept override;
    /// @brief Returns the "exportable" attribute of the object stored in the container. Object exportability does not depend on the volatility of its container.
    /// @name IsObjectExportable
    /// @returns true if can export false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01488
    /// @trace_id_dd=DD_CRYPTO_03867
    /// @needwork = ad
    /// @endcode
    bool IsObjectExportable() const noexcept override;
    /// @brief Returns the set "session" (or "temporary") attribute of the object, e.g., from keyderivationfunctionctx::DeriveKey().
    /// A "session" object can only be stored in a VolatileTrustedContainer! If this IOInterface is linked to a KeySlot, it always returns false.
    /// @name IsObjectSession
    /// @returns true if can is session object false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01489
    /// @trace_id_dd=DD_CRYPTO_03868
    /// @needwork = ad
    /// @endcode
    bool IsObjectSession() const noexcept override;
    /// @brief Returns the "volatility" attribute of the underlying buffer of this IOInterface.
    /// "Session" objects can only be stored in "volatile" containers. The contents of a "volatile" container will be destroyed along with the interface instance.
    /// @name IsVolatile
    /// @returns true if can is volatile object false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01490
    /// @trace_id_dd=DD_CRYPTO_03869
    /// @needwork = ad
    /// @endcode
    bool IsVolatile() const noexcept override;
    /// @brief Gets whether the underlying key store is valid. If the underlying resource is modified after the IOInterface is opened, the IOInterface becomes invalid.
    /// @name IsValid
    /// @returns true if can is Valid object false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01491
    /// @trace_id_dd=DD_CRYPTO_03870
    /// @needwork = ad
    /// @endcode
    bool IsValid() const noexcept override;
    /// @brief Gets whether the underlying KeySlot is writable. If this IOInterface is linked to a volatile trusted container, it always returns true.
    /// @name IsWritable
    /// @returns true if can is Writable object false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01492
    /// @trace_id_dd=DD_CRYPTO_03871
    /// @needwork = ad
    /// @endcode
    bool IsWritable() const noexcept override;

public:
    /// @brief Set SlotID
    /// @name SetSlotID
    /// @param nIpcSlotID IPC key slot ID
    /// @param nInterfaceID IO interface ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01493
    /// @trace_id_dd=DD_CRYPTO_03872
    /// @needwork = ad
    /// @endcode
    void SetSlotID(uint32_t const nIpcSlotID, uint32_t const nInterfaceID) noexcept;
    /// @brief Get SlotID
    /// @name GetSlotID
    /// @returns Key slot ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01494
    /// @trace_id_dd=DD_CRYPTO_03873
    /// @needwork = ad
    /// @endcode
    inline uint32_t GetSlotID() const noexcept { return nIpcSlotID_; }
    /// @brief Get IoInterfaceID
    /// @name GetIoInterfaceID
    /// @returns IoInterfaceID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01495
    /// @trace_id_dd=DD_CRYPTO_03874
    /// @needwork = ad
    /// @endcode
    inline uint32_t GetIoInterfaceID() const noexcept { return nIpcIoInterfaceID_; }
    /// @brief Get SlogName
    /// @name GetSlotName
    /// @returns Key slot name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01496
    /// @trace_id_dd=DD_CRYPTO_03875
    /// @needwork = ad
    /// @endcode
    ara::core::String GetSlotName() const noexcept { return stSlotName_; }
    /// @brief Get IPC client
    /// @name GetIpcClient
    /// @returns Reference to IPC client
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01497
    /// @trace_id_dd=DD_CRYPTO_03876
    /// @needwork = ad
    /// @endcode
    inline PIpcClient const& GetIpcClient() const noexcept { return ipcClient_; }

private:
    /// @brief IPC client object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03877
    /// @needwork = dda
    /// @endcode
    PIpcClient const& ipcClient_;
    /// @brief IPC key slot ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03878
    /// @needwork = dda
    /// @endcode
    uint32_t nIpcSlotID_;
    /// @brief IPC IO interface
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03879
    /// @needwork = dda
    /// @endcode
    uint32_t nIpcIoInterfaceID_;
    /// @brief Key slot name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03880
    /// @needwork = dda
    /// @endcode
    ara::core::String stSlotName_;
    /// @brief Parameterized constructor
    /// @param ipcClient IPC client
    /// @param nIpcSlotID IPC key slot ID
    /// @param nIpcIoInterfaceID
    /// @param slotName Key slot name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03881
    /// @needwork = dda
    /// @endcode
    PIoInterface_Ipc(PIpcClient const& ipcClient,
                     uint32_t const nIpcSlotID,
                     uint32_t const nIpcIoInterfaceID,
                     ara::core::StringView const& slotName) noexcept;

protected:
    /// @brief IPC function sending packets using PIpcReq_InterfaceBase and receiving responses with PIpcAsw_LogicHead
    /// @name   _IpcGetData
    /// @param aswMsg Response message managed by IPC
    /// @param stFuncName Function name used for IPC call
    /// @returns true if get data sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03882
    /// @needwork = dda
    /// @endcode
    bool _IpcGetData(PIpcAutoPacket& aswMsg, ara::core::StringView const& stFuncName) const noexcept;
    /// @brief IPC function template sending packets using PIpcReq_InterfaceBase and receiving responses with PIpcAsw_LogicHead
    /// @param stFuncName Function name used for IPC call
    /// @return true if get data sucess false otherwise
    /// @code{.isoft}
    /// @tparam T_Type
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_03883
    /// @needwork = dda
    /// @endcode
    template < typename T_Type >
    inline ara::core::Result< T_Type > _T_IpcGetData(ara::core::StringView const& stFuncName) const noexcept;
};
//********************************/
}  // namespace  isoft_def
}  // namespace keys
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_KEYS_PUHUA_IO_INTERFACE_IPC_H_
