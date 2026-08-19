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
/// @file       key_slot_prototype_props.h
/// @brief      AutoSar-Crypto Key Storage Module
/// @details    Prototype properties of a key slot.
/// @date       2021-12-29
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr> <td>2021-12-29 <td>1.0.0 <td>hanjingjing <td>Created initial version
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default Key Components/Key Slot
/// @interface_level=software
/// @trace_id_sr=SR_CRYPTO_05005
/// @unit_name=PIpcKeySlot
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_KEYS_KEY_SLOT_PROTOTYPE_PROPS_H_
#define ARA_CRYPTO_KEYS_KEY_SLOT_PROTOTYPE_PROPS_H_

#include "ara/crypto/common/base_id_types.h"
#include "ara/crypto/common/crypto_object_uid.h"

namespace ara {
namespace crypto {
namespace keys {
//********************************/
/// @brief Prototype properties of a key slot.
/// @brief Prototyped Properties of a Key Slot.
/// @trace_id_sws={SWS_CRYPT_30300}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02009}
/// @uptrace={RS_CRYPTO_02110}
/// @uptrace={RS_CRYPTO_02116}
#pragma pack(push)
#pragma pack(1)
/// @brief Key slot attributes
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02756
/// @trace_id_dd=DD_CRYPTO_05614
/// @trace_id_sr=SR_CRYPTO_05005
/// @needwork = ad
/// @qac [2198]possibly cannot be modified: This struct explicitly defines member functions.
/// @endcode
struct KeySlotPrototypeProps
{
public:
    /// @brief Unique smart pointer for the interface
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_30302}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02110}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_03240
    /// @trace_id_dd=DD_CRYPTO_06452
    /// @needwork = ad
    /// @endcode
    using Uptr = std::unique_ptr< KeySlotPrototypeProps >;

public:
    /// @brief Default constructor
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_30301}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02110}
    /// @endcode
    KeySlotPrototypeProps() = default;

