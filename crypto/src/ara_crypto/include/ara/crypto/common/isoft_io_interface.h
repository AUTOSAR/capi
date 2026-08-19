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
/// @file       isoft_io_interface.h
/// @brief      AutoSar-Crypto Encryption/Decryption common module
/// @details    Formal interface of IOInterface for saving and loading secure objects.
/// @date       2021-12-29
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <td> <td>2021-12-29 <td>1.0.0 <td>hanjingjing <td>Created initial version
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Reusable Functions/Reusable Functions Module
/// @interface_level=software
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=Common_api
/// @unit_description=IO Interface for saving and loading secure objects
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_KEYS_PUHUA_IO_INTERFACE_H_
#define ARA_CRYPTO_KEYS_PUHUA_IO_INTERFACE_H_

#include <ara/core/string.h>
#include <ara/core/string_view.h>

#include "ara/crypto/common/io_interface.h"
#include "ara/crypto/common/mem_region.h"
#include "ara/crypto/keys/key_slot_content_props.h"
#include "ara/crypto/keys/key_slot_prototype_props.h"

namespace ara {
namespace crypto {
namespace keys {
namespace isoft_def {
//********************************/
/// @brief Formal interface of IOInterface for saving and loading secure objects.
///         Actual saving and loading should be implemented via internal methods known to the trusted pair of crypto provider and storage provider.
///         Each object should be uniquely identified by its type and Crypto Object Unique Identifier (COUID). This interface assumes that objects in the container are compressed, i.e., have a minimal optimized size.
//********************************/
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02027
/// @trace_id_dd=DD_CRYPTO_04754
/// @needwork = ad
/// @endcode
class PIoInterface : public ara::crypto::IOInterface
{
public:
    /// @brief Default virtual destructor
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02028
    /// @trace_id_dd=DD_CRYPTO_04755
    /// @needwork = ad
    /// @endcode
    ~PIoInterface() noexcept override = default;
    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02029
    /// @trace_id_dd=DD_CRYPTO_04756
    /// @needwork = ad
    /// @endcode
    PIoInterface() = default;
    /// @brief Default copy assignment operator
    /// @param other Another instance of this class
    /// @return Reference to the object itself
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02030
    /// @trace_id_dd=DD_CRYPTO_04757
    /// @needwork = ad
    /// @endcode
    PIoInterface& operator=(PIoInterface const& other) = delete;
    /// @brief Default move assignment operator
    /// @param other Another instance of this class
    /// @return Reference to the object itself
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02031
    /// @trace_id_dd=DD_CRYPTO_04758
    /// @needwork = ad
    /// @endcode
    PIoInterface& operator=(PIoInterface&& other) = delete;
    /// @brief Default copy constructor
    /// @param other Another instance of this class
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02032
    /// @trace_id_dd=DD_CRYPTO_04759
    /// @needwork = ad
    /// @endcode
    PIoInterface(PIoInterface const& other) = delete;
    /// @brief Default move constructor
    /// @param other Another instance of this class
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02033
    /// @trace_id_dd=DD_CRYPTO_04760
    /// @needwork = ad
    /// @endcode
    PIoInterface(PIoInterface&& other) = delete;

public:  // IOInterface interface
    /// @brief Returns the actual allowed key/seed usage flags as defined by the key slot prototype of this "Actor" and the current content of the container.
    ///         Volatile containers have no prototype restrictions, but can define restrictions for the current instance of the object at runtime.
    ///         The value returned by this method is the bitwise AND of the common usage flags defined at runtime and the usage flags defined by the UserPermissions prototype for the current "Actor".
    ///         This method is particularly useful for empty permanent prototype containers.
    /// @return Key/seed usage flags
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02034
    /// @trace_id_dd=DD_CRYPTO_04761
    /// @needwork = ad
    /// @endcode
    AllowedUsageFlags GetAllowedUsage() const noexcept override = 0;
    /// @brief Returns the capacity of the underlying resource.
    /// @return Capacity of the underlying resource
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02035
    /// @trace_id_dd=DD_CRYPTO_04762
    /// @needwork = ad
    /// @endcode
    std::size_t GetCapacity() const noexcept override = 0;
    /// @brief Returns the CryptoObjectType of the object referenced by this IOInterface.
    /// @return Type of the referenced object
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02036
    /// @trace_id_dd=DD_CRYPTO_04763
    /// @needwork = ad
    /// @endcode
    CryptoObjectType GetCryptoObjectType() const noexcept override = 0;
    /// @brief Returns the COUID of the object stored in the IOInterface.
    ///         If the container is empty, this method returns CryptoObjectType::kUndefined. Unambiguous identification of a crypto object requires two components: CryptoObjectUid and CryptoObjectType.
    /// @return UUID of the referenced object
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02037
    /// @trace_id_dd=DD_CRYPTO_04764
    /// @needwork = ad
    /// @endcode
    CryptoObjectUid GetObjectId() const noexcept override = 0;
    /// @brief Returns the size of the object payload stored in the underlying buffer of the IOInterface.
    ///         If the container is empty, this method returns 0. The return value does not consider the object's meta-information attributes, but their size is fixed and common for all crypto objects independent of their actual type.
    ///         Space for the object's meta-information is provided automatically based on the object's implementation details.
    /// @return Payload size of the referenced object
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02038
    /// @trace_id_dd=DD_CRYPTO_04765
    /// @needwork = ad
    /// @endcode
    std::size_t GetPayloadSize() const noexcept override = 0;
    /// @brief Gets the vendor-specific ID of the primitive.
    /// @return Vendor-specific ID of the referenced object
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02039
    /// @trace_id_dd=DD_CRYPTO_04766
    /// @needwork = ad
    /// @endcode
    CryptoAlgId GetPrimitiveId() const noexcept override = 0;
    /// @brief Returns the content type restriction of this interface (identifying what type of data this container can hold).
    ///     If KeySlotPrototypeProps::mAllowContentTypeChange==TRUE, kUndefined should be returned.
    ///     If the container has a type restriction different from @c CryptoObjectType::kUndefined, only objects of said type can be saved to this container.
    ///     Volatile containers have no content type restrictions. CryptoObjectType::kUndefined identifies no restriction.
    /// @return Content type restriction of the referenced object
    /// @throw ???
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02040
    /// @trace_id_dd=DD_CRYPTO_04767
    /// @needwork = ad
    /// @endcode
    // PRQA S 2029 QAC /// @qac: False positive
    CryptoObjectType GetTypeRestriction() const noexcept override = 0;
    // PRQA L:QAC
    /// @brief Returns the "exportable" attribute of the object stored in the container. The exportability of an object does not depend on the volatility of its container.
    /// @return true if can export false otherwise
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02041
    /// @trace_id_dd=DD_CRYPTO_04768
    /// @needwork = ad
    /// @endcode
    bool IsObjectExportable() const noexcept override = 0;
    /// @brief Returns the "session" (or "temporary") attribute of the set object, e.g., from keyderivationfunctionctx::DeriveKey().
    ///         A "session" object can only be stored in a VolatileTrustedContainer! If this IOInterface is linked to a KeySlot, it always returns false.
    /// @return true if Session Object false otherwise
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02042
    /// @trace_id_dd=DD_CRYPTO_04769
    /// @needwork = ad
    /// @endcode
    bool IsObjectSession() const noexcept override = 0;
    /// @brief Returns the "volatile" attribute of the underlying buffer of this IOInterface.
    ///         "session" objects can only be stored in "volatile" containers. The contents of a "volatile" container are destroyed together with the interface instance.
    /// @brief Return volatility of the the underlying buffer of this IOInterface.
    ///      A "session" object can be stored to a "volatile" container only.
    ///      A content of a "volatile" container will be destroyed together with the interface instance.
    /// @return @c true if the container has a volatile nature (i.e. "temporary" or "in RAM") or @c false otherwise
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_10814}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02109}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02043
    /// @trace_id_dd=DD_CRYPTO_04770
    /// @needwork = ad
    /// @endcode
    bool IsVolatile() const noexcept override = 0;
    /// @brief Gets whether the underlying key storage is valid. If the underlying resource is modified after the IOInterface is opened, the IOInterface becomes invalid.
    /// @brief Get whether the underlying KeySlot is valid. An IOInterface is invalidated if the underlying resource has
    /// been modified after the IOInterface has been opened.
    /// @return true if the underlying resource can be valid, false otherwise
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_10823}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02004}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02044
    /// @trace_id_dd=DD_CRYPTO_04771
    /// @needwork = ad
    /// @endcode
    bool IsValid() const noexcept override = 0;
    /// @brief Gets whether the underlying KeySlot is writable. If this IOInterface is linked to a volatile trusted container, it always returns true.
    /// @return ture if keyslot writable otherwise false
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02045
    /// @trace_id_dd=DD_CRYPTO_04772
    /// @needwork = ad
    /// @endcode
    bool IsWritable() const noexcept override = 0;

public:  // PIoInterface custom interface
    /// @brief Indicates whether the content of this key slot can be changed, e.g., from storing a symmetric key to storing an RSA key.
    /// @return true if keyslot can be change false ohterwise
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02046
    /// @trace_id_dd=DD_CRYPTO_04773
    /// @needwork = ad
    /// @endcode
    virtual bool IsContentTypeChange() const noexcept = 0;
    /// @brief Indicates whether FC encryption should allocate sufficient storage space for the shadow copy of this key string.
    /// @return ture if has AllocateSpare false otherwise
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02047
    /// @trace_id_dd=DD_CRYPTO_04774
    /// @needwork = ad
    /// @endcode
    virtual bool IsAllocateSpareSlot() const noexcept = 0;
    /// @brief Specifies how many times this key slot can be updated, e.g.:
    ///         0: Indicates that the key-slot content will be pre-set during production
    ///         1: Indicates that the key slot content can only be updated once ("OTP").
    ///        -1: Negative value indicates that the key slot content can be updated unlimited times
    /// @return 0: Indicates that the key-slot content will be pre-set during production / 1: Indicates that the key slot content can only be updated once ("OTP") /
    /// -1: Negative value indicates that the key slot content can be updated unlimited times
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02048
    /// @trace_id_dd=DD_CRYPTO_04775
    /// @needwork = ad
    /// @endcode
    virtual int32_t GetMaxUpdateAllowed() const noexcept = 0;
    /// @brief Key-slot type configuration
    /// @return Key slot type
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02049
    /// @trace_id_dd=DD_CRYPTO_04776
    /// @needwork = ad
    /// @endcode
    virtual KeySlotType GetSlotType() const noexcept = 0;
    /// @brief Vendor-provided crypto primitive ID restricted by Key-slot.
    /// @return Algorithm ID
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02050
    /// @trace_id_dd=DD_CRYPTO_04777
    /// @needwork = ad
    /// @endcode
    virtual CryptoAlgId GetSlotPrimitiveId() const noexcept = 0;
    /// @brief Usage scope identifier restricted by Key-slot.
    /// @return Key slot usage restriction
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02051
    /// @trace_id_dd=DD_CRYPTO_04778
    /// @needwork = ad
    /// @endcode
    virtual AllowedUsageFlags GetSlotAllowedUsage() const noexcept = 0;
    /// @brief Gets whether there is a write operation on the raw data of the key slot
    /// @return true if HaveWriteAction false otherwise
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02052
    /// @trace_id_dd=DD_CRYPTO_04779
    /// @needwork = ad
    /// @endcode
    virtual bool IsHaveWriteAction() const noexcept = 0;
    /// @brief Save KeySlotContentProps
    /// @param contentProps Key attributes
    /// @return ture if save KeyContent sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02053
    /// @trace_id_dd=DD_CRYPTO_04780
    /// @needwork = ad
    /// @endcode
    virtual bool SaveKeyContent(KeySlotContentProps const& contentProps) const noexcept = 0;
    /// @brief Save KeySlotPrototypeProps
    /// @param slotProps Key slot attributes
    /// @return ture if save SlotProps sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02054
    /// @trace_id_dd=DD_CRYPTO_04781
    /// @needwork = ad
    /// @endcode
    virtual bool SaveSlotProps(KeySlotPrototypeProps const& slotProps) const noexcept = 0;
    /// @brief Set GetName_KvWriteIndex()
    /// @return true if set writeindex sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02055
    /// @trace_id_dd=DD_CRYPTO_04782
    /// @needwork = ad
    /// @endcode
    virtual bool SetWriteIndex() const noexcept = 0;
    /// @brief Set GetName_KvWriteSave() and commit persistent save
    /// @return true if commit sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02056
    /// @trace_id_dd=DD_CRYPTO_04783
    /// @needwork = ad
    /// @endcode
    virtual bool CommitSaveAction() const noexcept = 0;
    /// @brief Rollback any possible save operation
    /// @return true if rollback sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02057
    /// @trace_id_dd=DD_CRYPTO_04784
    /// @needwork = ad
    /// @endcode
    virtual bool RollbackSaveAction() const noexcept = 0;

public:  // PIoInterface custom interface
    /// @brief Initialize the IO interface
    /// @param stSlotName Key slot name
    /// @return ture if init Iointerface sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02058
    /// @trace_id_dd=DD_CRYPTO_04785
    /// @needwork = ad
    /// @endcode
    virtual bool InitIoInterface(ara::core::StringView const& stSlotName) noexcept = 0;
    /// @brief Get the current key slot name
    /// @return Key slot name
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02059
    /// @trace_id_dd=DD_CRYPTO_04786
    /// @needwork = ad
    /// @endcode
    virtual ara::core::StringView GetKeySlotName() const noexcept = 0;
    /// @brief Whether the IO interface is initialized
    /// @return ture if has already init false otherwise
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02060
    /// @trace_id_dd=DD_CRYPTO_04787
    /// @needwork = ad
    /// @endcode
    virtual bool IsInterfaceValid() const noexcept = 0;
    /// @brief Clear data
    /// @return true if clear sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02061
    /// @trace_id_dd=DD_CRYPTO_04788
    /// @needwork = ad
    /// @endcode
    virtual bool Clear() noexcept = 0;
    /// @brief Check if the slot is empty.
    /// @return ture is slot is empty false otherwise
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02062
    /// @trace_id_dd=DD_CRYPTO_04789
    /// @needwork = ad
    /// @endcode
    virtual bool IsEmpty() const noexcept = 0;
    /// @brief Read encryption algorithm information of the key data
    /// @param nAlgID Crypto primitive ID
    /// @param vecData Vector object receiving KeyKey data
    /// @return ture if read keykey sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02063
    /// @trace_id_dd=DD_CRYPTO_04790
    /// @needwork = ad
    /// @endcode
    virtual bool ReadKeyKey(CryptoAlgId& nAlgID, ara::core::Vector< uint8_t >& vecData) const noexcept = 0;
    /// @brief Read key data
    /// @param vecData Vector object receiving Key data
    /// @return true if read keydata sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02064
    /// @trace_id_dd=DD_CRYPTO_04791
    /// @needwork = ad
    /// @endcode
    virtual bool ReadKeyData(ara::core::Vector< uint8_t >& vecData) const noexcept = 0;
    /// @brief Save key data
    /// @param memKeyData Key data in memory
    /// @param nAlgID Crypto primitive ID
    /// @param memKeyKey Key used to encrypt the key
    /// @return true if save keydata sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02065
    /// @trace_id_dd=DD_CRYPTO_04792
    /// @needwork = ad
    /// @endcode
    virtual bool SaveKeyData(ReadOnlyMemRegion const& memKeyData,
                             CryptoAlgId nAlgID                 = 0U,
                             ReadOnlyMemRegion const& memKeyKey = ReadOnlyMemRegion()) const noexcept = 0;
    /// @brief Whether there is a deletion flag
    /// @return true if has deletefalg false otherwise
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02066
    /// @trace_id_dd=DD_CRYPTO_04793
    /// @needwork = ad
    /// @endcode
    virtual bool GetDeleteFlag() const noexcept { return false; }
    /// @brief Set the deletion flag
    /// @param nDelProcessID Deletion flag
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02067
    /// @trace_id_dd=DD_CRYPTO_04794
    /// @needwork = ad
    /// @endcode
    virtual void SetDeleteFlag(uint32_t nDelProcessID) const noexcept { std::ignore = nDelProcessID; }
    /// @brief Get HSM key slot ID
    /// @return HSM key slot ID
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02068
    /// @trace_id_dd=DD_CRYPTO_04795
    /// @needwork = ad
    /// @endcode
    virtual uint8_t GetHsmSlotID() const noexcept { return 0U; }
    /// @brief Get group ID
    /// @return HSM group ID
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02069
    /// @trace_id_dd=DD_CRYPTO_04796
    /// @needwork = ad
    /// @endcode
    virtual uint8_t GetHsmGroupID() const noexcept { return 0U; }
    /// @brief Get HSM directory ID
    /// @return HSM directory ID
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02070
    /// @trace_id_dd=DD_CRYPTO_04797
    /// @needwork = ad
    /// @endcode
    virtual uint8_t GetHsmCataLogID() const noexcept { return 0U; }
    /// @brief Reset key slot attribute information
    /// @param slotProps Key slot attributes
    /// @return true reset keyslot sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02071
    /// @trace_id_dd=DD_CRYPTO_04798
    /// @needwork = ad
    /// @endcode
    virtual bool ResetKeySlot(KeySlotPrototypeProps const& slotProps) const noexcept
    {
        std::ignore = slotProps;
        return true;
    }

public:  // PIoInterface custom functionality
    /// @brief Return KeySlotContentProps
    /// @return Key slot content attribute information
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02072
    /// @trace_id_dd=DD_CRYPTO_04799
    /// @needwork = ad
    /// @endcode
    KeySlotContentProps GetKeyContent() const noexcept
    {
        KeySlotContentProps keyCont;
        //Initialize KeySlot content attributes: KeySlotContentProps
        keyCont.mAlgId               = this->GetPrimitiveId();
        keyCont.mObjectSize          = this->GetPayloadSize();
        keyCont.mObjectType          = this->GetCryptoObjectType();
        keyCont.mObjectUid           = this->GetObjectId();
        keyCont.mContentAllowedUsage = this->GetAllowedUsage();
        if (kAllowPrototypedOnly == keyCont.mContentAllowedUsage) {
            ///- The AllowedUsageFlags of the object stored in this key slot are set to kAllowPrototypedOnly.
            //      In this case, this attribute must be respected when loading the content into a runtime instance (e.g., the AllowedUsageFlags of the SymmetricKey object should be set according to this attribute)
            keyCont.mContentAllowedUsage = this->GetSlotAllowedUsage();
        }
        return keyCont;
    }
    /// @brief Return KeySlotPrototypeProps
    /// @return Key slot attribute information
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02073
    /// @trace_id_dd=DD_CRYPTO_04800
    /// @needwork = ad
    /// @endcode
    KeySlotPrototypeProps GetSlotProps() const noexcept
    {
        KeySlotPrototypeProps slotProps;
        //Initialize KeySlot attribute data: KeySlotPrototypeProps
        //Encryption algorithm restriction (kAlgIdAny indicates no restriction).
        slotProps.mAlgId                  = this->GetSlotPrimitiveId();
        slotProps.mAllocateSpareSlot      = this->IsAllocateSpareSlot();
        slotProps.mAllowContentTypeChange = this->IsContentTypeChange();
        slotProps.mContentAllowedUsage    = this->GetSlotAllowedUsage();
        slotProps.mExportAllowed          = this->IsObjectExportable();
        slotProps.mMaxUpdateAllowed       = this->GetMaxUpdateAllowed();
        slotProps.mSlotType               = this->GetSlotType();
        slotProps.mSlotCapacity           = this->GetCapacity();
        slotProps.mObjectType             = this->GetTypeRestriction();
        return slotProps;
    }
    /// @brief: Save key
    /// @param pKey Template key pointer
    /// @param memKeyData Key data in memory
    /// @return true if save key to slot sucess false otherwise
    /// @code{.isoft}
    /// @tparam T_Key
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02074
    /// @trace_id_dd=DD_CRYPTO_04801
    /// @needwork = ad
    /// @endcode
    template < typename T_Key >
    bool SaveKeyToSlot(T_Key const* const pKey, ReadOnlyMemRegion const& memKeyData) const noexcept
    {
        try {
            PIoInterface const* const pIoInterface{this};
            if (!pKey->IsSession()) {
                return false;
            }
            //Save Slot attributes
            keys::KeySlotPrototypeProps slotProps{GetSlotProps()};
            //Whether the key slot allows: AlgId verification
            CryptoAlgId const nSelfAlgId{pKey->GetCryptoPrimitiveId()->GetPrimitiveId()};
            //Encryption algorithm restriction (kAlgIdAny indicates no restriction).
            if ((kAlgIdAny != slotProps.mAlgId) && (nSelfAlgId != slotProps.mAlgId)) {
                return false;
            }
            //Whether the key slot allows: changing key slot content type
            if (false == slotProps.mAllowContentTypeChange) {
                if ((CryptoObjectType::kUndefined != slotProps.mObjectType)
                    && (CryptoObjectType::kSymmetricKey != slotProps.mObjectType)) {
                    return false;
                }
            }
            ///   TODO: There is major logic here
            slotProps.mExportAllowed = pKey->IsExportable();
            //Whether the key slot allows: number of updates
            if ((0 == slotProps.mMaxUpdateAllowed))  //Will it prevent the write call initialized by Slog
            {
                if (false == pIoInterface->IsEmpty()) {
                    return false;
                }
            }
            //Whether the key slot allows: slot capacity out of bounds
            if (slotProps.mSlotCapacity < pKey->GetPayloadSize()) {
                return false;
            }
            //Maintain the number of writable times
            if (slotProps.mMaxUpdateAllowed > 0) {
                slotProps.mMaxUpdateAllowed = slotProps.mMaxUpdateAllowed - 1;
            }
            //Write back: 2022-04-25 only modified mExportAllowed attribute
            if (false == pIoInterface->SaveSlotProps(slotProps)) {
                return false;
            }
            //Save Content attributes
            keys::KeySlotContentProps contentProps;
            contentProps.mAlgId      = pKey->GetCryptoPrimitiveId()->GetPrimitiveId();
            contentProps.mObjectSize = pKey->GetPayloadSize();
            contentProps.mObjectType = pKey->GetkObjectType();
            contentProps.mObjectUid  = pKey->GetObjectId().mCouid;
            if (0 == slotProps.mMaxUpdateAllowed) {
                //- mMaxUpdatesAllowed==0, in this case the content is provided during production, and AllowedUsageFlags are modeled using this attribute
                if (true == pIoInterface->IsEmpty()) {
                    contentProps.mContentAllowedUsage = slotProps.mContentAllowedUsage;
                }
            } else {
                contentProps.mContentAllowedUsage = pKey->GetAllowedUsage();
            }
            if (false == pIoInterface->SaveKeyContent(contentProps)) {
                return false;
            }
            //Save key: directly use the encryption strategy within IOInterface
            CryptoAlgId nAlgID{kAlgIdAny};
            ara::core::Vector< uint8_t > vecKeyKey;
            std::ignore = pIoInterface->ReadKeyKey(nAlgID, vecKeyKey);
            if (false == pIoInterface->SaveKeyData(memKeyData, nAlgID, ara::crypto::ReadOnlyMemRegion{vecKeyKey})) {
                return false;
            }
            if (false == pIoInterface->CommitSaveAction()) {
                return false;
            }
            return true;
        } catch (...) {
            return false;
        }
    }

protected:
};
//********************************/
}  // namespace  isoft_def
}  // namespace keys
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_KEYS_PUHUA_IO_INTERFACE_H_
