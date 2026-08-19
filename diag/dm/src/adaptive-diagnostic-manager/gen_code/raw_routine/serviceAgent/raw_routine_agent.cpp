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
/// @file       raw_routine_agent.cpp
/// @brief
/// @details
/// @date       2024-12-28
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================

#include "raw_routine_agent.h"

#include "raw_routine_proxy.h"

namespace isoft {
namespace dm {
namespace dic {

RawRoutineAgent::RawRoutineAgent(uint16_t const& instanceId, uint32_t const& serviceInstanceId)
{
    proxy_ = std::make_unique< RawRoutineProxy >(instanceId, serviceInstanceId);
}

ara::core::Future< std::vector< std::uint8_t > > RawRoutineAgent::Start(
    std::vector< std::uint8_t >& requestData,
    MetaInfoMap& metaInfo,
    std::shared_ptr< CancellationEvent >& cancellationEventPtr) noexcept
{
    return proxy_->Start(requestData, metaInfo, cancellationEventPtr);
}

ara::core::Future< std::vector< std::uint8_t > > RawRoutineAgent::Stop(
    std::vector< std::uint8_t >& requestData,
    MetaInfoMap& metaInfo,
    std::shared_ptr< CancellationEvent >& cancellationEventPtr) noexcept
{
    return proxy_->Stop(requestData, metaInfo, cancellationEventPtr);
}

ara::core::Future< std::vector< std::uint8_t > > RawRoutineAgent::RequestResults(
    std::vector< std::uint8_t >& requestData,
    MetaInfoMap& metaInfo,
    std::shared_ptr< CancellationEvent >& cancellationEventPtr) noexcept
{
    return proxy_->RequestResults(requestData, metaInfo, cancellationEventPtr);
}
}  // namespace dic
}  // namespace dm
}  // namespace isoft