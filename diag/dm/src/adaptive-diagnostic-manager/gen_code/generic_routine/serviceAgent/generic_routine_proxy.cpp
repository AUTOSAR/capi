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
/// @file       generic_routine_proxy.cpp
/// @brief
/// @details
/// @date       2024-12-23
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================

#include "generic_routine_proxy.h"

#include <thread>

#include "ara/diag/diag_error_domain.h"

namespace isoft {
namespace dm {
namespace dic {

using isoft::dm::RetData;

constexpr uint8_t kFuncIDStart{1U};
constexpr uint8_t kFuncIDStop{2U};
constexpr uint8_t kFuncIDRequestResults{3U};

GenericRoutineProxy::GenericRoutineProxy(uint16_t const& instanceId, uint32_t const& serviceInstanceId)
    : BussinessClientProxy{serviceInstanceId, instanceId}
{
    // AccessClient::GetComProxy().RegisterNotificationCallBack(instanceId,
    //  std::move([this](FuncData const& funcData){Notify(funcData);}));
}

ara::core::Future< std::vector< std::uint8_t > > GenericRoutineProxy::Start(
    std::uint16_t routineId,
    std::vector< std::uint8_t >& requestData,
    MetaInfoMap& metaInfo,
    std::shared_ptr< CancellationEvent >& cancellationEventPtr) noexcept
{
    constexpr uint8_t kFunc_Id{kFuncIDStart};
    static uint8_t s_CallId{0U};

    ara::core::Future< std::vector< std::uint8_t > > result{AsyncCallFunc< std::vector< std::uint8_t > >(
        kFunc_Id, s_CallId, cancellationEventPtr, routineId, requestData, metaInfo)};

    return result;
}

ara::core::Future< std::vector< std::uint8_t > > GenericRoutineProxy::Stop(
    std::uint16_t routineId,
    std::vector< std::uint8_t >& requestData,
    MetaInfoMap& metaInfo,
    std::shared_ptr< CancellationEvent >& cancellationEventPtr) noexcept
{
    constexpr uint8_t kFunc_Id{kFuncIDStop};
    static uint8_t s_CallId{0U};

    ara::core::Future< std::vector< std::uint8_t > > result{AsyncCallFunc< std::vector< std::uint8_t > >(
        kFunc_Id, s_CallId, cancellationEventPtr, routineId, requestData, metaInfo)};

    return result;
}

ara::core::Future< std::vector< std::uint8_t > > GenericRoutineProxy::RequestResults(
    std::uint16_t routineId,
    std::vector< std::uint8_t >& requestData,
    MetaInfoMap& metaInfo,
    std::shared_ptr< CancellationEvent >& cancellationEventPtr) noexcept
{
    constexpr uint8_t kFunc_Id{kFuncIDRequestResults};
    static uint8_t s_CallId{0U};

    ara::core::Future< std::vector< std::uint8_t > > result{AsyncCallFunc< std::vector< std::uint8_t > >(
        kFunc_Id, s_CallId, cancellationEventPtr, routineId, requestData, metaInfo)};

    return result;
}

}  // namespace dic
}  // namespace dm
}  // namespace isoft
