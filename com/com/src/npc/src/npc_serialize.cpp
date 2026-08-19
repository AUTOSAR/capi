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
/// @file       npc_serialize.cpp
/// @brief      Binding layer serialization implementation file
/// @details
/// @date       2021-12-31
/// @author     mazelin
/// @version    1.2.0
///
/// ================================================================

#include "ara/com/internal/npc/npc_serialize.h"

#include "ara/com/internal/npc/npc_runtime.h"

namespace ara {
namespace com {
namespace internal {
namespace npc {
namespace serialize {
bool DeserializeErrorCode(ara::core::ErrorCode& value,
                          ara::core::ErrorDomain::CodeType const& errorCode,
                          ara::core::ErrorDomain::IdType const& domainId,
                          ara::core::ErrorDomain::SupportDataType const& supportData) noexcept
{
    auto const* domain{internal::GetInstance().GetErrorDomain(domainId)};
    if (domain == nullptr) {
        ComLogWarning("deserialize error code error: get error domain failed", GenArg(domainId), GenArg(errorCode),
                      GenArg(supportData), GenArg(value));
        return false;
    }
    value = {errorCode, *domain, supportData};
    return true;
}
}  // namespace serialize
}  // namespace npc
}  // namespace internal
}  // namespace com
}  // namespace ara