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
/// @file       timestamp_provider.cpp
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

#include "ara/idsm/timestamp_provider.h"

#include <ara/log/logger.h>

#include "idsc_client.h"

namespace ara {
namespace idsm {

/// @brief  [SWS_AIDSM_20101] Register a callback for providing timestamps to the IdsM .
/// @param callback
/// @exception Stack overflow exception
void RegisterTimestampProvider(std::function< TimestampType() > const& callback)
{
    IdsmClient* const client{IdsmClient::GetInstance()};
    if (nullptr == client) {
        ara::log::Logger const& logger{ara::log::CreateLogger(
            ara::core::StringView("clcb"), ara::core::StringView("RegisterTimestampProvider context"),
            ara::log::LogLevel::kVerbose)};
        logger.LogError() << "IdsmClient::GetInstance is nullptr!";
        return;
    }
    client->SetTimeProviderCB(callback);
}

TimestampProvider::TimestampProvider(const ara::core::InstanceSpecifier& instance) : instanceId_{instance}  // NOLINT
{
    /// TODO
}

TimestampProvider::TimestampProvider(TimestampProvider&& tp) noexcept : instanceId_{tp.instanceId_}
{
    /// TODO
}

TimestampProvider& TimestampProvider::operator=(TimestampProvider&& tp) noexcept
{
    std::ignore = tp;
    return *this;
    /// TODO
}

ara::core::Result< void > TimestampProvider::Offer()
{
    /// TODO
    return ara::core::Result< void >::FromValue();
}

void TimestampProvider::StopOffer()
{
    /// TODO
}

}  // namespace idsm
}  // namespace ara