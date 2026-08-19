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
/// @file       isoft_context_data_provider.cpp
/// @brief
/// @details
/// @date       2023-02-16
/// @author     niuliming
/// @version    1.2.0
///
/// ================================================================

#include "ara/idsm/isoft/isoft_context_data_provider.h"

namespace ara {
namespace idsm {
namespace isoft_def {
PuhuaContextDataProvider::PuhuaContextDataProvider(const ara::core::InstanceSpecifier& instance,
                                                   std::size_t additionalBytes,
                                                   std::size_t originalContextDataOffset) noexcept
    : ContextDataProvider(instance, additionalBytes, originalContextDataOffset)
{
    /// TODO
}

PuhuaContextDataProvider::PuhuaContextDataProvider(PuhuaContextDataProvider&& ctxData) noexcept
    : ContextDataProvider(std::move(ctxData))
{
    /// TODO
}

PuhuaContextDataProvider& PuhuaContextDataProvider::operator=(PuhuaContextDataProvider&& ctxData) noexcept
{
    std::ignore = ctxData;
    return *this;
    /// TODO
}

ara::core::Result< std::size_t > PuhuaContextDataProvider::ModifyContextData(
    ara::core::Span< std::uint8_t > contextData, EventIdType event)
{
    std::ignore = contextData;
    std::ignore = event;
    /// TODO
    return ara::core::Result< std::size_t >::FromValue(0);
}
}  // namespace isoft_def

}  // namespace idsm
}  // namespace ara