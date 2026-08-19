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
/// @file       context_data_provider.cpp
/// @brief
/// @details
/// @date       2023-02-16
/// @author     niuliming
/// @version    1.2.0
///
/// ================================================================

#include "ara/idsm/context_data_provider.h"

namespace ara {
namespace idsm {
ContextDataProvider::ContextDataProvider(const ara::core::InstanceSpecifier& instance,  // NOLINT
                                         std::size_t additionalBytes,
                                         std::size_t originalContextDataOffset) noexcept
    : instanceId_{instance}  // NOLINT
{
    std::ignore = additionalBytes;
    std::ignore = originalContextDataOffset;
    /// TODO
}

ContextDataProvider::ContextDataProvider(ContextDataProvider&& ctxData) noexcept
    : instanceId_{std::move(ctxData.instanceId_)}
{
    /// TODO
}

ContextDataProvider& ContextDataProvider::operator=(ContextDataProvider&& ctxData) noexcept
{
    std::ignore = ctxData;
    return *this;
    /// TODO
}

ara::core::Result< void > ContextDataProvider::Offer()
{
    /// TODO
    return ara::core::Result< void >::FromValue();
}

void ContextDataProvider::StopOffer()
{
    /// TODO
}

}  // namespace idsm
}  // namespace ara