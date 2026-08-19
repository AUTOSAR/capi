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
/// @file       config_field.h
/// @brief      The config fields of phm_contribute.json.
/// @details
/// @date       2024-07-16
/// @author     wangyanlong
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/PlatformHealthManagement/ConfigManager
/// @unit_description=The config fields of phm_contribute.json.
/// @trace_id_sr=SR_PHM_02001,SR_PHM_02002,SR_PHM_02003,SR_PHM_02004,SR_PHM_02005,
/// SR_PHM_02006,SR_PHM_02007,SR_PHM_02008,SR_PHM_02009,SR_PHM_020010
/// @unit_name=ConfigField
/// @interface_level=unit
/// @endcode
///
/// ================================================================

#ifndef ARA_PHM_INTERNAL_CONFIG_FIELD_H_
#define ARA_PHM_INTERNAL_CONFIG_FIELD_H_

#include <ara/core/string_view.h>
namespace ara {
namespace phm {
namespace internal {
/// @brief alive supervision.
/// @return "aliveSupervision"
/// @trace_id_sr=SR_PHM_02002,
/// @needwork = ad
inline ara::core::StringView GetAliveSupervision() noexcept
{
    return std::move(ara::core::StringView{"aliveSupervision"});
}

/// @brief deadline supervision.
/// @return "deadlineSupervision"
/// @trace_id_sr=SR_PHM_02003
/// @needwork = ad
inline ara::core::StringView GetDeadlineSupervision() noexcept
{
    return std::move(ara::core::StringView{"deadlineSupervision"});
}

/// @brief logical supervision.
/// @return "logicalSupervision"
/// @trace_id_sr=SR_PHM_02004
/// @needwork = ad
inline ara::core::StringView GetLogicalSupervision() noexcept
{
    return std::move(ara::core::StringView{"logicalSupervision"});
}

/// @brief local supervision.
/// @return "localSupervision"
/// @trace_id_sr=SR_PHM_02005
/// @needwork = ad
inline ara::core::StringView GetLocalSupervision() noexcept
{
    return std::move(ara::core::StringView{"localSupervision"});
}

/// @brief name of the object.
/// @return "shortName"
/// @trace_id_sr=SR_PHM_02001,SR_PHM_02002,SR_PHM_02003,SR_PHM_02004,SR_PHM_02005,
///              SR_PHM_02006,SR_PHM_02007,SR_PHM_02008,SR_PHM_02009,SR_PHM_020010
/// @needwork = ad
inline ara::core::StringView GetShortName() noexcept { return std::move(ara::core::StringView{"shortName"}); }

/// @brief fqn of the object.
/// @return "fqn"
/// @trace_id_sr=SR_PHM_02001,SR_PHM_02002,SR_PHM_02003,SR_PHM_02004,SR_PHM_02005,
///              SR_PHM_02006,SR_PHM_02007,SR_PHM_02008,SR_PHM_02009,SR_PHM_020010
/// @needwork = ad
inline ara::core::StringView GetFqn() noexcept { return std::move(ara::core::StringView{"fqn"}); }

/// @brief name of process.
/// @return "processName"
/// @trace_id_sr=SR_PHM_02001,SR_PHM_02002,SR_PHM_02003,SR_PHM_02004,SR_PHM_02005,
///              SR_PHM_02006,SR_PHM_02007,SR_PHM_02008,SR_PHM_02009,SR_PHM_020010
/// @needwork = ad
inline ara::core::StringView GetProcessName() noexcept { return std::move(ara::core::StringView{"processName"}); }

/// @brief identifier of required port.
/// @return "IdentifierR"
/// @trace_id_sr=SR_PHM_02001,SR_PHM_02002,SR_PHM_02003,SR_PHM_02004,SR_PHM_02005,
///              SR_PHM_02006,SR_PHM_02007,SR_PHM_02008,SR_PHM_02009,SR_PHM_020010
/// @needwork = ad
inline ara::core::StringView GetIdentifierR() noexcept { return std::move(ara::core::StringView{"IdentifierR"}); }

/// @brief meta model identifier.
/// @return "metaModelIdentifier"
/// @trace_id_sr=SR_PHM_02001,SR_PHM_02002,SR_PHM_02003,SR_PHM_02004,SR_PHM_02005,
///              SR_PHM_02006,SR_PHM_02007,SR_PHM_02008,SR_PHM_02009,SR_PHM_020010
/// @needwork = ad
inline ara::core::StringView GetMetaModelIdentifier() noexcept
{
    return std::move(ara::core::StringView{"metaModelIdentifier"});
}

/// @brief recovery notification.
/// @return "recoveryNotification"
/// @trace_id_sr=SR_PHM_02007
/// @needwork = ad
inline ara::core::StringView GetRecoveryNotification() noexcept
{
    return std::move(ara::core::StringView{"recoveryNotification"});
}
}  // namespace internal
}  // namespace phm
}  // namespace ara

#endif  // ARA_PHM_INTERNAL_CONFIG_FIELD_H_
