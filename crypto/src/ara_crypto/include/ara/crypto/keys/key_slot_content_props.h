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
/// @file       key_slot_content_props.h
/// @brief      AutoSar-Crypto Key Storage Module
/// @details    Properties of the current key slot content.
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
/// @module_path=/CRYPTO/Default Key Components/Key Slot
/// @interface_level=software
/// @trace_id_sr=SR_CRYPTO_05005
/// @unit_name=PIpcKeySlot
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_KEYS_KEY_SLOT_CONTENT_PROPS_H_
#define ARA_CRYPTO_KEYS_KEY_SLOT_CONTENT_PROPS_H_

#include "ara/crypto/common/base_id_types.h"
#include "ara/crypto/common/crypto_object_uid.h"

namespace ara {
namespace crypto {
namespace keys {
//********************************/
/// @brief Properties of the current key slot content, i.e., properties of the instance currently stored in the key slot.
///         The value of the mAllowedUsage field is the bitwise AND of the common usage flags defined at runtime and the usage flags defined by the UserPermissions prototype for the current "Actor".
/// @brief Properties of current Key Slot Content, i.e. of a current instance stored to the Key Slot.
/// A value of the @c mAllowedUsage field is bitwise AND of the common usage flags defined at run-time and
///       the usage flags defined by the @c UserPermissions prototype for current "Actor".
/// @trace_id_sws={SWS_CRYPT_30500}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02005}
/// @uptrace={RS_CRYPTO_02111}
#pragma pack(push)
#pragma pack(1)
/// @brief Key slot content context attributes
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02760
/// @trace_id_dd=DD_CRYPTO_05618
/// @trace_id_sr=SR_CRYPTO_05005
/// @needwork = ad
/// @qac [2198]possibly cannot be modified: This struct explicitly defines member functions.
/// @endcode
struct KeySlotContentProps
{
public:
    /// @brief Shared pointer for the interface    //Comment does not match definition
    /// @brief shared pointer of interface
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_30511}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02111}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_03241
    /// @trace_id_dd=DD_CRYPTO_06453
    /// @needwork = ad
    /// @endcode
    using Uptr = std::unique_ptr< KeySlotContentProps >;

public:
    /// @brief Default constructor
    /// @brief set content properties
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_30510}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02111}
    /// @endcode
    KeySlotContentProps() = default;

public:
    /// @brief Encryption algorithm of the actual object stored in the slot.
    /// @brief Cryptoalgorithm of actual object stored to the slot.
    /// @trace_id_sws={SWS_CRYPT_30503}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02111}
    CryptoAlgId mAlgId{kAlgIdUndefined};

    /// @brief Actual size of the object currently stored in the slot.
    /// @brief Actual size of an object currently stored to the slot.
    /// @trace_id_sws={SWS_CRYPT_30505}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02111}
    std::size_t mObjectSize{0};
    /// @brief Actual type of the object stored in the slot.
    /// @brief Actual type of an object stored to the slot.
    /// @trace_id_sws={SWS_CRYPT_30508}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02111}
    CryptoObjectType mObjectType{CryptoObjectType::kUndefined};
    /// @brief UID of the crypto object stored in the slot.
    /// @brief UID of a Crypto Object stored to the slot.
    /// @trace_id_sws={SWS_CRYPT_30501}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02111}
    CryptoObjectUid mObjectUid;

    /// @brief Actual usage restriction flags of the object stored in the slot of the current "Actor".
    /// @brief Actual usage restriction flags of an object stored to the slot for the current "Actor".
    /// @trace_id_sws={SWS_CRYPT_30506}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02111}
    AllowedUsageFlags mContentAllowedUsage{kAllowPrototypedOnly};
};
#pragma pack(pop)
//********************************/
/// @brief Comparison operator "equal" for KeySlotContentProps operands.
/// @brief Comparison operator "equal" for @c KeySlotContentProps operands.
/// @param lhs  left-hand side operand
/// @param rhs  right-hand side operand
/// @returns @c true if all members' values of @c lhs is equal to @c rhs, and @c false otherwise
/// @code{.isoft}
/// @trace_id_sws={SWS_CRYPT_30550}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02111}
/// @threadsafety={Thread-safe}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02762
/// @trace_id_dd=DD_CRYPTO_05620
/// @trace_id_sr=SR_CRYPTO_05005==
/// @needwork = ad
/// @endcode
inline constexpr bool operator==(KeySlotContentProps const &lhs, KeySlotContentProps const &rhs) noexcept
{
    return (lhs.mObjectUid == rhs.mObjectUid) && (lhs.mContentAllowedUsage == rhs.mContentAllowedUsage)
           && (lhs.mAlgId == rhs.mAlgId) && (lhs.mObjectSize == rhs.mObjectSize)
           && (lhs.mObjectType == rhs.mObjectType);
}
//********************************/
/// @brief Comparison operator "not equal" for KeySlotContentProps operands.
/// @brief Comparison operator "not equal" for @c KeySlotContentProps operands.
/// @param lhs  left-hand side operand
/// @param rhs  right-hand side operand
/// @returns @c true if at least one member of @c lhs has a value not equal to correspondent member of @c rhs, and @c
/// false otherwise
/// @code{.isoft}
/// @trace_id_sws={SWS_CRYPT_30551}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02111}
/// @threadsafety={Thread-safe}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02763
/// @trace_id_dd=DD_CRYPTO_05621
/// @trace_id_sr=SR_CRYPTO_05005!=
/// @needwork = ad
/// @endcode
inline constexpr bool operator!=(KeySlotContentProps const &lhs, KeySlotContentProps const &rhs) noexcept
{
    return !(lhs == rhs);
}
//********************************/
}  // namespace keys
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_KEYS_KEY_SLOT_CONTENT_PROPS_H_
