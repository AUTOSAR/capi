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
/// @file       isoft_timestamp_provider.cpp
/// @brief      Time callback provider
/// @details
/// @date       2022-12-27
/// @author     niuliming
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/idsm/lib client
/// @interface_level=module
/// @trace_id_sr=SRS_IDSM_0005
/// @unit_description=
/// @endcode
///
/// ================================================================

#include "ara/idsm/isoft/isoft_timestamp_provider.h"

namespace ara {
namespace idsm {
namespace isoft_def {
PuhuaTimestampProvider::PuhuaTimestampProvider(const ara::core::InstanceSpecifier& instance)
    : TimestampProvider(instance)
{
    /// TODO
}

PuhuaTimestampProvider::PuhuaTimestampProvider(PuhuaTimestampProvider&& tp) noexcept : TimestampProvider(std::move(tp))
{
    /// TODO
}

PuhuaTimestampProvider& PuhuaTimestampProvider::operator=(PuhuaTimestampProvider&& tp) noexcept
{
    std::ignore = tp;
    return *this;
    /// TODO
}

TimestampType PuhuaTimestampProvider::GetTimestamp()
{
    /// TODO
    return 0;
}

}  // namespace isoft_def
}  // namespace idsm
}  // namespace ara