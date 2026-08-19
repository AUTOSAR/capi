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
/// @file       isoft_io_interface_kv_share.h
/// @brief      AutoSar-Crypto encryption and decryption shared module
/// @details    Formal interface of IOInterface, used to save and load security objects: implementation version where all slots share the same KV database.
/// @date       2022-04-12
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par modification log:
/// < table>
/// < tr><th>Date        <th>Version  <th>Author      <th>Description
/// < tr><td>2022-04-12  <td>1.0.0    <td>HAN YUXIN      <td>Create initial version
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/key manager/IO Interface
/// @interface_level=module
/// @trace_id_sr=SR_CRYPTO_01008
/// @unit_name=PIoInterface_KvShare
/// @unit_description=Interface for saving and loading security objects
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_KEYS_PUHUA_IO_INTERFACE_KV_SHARE_H_
#define ARA_CRYPTO_KEYS_PUHUA_IO_INTERFACE_KV_SHARE_H_

#include <ara/per/key_value_storage.h>

#include "ara/crypto/common/isoft_io_interface.h"
#include "ara/crypto/common/isoft_io_interface_mem.h"
#include "ara/crypto/cryp/crypto_provider.h"
#include "ara/crypto/cryp/symmetric/isoft_ctx_symmetric_block.h"
#include "ara/crypto/keys/keyslot.h"