public:
    /// @brief Encryption algorithm restriction (kAlgIdAny indicates no restriction). The algorithm can be partially specified: family & length, mode, padding.
    /// @brief Cryptoalgorithm restriction (@c kAlgIdAny means without restriction).
    ///           The algorithm can be specified partially: family & length, mode, padding.
    /// @trace_id_sws={SWS_CRYPT_30306}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02110}
    CryptoAlgId mAlgId{kAlgIdUndefined};
    /// @brief Restriction on the type of object that can be stored in the slot. If this field contains CryptoObjectType::kUnknown, there is no type restriction.
    /// @brief Restriction of an object type that can be stored the slot.
    ///         If this field contains @c CryptoObjectType::kUnknown then without restriction of the type.
    /// @trace_id_sws={SWS_CRYPT_30308}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02110}
    CryptoObjectType mObjectType{CryptoObjectType::kUndefined};
    /// @brief Indicates how the content is used. The following are use cases for this attribute:
    ///         -
    ///         The AllowedUsageFlags of the object stored in this key slot are set to kAllowPrototypedOnly. In this case, this attribute must be respected when loading the content into a runtime instance (e.g., the AllowedUsageFlags of the SymmetricKey object should be set according to this attribute)
    ///         - mMaxUpdatesAllowed==0, in which case the content is provided during production, and AllowedUsageFlags are modeled using this attribute
    ///         - When flexibly updating this key slot, the AllowedUsageFlags of the runtime object will later override this attribute when loaded from this key slot
    /// @brief Indicates how the content may be used. The following use cases of this attribute are considered:
    ///        - the object to be stored in this key-slot has it's AllowedUsageFlags set to kAllowPrototypedOnly.
    ///          In this case this attribute must be observed when loading the content into a runtime instance
    ///          (e.g. the AllowedUsageFlags of a SymmetricKey object should be set according to this attribute)
    ///        - mMaxUpdatesAllowed==0, in this case the content is provided during production while the
    ///        AllowedUsageFlags
    ///          is modeled using this attribute
    ///        - when this key-slot is flexibly updated the runtime object's AllowedUsageFlags override this attribute
    ///          upon a later loading from this key-slot
    /// @trace_id_sws={SWS_CRYPT_30313}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02110}
    AllowedUsageFlags mContentAllowedUsage{kAllowPrototypedOnly};
    /// @brief Indicates whether FC encryption should allocate sufficient storage space for the shadow copy of this key string.
    /// @brief Indicates whether FC Crypto shall allocate sufficient storage space for a shadow copy of this KeySlot.
    /// @trace_id_sws={SWS_CRYPT_30309}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02110}
    bool mAllocateSpareSlot{false};
    /// @brief Indicates whether the content of this key slot can be changed, e.g., from storing a symmetric key to storing an RSA key.
    ///         If set to false, the mObjectType of this KeySlotPrototypeProps must be [valid]and [cannot be changed](i.e., only objects of mObjectType can be stored in this key slot).
    /// @brief Indicates whether the content of this key-slot may be changed, e.g. from storing a symmetric key to
    /// storing an RSA key.
    ///        If this is set to false, then the mObjectType of this KeySlotPrototypeProps must be a) valid and b)
    ///        cannot be changed ( i.e. only objects of mObjectType may be stored in this key-slot).
    /// @trace_id_sws={SWS_CRYPT_30310}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02110}
    bool mAllowContentTypeChange{false};
    /// @brief Whether the key-slot content can be exported.
    /// @brief Indicates whether the key-slot content may be exported.
    /// @trace_id_sws={SWS_CRYPT_30312}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02110}
    bool mExportAllowed{false};
    /// @brief Specifies how many times this key slot can be updated, e.g.:
    ///         0: Indicates that the key-slot content will be pre-set during production (modification/writing not allowed);
    ///         1: Indicates that the key slot content can only be updated once ("OTP");
    ///        -1: Negative value indicates that the key slot content can be updated unlimited times.
    /// @brief Specifies how many times this key-slot may be updated, e.g.:
    ///        - a value of 0 means the key-slot content will be pre-set during production
    ///        - a value of 1 means the key-slot content can be updated only once ("OTP")
    ///        - a negative value means the key-slot content can be updated inifinitely
    /// @trace_id_sws={SWS_CRYPT_30311}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02110}
    std::int32_t mMaxUpdateAllowed{0};
    /// @brief Key-slot type configuration: All key slots used by adaptive machines for providing services such as secure communication, diagnostics, update, secure storage, etc., use the kMachine type.
    ///         All key slots intended for use by adaptive user applications must use kApplication.
    ///         Key manager user applications can also define kMachine key slots; in this case, the integrator must match a corresponding machine key slot to manage.
    /// @brief Key-slot type configuration: all key-slots used by the adaptive machine to provide serives such as
    ///		   secure communication, diagnostics, updates, secure storage etc. shall use the type kMachine.
    ///        All key-slots that will be used by the adaptive user application must use kApplication.
    ///		   A key-manager user application may define kMachine key-slots as well; in this case the integrator
    ///        must match a corresponding machine key-slot to be managed.
    /// @trace_id_sws={SWS_CRYPT_30305}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02110}
    KeySlotType mSlotType{KeySlotType::kUnDefined};
    /// @brief Slot capacity, in bytes.
    /// @brief Capacity of the slot in bytes.
    /// @trace_id_sws={SWS_CRYPT_30307}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02110}
    std::size_t mSlotCapacity{0};
};
#pragma pack(pop)
//********************************/
/// @brief Comparison operator "equal" for KeySlotPrototypeProps operands.
/// @brief Comparison operator "equal" for @c KeySlotPrototypeProps operands.
/// @param lhs  left-hand side operand
/// @param rhs  right-hand side operand
/// @returns @c true if all members' values of @c lhs is equal to @c rhs, and @c false otherwise
/// @code{.isoft}
/// @trace_id_sws={SWS_CRYPT_30350}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02110}
/// @threadsafety={Thread-safe}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02758
/// @trace_id_dd=DD_CRYPTO_05616
/// @trace_id_sr=SR_CRYPTO_05005==
/// @needwork = ad
/// @endcode
inline constexpr bool operator==(KeySlotPrototypeProps const& lhs, KeySlotPrototypeProps const& rhs) noexcept
{
    return (lhs.mContentAllowedUsage == rhs.mContentAllowedUsage) && (lhs.mExportAllowed == rhs.mExportAllowed)
           && (lhs.mMaxUpdateAllowed == rhs.mMaxUpdateAllowed)
           && (lhs.mAllowContentTypeChange == rhs.mAllowContentTypeChange)
           && (lhs.mAllocateSpareSlot == rhs.mAllocateSpareSlot) && (lhs.mAlgId == rhs.mAlgId)
           && (lhs.mSlotCapacity == rhs.mSlotCapacity) && (lhs.mObjectType == rhs.mObjectType)
           && (lhs.mSlotType == rhs.mSlotType);
}
//********************************/
/// @brief Comparison operator "not equal" for KeySlotPrototypeProps operands.
/// @brief Comparison operator "not equal" for @c KeySlotPrototypeProps operands.
/// @param lhs  left-hand side operand
/// @param rhs  right-hand side operand
/// @returns @c true if at least one member of @c lhs has a value not equal to correspondent member of @c rhs, and @c
/// false otherwise
/// @code{.isoft}
/// @trace_id_sws={SWS_CRYPT_30351}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02110}
/// @threadsafety={Thread-safe}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02759
/// @trace_id_dd=DD_CRYPTO_05617
/// @trace_id_sr=SR_CRYPTO_05005!=
/// @needwork = ad
/// @endcode
inline constexpr bool operator!=(KeySlotPrototypeProps const& lhs, KeySlotPrototypeProps const& rhs) noexcept
{
    return !(lhs == rhs);
}
//********************************/
}  // namespace keys
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_KEYS_KEY_SLOT_PROTOTYPE_PROPS_H_
