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
/// @file       generic_uds_service_agent.cpp
/// @brief
/// @details
/// @date       2024-12-23
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================

#include "generic_uds_service_agent.h"

namespace isoft {
namespace dm {
namespace dis {

GenericUdsServiceAgent::GenericUdsServiceAgent(uint32_t const& serviceInstanceId)
    : proxy_{std::make_unique< GenericUdsServiceProxy >(serviceInstanceId)}
{
}

void GenericUdsServiceAgent::RegisterService(ara::diag::GenericUDSService* const& gdi) { proxy_->RegisterService(gdi); }

void GenericUdsServiceAgent::SetReentrancyType(ara::diag::ReentrancyType const& reentrancyType) noexcept
{
    proxy_->SetReentrancyType(reentrancyType);
}
}  // namespace dis
}  // namespace dm
}  // namespace isoft