namespace ara {
namespace crypto {
namespace keys {
namespace isoft_def {
//********************************/
/// @brief Formal interface of IOInterface, used to save and load security objects.
///         Actual saving and loading should be implemented through known internal methods in the trusted pair of encryption provider and storage provider.
///         Each object should be uniquely identified by its type and cryptographic object unique identifier (COUID). This interface assumes that objects in the container are compressed, i.e., have a minimal optimized size.
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_CRYPTO_03095
/// @trace_id_dd=DD_CRYPTO_06158
/// @needwork = ad
/// @endcode
class PIoInterface_KvShare : public PIoInterface
{
public:
    /// @brief Unique smart pointer of the interface.
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_03095
    /// @trace_id_dd=DD_CRYPTO_06483
    /// @needwork = dd
    /// @endcode
    using Uptr = std::unique_ptr< PIoInterface_KvShare >;

public:
    /// @brief Constructor
    /// @name   PIoInterface_KvShare
    /// @param cryptoProvider Encryption provider
    /// @param stStorageName Persistent database name
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_03096
    /// @trace_id_dd=DD_CRYPTO_06159
    /// @needwork = ad
    /// @endcode
    PIoInterface_KvShare(ara::crypto::cryp::CryptoProvider& cryptoProvider,
                         ara::core::StringView const& stStorageName) noexcept;
    /// @brief Default virtual destructor
    /// @name  ~PIoInterface_KvShare
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_03097
    /// @trace_id_dd=DD_CRYPTO_06160
    /// @needwork = ad
    /// @endcode
    ~PIoInterface_KvShare() noexcept override;
    /// @brief Default copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_03098
    /// @trace_id_dd=DD_CRYPTO_06161
    /// @needwork = ad
    /// @endcode
    PIoInterface_KvShare(PIoInterface_KvShare const& other) = delete;
    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_03099
    /// @trace_id_dd=DD_CRYPTO_06162
    /// @needwork = ad
    /// @endcode
    PIoInterface_KvShare(PIoInterface_KvShare&& other) = delete;
    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_03100
    /// @trace_id_dd=DD_CRYPTO_06163
    /// @needwork = ad
    /// @endcode
    PIoInterface_KvShare& operator=(PIoInterface_KvShare const& other) = delete;
    /// @brief Default move assignment function
    /// @param other Another object instance of this class
    /// @return  *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_03101
    /// @trace_id_dd=DD_CRYPTO_06164
    /// @needwork = ad
    /// @endcode
    PIoInterface_KvShare& operator=(PIoInterface_KvShare&& other) = delete;

public:  // IOInterface interface
    /// @brief Return the actual allowed key/seed usage flags as defined by the key slot prototype and the current content of the container for this "Actor".
    ///         Volatile containers have no prototype restrictions, but can define restrictions for the current instance of the object at runtime.
    ///         The value returned by this method is the bitwise AND of the common usage flags defined at runtime and the usage flags defined by the UserPermissions prototype for the current "Actor".
    ///         This method is particularly useful for empty permanent prototype containers.
    /// @name   GetAllowedUsage
    /// @returns a combination of bit-flags that specifies allowed applications of the object
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_03102
    /// @trace_id_dd=DD_CRYPTO_06165
    /// @needwork = ad
    /// @endcode
    AllowedUsageFlags GetAllowedUsage() const noexcept override;
    /// @brief Return the capacity of the underlying resource. In bytes.
    /// @name   GetCapacity
    /// @returns Capacity of the underlying resource
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_03103
    /// @trace_id_dd=DD_CRYPTO_06166
    /// @needwork = ad
    /// @endcode
    std::size_t GetCapacity() const noexcept override;
    /// @brief Return the cryptobjecttype of the object referenced by this IOInterface.
    /// @name   GetCryptoObjectType
    /// @returns Cryptobjecttype of the referenced object
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_03104
    /// @trace_id_dd=DD_CRYPTO_06167
    /// @needwork = ad
    /// @endcode
    CryptoObjectType GetCryptoObjectType() const noexcept override;
    /// @brief Return the COUID of the object stored in the IOInterface.
    ///         If the container is empty, this method returns cryptobjecttype::KUndefined. Unambiguous identification of a cryptographic object requires two components: cryptobjectuid and cryptobjecttype.
    /// @name   GetObjectId
    /// @returns COUID of the object stored in the IOInterface
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_03105
    /// @trace_id_dd=DD_CRYPTO_06168
    /// @needwork = ad
    /// @endcode
    CryptoObjectUid GetObjectId() const noexcept override;
    /// @brief Return the size of the object payload stored in the underlying buffer of the IOInterface.
    ///         If the container is empty, this method returns 0. The return value does not consider the meta-information attributes of the object, but their size is fixed and common to all cryptographic objects independent of their actual type.
    ///         Automatically provide space for the object's meta-information according to the implementation details of the object.
    /// @name   GetPayloadSize
    /// @returns Size of the object payload
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_03106
    /// @trace_id_dd=DD_CRYPTO_06169
    /// @needwork = ad
    /// @endcode
    std::size_t GetPayloadSize() const noexcept override;
    /// @brief Get the vendor-specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Vendor-specific ID of the primitive
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_03107
    /// @trace_id_dd=DD_CRYPTO_06170
    /// @needwork = ad
    /// @endcode
    CryptoAlgId GetPrimitiveId() const noexcept override;
    /// @brief Return the content type restriction of this interface (identifying what type of data this container can hold).
    /// @return Content type restriction of the interface
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_03108
    /// @trace_id_dd=DD_CRYPTO_06171
    /// @needwork = ad
    /// @endcode
    CryptoObjectType GetTypeRestriction() const noexcept override;
    /// @brief Return the "exportable" attribute of the object stored in the container. The exportability of an object does not depend on the volatility of its container.
    /// @name   IsObjectExportable
    /// @returns true if can export false otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_03109
    /// @trace_id_dd=DD_CRYPTO_06172
    /// @needwork = ad
    /// @endcode
    bool IsObjectExportable() const noexcept override;
    /// @brief Return the set "session" (or "temporary") attribute of the object, e.g., keyderivationfunctionctx::DeriveKey().
    ///         A "session" object can only be stored in a VolatileTrustedContainer! If this IOInterface is linked to a KeySlot, it always returns false.
    /// @name   IsObjectSession
    /// @returns true if is session object false otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_03110
    /// @trace_id_dd=DD_CRYPTO_06173
    /// @needwork = ad
    /// @endcode
    bool IsObjectSession() const noexcept override;
    /// @brief Return the "volatile" attribute of the underlying buffer of this IOInterface.
    ///         "Session" objects can only be stored in "volatile" containers. The contents of a "volatile" container will be destroyed together with the interface instance.
    /// @name   IsVolatile
    /// @returns true if is volatile object false otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_03111
    /// @trace_id_dd=DD_CRYPTO_06174
    /// @needwork = ad
    /// @endcode
    bool IsVolatile() const noexcept override;
    /// @brief Get whether the underlying key database is valid. If the underlying resource is modified after the IOInterface is opened, the IOInterface will become invalid.
    /// @name   IsValid
    /// @returns true if is Valid object false otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_03112
    /// @trace_id_dd=DD_CRYPTO_06175
    /// @needwork = ad
    /// @endcode
    bool IsValid() const noexcept override;
    /// @brief Get whether the underlying KeySlot is writable. If this IOInterface is linked to a volatile trusted container, it always returns true.
    /// @name   IsWritable
    /// @returns true if is Writable object false otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_03113
    /// @trace_id_dd=DD_CRYPTO_06176
    /// @needwork = ad
    /// @endcode
    bool IsWritable() const noexcept override;

public:  // PIoInterface custom interface
    /// @brief Indicates whether the content of this key slot can be changed, e.g., from storing a symmetric key to storing an RSA key.
    /// @name   IsContentTypeChange
    /// @returns true if the keyslot content can change false otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_03114
    /// @trace_id_dd=DD_CRYPTO_06177
    /// @needwork = ad
    /// @endcode
    bool IsContentTypeChange() const noexcept override;
    /// @brief Indicates whether FC encryption should allocate sufficient storage space for the shadow copy of this key string.
    /// @name   IsAllocateSpareSlot
    /// @returns true if the keyslot Allocate other Spare false otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_03115
    /// @trace_id_dd=DD_CRYPTO_06178
    /// @needwork = ad
    /// @endcode
    bool IsAllocateSpareSlot() const noexcept override;
    /// @brief Specifies the number of times this key slot can be updated, for example:
    ///         0: Indicates that the key-slot content will be preset during production
    ///         1: Indicates that the key slot content can be updated only once ("OTP").
    ///        -1: Negative value indicates that the key slot content can be updated unlimited times
    /// @name   GetMaxUpdateAllowed
    /// @returns
    /// 0: Indicates that the key-slot content will be preset during production, 1: Indicates that the key slot content can be updated only once ("OTP"), -1: Negative value indicates that the key slot content can be updated unlimited times
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_03116
    /// @trace_id_dd=DD_CRYPTO_06179
    /// @needwork = ad
    /// @endcode
    int32_t GetMaxUpdateAllowed() const noexcept override;
    /// @brief Key-slot type configuration
    /// @name   GetSlotType
    /// @returns Key slot type
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_03117
    /// @trace_id_dd=DD_CRYPTO_06180
    /// @needwork = ad
    /// @endcode
    KeySlotType GetSlotType() const noexcept override;
    /// @brief Vendor-provided cryptographic primitive ID for key-slot restrictions.
    /// @name   GetSlotPrimitiveId
    /// @returns Cryptographic primitive ID
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_03118
    /// @trace_id_dd=DD_CRYPTO_06181
    /// @needwork = ad
    /// @endcode
    CryptoAlgId GetSlotPrimitiveId() const noexcept override;
    /// @brief Usage scope identifier for key-slot restrictions.
    /// @name   GetSlotAllowedUsage
    /// @returns Usage scope identifier
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_03119
    /// @trace_id_dd=DD_CRYPTO_06182
    /// @needwork = ad
    /// @endcode
    AllowedUsageFlags GetSlotAllowedUsage() const noexcept override;
    /// @brief Get whether there is a write operation on the raw data of the key slot
    /// @name   IsHaveWriteAction
    /// @returns true if can write false otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_03120
    /// @trace_id_dd=DD_CRYPTO_06183
    /// @needwork = ad
    /// @endcode
    bool IsHaveWriteAction() const noexcept override;
    /// @brief Save KeySlotContentProps
    /// @name   SaveKeyContent
    /// @param contentProps Key attributes
    /// @returns true if save sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_03121
    /// @trace_id_dd=DD_CRYPTO_06184
    /// @needwork = ad
    /// @endcode
    bool SaveKeyContent(ara::crypto::keys::KeySlotContentProps const& contentProps) const noexcept override;
    /// @brief Save KeySlotPrototypeProps
    /// @name   SaveSlotProps
    /// @param slotProps Key slot attributes
    /// @returns true if save sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_03122
    /// @trace_id_dd=DD_CRYPTO_06185
    /// @needwork = ad
    /// @endcode
    bool SaveSlotProps(ara::crypto::keys::KeySlotPrototypeProps const& slotProps) const noexcept override;
    /// @brief Set GetName_KvWriteIndex()
    /// @name   SetWriteIndex
    /// @returns true if Set WriteIndex sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_03123
    /// @trace_id_dd=DD_CRYPTO_06186
    /// @needwork = ad
    /// @endcode
    bool SetWriteIndex() const noexcept override;
    /// @brief Set GetName_KvWriteSave() and commit persistent saving
    /// @name   CommitSaveAction
    /// @returns true if CommitSaveAction sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_03124
    /// @trace_id_dd=DD_CRYPTO_06187
    /// @needwork = ad
    /// @endcode
    bool CommitSaveAction() const noexcept override;
    /// @brief Roll back possible save operations
    /// @name   RollbackSaveAction
    /// @returns true if RollbackSaveAction sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_03125
    /// @trace_id_dd=DD_CRYPTO_06188
    /// @needwork = ad
    /// @endcode
    bool RollbackSaveAction() const noexcept override;

public:  // PIoInterface custom interface
    /// @brief Initialize the IO interface
    /// @name   InitIoInterface
    /// @param stSlotName Key slot name
    /// @returns  true if Init IoInterface sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_03126
    /// @trace_id_dd=DD_CRYPTO_06189
    /// @needwork = ad
    /// @endcode
    bool InitIoInterface(ara::core::StringView const& stSlotName) noexcept override;
    /// @name   GetKeySlotName
    /// @returns Name of the key slot
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_03127
    /// @trace_id_dd=DD_CRYPTO_06190
    /// @needwork = ad
    /// @endcode
    ara::core::StringView GetKeySlotName() const noexcept override;
    /// @brief Whether the IO interface is initialized
    /// @name   IsInterfaceValid
    /// @returns true if init sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_03128
    /// @trace_id_dd=DD_CRYPTO_06191
    /// @needwork = ad
    /// @endcode
    bool IsInterfaceValid() const noexcept override;
    /// @brief Clear data
    /// @name   Clear
    /// @returns true if clear sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_03129
    /// @trace_id_dd=DD_CRYPTO_06192
    /// @needwork = ad
    /// @endcode
    bool Clear() noexcept override;
    /// @brief Check whether the slot is empty.
    /// @name   IsEmpty
    /// @returns true if id empty false otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_03130
    /// @trace_id_dd=DD_CRYPTO_06193
    /// @needwork = ad
    /// @endcode
    bool IsEmpty() const noexcept override;
    /// @brief Read the encryption algorithm information of the key data
    /// @name   ReadKeyKey
    /// @param nAlgID Cryptographic primitive ID
    /// @param vecData Vector object to receive KeyKey data
    /// @returns true if read keykey sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_03131
    /// @trace_id_dd=DD_CRYPTO_06194
    /// @needwork = ad
    /// @endcode
    bool ReadKeyKey(CryptoAlgId& nAlgID, ara::core::Vector< uint8_t >& vecData) const noexcept override;
    /// @brief Read key data
    /// @name   ReadKeyData
    /// @param vecData Vector object to receive Key data
    /// @returns true if read keydata sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_03132
    /// @trace_id_dd=DD_CRYPTO_06195
    /// @needwork = ad
    /// @endcode
    bool ReadKeyData(ara::core::Vector< uint8_t >& vecData) const noexcept override;
    /// @brief Save key data
    /// @name   SaveKeyData
    /// @param memKeyData Key data in memory
    /// @param nAlgID Cryptographic primitive ID
    /// @param memKeyKey Key used to encrypt the key
    /// @return  true if save keydata sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_03133
    /// @trace_id_dd=DD_CRYPTO_06196
    /// @needwork = ad
    /// @endcode
    bool SaveKeyData(ReadOnlyMemRegion const& memKeyData,
                     CryptoAlgId nAlgID                 = kAlgIdAny,
                     ReadOnlyMemRegion const& memKeyKey = ReadOnlyMemRegion()) const noexcept override;
    /// @brief Whether there is a deletion flag
    /// @name   GetDeleteFlag
    /// @returns true if get DeleteFlag sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_03134
    /// @trace_id_dd=DD_CRYPTO_06197
    /// @needwork = ad
    /// @endcode
    bool GetDeleteFlag() const noexcept override;
    /// @brief Set the deletion flag
    /// @name   SetDeleteFlag
    /// @param nDelProcessID Deletion flag
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_03135
    /// @trace_id_dd=DD_CRYPTO_06198
    /// @needwork = ad
    /// @endcode
    void SetDeleteFlag(uint32_t nDelProcessID) const noexcept override;

