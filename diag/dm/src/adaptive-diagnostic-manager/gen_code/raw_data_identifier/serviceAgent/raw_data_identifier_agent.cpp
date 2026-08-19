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
/// @file       raw_data_identifier_agent.cpp
/// @brief
/// @details
/// @date       2024-12-28
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================

#include "raw_data_identifier_agent.h"

#include "cancellation/cancellation_event.h"
#include "raw_data_identifier_proxy.h"

namespace isoft {
namespace dm {
namespace dic {

RawDataIdentifierAgent::RawDataIdentifierAgent(uint16_t const& instanceId, uint32_t const& serviceInstanceId)
{
    proxy_ = std::make_shared< RawDataIdentifierProxy >(instanceId, serviceInstanceId);
}

ara::core::Future< ara::core::Vector< std::uint8_t > > RawDataIdentifierAgent::Read(
    MetaInfoMap const& metaInfo, std::shared_ptr< CancellationEvent >& cancelEventPtr)
{
    return proxy_->Read(metaInfo, cancelEventPtr);
}

ara::core::Future< void > RawDataIdentifierAgent::Write(ara::core::Vector< std::uint8_t > requestData,
                                                        MetaInfoMap const& metaInfo,
                                                        std::shared_ptr< CancellationEvent >& cancelEventPtr)
{
    return proxy_->Write(std::move(requestData), metaInfo, cancelEventPtr);
}

}  // namespace dic
}  // namespace dm
}  // namespace isoft