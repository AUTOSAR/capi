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
/// @file       generic_uds_service_proxy.cpp
/// @brief
/// @details
/// @date       2024-12-23
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================

#include "generic_uds_service_proxy.h"

#include <thread>

#include "ara/diag/diag_error_domain.h"

namespace isoft {
namespace dm {
namespace dic {

using isoft::dm::RetData;

constexpr uint8_t kFuncIDHandleMessage{1U};

GenericUdsServiceProxy::GenericUdsServiceProxy(uint16_t const& instanceId, uint32_t const& serviceInstanceId)
    : BussinessClientProxy{serviceInstanceId, instanceId}
{
    // AccessClient::GetComProxy().RegisterNotificationCallBack(instanceId,
    //  std::move([this](FuncData const& funcData){Notify(funcData);}));
}

ara::core::Future< ara::core::Vector< std::uint8_t > > GenericUdsServiceProxy::HandleMessage(
    std::uint8_t sid,
    std::vector< std::uint8_t >& requestData,
    MetaInfoMap& metaInfo,
    std::shared_ptr< CancellationEvent >& cancellationEventPtr) noexcept
{
    constexpr uint8_t kFunc_Id{kFuncIDHandleMessage};
    static uint8_t s_CallId{0U};

    ara::core::Future< ara::core::Vector< std::uint8_t > > result{AsyncCallFunc< ara::core::Vector< std::uint8_t > >(
        kFunc_Id, s_CallId, cancellationEventPtr, sid, requestData, metaInfo)};

    return result;
}

}  // namespace dic
}  // namespace dm
}  // namespace isoft
