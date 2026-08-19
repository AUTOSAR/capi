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
/// @file       isoft_io_interface_kv_share.cpp
/// @brief      AutoSar-Crypto Common Encryption/Decryption Module
/// @details
/// @date       2022-04-12
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Key Manager/IO Interface
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_01008
/// @unit_name=PIoInterface_KvShare
/// @unit_description=Interface for saving and loading security objects
/// @endcode
///
/// ================================================================

#include "ara/crypto/ksp/isoft_io_interface_kv_share.h"

#include "ara/core/span.h"
#include "ara/core/vector.h"
#include "ara/crypto/common/isoft_log_api.h"
#include "ara/crypto/cryp/cryobj/isoft_key_symmetric.h"
#include "ara/crypto/cryp/isoft_crypto_provider.h"
#include "ara/log/log_stream.h"

namespace ara {
namespace crypto {
namespace keys {
namespace isoft_def {
//********************************/
/// @brief Formal interface of IOInterface for saving and loading security objects: an implementation version where all slots share the same KV store.
///         Actual saving and loading should be implemented through known internal methods of the trusted pair consisting of the crypto provider and storage provider.
///         Each object should be uniquely identified by its type and Crypto Object Unique Identifier (COUID). This interface assumes objects in the container are compressed, i.e., have a minimally optimized size.
/// @name  PIoInterface_KvShare
/// @param cryptoProvider Crypto provider
/// @param stStorageName Persistent storage name
PIoInterface_KvShare::PIoInterface_KvShare(ara::crypto::cryp::CryptoProvider &cryptoProvider,
                                           ara::core::StringView const &stStorageName) noexcept
    : PIoInterface{}  // NOLINT
    , cryptoProvider_{cryptoProvider}
    , stStorageName_{stStorageName}
{
}
/// @brief Default destructor
/// @name  ~PIoInterface_KvShare
/// @returns
PIoInterface_KvShare::~PIoInterface_KvShare() noexcept = default;
/// @brief Returns the actual allowed key/seed usage flags defined by the "Actor's" key slot prototype and the current content of the container.
///         Volatile containers have no prototype restrictions, but runtime restrictions can be defined for the current instance of the object.
///         The value returned by this method is the bitwise AND of the common usage flags defined at runtime and the usage flags defined by the UserPermissions prototype for the current "Actor".
///         This method is particularly useful for empty persistent prototype containers.
/// @name  GetAllowedUsage
/// @returns  a combination of bit-flags that specifies allowed applications of the object
AllowedUsageFlags PIoInterface_KvShare::GetAllowedUsage() const noexcept
{
    return slotData_.GetKeySlotContentProps().mContentAllowedUsage;
}
/// @brief Returns the capacity of the underlying resource.
/// @name  GetCapacity
/// @returns Capacity of the underlying resource
std::size_t PIoInterface_KvShare::GetCapacity() const noexcept
{
    return slotData_.GetKeySlotPrototypeProps().mSlotCapacity;
}
/// @brief Returns the cryptobjecttype of the object referenced by this IOInterface.
/// @name  GetCryptoObjectType
/// @returns  Cryptobjecttype of the referenced object
CryptoObjectType PIoInterface_KvShare::GetCryptoObjectType() const noexcept
{
    return slotData_.GetKeySlotContentProps().mObjectType;
}
/// @brief Returns the COUID of the object stored to the IOInterface.
///         If the container is empty, this method returns cryptobjecttype::KUndefined. Explicit identification of a crypto object requires two components: cryptobjectuid and cryptobjecttype.
/// @name  GetObjectId
/// @returns  COUID of the object stored to the IOInterface
CryptoObjectUid PIoInterface_KvShare::GetObjectId() const noexcept
{
    return slotData_.GetKeySlotContentProps().mObjectUid;
}
/// @brief Returns the size of the object payload stored in the underlying buffer of the IOInterface.
///         If the container is empty, this method returns 0. The return value does not consider the object's meta-information attributes, but their size is fixed and common for all crypto objects regardless of their actual type.
///         Automatic space is provided for the object's meta-information according to implementation details.
/// @name  GetPayloadSize
/// @returns  Object payload size
std::size_t PIoInterface_KvShare::GetPayloadSize() const noexcept
{
    return slotData_.GetKeySlotContentProps().mObjectSize;
}
/// @brief Gets the vendor-specific ID of the primitive.
/// @name  GetPrimitiveId
/// @returns  Vendor-specific ID of the primitive
CryptoAlgId PIoInterface_KvShare::GetPrimitiveId() const noexcept { return slotData_.GetKeySlotContentProps().mAlgId; }
/// @brief Returns the content type restriction of this interface (identifies what type of data this container can hold).
/// @return Interface content type restriction
CryptoObjectType PIoInterface_KvShare::GetTypeRestriction() const noexcept
{
    if (IsContentTypeChange()) {
        return CryptoObjectType::kUndefined;
    }
    return slotData_.GetKeySlotPrototypeProps().mObjectType;
}
/// @brief Returns the "exportable" attribute of the object stored in the container. An object's exportability does not depend on the volatility of its container.
/// @name  IsObjectExportable
/// @returns  true if can export false otherwise
bool PIoInterface_KvShare::IsObjectExportable() const noexcept
{
    return slotData_.GetKeySlotPrototypeProps().mExportAllowed;
}
/// @brief Returns the "session" (or "temporary") attribute set for the object, e.g., keyderivationfunctionctx::DeriveKey().
///         A "session" object can only be stored in a VolatileTrustedContainer! If this IOInterface is linked to a KeySlot, it always returns false.
/// @name  IsObjectSession
/// @returns  true if is session object false otherwise
inline bool PIoInterface_KvShare::IsObjectSession() const noexcept { return bObjectSession_; }
/// @brief Returns the "volatile" attribute of the underlying buffer of this IOInterface.
///         "Session" objects can only be stored in "volatile" containers. The content of a "volatile" container is destroyed along with the interface instance.
/// @name  IsVolatile
/// @returns  true if is volatile object false otherwise
inline bool PIoInterface_KvShare::IsVolatile() const noexcept { return bVolatile_; }
/// @brief Checks if the underlying keystore is valid. If the underlying resource is modified after the IOInterface is opened, the IOInterface becomes invalid.
/// @name  IsValid
/// @returns true if is Valid object false otherwise
bool PIoInterface_KvShare::IsValid() const noexcept
{
    /// @returns true if the underlying resource can be valid, false otherwise
    uint32_t const nSaveWriteIndex{_GetSaveWriteIndex()};
    return nSaveWriteIndex == slotData_.GetWriteIndex();
}
/// @brief Checks if the underlying KeySlot is writable. If this IOInterface is linked to a volatile trusted container, it always returns true.
/// @name  IsWritable
/// @returns  true if is Writable object false otherwise
bool PIoInterface_KvShare::IsWritable() const noexcept
{
    int32_t const nMaxUpdateAllowed{GetMaxUpdateAllowed()};
    if (nMaxUpdateAllowed < 0) {
        return true;  // -1 means always writable
    }
    if (0 == nMaxUpdateAllowed) {
        if (IsEmpty()) {
            return true;
        }
        return false;
    }
    return slotData_.GetWriteIndex() < static_cast< uint32_t >(nMaxUpdateAllowed);
}
//***************/
/// @brief Indicates whether the content of this key slot can be changed, e.g., changing from storing a symmetric key to storing an RSA key.
/// @name  IsContentTypeChange
/// @returns  true if the keyslot content can change false otherwise
bool PIoInterface_KvShare::IsContentTypeChange() const noexcept
{
    return slotData_.GetKeySlotPrototypeProps().mAllowContentTypeChange;
}
/// @brief Indicates whether FC encryption should allocate sufficient storage space for the shadow copy of this key string.
/// @name  IsAllocateSpareSlot
/// @returns  true if the keyslot Allocate other Spare false otherwise
bool PIoInterface_KvShare::IsAllocateSpareSlot() const noexcept
{
    return slotData_.GetKeySlotPrototypeProps().mAllocateSpareSlot;
}
/// @brief Specifies how many times this key slot can be updated, e.g.:
///         0: Indicates the key-slot content will be preset during production;
///         1: Indicates the key-slot content can only be updated once ("OTP");
///        -1: Negative value indicates the key-slot content can be updated infinitely.
/// @name  GetMaxUpdateAllowed
/// @returns
/// 0: Indicates the key-slot content will be preset during production (modification/writing not allowed);
int32_t PIoInterface_KvShare::GetMaxUpdateAllowed() const noexcept
{
    ///         0: Indicates the key-slot content will be preset during production (modification/writing not allowed);
    ///         1: Indicates the key-slot content can only be updated once ("OTP");
    ///        -1: Negative value indicates the key-slot content can be updated infinitely.
    return slotData_.GetKeySlotPrototypeProps().mMaxUpdateAllowed;
}
/// @brief Key-slot type configuration
/// @name  GetSlotType
/// @returns  Key slot type
KeySlotType PIoInterface_KvShare::GetSlotType() const noexcept
{
    return slotData_.GetKeySlotPrototypeProps().mSlotType;
}
/// @brief Vendor-provided cryptographic primitive ID restricted by the key-slot.
/// @name  GetSlotPrimitiveId
/// @returns  Cryptographic primitive ID
CryptoAlgId PIoInterface_KvShare::GetSlotPrimitiveId() const noexcept
{
    return slotData_.GetKeySlotPrototypeProps().mAlgId;
}
/// @brief Usage scope identifier restricted by the key-slot. slotProps_
/// @name  GetSlotAllowedUsage
/// @returns  Usage scope identifier
AllowedUsageFlags PIoInterface_KvShare::GetSlotAllowedUsage() const noexcept
{
    return slotData_.GetKeySlotPrototypeProps().mContentAllowedUsage;
}
/// @brief Checks if there has been a write operation on the raw data of the key slot
/// @name  IsHaveWriteAction
/// @returns  true if can write false otherwise
bool PIoInterface_KvShare::IsHaveWriteAction() const noexcept
{
    uint32_t const nSaveWriteIndex{_GetSaveWriteIndex()};
    return slotData_.GetWriteIndex() > nSaveWriteIndex;
}
/// @brief Saves KeySlotContentProps
/// @name  SaveKeyContent
/// @param contentProps Key properties
/// @returns  true if save sucess false otherwise
bool PIoInterface_KvShare::SaveKeyContent(KeySlotContentProps const &contentProps) const noexcept
{
    slotData_.SetKeySlotContentProps(contentProps);
    return true;
}
/// @brief Saves KeySlotPrototypeProps
/// @name  SaveSlotProps
/// @param slotProps Key slot properties
/// @returns  true if save sucess false otherwise
bool PIoInterface_KvShare::SaveSlotProps(KeySlotPrototypeProps const &slotProps) const noexcept
{
    slotData_.SetKeySlotPrototypeProps(slotProps);
    return true;
}
/// @brief Sets GetName_KvWriteIndex()/GetName_KvWriteSave()
/// @name  SetWriteIndex
/// @returns  true if Set WriteIndex sucess false otherwise
bool PIoInterface_KvShare::SetWriteIndex() const noexcept
{
    slotData_.SetWriteIndex(slotData_.GetWriteIndex() + 1U);
    return true;
}
/// @brief Sets GetName_KvWriteSave() and commits persistent save
/// @name  CommitSaveAction
/// @returns  true if CommitSaveAction sucess false otherwise
bool PIoInterface_KvShare::CommitSaveAction() const noexcept
{
    // First synchronize write operation count
    uint32_t const nNeedLen{
        static_cast< uint32_t >(sizeof(PKeySlotData_InKv) + slotData_.GetKeyKeySize() + slotData_.GetKeyDataSize())};
    ara::core::Vector< uint8_t > vecData;
    vecData.resize(static_cast< std::size_t >(nNeedLen));
    uint8_t *pData{vecData.data()};
    PKeySlotData_InKv *const pKvData{static_cast< PKeySlotData_InKv * >(static_cast< void * >(pData))};
    pKvData->SetWriteIndex(slotData_.GetWriteIndex());
    pKvData->SetKeySlotContentProps(slotData_.GetKeySlotContentProps());
    pKvData->SetKeySlotPrototypeProps(slotData_.GetKeySlotPrototypeProps());
    pKvData->SetKeyAlgID(slotData_.GetKeyAlgID());
    pKvData->SetKeyKeyLen(static_cast< uint16_t >(slotData_.GetKeyKeySize()));
    pKvData->SetKeyDataLen(static_cast< uint16_t >(slotData_.GetKeyDataSize()));
    pData += sizeof(PKeySlotData_InKv);
    std::ignore = memcpy(pData, slotData_.GetKeyKeydata(), static_cast< std::size_t >(pKvData->GetKeyKeyLen()));
    pData += static_cast< int32_t >(pKvData->GetKeyKeyLen());
    std::ignore = memcpy(pData, slotData_.GetKeydata(), static_cast< std::size_t >(pKvData->GetKeyDataLen()));
    pData += static_cast< int32_t >(pKvData->GetKeyDataLen());
    ara::core::Result< void > const resVoid{pKvStorage_->SetValue(T_StringView(stKeySlotName_), vecData)};
    if (!resVoid.HasValue()) {
        ara::crypto::isoft_def::LogInfo() << "CommitSaveAction SetValue failed";
        return false;
    }

    // Store all data into specific fields of the KV store
    ara::core::Result< void > const result{pKvStorage_->SyncToStorage()};

    return result.HasValue();
}
/// @brief Rolls back possible save operations
/// @name  RollbackSaveAction
/// @returns  true if RollbackSaveAction sucess false otherwise
bool PIoInterface_KvShare::RollbackSaveAction() const noexcept
{
    if (false == pKvStorage_.operator bool()) {
        return false;
    }
    std::ignore = _InitLoadSlotData();
    return true;
}

/// @brief Initializes the IO interface
/// @name  InitIoInterface
/// @param stSlotName Key slot name
/// @returns  true if Init IoInterface sucess false otherwise
bool PIoInterface_KvShare::InitIoInterface(ara::core::StringView const &stSlotName) noexcept
{
    ara::core::InstanceSpecifier const insKv{T_StringView(stStorageName_)};

    ara::core::Result< ara::per::SharedHandle< ara::per::KeyValueStorage > > const resultOpen{
        ara::per::OpenKeyValueStorage(insKv)};
    if (false == resultOpen.HasValue()) {
        ara::crypto::isoft_def::LogWarn() << "OpenKeyValueStorage failed";
        return false;
    }
    pKvStorage_    = resultOpen.Value();
    stKeySlotName_ = stSlotName;
    if (!_InitLoadSlotData()) {
        ara::crypto::isoft_def::LogInfo() << "InitLoadSlotData failed";
    }
    return true;
}
/// @brief Gets the current key slot name
/// @name  GetKeySlotName
/// @returns  Key slot name
inline ara::core::StringView PIoInterface_KvShare::GetKeySlotName() const noexcept
{
    return T_StringView(stKeySlotName_);
}
/// @brief Checks if the IO interface is initialized
/// @name  IsInterfaceValid
/// @returns  bool
bool PIoInterface_KvShare::IsInterfaceValid() const noexcept { return pKvStorage_.operator bool(); }
/// @brief Clears data
/// @name  Clear
/// @returns  true if clear sucess false otherwise
bool PIoInterface_KvShare::Clear() noexcept
{
    if (false == pKvStorage_.operator bool()) {
        return false;
    }

    // 2022-12-26 Deleting all Keys is not allowed
    std::ignore = pKvStorage_->ResetKey(T_StringView(stKeySlotName_));
    return true;
}
/// @brief Checks if the slot is empty.
/// @name  IsEmpty
/// @returns  true if id empty false otherwise
bool PIoInterface_KvShare::IsEmpty() const noexcept
{
    if (false == pKvStorage_.operator bool()) {
        return false;
    }

    ara::core::Result< bool > const resultExist{pKvStorage_->KeyExists(T_StringView(stKeySlotName_))};

    if (false == resultExist.HasValue()) {
        return false;
    }
    if (false == resultExist.Value()) {
        return true;
    }
    // Should we further check the length of the current KeyData?
    return slotData_.GetKeyDataSize() <= 0U;
}
/// @brief Reads the encryption algorithm information of the key data
/// @param nAlgID Encryption primitive ID
/// @param vecData Vector object receiving the KeyKey data
/// @name  ReadKeyKey
/// @returns  true if read keykey sucess false otherwise
bool PIoInterface_KvShare::ReadKeyKey(CryptoAlgId &nAlgID, ara::core::Vector< uint8_t > &vecData) const noexcept
{
    if (false == pKvStorage_.operator bool()) {
        return false;
    }
    if (nAlgID > 0U) {
    }  // for qac
    nAlgID = slotData_.GetKeyAlgID();
    internal::PAutoBuff const &keyDataBuff{*slotData_.GetPAutoBuff()};
    for (uint32_t i{0U}; i < keyDataBuff.size(); ++i) {
        vecData.push_back(keyDataBuff.at(i));
    }
    return true;
}
/// @brief Reads key data
/// @param vecData Vector object receiving the Key data
/// @name  ReadKeyData
/// @returns  true if read keydata sucess false otherwise
bool PIoInterface_KvShare::ReadKeyData(ara::core::Vector< uint8_t > &vecData) const noexcept
{
    if (false == pKvStorage_.operator bool()) {
        return false;
    }
    CryptoAlgId const nAlgID{slotData_.GetKeyAlgID()};
    vecData.clear();
    if (kAlgIdAny == nAlgID) {
        for (uint32_t i{0U}; i < slotData_.GetKeyDataSize(); ++i) {
            vecData.push_back(slotData_.GetPAutoBuff()->at(i));
        }
    } else  // Here, data is decrypted according to KeyAlgID
    {
        ReadOnlyMemRegion const memKeyData{slotData_.GetKeydata(),
                                           static_cast< std::size_t >(slotData_.GetKeyDataSize())};
        if (false == _SymmetricKeyData(vecData, memKeyData, nAlgID, CryptoTransform::kDecrypt)) {
            return false;
        }
    }
    return true;
}
/// @brief Saves key data
/// @param memKeyData Key data in memory
/// @param nAlgID Encryption primitive ID
/// @param memKeyKey Key used to encrypt the key
/// @name  SaveKeyData
/// @returns  true if save keydata sucess false otherwise
bool PIoInterface_KvShare::SaveKeyData(ReadOnlyMemRegion const &memKeyData,
                                       CryptoAlgId nAlgID,
                                       ReadOnlyMemRegion const &memKeyKey) const noexcept
{
    std::size_t const memLen{memKeyData.size()};
    if ((slotData_.GetKeySlotContentProps().mObjectType == CryptoObjectType::kSymmetricKey)
        || (slotData_.GetKeySlotContentProps().mObjectType == CryptoObjectType::kSecretSeed)) {
        uint64_t nNeedLen{0U};
        ara::core::Result< uint64_t > const resMemData{cryptoProvider_.GetPayloadStorageSize(
            slotData_.GetKeySlotContentProps().mObjectType, slotData_.GetKeySlotContentProps().mAlgId)};
        if (resMemData.HasValue()) {
            nNeedLen = resMemData.Value();
        }
        /// Symmetric key length grows when wrapped (aes increases by 8U, des by 16U)
        if ((memLen == 0U)
            || ((memLen != nNeedLen) && (memLen != nNeedLen + kInt_8U) && (memLen != nNeedLen + kInt_16U))) {
            ara::crypto::isoft_def::LogWarn() << "Invalid SymmetricKeyData len:" << memLen;
            return false;
        }
    }

    if ((slotData_.GetKeySlotContentProps().mObjectType == CryptoObjectType::kPrivateKey)
        || (slotData_.GetKeySlotContentProps().mObjectType == CryptoObjectType::kPublicKey)) {
        uint64_t nDerLen{0U};
        uint64_t nPemLen{0U};
        ara::core::Result< uint64_t > resMemData{cryptoProvider_.GetSerializedSize(
            slotData_.GetKeySlotContentProps().mObjectType, slotData_.GetKeySlotContentProps().mAlgId,
            Serializable::kFormatPemEncoded)};
        if (resMemData.HasValue()) {
            nPemLen = resMemData.Value();
        }
        resMemData = cryptoProvider_.GetSerializedSize(slotData_.GetKeySlotContentProps().mObjectType,
                                                       slotData_.GetKeySlotContentProps().mAlgId,
                                                       Serializable::kFormatDerEncoded);
        if (resMemData.HasValue()) {
            nDerLen = resMemData.Value();
        }
        /// rsa format private key is in range [len, len+5]
        if ((memLen == 0U)
            || (((memLen < nPemLen) || (memLen > nPemLen + kInt_30U))
                && ((memLen < nDerLen) || (memLen > nDerLen + kInt_25U)))) {
            ara::crypto::isoft_def::LogWarn() << "Invalid AymmetricKeyData len:" << memLen;
            return false;
        }
    }

    if (false == pKvStorage_.operator bool()) {
        return false;
    }
    if (memKeyData.empty()) {
        return false;
    }
    // Save the encrypted material of the key
    slotData_.SetKeyAlgID(static_cast< uint32_t >(nAlgID));
    if (false == memKeyKey.empty()) {
        std::ignore = slotData_.SetKeyKeyData(memKeyKey.data(), static_cast< uint32_t >(memKeyKey.size()), false);
    } else {
        slotData_.ResetKeyKeyData();
    }
    // Save key data
    if (kAlgIdAny == nAlgID) {
        std::ignore = slotData_.SetKeyData(memKeyData.data(), static_cast< uint32_t >(memKeyData.size()));
    } else  // Here, data is encrypted according to KeyAlgID
    {
        ara::core::Vector< uint8_t > vecData;
        if (false == _SymmetricKeyData(vecData, memKeyData, nAlgID, CryptoTransform::kEncrypt)) {
            return false;
        }
        uint8_t *const pData{vecData.data()};
        // Save the encrypted key ciphertext
        std::ignore = slotData_.SetKeyData(pData, static_cast< uint32_t >(vecData.size()));
    }
    return true;
}
/// @brief Checks if there is a deletion flag
/// @name  GetDeleteFlag
/// @returns  true if get DeleteFlag sucess false otherwise
bool PIoInterface_KvShare::GetDeleteFlag() const noexcept { return (slotData_.GetDeleteFlag() != 0U); }
/// @brief Sets the deletion flag
/// @param nDelProcessID Deletion flag
/// @name  SetDeleteFlag
void PIoInterface_KvShare::SetDeleteFlag(uint32_t nDelProcessID) const noexcept
{
    slotData_.SetDeleteFlag(nDelProcessID);
}
/// @brief Resets key slot property information
/// @param slotProps Key slot properties
/// @return  success/failed
bool PIoInterface_KvShare::ResetKeySlot(KeySlotPrototypeProps const &slotProps) const noexcept
{
    slotData_.Reset();
    slotData_.SetKeySlotPrototypeProps(slotProps);
    slotData_.SetWriteIndex(0U);
    return CommitSaveAction();
}
//********************************/
/// @brief Reads value from KV store
/// @tparam T_Data
template < typename T_Data >
/// @brief Reads value from KV store
/// @name  _GetKvStorageData
/// @param stName KEY name within the KV store
/// @param nDef Default value
/// @returns  TKV store value
T_Data PIoInterface_KvShare::_GetKvStorageData(ara::core::StringView const &stName, T_Data const nDef) const noexcept
{
    if (false == pKvStorage_.operator bool()) {
        return nDef;
    }
    if (false == pKvStorage_->KeyExists(stName)) {
        return nDef;
    }
    ara::core::Result< T_Data > const resultKv{std::move(pKvStorage_->GetValue< T_Data >(stName))};
    if (false == resultKv.HasValue()) {
        return nDef;
    }
    return resultKv.Value();
}
/// @brief Sets value in the KV store
/// @tparam T_Data
/// @param stName KEY name within the KV store
/// @param nData Integer data
/// @return true if set kv storage sucess false otherwise
template < typename T_Data >
bool PIoInterface_KvShare::_SetKvStorageData(ara::core::StringView const &stName, T_Data const nData) const noexcept
{
    if (false == pKvStorage_.operator bool()) {
        return false;
    }
    ara::core::Result< void > const resultKv{std::move(pKvStorage_->SetValue< T_Data >(stName, nData))};
    if (false == resultKv.HasValue()) {
        return false;
    }
    return true;
}
/// @brief Encrypts/decrypts key data
/// @param vecData Input plaintext/ciphertext, output ciphertext/plaintext
/// @param memKeyData Key data in memory
/// @param nAlgID Encryption primitive ID
/// @param eTransform Encryption direction: encrypt or decrypt
/// @name  _SymmetricKeyData
/// @returns  true for success, false for failure
bool PIoInterface_KvShare::_SymmetricKeyData(ara::core::Vector< uint8_t > &vecData,
                                             ReadOnlyMemRegion const &memKeyData,
                                             CryptoAlgId const nAlgID,
                                             CryptoTransform const eTransform) const noexcept
{
    // Load the key used to encrypt key data
    cryp::SymmetricKey::Uptrc const pSymmetricKey{_LoadKeyKey(nAlgID)};
    if (false == pSymmetricKey.operator bool()) {
        return false;
    }
    // Generate symmetric encryption context
    ara::core::Result< ara::crypto::cryp::SymmetricBlockCipherCtx::Uptr > const resultCtxSymmetric{
        cryptoProvider_.CreateSymmetricBlockCipherCtx(nAlgID)};
    if (false == resultCtxSymmetric.HasValue()) {
        return false;
    }
    // Set key, perform encryption/decryption action
    ara::core::Result< void > const resultSetKey{
        resultCtxSymmetric.Value()->SetKey(*(pSymmetricKey.get()), eTransform)};
    if (resultSetKey.HasValue() == false) {
        return false;
    }
    ara::core::Result< ara::core::Vector< ara::core::Byte > > const resultDecrypt{
        resultCtxSymmetric.Value()->ProcessBlocks(memKeyData)};
    if (false == resultDecrypt.HasValue()) {
        return false;
    }
    // Dump data
    vecData.clear();
    ara::core::Vector< ara::core::Byte > const &vecDecrypt{resultDecrypt.Value()};
    for (size_t i{0U}; i < vecDecrypt.size(); i++) {
        vecData.push_back(vecDecrypt[i].operator u_char());
    }

    return true;
}
/// @brief Loads the key encryption key
/// @param nAlgID Encryption primitive ID
/// @name  _LoadKeyKey
/// @returns  SymmetricKey instance
cryp::SymmetricKey::Uptrc PIoInterface_KvShare::_LoadKeyKey(CryptoAlgId const nAlgID) const noexcept
{
    if (false == pKvStorage_.operator bool()) {
        return {nullptr};
    }
    ReadOnlyMemRegion const memKeyKey{slotData_.GetKeyKeydata(), static_cast< std::size_t >(slotData_.GetKeyKeySize())};

    ara::core::Result< ara::crypto::cryp::SymmetricKey::Uptrc > resultSymmetricKey{
        cryp::isoft_def::PCryptoProvider::GenerateSymmetricKeyEx(
            nAlgID, memKeyKey, kAllowDataEncryption | kAllowDataDecryption, true, false)};
    if (false == resultSymmetricKey.HasValue()) {
        return {nullptr};
    }
    return std::move(resultSymmetricKey).Value();
}
/// @brief Loads key information from persistence
/// @return
/// @name  _InitLoadSlotData
/// @returns  true if init kv sucess false otherwise
bool PIoInterface_KvShare::_InitLoadSlotData() const noexcept
{
    slotData_.Reset();
    if (false == pKvStorage_.operator bool()) {
        return false;
    }
    ara::core::Result< ara::core::Vector< uint8_t > > const resultData{
        pKvStorage_->GetValue< ara::core::Vector< uint8_t > >(T_StringView(stKeySlotName_))};
    if (false == resultData.HasValue()) {
        ara::crypto::isoft_def::LogWarn() << "InitLoadSlotData GetValue failed, KeySlot = " << stKeySlotName_.c_str();
        return false;
    }
    ara::core::Vector< uint8_t > const &vecData{resultData.Value()};
    if (vecData.empty()) {
        ara::crypto::isoft_def::LogInfo() << "GetValue Null";
        return false;
    }
    uint8_t const *pData{vecData.data()};
    PKeySlotData_InKv const *const pKvData{
        static_cast< PKeySlotData_InKv const * >(static_cast< void const * >(pData))};
    (*(static_cast< PKeySlotData_InKv * >(&slotData_))).CopyToMySelf(*pKvData);

    pData += sizeof(PKeySlotData_InKv);
    std::ignore
        = slotData_.SetKeyKeyData(pData, static_cast< uint32_t >(pKvData->GetKeyKeyLen()));  // Key encryption key
    pData += static_cast< int32_t >(pKvData->GetKeyKeyLen());
    std::ignore = slotData_.SetKeyData(pData, static_cast< uint32_t >(pKvData->GetKeyDataLen()));  // Key content
    return true;
}
/// @brief Returns the SaveIndex saved in the database
/// @return
/// @name  _GetSaveWriteIndex
/// @returns  SaveIndex saved in the database
uint32_t PIoInterface_KvShare::_GetSaveWriteIndex() const noexcept
{
    if (false == pKvStorage_.operator bool()) {
        return 0U;
    }
    ara::core::Result< ara::core::Vector< uint8_t > > const resultData{
        pKvStorage_->GetValue< ara::core::Vector< uint8_t > >(T_StringView(stKeySlotName_))};
    if (false == resultData.HasValue()) {
        return 0U;
    }
    ara::core::Vector< uint8_t > const &vecData{resultData.Value()};
    if (vecData.empty()) {
        return 0U;
    }
    uint8_t const *const pData{vecData.data()};
    PKeySlotData_InKv const *const pKvData{
        static_cast< PKeySlotData_InKv const * >(static_cast< void const * >(pData))};
    return pKvData->GetWriteIndex();
}
//***************/
/// @brief Template declaration _GetKvStorageData
/// @param stName KEY name within the KV store
/// @param nDef Default value
/// @return
template uint32_t PIoInterface_KvShare::_GetKvStorageData(ara::core::StringView const &stName,
                                                          uint32_t nDef) const noexcept;
/// @brief Template declaration _GetKvStorageData
/// @param stName KEY name within the KV store
/// @param nDef Default value
/// @return
template uint64_t PIoInterface_KvShare::_GetKvStorageData(ara::core::StringView const &stName,
                                                          uint64_t nDef) const noexcept;
/// @brief Template declaration _GetKvStorageData
/// @param stName KEY name within the KV store
/// @param nDef Default value
/// @return
template int32_t PIoInterface_KvShare::_GetKvStorageData(ara::core::StringView const &stName,
                                                         int32_t nDef) const noexcept;
/// @brief Template declaration _SetKvStorageData
/// @param stName KEY name within the KV store
/// @param nData Integer data
/// @return
template bool PIoInterface_KvShare::_SetKvStorageData(ara::core::StringView const &stName,
                                                      uint32_t nData) const noexcept;
/// @brief Template declaration _SetKvStorageData
/// @param stName KEY name within the KV store
/// @param nData Integer data
/// @return
template bool PIoInterface_KvShare::_SetKvStorageData(ara::core::StringView const &stName,
                                                      uint64_t nData) const noexcept;
/// @brief Template declaration _SetKvStorageData
/// @param stName KEY name within the KV store
/// @param nData Integer data
/// @return
template bool PIoInterface_KvShare::_SetKvStorageData(ara::core::StringView const &stName,
                                                      int32_t nData) const noexcept;
//********************************/
}  // namespace  isoft_def
}  // namespace keys
}  // namespace crypto
}  // namespace ara
