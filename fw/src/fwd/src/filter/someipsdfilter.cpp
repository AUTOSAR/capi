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
/// @file       someipsdfilter.cpp
/// @brief      someipsdfilter filter
/// @details    someipsdfilter filter
/// @date       2025-10-15
/// @author     jzy
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/fw/Filter Mode
/// @interface_level=module
/// @trace_id_sr=SR_FW_0005
/// @unit_name=Filter_SomeipSd
/// @unit_description=Firewall someipSd filter
/// @endcode
///
/// ================================================================
///
///
/// ================================================================

#include "someipsdfilter.h"

#include "ara/fw/internal/log_api.h"
namespace ara {
namespace fw {
namespace internal {

/// @brief udpfilter constructor
/// @param vecUdpRules udp rule validation set
/// @param action action
/// @param inOutFlag hook point in/out
// SomeIpSdFilter::SomeIpSdFilter(ara::core::Vector< PRuleParse > vecUdpRules,
//                            int const& action,
//                            int32_t const& inOutFlag,
//                            int32_t pRate,
//                            int32_t qLength) noexcept
//     : vecUdpRules_(std::move(vecUdpRules))
//     , unpUdpEngine_(std::make_unique< UdpEngine >(action, inOutFlag, pRate,
//     qLength))
// {
// }

/// @brief Rule filtering
void SomeIpSdFilter::RulesFilter() noexcept { LogInfo() << "SomeIpSdFilter::RulesFilter() in!"; }

/// @brief Get rules
void SomeIpSdFilter::GetRules() noexcept {}

}  // namespace internal
}  // namespace fw
}  // namespace ara