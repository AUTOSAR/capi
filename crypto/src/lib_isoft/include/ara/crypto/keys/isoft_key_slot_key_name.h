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
/// @file       isoft_key_slot_key_name.h
/// @brief      AutoSar-Crypto Key Storage Module
/// @details    List of KEYs used by KeySlot with KV library.
/// @date       2021-12-29
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr><td>2021-12-29  <td>1.0.0    <td>hanjingjing  <td>Initial version creation
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default Key Components/Key Provider
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=PIpcKeyProvider
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_KEYS_PUHUA_KEY_SLOT_KEY_NAME_H_
#define ARA_CRYPTO_KEYS_PUHUA_KEY_SLOT_KEY_NAME_H_

#include <ara/core/string_view.h>

#include <cstdint>

namespace ara {
namespace crypto {
namespace keys {
namespace isoft_def {
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03295
/// @trace_id_dd=DD_CRYPTO_06542
/// @needwork = ad
/// @endcode
constexpr int32_t kKvName_Count{16};
//********************************/ // If code obfuscation is needed, the following strings can be changed to numbers
/// @brief Total write count before saving stored in KV library
/// @return  kvWriteIndex
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03296
/// @trace_id_dd=DD_CRYPTO_06543
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
inline ara::core::StringView GetName_KvWriteIndex() noexcept { return ara::core::StringView{"kvWriteIndex"}; }
/// @brief Total write count stored in KV library: Synchronized with kvWriteIndex every time saving occurs
/// @return  kvWriteSave
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03297
/// @trace_id_dd=DD_CRYPTO_06544
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
inline ara::core::StringView GetName_KvWriteSave() noexcept { return ara::core::StringView{"kvWriteSave"}; }
//***************/
/// @brief Crypto algorithm of the actual object stored in the slot.
/// @return  Obj.AlgId
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03298
/// @trace_id_dd=DD_CRYPTO_06545
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
inline ara::core::StringView GetName_KvContent_AlgId() noexcept { return ara::core::StringView{"Obj.AlgId"}; }
/// @brief Actual size of the object currently stored in the slot.
/// @return  Obj.Size
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03299
/// @trace_id_dd=DD_CRYPTO_06546
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
inline ara::core::StringView GetName_KvContent_Size() noexcept { return ara::core::StringView{"Obj.Size"}; }
/// @brief Actual type of the object stored in the slot.
/// @return   Obj.Type
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03300
/// @trace_id_dd=DD_CRYPTO_06547
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
inline ara::core::StringView GetName_KvContent_Type() noexcept { return ara::core::StringView{"Obj.Type"}; }
/// @brief UID of the crypto object stored in the slot.
/// @return   Obj.UidMs
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03301
/// @trace_id_dd=DD_CRYPTO_06548
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
inline ara::core::StringView GetName_KvContent_UidMs() noexcept { return ara::core::StringView{"Obj.UidMs"}; }
/// @brief UID of the crypto object stored in the slot.
/// @return   Obj.UidLs
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03302
/// @trace_id_dd=DD_CRYPTO_06549
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
inline ara::core::StringView GetName_KvContent_UidLs() noexcept { return ara::core::StringView{"Obj.UidLs"}; }
/// @brief UID of the crypto object stored in the slot.
/// @return  Obj.UidVersion
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03303
/// @trace_id_dd=DD_CRYPTO_06550
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
inline ara::core::StringView GetName_KvContent_UidVersion() noexcept { return ara::core::StringView{"Obj.UidVersion"}; }
/// @brief Actual usage restriction flags of the object stored in the current "Actor" slot.
/// @return Obj.AllowedUsage
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03304
/// @trace_id_dd=DD_CRYPTO_06551
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
inline ara::core::StringView GetName_KvContent_AllowedUsage() noexcept
{
    return ara::core::StringView{"Obj.AllowedUsage"};
}
//***************/
/// @brief Crypto algorithm restriction (kAlgIdAny means no restriction).
/// @return Slot.AlgId
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03305
/// @trace_id_dd=DD_CRYPTO_06552
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
inline ara::core::StringView GetName_KvKeySlot_AlgId() noexcept { return ara::core::StringView{"Slot.AlgId"}; }
/// @brief Indicates whether FC crypto should allocate enough storage space for the shadow copy of this key string.
/// @return Slot.AllocateSpareSlot
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03306
/// @trace_id_dd=DD_CRYPTO_06553
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
inline ara::core::StringView GetName_KvKeySlot_AllocateSpareSlot() noexcept
{
    return ara::core::StringView{"Slot.AllocateSpareSlot"};
}
/// @brief Indicates whether the content of this key slot can be changed, e.g., from storing a symmetric key to storing an RSA key.
/// @return Slot.ContentTypeChange
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03307
/// @trace_id_dd=DD_CRYPTO_06554
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
inline ara::core::StringView GetName_KvKeySlot_ContentTypeChange() noexcept
{
    return ara::core::StringView{"Slot.ContentTypeChange"};
}
/// @brief Actual usage restriction flags of the object stored in the current "Actor" slot.
/// @return Slot.AllowedUsage
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03308
/// @trace_id_dd=DD_CRYPTO_06555
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
inline ara::core::StringView GetName_KvKeySlot_AllowedUsage() noexcept
{
    return ara::core::StringView{"Slot.AllowedUsage"};
}
/// @brief Whether the key-slot content is exportable.
/// @return Slot.Exportable
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03309
/// @trace_id_dd=DD_CRYPTO_06556
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
inline ara::core::StringView GetName_KvKeySlot_Exportable() noexcept
{
    return ara::core::StringView{"Slot.Exportable"};
}
/// @brief Specifies how many times this key slot can be updated, e.g.: 1, 0, -1
/// @return Slot.MaxUpdateAllowed
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03310
/// @trace_id_dd=DD_CRYPTO_06557
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
inline ara::core::StringView GetName_KvKeySlot_MaxUpdateAllowed() noexcept
{
    return ara::core::StringView{"Slot.MaxUpdateAllowed"};
}
/// @brief Key slot type
/// @return Slot.Type
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03311
/// @trace_id_dd=DD_CRYPTO_06558
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
inline ara::core::StringView GetName_KvKeySlot_Type() noexcept { return ara::core::StringView{"Slot.Type"}; }
/// @brief Slot capacity in bytes.
/// @return Slot.Capacity
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03312
/// @trace_id_dd=DD_CRYPTO_06559
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
inline ara::core::StringView GetName_KvKeySlot_Capacity() noexcept { return ara::core::StringView{"Slot.Capacity"}; }
/// @brief Restriction on the type of object that can be stored in the slot. If this field contains cryptobject Type::kUnknown, there is no type restriction.
/// @return Slot.RestrictionType
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03313
/// @trace_id_dd=DD_CRYPTO_06560
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
inline ara::core::StringView GetName_KvKeySlot_RestrictionType() noexcept
{
    return ara::core::StringView{"Slot.RestrictionType"};
}
/// @brief Actual content saved within the key slot.
/// @return Slot.KeyData
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03314
/// @trace_id_dd=DD_CRYPTO_06561
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
inline ara::core::StringView GetName_KvKeySlot_KeyData() noexcept { return ara::core::StringView{"Slot.KeyData"}; }
/// @brief Algorithm ID for the key content encrypted by the key slot.
/// @return Slot.KeyAlgID
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03315
/// @trace_id_dd=DD_CRYPTO_06562
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
inline ara::core::StringView GetName_KvKeySlot_KeyAlgID() noexcept { return ara::core::StringView{"Slot.KeyAlgID"}; }
/// @brief Key for the key content encrypted by the key slot.
/// @return Slot.KeyKey
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03316
/// @trace_id_dd=DD_CRYPTO_06563
/// @trace_id_sr=SR_CRYPTO_06005
/// @needwork = ad
/// @endcode
inline ara::core::StringView GetName_KvKeySlot_KeyKey() noexcept { return ara::core::StringView{"Slot.KeyKey"}; }
//********************************/
}  // namespace  isoft_def
}  // namespace keys
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_KEYS_PUHUA_KEY_SLOT_KEY_NAME_H_
