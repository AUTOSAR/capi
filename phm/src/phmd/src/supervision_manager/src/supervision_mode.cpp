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
/// @file       supervision_mode.cpp
/// @brief
/// @details
/// @date       2024-06-20
/// @author     wangyanlong
/// @version    1.2.0
///
/// ================================================================

#include "ara/phm/internal/svmanager/supervision_mode.h"

#include "ara/phm/internal/phm_log.h"

namespace ara {
namespace phm {
namespace internal {
/// @brief Constructor.
/// @param fgName name of function group.
/// @param fgState state of function group.
/// @param baseSupervisionFqn fqns of base supervision activated in this supervision mode.
/// @param processInfos process info in this function group state.
SupervisionMode::SupervisionMode(ara::core::String fgName,
                                 ara::core::String fgState,
                                 ara::core::Vector< ara::core::String > const& baseSupervisionFqn,
                                 ara::core::Map< ara::core::String, int32_t > const& processInfos) noexcept
    : kFgName{std::move(fgName)}, kFgState{std::move(fgState)}, supervisionsFqn_{}, processInfo_{}
{
    LOG_INFO << "SupervisionMode::SupervisionMode, fg name " << kFgName << ", fg state " << kFgState
             << ", active base supervision count " << baseSupervisionFqn.size();

    for (auto const& element : processInfos) {
        processInfo_[element.first] = element.second;
    }

    for (ara::core::String const& fqn : baseSupervisionFqn) {
        std::ignore = supervisionsFqn_.insert(fqn);
    }
}

/// @brief Returns fqn of supervisions activated in this supervison mode.
/// @return fqn of supervisions activated in this supervison mode.
std::set< ara::core::String > SupervisionMode::GetSupervisions() const noexcept { return supervisionsFqn_; }

/// @brief Returns execution error code of process running in this supervision mode.
/// @param processName name of process running in this supervision mode.
/// @return execution error code of process running in this supervision mode.
int32_t SupervisionMode::GetProcessExecutionError(ara::core::String const& processName) noexcept
{
    if (processInfo_.count(processName) == 0U) {
        LOG_ERROR << "execution error of process " << processName.c_str() << " not found, in fg " << kFgName.c_str()
                  << ", state " << kFgState.c_str();
        return 0;
    }
    return processInfo_[processName];
}

/// @brief Returns state of function group of supervision mode.
/// @return state of function group of supervision mode.
ara::core::String SupervisionMode::GetFgState() const noexcept { return kFgState; }

/// @brief Returns the process info of this supervision mode.
/// @return the process info of this supervision mode.
ara::core::Map< ara::core::String, int32_t > SupervisionMode::GetProcessInfo() const noexcept { return processInfo_; }

}  // namespace internal
}  // namespace phm
}  // namespace ara
