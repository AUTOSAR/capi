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
/// @file       security_access_agent.cpp
/// @brief
/// @details
/// @date       2024-12-23
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================

#include "security_access_agent.h"

namespace isoft {
namespace dm {
namespace dic {

SecurityAccessAgent::SecurityAccessAgent(uint16_t const& instanceId, uint32_t const& serviceInstanceId)
{
    proxy_ = std::make_unique< SecurityAccessProxy >(instanceId, serviceInstanceId);
}

ara::core::Future< std::vector< std::uint8_t > > SecurityAccessAgent::GetSeed(
    std::vector< std::uint8_t > const& securityAccessDataRecord,
    MetaInfoMap& metaInfo,
    std::shared_ptr< CancellationEvent >& cancellationEventPtr) noexcept
{
    return proxy_->GetSeed(securityAccessDataRecord, metaInfo, cancellationEventPtr);
}

ara::core::Future< std::uint32_t > SecurityAccessAgent::CompareKey(
    std::vector< std::uint8_t > const& key,
    MetaInfoMap& metaInfo,
    std::shared_ptr< CancellationEvent >& cancellationEventPtr) noexcept
{
    return proxy_->CompareKey(key, metaInfo, cancellationEventPtr);
}

}  // namespace dic
}  // namespace dm
}  // namespace isoft