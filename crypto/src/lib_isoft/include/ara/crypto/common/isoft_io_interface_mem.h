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
/// @file       isoft_io_interface_mem.h
/// @brief      AutoSar-AP encryption and decryption common module
/// @details    Formal interface of IOInterface, used to save and load security objects: implementation version of KV database.
/// @date       2022-04-12
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Reusable Functions/Reusable Functions Module
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=Common_api
/// @unit_description=Key slot data: KV database
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_KEYS_PUHUA_IO_INTERFACE_MEM_H_
#define ARA_CRYPTO_KEYS_PUHUA_IO_INTERFACE_MEM_H_

#include <cstring>

#include "ara/crypto/common/isoft_data_type.h"
#include "ara/crypto/common/isoft_io_interface.h"
#include "ara/crypto/cryp/isoft_auto_buff.h"

namespace ara {
namespace crypto {
namespace keys {
namespace isoft_def {
//********************************/
/// @brief Formal interface of IOInterface, used to save and load security objects.
///       The actual saving and loading should be implemented through internal methods known to the trusted pair of crypto provider and storage provider.
///       Each object should be uniquely identified by its type and Crypto Object Unique Identifier (COUID). This interface assumes that objects in the container are compressed, i.e., have a minimal optimized size.
//********************************/
/// @brief Key slot data: KV database
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00001
/// @trace_id_dd=DD_CRYPTO_00001
/// @needwork = ad
/// @endcode
class PKeySlotData_InKv
{
public:
    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00002
    /// @needwork = dda
    /// @endcode
    PKeySlotData_InKv() = default;
    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00003
    /// @needwork = dda
    /// @endcode
    virtual ~PKeySlotData_InKv() = default;
    /// @brief Constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00004
    /// @needwork = dda
    /// @endcode
    PKeySlotData_InKv(PKeySlotData_InKv const& other) = delete;
    /// @brief Constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00005
    /// @needwork = dda
    /// @endcode
    PKeySlotData_InKv(PKeySlotData_InKv&& other) = delete;
    /// @brief Default copy assignment operator
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00006
    /// @needwork = dda
    /// @endcode
    PKeySlotData_InKv& operator=(PKeySlotData_InKv const& other) = delete;
    /// @brief Default move assignment operator
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00007
    /// @needwork = dda
    /// @endcode
    PKeySlotData_InKv& operator=(PKeySlotData_InKv&& other) = delete;

private:
    /// @brief Save sequence number
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00008
    /// @needwork = dda
    /// @endcode
    uint32_t nWriteIndex_{0};
    /// @brief Attributes of key slot content
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00009
    /// @needwork = dda
    /// @endcode
    KeySlotContentProps keyCont_;
    /// @brief Attributes of the key slot itself
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00010
    /// @needwork = dda
    /// @endcode
    KeySlotPrototypeProps slotProps_;
    /// @brief Key encryption algorithm ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00011
    /// @needwork = dda
    /// @endcode
    uint32_t nKeyAlgID_{0};
    /// @brief Delete flag: stores the process ID of the delete operation
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00012
    /// @needwork = dda
    /// @endcode
    uint32_t nDeleteFlag_{0};
    /// @brief Reserved item
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00013
    /// @needwork = dda
    /// @endcode
    uint32_t nReserver_[kInt_8U]{0, 0, 0, 0, 0, 0, 0, 0};
    /// @brief KeyKey length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00014
    /// @needwork = dda
    /// @endcode
    uint16_t nKeyKeyLen_{0};
    /// @brief Key data length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00015
    /// @needwork = dda
    /// @endcode
    uint16_t nKeyDataLen_{0};
    /// @brief Key slot ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00016
    /// @needwork = dda
    /// @endcode
    uint8_t nSlotID_{0};
    /// @brief Key slot group ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00017
    /// @needwork = dda
    /// @endcode
    uint8_t nGroupID_{0};
    /// @brief Key slot catalog ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00018
    /// @needwork = dda
    /// @endcode
    uint8_t nCatalogID_{0U};

public:
    /// @brief Get the save sequence number
    /// @return Write count
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00019
    /// @needwork = dda
    /// @endcode
    uint32_t GetWriteIndex() const noexcept { return nWriteIndex_; }
    /// @brief Set the save sequence number
    /// @param nWriteIndex Save sequence number
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00020
    /// @needwork = dda
    /// @endcode
    void SetWriteIndex(uint32_t const nWriteIndex) noexcept { nWriteIndex_ = nWriteIndex; }
    /// @brief Get the attributes of key slot content
    /// @return Key slot content attribute information
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00021
    /// @needwork = dda
    /// @endcode
    KeySlotContentProps GetKeySlotContentProps() const noexcept { return keyCont_; }
    /// @brief Set the attributes of key slot content
    /// @param nKeySlotContentProps Attributes of key slot content
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00022
    /// @needwork = dda
    /// @endcode
    void SetKeySlotContentProps(KeySlotContentProps const& nKeySlotContentProps) noexcept
    {
        keyCont_ = nKeySlotContentProps;
    }
    /// @brief Get the attributes of the key slot itself
    /// @return Key slot own attribute information
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00023
    /// @needwork = dda
    /// @endcode
    KeySlotPrototypeProps GetKeySlotPrototypeProps() const noexcept { return slotProps_; }
    /// @brief Set the attributes of the key slot itself
    /// @param nKeySlotPrototypeProps Attributes of the key slot itself
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00024
    /// @needwork = dda
    /// @endcode
    void SetKeySlotPrototypeProps(KeySlotPrototypeProps const& nKeySlotPrototypeProps) noexcept
    {
        slotProps_ = nKeySlotPrototypeProps;
    }
    /// @brief Set the storage capacity upper limit of the key slot
    /// @param nCapacity Maximum storage capacity threshold
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00025
    /// @needwork = dda
    /// @endcode
    void SetKeySlotPrototypePropsCapacity(size_t const nCapacity) noexcept { slotProps_.mSlotCapacity = nCapacity; }
    /// @brief Get the key encryption algorithm ID
    /// @return Encryption algorithm ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00026
    /// @needwork = dda
    /// @endcode
    uint32_t GetKeyAlgID() const noexcept { return nKeyAlgID_; }
    /// @brief Set the key encryption algorithm ID
    /// @param nKeyAlgID Key encryption algorithm ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00027
    /// @needwork = dda
    /// @endcode
    void SetKeyAlgID(uint32_t const nKeyAlgID) noexcept { nKeyAlgID_ = nKeyAlgID; }
    /// @brief Get the delete flag
    /// @return Delete flag
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00028
    /// @needwork = dda
    /// @endcode
    uint32_t GetDeleteFlag() const noexcept { return nDeleteFlag_; }
    /// @brief Set the delete flag
    /// @param nDeleteFlag Delete identifier
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00029
    /// @needwork = dda
    /// @endcode
    void SetDeleteFlag(uint32_t const nDeleteFlag) noexcept { nDeleteFlag_ = nDeleteFlag; }
    /// @brief Get the KeyKey length
    /// @return KeyKey length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00030
    /// @needwork = dda
    /// @endcode
    uint16_t GetKeyKeyLen() const noexcept { return nKeyKeyLen_; }
    /// @brief Set the KeyKey length
    /// @param nKeyKeyLen KeyKey length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00031
    /// @needwork = dda
    /// @endcode
    void SetKeyKeyLen(uint16_t const nKeyKeyLen) noexcept { nKeyKeyLen_ = nKeyKeyLen; }
    /// @brief Get the key data length
    /// @return Key data length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00032
    /// @needwork = dda
    /// @endcode
    uint16_t GetKeyDataLen() const noexcept { return nKeyDataLen_; }
    /// @brief Set the key data length
    /// @param nKeyDataLen Key data length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00033
    /// @needwork = dda
    /// @endcode
    void SetKeyDataLen(uint16_t const nKeyDataLen) noexcept { nKeyDataLen_ = nKeyDataLen; }
    /// @brief Get the key slot ID
    /// @return Key slot ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00034
    /// @needwork = dda
    /// @endcode
    uint8_t GetSlotID() const noexcept { return nSlotID_; }
    /// @brief Set the key slot ID
    /// @param nSlotID Key slot ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00035
    /// @needwork = dda
    /// @endcode
    void SetSlotID(uint8_t const nSlotID) noexcept { nSlotID_ = nSlotID; }
    /// @brief Get the key slot group ID
    /// @return Group ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00036
    /// @needwork = dda
    /// @endcode
    uint8_t GetGroupID() const noexcept { return nGroupID_; }
    /// @brief Set the key slot group ID
    /// @param nGroupID Group ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00037
    /// @needwork = dda
    /// @endcode
    void SetGroupID(uint8_t const nGroupID) noexcept { nGroupID_ = nGroupID; }
    /// @brief Get the key slot catalog ID
    /// @return Catalog ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00038
    /// @needwork = dda
    /// @endcode
    uint8_t GetCatalogID() const noexcept { return nCatalogID_; }
    /// @brief Set the key slot catalog ID
    /// @param nCatalogID Catalog ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00039
    /// @needwork = dda
    /// @endcode
    void SetCatalogID(uint8_t const nCatalogID) noexcept { nCatalogID_ = nCatalogID; }
    /// @brief Reset the reserved item data to 0
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00040
    /// @needwork = dda
    /// @endcode
    void ResetReserver() noexcept { std::ignore = memset(static_cast< void* >(nReserver_), 0, sizeof(nReserver_)); }
    /// @brief Copy the given PKeySlotData_InKv object
    /// @param pKeySlotDataInKv
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00041
    /// @needwork = dda
    /// @endcode
    void CopyToMySelf(PKeySlotData_InKv const& pKeySlotDataInKv) noexcept
    {
        if (this != &pKeySlotDataInKv) {
            /// Save sequence number
            SetWriteIndex(pKeySlotDataInKv.GetWriteIndex());
            /// Save attributes of key slot content
            SetKeySlotContentProps(pKeySlotDataInKv.GetKeySlotContentProps());
            /// Save attributes of the key slot itself
            SetKeySlotPrototypeProps(pKeySlotDataInKv.GetKeySlotPrototypeProps());
            /// Save key encryption algorithm ID
            SetKeyAlgID(pKeySlotDataInKv.GetKeyAlgID());
            /// Save delete flag: stores the process ID of the delete operation
            SetDeleteFlag(pKeySlotDataInKv.GetDeleteFlag());
            /// Save nKeyKeyLen_
            SetKeyKeyLen(pKeySlotDataInKv.GetKeyKeyLen());
            /// Save nKeyDataLen_
            SetKeyDataLen(pKeySlotDataInKv.GetKeyDataLen());
            /// Save nSlotID_
            SetSlotID(pKeySlotDataInKv.GetSlotID());
            /// Save nGroupID_
            SetGroupID(pKeySlotDataInKv.GetGroupID());
            /// Save nCatalogID_
            SetCatalogID(pKeySlotDataInKv.GetCatalogID());
        }
    }
    /// @brief Refactor KeySlotContentProps
    /// @code{.isoft}
    /// @interface_level=software
    /// @needwork = dda
    /// @endcode
    inline void ResetKeySlotContentProps() noexcept
    {
        keyCont_.mAlgId                            = kAlgIdUndefined;
        keyCont_.mObjectSize                       = 0U;
        keyCont_.mObjectType                       = CryptoObjectType::kUndefined;
        keyCont_.mObjectUid.mGeneratorUid.mQwordMs = 0U;
        keyCont_.mObjectUid.mGeneratorUid.mQwordLs = 0U;
        keyCont_.mObjectUid.mVersionStamp          = 0U;
        keyCont_.mContentAllowedUsage              = kAllowPrototypedOnly;
    }
    /// @brief Reset all attributes
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02757
    /// @trace_id_dd=DD_CRYPTO_05615
    /// @needwork = ad
    /// @endcode
    inline void ResetKeySlotPrototypeProps() noexcept
    {
        slotProps_.mAlgId                  = kAlgIdUndefined;
        slotProps_.mAllocateSpareSlot      = false;
        slotProps_.mAllowContentTypeChange = false;
        slotProps_.mContentAllowedUsage    = kAllowPrototypedOnly;
        slotProps_.mExportAllowed          = false;
        slotProps_.mMaxUpdateAllowed       = 0;
        slotProps_.mSlotType               = KeySlotType::kApplication;
        slotProps_.mSlotCapacity           = 0U;
        slotProps_.mObjectType             = CryptoObjectType::kUndefined;
    }
};
/// @brief Key slot data: memory
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00002
/// @trace_id_dd=DD_CRYPTO_00042
/// @needwork = ad
/// @endcode
class PKeySlotData_Mem final : public PKeySlotData_InKv
{
public:
    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00043
    /// @needwork = dda
    /// @endcode
    PKeySlotData_Mem() = default;
    /// @brief Default destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00044
    /// @needwork = dda
    /// @endcode
    ~PKeySlotData_Mem() final = default;
    /// @brief Constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00045
    /// @needwork = dda
    /// @endcode
    PKeySlotData_Mem(PKeySlotData_Mem const& other) = delete;
    /// @brief Constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00046
    /// @needwork = dda
    /// @endcode
    PKeySlotData_Mem(PKeySlotData_Mem&& other) = delete;
    /// @brief Default move assignment operator
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00047
    /// @needwork = dda
    /// @endcode
    PKeySlotData_Mem& operator=(PKeySlotData_Mem&& other) = delete;
    /// @brief Overloaded = operator
    /// @param slotData Mem type key data
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00048
    /// @needwork = dda
    /// @endcode
    PKeySlotData_Mem& operator=(PKeySlotData_Mem const& slotData) & noexcept
    {
        if (this != &slotData) {
            SetWriteIndex(slotData.GetWriteIndex());
            SetKeySlotContentProps(slotData.GetKeySlotContentProps());
            SetKeySlotPrototypeProps(slotData.GetKeySlotPrototypeProps());
            std::ignore = keyData_.SetData(slotData.keyData_.Data(0U), slotData.keyData_.size());
            SetKeyAlgID(slotData.GetKeyAlgID());
            std::ignore = keyKey_.SetData(slotData.GetKeyKeydata(), slotData.keyKey_.size());
            SetCatalogID(slotData.GetCatalogID());
            SetSlotID(slotData.GetSlotID());
            SetGroupID(slotData.GetGroupID());
        }
        return *this;
    }
    /// @brief Reset all data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00049
    /// @needwork = dda
    /// @endcode
    void Reset() noexcept;

private:
    /// @brief Key encryption key
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00050
    /// @needwork = dda
    /// @endcode
    internal::PAutoBuff keyKey_;
    /// @brief Key ciphertext
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00051
    /// @needwork = dda
    /// @endcode
    internal::PAutoBuff keyData_;

public:
    /// @brief Set KeyKey data
    /// @param pData Data: starting address of the memory
    /// @param nLen Data length: in bytes
    /// @param bForceNew Whether to force creation of new data
    /// @return true if set keykey sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00052
    /// @needwork = dda
    /// @endcode
    bool SetKeyKeyData(uint8_t const* const pData, uint32_t const nLen, bool const bForceNew = true) noexcept
    {
        return keyKey_.SetData(pData, nLen, bForceNew);
    }
    /// @brief Reset KeyKey data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00053
    /// @needwork = dda
    /// @endcode
    void ResetKeyKeyData() noexcept { keyKey_.ResetData(); }
    /// @brief GetKeyKeySize
    /// @return KeyKey size
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00054
    /// @needwork = dda
    /// @endcode
    uint32_t GetKeyKeySize() const noexcept { return keyKey_.size(); }
    /// @brief GetKeyKeydata
    /// @return Key data pointer instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00055
    /// @needwork = dda
    /// @endcode
    uint8_t const* GetKeyKeydata() const noexcept { return keyKey_.Data(); }
    /// @brief Set key data
    /// @param pData Data: starting address of the memory
    /// @param nLen Data length: in bytes
    /// @param bForceNew Whether to force creation of new data
    /// @return true if setkeydata sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00056
    /// @needwork = dda
    /// @endcode
    bool SetKeyData(uint8_t const* const pData, uint32_t const nLen, bool const bForceNew = true) noexcept
    {
        return keyData_.SetData(pData, nLen, bForceNew);
    }
    /// @brief Reset key data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00057
    /// @needwork = dda
    /// @endcode
    void ResetKeyData() noexcept { keyData_.ResetData(); }
    /// @brief GetKeyDataSize
    /// @return Key data size
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00058
    /// @needwork = dda
    /// @endcode
    uint32_t GetKeyDataSize() const noexcept { return keyData_.size(); }
    /// @brief GetKeydata
    /// @return Key data pointer instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00059
    /// @needwork = dda
    /// @endcode
    uint8_t const* GetKeydata() const noexcept { return keyData_.Data(); }
    /// @brief GetPAutoBuff
    /// @return Autobuff instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00060
    /// @needwork = dda
    /// @endcode
    internal::PAutoBuff const* GetPAutoBuff() const noexcept { return &keyData_; }
};
//********************************/
/// @brief Formal interface of OInterface: shared KV
class PIoInterface_KvShare;
/// @brief Formal interface of OInterface: memory
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00003
/// @trace_id_dd=DD_CRYPTO_00061
/// @needwork = ad
/// @endcode
class PIoInterface_Mem : public PIoInterface
{
public:
    /// @brief Unique smart pointer of the interface.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00003
    /// @trace_id_dd=DD_CRYPTO_06236
    /// @needwork = dd
    /// @endcode
    using Uptr = std::unique_ptr< PIoInterface_Mem >;

public:
    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00062
    /// @needwork = dda
    /// @endcode
    PIoInterface_Mem() = delete;
    /// @brief Parameterized constructor
    /// @param bVolatile Whether it is volatile
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00063
    /// @needwork = dda
    /// @endcode
    explicit PIoInterface_Mem(bool const bVolatile) noexcept;
    /// @brief Default virtual destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00064
    /// @needwork = dda
    /// @endcode
    ~PIoInterface_Mem() noexcept override = default;

public:
    /// @brief Default copy assignment operator
    /// @param other Another object instance of this class
    /// @returns *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00065
    /// @needwork = dda
    /// @endcode
    PIoInterface_Mem& operator=(PIoInterface_Mem const& other) noexcept = delete;
    /// @brief Default move assignment operator
    /// @param other Another object instance of this class
    /// @returns *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00066
    /// @needwork = dda
    /// @endcode
    PIoInterface_Mem& operator=(PIoInterface_Mem&& other) noexcept = delete;
    /// @brief Default copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00067
    /// @needwork = dda
    /// @endcode
    PIoInterface_Mem(PIoInterface_Mem const& other) = delete;
    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00068
    /// @needwork = dda
    /// @endcode
    PIoInterface_Mem(PIoInterface_Mem&& other) = delete;

public:  // IOInterface interface
    /// @brief Returns the actual allowed key/seed usage flags as defined by the key slot prototype and the current content of the container for this "Actor".
    ///         Volatile containers do not have any prototype restrictions, but can define restrictions for the current instance of the object at runtime.
    ///         The value returned by this method is the bitwise AND of the common usage flags defined at runtime and the usage flags defined by the UserPermissions prototype for the current "Actor".
    ///         This method is particularly useful for empty permanent prototype containers.
    /// @name   GetAllowedUsage
    /// @returns Usage flags
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00069
    /// @needwork = dda
    /// @endcode
    AllowedUsageFlags GetAllowedUsage() const noexcept override;
    /// @brief Returns the capacity of the underlying resource. In bytes.
    /// @name   GetCapacity
    /// @returns Capacity of the underlying resource
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00070
    /// @needwork = dda
    /// @endcode
    std::size_t GetCapacity() const noexcept override;
    /// @brief Returns the CryptoObjectType of the object referenced by this IOInterface.
    /// @name   GetCryptoObjectType
    /// @returns CryptoObjectType of the referenced object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00071
    /// @needwork = dda
    /// @endcode
    CryptoObjectType GetCryptoObjectType() const noexcept override;
    /// @brief Returns the COUID of the object stored in the IOInterface.
    ///         If the container is empty, this method returns CryptoObjectType::kUndefined. Explicit identification of a crypto object requires two components: CryptoObjectUid and CryptoObjectType.
    /// @name   GetObjectId
    /// @returns COUID of the object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00072
    /// @needwork = dda
    /// @endcode
    CryptoObjectUid GetObjectId() const noexcept override;
    /// @brief Returns the size of the object payload stored in the underlying buffer of the IOInterface.
    ///         If the container is empty, this method returns 0. The return value does not consider the meta-information attributes of the object, but their size is fixed and common to all crypto objects independent of their actual type.
    ///         Provides automatic space for the object's meta-information based on the implementation details of the object.
    /// @name   GetPayloadSize
    /// @returns Size of the object payload in the underlying buffer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00073
    /// @needwork = dda
    /// @endcode
    std::size_t GetPayloadSize() const noexcept override;
    /// @brief Get the vendor-specific ID of the primitive.
    /// @name   GetPrimitiveId
    /// @returns Vendor-specific ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00074
    /// @needwork = dda
    /// @endcode
    CryptoAlgId GetPrimitiveId() const noexcept override;
    /// @brief Returns the content type restriction of this interface (identifies what type of data this container can hold).
    /// @return
    /// If KeySlotPrototypeProps::mAllowContentTypeChange == TRUE, kUndefined should be returned.
    /// If the container has a type restriction different from @c CryptoObjectType::kUndefined, only objects of that type can be saved to this container.
    /// Volatile containers do not have any content type restrictions. CryptoObjectType::kUndefined identifies no restriction.
    /// @name GetTypeRestriction
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00075
    /// @needwork = dda
    /// @endcode
    CryptoObjectType GetTypeRestriction() const noexcept override;
    /// @brief Returns the "exportable" attribute of the object stored in the container. The exportability of an object does not depend on the volatility of its container.
    /// @name   IsObjectExportable
    /// @returns true if export object false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00076
    /// @needwork = dda
    /// @endcode
    bool IsObjectExportable() const noexcept override;
    /// @brief Returns the "session" (or "temporary") attribute of the set object, e.g., KeyDerivationFunctionCtx::DeriveKey().
    ///         A "session" object can only be stored in a VolatileTrustedContainer! If this IOInterface is linked to a KeySlot, it always returns false.
    /// @name   IsObjectSession
    /// @returns true if Session object false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00077
    /// @needwork = dda
    /// @endcode
    inline bool IsObjectSession() const noexcept override { return true; }
    /// @brief Returns the "volatility" attribute of the underlying buffer of this IOInterface.
    ///         "Session" objects can only be stored in "volatile" containers. The contents of a "volatile" container will be destroyed together with the interface instance.
    /// @name   IsVolatile
    /// @returns true if Volatile object false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00078
    /// @needwork = dda
    /// @endcode
    inline bool IsVolatile() const noexcept override { return bVolatile_; }
    /// @brief Get whether the underlying key storage is valid. If the underlying resource is modified after the IOInterface is opened, the IOInterface will become invalid.
    /// @name   IsValid
    /// @returns true if Valid false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00079
    /// @needwork = dda
    /// @endcode
    bool IsValid() const noexcept override;
    /// @brief Get whether the underlying KeySlot is writable. If this IOInterface is linked to a volatile trusted container, always returns true.
    /// @name   IsWritable
    /// @returns true if IsWritable false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00080
    /// @needwork = dda
    /// @endcode
    bool IsWritable() const noexcept override;

public:  // PIoInterface custom interface
    /// @brief Indicates whether the content of this key slot can be changed, e.g., from storing a symmetric key to storing an RSA key.
    /// @name   IsContentTypeChange
    /// @returns true if can change false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00081
    /// @needwork = dda
    /// @endcode
    bool IsContentTypeChange() const noexcept override;
    /// @brief Indicates whether FC encryption should allocate sufficient storage space for the shadow copy of this key string.
    /// @name   IsAllocateSpareSlot
    /// @returns true if AllocateSpare false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00082
    /// @needwork = dda
    /// @endcode
    bool IsAllocateSpareSlot() const noexcept override;
    /// @brief Specifies how many times this key slot can be updated, e.g.:
    ///         0: indicates that the key-slot content will be pre-set during production
    ///         1: indicates that the key slot content can be updated only once ("OTP").
    ///        -1: negative value indicates that the key slot content can be updated unlimited times
    /// @name   GetMaxUpdateAllowed
    /// @returns Number of times the key slot can be updated
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00083
    /// @needwork = dda
    /// @endcode
    int32_t GetMaxUpdateAllowed() const noexcept override;
    /// @brief Key-slot type configuration
    /// @name   GetSlotType
    /// @returns Key slot type
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00084
    /// @needwork = dda
    /// @endcode
    KeySlotType GetSlotType() const noexcept override;
    /// @brief Vendor-provided crypto primitive ID for Key-slot restrictions.
    /// @name   GetSlotPrimitiveId
    /// @returns Crypto primitive ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00085
    /// @needwork = dda
    /// @endcode
    CryptoAlgId GetSlotPrimitiveId() const noexcept override;
    /// @brief Usage scope identifier for Key-slot restrictions.
    /// @name   GetSlotAllowedUsage
    /// @returns Usage scope identifier
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00086
    /// @needwork = dda
    /// @endcode
    AllowedUsageFlags GetSlotAllowedUsage() const noexcept override;
    /// @brief Get whether there is a write operation on the raw data of the key slot
    /// @name   IsHaveWriteAction
    /// @returns true can write false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00087
    /// @needwork = dda
    /// @endcode
    bool IsHaveWriteAction() const noexcept override;
    /// @brief Save KeySlotContentProps
    /// @name   SaveKeyContent
    /// @param keyContent
    /// @returns true if save sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00088
    /// @needwork = dda
    /// @endcode
    bool SaveKeyContent(KeySlotContentProps const& keyContent) const noexcept override;
    /// @brief Save KeySlotPrototypeProps
    /// @name   SaveSlotProps
    /// @param slotProps Key slot attributes
    /// @returns true if save sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00089
    /// @needwork = dda
    /// @endcode
    bool SaveSlotProps(KeySlotPrototypeProps const& slotProps) const noexcept override;
    /// @brief Set GetName_KvWriteIndex()
    /// @name   SetWriteIndex
    /// @returns ture set write index false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00090
    /// @needwork = dda
    /// @endcode
    bool SetWriteIndex() const noexcept override;
    /// @brief Set GetName_KvWriteSave() and commit persistent save
    /// @name   CommitSaveAction
    /// @returns true if commit sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00091
    /// @needwork = dda
    /// @endcode
    bool CommitSaveAction() const noexcept override;
    /// @brief Roll back any possible save operation
    /// @name   RollbackSaveAction
    /// @returns ture if rollback sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00092
    /// @needwork = dda
    /// @endcode
    bool RollbackSaveAction() const noexcept override;

public:  // PIoInterface custom interface
    /// @brief Initialize the IO interface
    /// @name   InitIoInterface
    /// @param stSlotName Key slot name
    /// @returns true if init sucess flase otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00093
    /// @needwork = dda
    /// @endcode
    bool InitIoInterface(ara::core::StringView const& stSlotName) noexcept override;
    /// @brief Get the current key slot name
    /// @name   GetKeySlotName
    /// @returns
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00094
    /// @needwork = dda
    /// @endcode
    inline ara::core::StringView GetKeySlotName() const noexcept override
    {
        return {stSlotName_.data(), stSlotName_.length()};
    }
    /// @brief Whether the IO interface is initialized
    /// @name   IsInterfaceValid
    /// @returns ture if has already inited false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00095
    /// @needwork = dda
    /// @endcode
    bool IsInterfaceValid() const noexcept override;
    /// @brief Clear data
    /// @name   Clear
    /// @returns true if clear sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00096
    /// @needwork = dda
    /// @endcode
    bool Clear() noexcept override;
    /// @brief Check if the slot is empty.
    /// @name   IsEmpty
    /// @returns true if is empty false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00097
    /// @needwork = dda
    /// @endcode
    bool IsEmpty() const noexcept override;
    /// @brief Read the encryption algorithm information of the key data
    /// @name   ReadKeyKey
    /// @param nAlgID Encryption primitive ID
    /// @param vecData Vector object receiving KeyKey data
    /// @returns true if read keykey sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00098
    /// @needwork = dda
    /// @endcode
    bool ReadKeyKey(CryptoAlgId& nAlgID, ara::core::Vector< uint8_t >& vecData) const noexcept override;
    /// @brief Read key data
    /// @name   ReadKeyData
    /// @param vecData Vector object receiving Key data
    /// @returns true if read keydate sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00099
    /// @needwork = dda
    /// @endcode
    bool ReadKeyData(ara::core::Vector< uint8_t >& vecData) const noexcept override;
    /// @brief Save key data
    /// @name   SaveKeyData
    /// @param memKeyData Key data in memory
    /// @param nAlgID Encryption primitive ID
    /// @param memKeyKey Key used to encrypt the key
    /// @return true if save keydate sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00100
    /// @needwork = dda
    /// @endcode
    bool SaveKeyData(ReadOnlyMemRegion const& memKeyData,
                     CryptoAlgId nAlgID                 = 0U,
                     ReadOnlyMemRegion const& memKeyKey = ReadOnlyMemRegion()) const noexcept override;
    /// @brief Set the maximum storage capacity threshold
    /// @name   SetCapacity
    /// @param nCapacity Maximum storage capacity threshold
    /// @returns true if set capacity sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00101
    /// @needwork = dda
    /// @endcode
    bool SetCapacity(uint32_t const& nCapacity) const noexcept;
    /// @brief Set the HSM version key slot ID
    /// @param nSlotID Key slot ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00102
    /// @needwork = dda
    /// @endcode
    void SetHsmSlotID(uint8_t const nSlotID) const noexcept { slotDataTemp_.SetSlotID(nSlotID); }
    /// @brief Set the HSM version group ID
    /// @param nGroupID Group ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00103
    /// @needwork = dda
    /// @endcode
    void SetHsmGroupID(uint8_t const nGroupID) const noexcept { slotDataTemp_.SetGroupID(nGroupID); }
    /// @brief Set the HSM version catalog ID
    /// @param nCatalogID Catalog ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00104
    /// @needwork = dda
    /// @endcode
    void SetHsmCatalogID(uint8_t const nCatalogID) const noexcept { slotDataTemp_.SetCatalogID(nCatalogID); }
    /// @brief Get the HSM key slot ID
    /// @return Key slot ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00105
    /// @needwork = dda
    /// @endcode
    uint8_t GetHsmSlotID() const noexcept override { return slotDataSave_.GetSlotID(); }
    /// @brief Get the HSM group ID
    /// @return Group ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00106
    /// @needwork = dda
    /// @endcode
    uint8_t GetHsmGroupID() const noexcept override { return slotDataSave_.GetGroupID(); }
    /// @brief Get the HSM catalog ID
    /// @return Catalog ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00107
    /// @needwork = dda
    /// @endcode
    uint8_t GetHsmCataLogID() const noexcept override { return slotDataSave_.GetCatalogID(); }
    /// @brief Set whether the iointerface is valid
    /// @param bIsvalid Whether it is valid
    /// @returns  true if is valid false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00108
    /// @needwork = dda
    /// @endcode
    inline void SetIsValid(bool const bIsvalid) noexcept { bValid_ = bIsvalid; }

private:
    /// @brief Key slot name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00109
    /// @needwork = dda
    /// @endcode
    ara::core::String stSlotName_;
    /// @brief Whether it is volatile   // Many attributes may need to be initialized according to the configuration in Xml
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00110
    /// @needwork = dda
    /// @endcode
    bool bVolatile_;
    /// @brief Key slot data: temporary data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00111
    /// @needwork = dda
    /// @endcode
    mutable PKeySlotData_Mem slotDataTemp_;
    /// @brief Key slot data: actual data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00112
    /// @needwork = dda
    /// @endcode
    mutable PKeySlotData_Mem slotDataSave_;
    /// @brief Whether the iointerface is valid. If the content of the slot is modified after the slot is opened, the iointerface becomes invalid.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00113
    /// @needwork = dda
    /// @endcode
    bool bValid_;
};
//********************************/
}  // namespace  isoft_def
}  // namespace keys
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_KEYS_PUHUA_IO_INTERFACE_MEM_H_
