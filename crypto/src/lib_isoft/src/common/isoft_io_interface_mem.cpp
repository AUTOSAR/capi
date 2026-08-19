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
/// @file       isoft_io_interface_mem.cpp
/// @brief      AutoSar-Crypto encryption/decryption common module
/// @details
/// @date       2022-04-12
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/ReuseFunction/ReuseFunctionModule
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=PKeySlotData_InKv
/// @unit_description=Key slot data: KV database
/// @endcode
///
/// ================================================================

#include "ara/crypto/common/isoft_io_interface_mem.h"

#include "ara/crypto/common/isoft_common_api.h"

namespace ara {
namespace crypto {
namespace keys {
namespace isoft_def {
//********************************/
/// @brief Formal interface of IOInterface for saving and loading security objects.
///         The actual saving and loading should be implemented through internal methods known to the trusted pair of crypto provider and storage provider.
///         Each object should be uniquely identified by its type and crypto object unique identifier (COUID). This interface assumes that objects in the container are compressed, i.e., have a minimal optimized size.
//********************************/
/// @brief Reset all data
void PKeySlotData_Mem::Reset() noexcept
{
    SetWriteIndex(0U);
    ResetKeySlotContentProps();
    ResetKeySlotPrototypeProps();
    keyData_.ResetData();
    SetKeyAlgID(0U);
    SetDeleteFlag(0U);
    ResetReserver();
    keyKey_.ResetData();
}
/// @brief Parameterized constructor
/// @param bVolatile whether it is volatile
/// @returns
/// @throws
PIoInterface_Mem::PIoInterface_Mem(bool const bVolatile) noexcept
    : PIoInterface{}  // NOLINT
    , stSlotName_{}   // NOLINT
    , bVolatile_{bVolatile}
    , slotDataTemp_{}  // NOLINT
    , slotDataSave_{}  // NOLINT
    , bValid_{true}
{
}
/// @brief Returns the actual allowed key/seed usage flags defined by the key slot prototype of this "Actor" and the current content of the container.
///         Volatile containers have no prototype restrictions, but can define restrictions for the current instance of the object at runtime.
///         The value returned by this method is the bitwise AND of the common usage flags defined at runtime and the usage flags defined by the UserPermissions prototype for the current "Actor".
///         This method is particularly useful for empty permanent prototype containers.
/// @name  GetAllowedUsage
/// @returns  allowed usage restrictions
AllowedUsageFlags PIoInterface_Mem::GetAllowedUsage() const noexcept
{
    return slotDataSave_.GetKeySlotContentProps().mContentAllowedUsage;
}
/// @brief Returns the capacity of the underlying resource.
/// @name  GetCapacity
/// @returns  underlying resource capacity
std::size_t PIoInterface_Mem::GetCapacity() const noexcept
{
    return slotDataSave_.GetKeySlotPrototypeProps().mSlotCapacity;
}
/// @brief Returns the cryptobjecttype of the object referenced by this IOInterface.
/// @name  GetCryptoObjectType
/// @returns  key object type
CryptoObjectType PIoInterface_Mem::GetCryptoObjectType() const noexcept
{
    return slotDataSave_.GetKeySlotContentProps().mObjectType;
}
/// @brief Returns the COUID of the object stored in the IOInterface.
///         If the container is empty, this method returns cryptobjecttype::KUndefined. Explicit identification of a crypto object requires two components: cryptobjectuid and cryptobjecttype.
/// @name  GetObjectId
/// @returns  crypto object UUID
CryptoObjectUid PIoInterface_Mem::GetObjectId() const noexcept
{
    return slotDataSave_.GetKeySlotContentProps().mObjectUid;
}
/// @brief Returns the size of the object payload stored in the underlying buffer of the IOInterface.
///         If the container is empty, this method returns 0. The return value does not consider the object's meta-information attributes, but their size is fixed and common to all crypto objects independent of their actual type.
///         Automatic space for the object's meta-information is provided according to the implementation details of the object.
/// @name  GetPayloadSize
/// @returns  crypto object payload size
std::size_t PIoInterface_Mem::GetPayloadSize() const noexcept
{
    return slotDataSave_.GetKeySlotContentProps().mObjectSize;
}
/// @brief Get the vendor-specific ID of the primitive.
/// @name  GetPrimitiveId
/// @returns  crypto algorithm ID
CryptoAlgId PIoInterface_Mem::GetPrimitiveId() const noexcept { return slotDataSave_.GetKeySlotContentProps().mAlgId; }
/// @brief Returns the content type restriction of this interface (identifying what type of data this container can hold).
/// @return If KeySlotPrototypeProps::mAllowContentTypeChange==TRUE, kUndefined should be returned.
/// If the container has a type restriction different from @c CryptoObjectType::kUndefined, only objects of that type can be saved into this container.
/// Volatile containers have no content type restrictions.
/// CryptoObjectType::kUndefined identifies no restriction.
CryptoObjectType PIoInterface_Mem::GetTypeRestriction() const noexcept
{
    if (IsContentTypeChange()) {
        return CryptoObjectType::kUndefined;
    }
    /// @returns an object type of allowed content (@c CryptoObjectType::kUndefined means without restriction)
    return slotDataSave_.GetKeySlotPrototypeProps().mObjectType;
}
/// @brief Returns the "exportable" attribute of the object stored in the container. The exportability of an object does not depend on the volatility of its container.
/// @name  IsObjectExportable
/// @returns  ture if can export false otherwise
bool PIoInterface_Mem::IsObjectExportable() const noexcept
{
    return slotDataSave_.GetKeySlotPrototypeProps().mExportAllowed;
}
/// @brief Get whether the underlying key store is valid. If the underlying resource is modified after the IOInterface is opened, the IOInterface will become invalid.
/// @name  IsValid
/// @returns  true if is valid false otherwise
bool PIoInterface_Mem::IsValid() const noexcept
{
    /// @returns true if the underlying resource can be valid, false otherwise
    /// If the slot content changes after the iointerface is opened, it becomes invalid
    return bValid_;
}
/// @brief Get whether the underlying KeySlot is writable. If this IOInterface is linked to a volatile trusted container, always returns true.
/// @name  IsWritable
/// @returns  true if is writeable false otherwise
bool PIoInterface_Mem::IsWritable() const noexcept
{
    int32_t const nMaxUpdateAllowed{GetMaxUpdateAllowed()};
    if (nMaxUpdateAllowed < 0) {
        return true;
    }
    if (0 == nMaxUpdateAllowed) {
        return IsEmpty();
    }
    return slotDataTemp_.GetWriteIndex() < static_cast< uint32_t >(nMaxUpdateAllowed);
}
//***************/
/// @brief Indicates whether the content of this key slot can be changed, e.g., from storing a symmetric key to storing an RSA key.
/// @name  IsContentTypeChange
/// @returns  true if can be change false otherwise
bool PIoInterface_Mem::IsContentTypeChange() const noexcept
{
    return slotDataSave_.GetKeySlotPrototypeProps().mAllowContentTypeChange;
}
/// @brief Indicates whether FC encryption should allocate sufficient storage space for the shadow copy of this key string.
/// @name  IsAllocateSpareSlot
/// @returns  true if sAllocateSpare false otherwise
bool PIoInterface_Mem::IsAllocateSpareSlot() const noexcept
{
    return slotDataSave_.GetKeySlotPrototypeProps().mAllocateSpareSlot;
}
/// @brief Specifies how many times this key slot can be updated, for example:
///         0: indicates that the key-slot content will be preset during production;
///         1: indicates that the key slot content can be updated only once ("OTP");
///        -1: a negative value indicates that the key slot content can be updated unlimited times.
/// @name  GetMaxUpdateAllowed
/// @returns  the number of times the key slot can be updated
int32_t PIoInterface_Mem::GetMaxUpdateAllowed() const noexcept
{
    return slotDataSave_.GetKeySlotPrototypeProps().mMaxUpdateAllowed;
}
/// @brief Key-slot type configuration
/// @name  GetSlotType
/// @returns key slot type
KeySlotType PIoInterface_Mem::GetSlotType() const noexcept
{
    return slotDataSave_.GetKeySlotPrototypeProps().mSlotType;
}
/// @brief Vendor-provided crypto primitive ID for key-slot restrictions.
/// @name  GetSlotPrimitiveId
/// @returns  crypto primitive ID
CryptoAlgId PIoInterface_Mem::GetSlotPrimitiveId() const noexcept
{
    return slotDataSave_.GetKeySlotPrototypeProps().mAlgId;
}
/// @brief Usage scope identifier for key-slot restrictions.
/// @name  GetSlotAllowedUsage
/// @returns  usage scope identifier
AllowedUsageFlags PIoInterface_Mem::GetSlotAllowedUsage() const noexcept
{
    return slotDataSave_.GetKeySlotPrototypeProps().mContentAllowedUsage;
}
/// @brief Check whether the raw data of the key slot has been written to
/// @name  IsHaveWriteAction
/// @returns true can write false otherwise
bool PIoInterface_Mem::IsHaveWriteAction() const noexcept
{
    return slotDataTemp_.GetWriteIndex() > slotDataSave_.GetWriteIndex();
}
/// @brief Save KeySlotContentProps
/// @param keyContent key content properties
/// @name  SaveKeyContent
/// @returns  true if save sucess false otherwise
bool PIoInterface_Mem::SaveKeyContent(KeySlotContentProps const& keyContent) const noexcept
{
    slotDataTemp_.SetKeySlotContentProps(keyContent);
    return true;
}
/// @brief Save KeySlotPrototypeProps
/// @param slotProps key slot properties
/// @name  SaveSlotProps
/// @returns  true if save sucess false otherwise
bool PIoInterface_Mem::SaveSlotProps(KeySlotPrototypeProps const& slotProps) const noexcept
{
    slotDataTemp_.SetKeySlotPrototypeProps(slotProps);
    return true;
}
/// @brief Set GetName_KvWriteIndex()/GetName_KvWriteSave()
/// @name  SetWriteIndex
/// @returns ture set write index false otherwise
bool PIoInterface_Mem::SetWriteIndex() const noexcept
{
    slotDataTemp_.SetWriteIndex(slotDataTemp_.GetWriteIndex() + 1U);
    return true;
}
/// @brief Set GetName_KvWriteSave() and commit persistent save
/// @name  CommitSaveAction
/// @returns true if commit sucess false otherwise
bool PIoInterface_Mem::CommitSaveAction() const noexcept
{
    slotDataSave_ = slotDataTemp_;
    return true;
}
/// @brief Roll back possible save operations
/// @name  RollbackSaveAction
/// @returns  ture if rollback sucess false otherwise
bool PIoInterface_Mem::RollbackSaveAction() const noexcept
{
    slotDataTemp_ = slotDataSave_;
    return true;
}

/// @brief Initialize IO interface
/// @param stSlotName key slot name
/// @name  InitIoInterface
/// @returns  true if init sucess flase otherwise
bool PIoInterface_Mem::InitIoInterface(ara::core::StringView const& stSlotName) noexcept
{
    stSlotName_ = stSlotName;
    slotDataSave_.Reset();
    slotDataTemp_.Reset();
    return true;
}
/// @brief Whether the IO interface is initialized
/// @name  IsInterfaceValid
/// @returns  ture if has already inited false otherwise
bool PIoInterface_Mem::IsInterfaceValid() const noexcept { return false == stSlotName_.empty(); }
/// @brief Clear data
/// @name  Clear
/// @returns  true if clear sucess false otherwise
bool PIoInterface_Mem::Clear() noexcept
{
    slotDataSave_.Reset();
    slotDataTemp_.Reset();
    return true;
}
/// @brief Check whether the slot is empty.
/// @name  IsEmpty
/// @returns  true is empty false otherwise
bool PIoInterface_Mem::IsEmpty() const noexcept { return slotDataSave_.GetKeyDataSize() <= 0U; }
/// @brief Read encryption algorithm information of the key data
/// @param nAlgID crypto primitive ID
/// @param vecData Vector object receiving KeyKey data
/// @return true if read keykey sucess false otherwise
bool PIoInterface_Mem::ReadKeyKey(CryptoAlgId& nAlgID, ara::core::Vector< uint8_t >& vecData) const noexcept
{
    if (nAlgID > 0U) {
    }  // for qac
    nAlgID = slotDataSave_.GetKeyAlgID();
    vecData.clear();
    vecData.resize(static_cast< std::size_t >(slotDataSave_.GetKeyKeySize()));
    std::ignore
        = memcpy(static_cast< void* >(vecData.data()), static_cast< void const* >(slotDataSave_.GetKeyKeydata()),
                 static_cast< std::size_t >(slotDataSave_.GetKeyKeySize()));
    return true;
}
/// @brief Read key data
/// @param vecData Vector object receiving Key data
/// @name  ReadKeyData
/// @returns  true if read keydate sucess false otherwise
bool PIoInterface_Mem::ReadKeyData(ara::core::Vector< uint8_t >& vecData) const noexcept
{
    vecData.clear();
    vecData.resize(static_cast< std::size_t >(slotDataSave_.GetKeyDataSize()));
    std::ignore = memcpy(static_cast< void* >(vecData.data()), static_cast< void const* >(slotDataSave_.GetKeydata()),
                         static_cast< std::size_t >(slotDataSave_.GetKeyDataSize()));
    return true;
}
/// @brief Save key data
/// @name  SaveKeyData
/// @param memKeyData key data in memory
/// @param nAlgID crypto primitive ID
/// @param memKeyKey key used to encrypt the key
/// @return true if save keydate sucess false otherwise
bool PIoInterface_Mem::SaveKeyData(ReadOnlyMemRegion const& memKeyData,
                                   CryptoAlgId nAlgID,
                                   ReadOnlyMemRegion const& memKeyKey) const noexcept
{
    slotDataTemp_.SetKeyAlgID(static_cast< uint32_t >(nAlgID));
    if (false == memKeyKey.empty()) {
        bool const ret{slotDataTemp_.SetKeyKeyData(memKeyKey.data(), static_cast< uint32_t >(memKeyKey.size()), true)};
        std::ignore = ret;
    } else {
        slotDataTemp_.ResetKeyKeyData();
    }
    return slotDataTemp_.SetKeyData(memKeyData.data(), static_cast< uint32_t >(memKeyData.size()), true);
}
/// @brief Set maximum storage capacity threshold
/// @param nCapacity maximum storage capacity threshold
/// @name  SetCapacity
/// @returns  true if set capacity sucess false otherwise
bool PIoInterface_Mem::SetCapacity(uint32_t const& nCapacity) const noexcept
{
    slotDataTemp_.SetKeySlotPrototypePropsCapacity(static_cast< size_t >(nCapacity));
    return true;
}
//********************************/
}  // namespace  isoft_def
}  // namespace keys
}  // namespace crypto
}  // namespace ara