    /// @brief Reset key slot attribute information
    /// @param slotProps Key slot attributes
    /// @return success/failed
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_03136
    /// @trace_id_dd=DD_CRYPTO_06199
    /// @needwork = ad
    /// @endcode
    bool ResetKeySlot(KeySlotPrototypeProps const& slotProps) const noexcept override;

private:
    /// @brief Provider used for encryption/decryption of this object
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06200
    /// @needwork = dda
    /// @endcode
    ara::crypto::cryp::CryptoProvider& cryptoProvider_;
    /// @brief Persistent KV engine
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06201
    /// @needwork = dda
    /// @endcode
    mutable ara::per::SharedHandle< ara::per::KeyValueStorage > pKvStorage_{};
    /// @brief Persistent storage name
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06202
    /// @needwork = dda
    /// @endcode
    ara::core::String stStorageName_{};
    /// @brief Bound key slot name, also used as the Key in the KV database
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06203
    /// @needwork = dda
    /// @endcode
    ara::core::String stKeySlotName_{};
    /// @brief Cache data for reading/writing key slots
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06204
    /// @needwork = dda
    /// @endcode
    mutable PKeySlotData_Mem slotData_{};
    /// @brief Many attributes may need to be initialized according to the configuration in XML
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06205
    /// @needwork = dda
    /// @endcode
    bool bObjectSession_{false};
    /// @brief Many attributes may need to be initialized according to the configuration in XML
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06206
    /// @needwork = dda
    /// @endcode
    bool bVolatile_{false};

protected:
    /// @brief Read the value from the KV database
    /// @name   _GetKvStorageData
    /// @param stName KEY name in the KV database
    /// @param nDef Default value
    /// @return Value from the KV database
    /// @code{.isoft}
    /// @tparam T_Data
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06208
    /// @needwork = dda
    /// @endcode
    template < typename T_Data >
    inline T_Data _GetKvStorageData(ara::core::StringView const& stName, T_Data const nDef) const noexcept;
    /// @brief Set the value in the KV database
    /// @name   _SetKvStorageData
    /// @param stName Key value in the KV pair: string
    /// @param nData Value value in the KV pair: integer
    /// @return true if set kv storage sucess false otherwise
    /// @code{.isoft}
    /// @tparam T_Data
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06209
    /// @needwork = dda
    /// @endcode
    template < typename T_Data >
    inline bool _SetKvStorageData(ara::core::StringView const& stName, T_Data const nData) const noexcept;
    /// @brief Encrypt/decrypt key data
    /// @name   _SymmetricKeyData
    /// @param vecData Vector object: input plaintext/ciphertext, output ciphertext/plaintext
    /// @param memKeyData Key data in memory
    /// @param nAlgID Cryptographic primitive ID
    /// @param eTransform Encryption direction: encrypt or decrypt
    /// @return true Encryption success, false Decryption failure
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06210
    /// @needwork = dda
    /// @endcode
    bool _SymmetricKeyData(ara::core::Vector< uint8_t >& vecData,
                           ReadOnlyMemRegion const& memKeyData,
                           CryptoAlgId const nAlgID,
                           CryptoTransform const eTransform) const noexcept;
    /// @brief Load the key used to encrypt the key
    /// @name   _LoadKeyKey
    /// @param nAlgID Cryptographic primitive ID
    /// @returns SymmetricKey instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06211
    /// @needwork = dda
    /// @endcode
    cryp::SymmetricKey::Uptrc _LoadKeyKey(CryptoAlgId const nAlgID) const noexcept;
    /// @brief Initialize and load SlotData from the KV database
    /// @name   _InitLoadSlotData
    /// @returns true if init kv sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06212
    /// @needwork = dda
    /// @endcode
    bool _InitLoadSlotData() const noexcept;
    /// @brief Return the SaveIndex saved in the database
    /// @name   _GetSaveWriteIndex
    /// @returns SaveIndex saved in the database
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06213
    /// @needwork = dda
    /// @endcode
    uint32_t _GetSaveWriteIndex() const noexcept;
};
//********************************/
}  // namespace  isoft_def
}  // namespace keys
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_KEYS_PUHUA_IO_INTERFACE_KV_SHARE_H_
