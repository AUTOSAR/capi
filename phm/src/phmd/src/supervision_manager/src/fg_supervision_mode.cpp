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
/// @file       fg_supervision_mode.cpp
/// @brief
/// @details
/// @date       2024-06-20
/// @author     wangyanlong
/// @version    1.2.0
///
/// ================================================================

#include "ara/phm/internal/svmanager/fg_supervision_mode.h"

#include "ara/phm/internal/phm_log.h"

namespace ara {
namespace phm {
namespace internal {
/// @brief Creation of a FgSupervisionMode.
/// @param fgName name of function group.
/// @param supervisionMode container of supervision mode.
FgSupervisionMode::FgSupervisionMode(
    ara::core::String const& fgName,
    ara::core::Vector< std::shared_ptr< SupervisionMode > > supervisionMode) noexcept  //NOLINT
    : kFgName{fgName}, fgState_{"Off"}, supervisionMode_{}, processPidMap_{}, process_{}, processStateChangeMap_{}
{
    LOG_INFO << "construct FgSupervisionMode, fg name " << kFgName << ", state count " << supervisionMode.size();
    for (std::shared_ptr< SupervisionMode >& mode : supervisionMode) {
        supervisionMode_[mode->GetFgState()] = mode;

        for (auto const& element : mode->GetProcessInfo()) {
            processPidMap_[element.first] = 0U;
            std::ignore                   = process_.insert(element.first);
            LOG_DEBUG << "process " << element.first << " belongs to function group " << fgName;
        }
    }
}

/// @brief Returns the name of function group.
/// @return the name of function group.
ara::core::String FgSupervisionMode::GetFgName() const noexcept { return kFgName; }

/// @brief Returns current state of function group.
/// @return current state of function group.
ara::core::String FgSupervisionMode::GetFgState() const noexcept
{
    LOG_DEBUG << kFgName.c_str() << ", FgSupervisionMode::GetFgState:" << fgState_.c_str();
    return fgState_;
}

/// @brief Set stae of function group.
/// @param state new state of function group.
void FgSupervisionMode::SetState(ara::core::String const& state) noexcept
{
    LOG_DEBUG << "state of fg " << kFgName.c_str() << ", " << fgState_.c_str() << "-->" << state.c_str();
    fgState_ = state;
}

/// @brief Returns current supervision mode.
/// @param state state of function group.
/// @return current supervision mode.
std::shared_ptr< SupervisionMode > FgSupervisionMode::GetSupervisionMode(ara::core::String const& state) noexcept
{
    if (supervisionMode_.count(state) > static_cast< size_t >(0)) {
        return supervisionMode_[state];
    }
    LOG_WARN << "FgSupervisionMode::GetSupervisionMode, state " << state.c_str() << ", fg " << kFgName.c_str()
             << " not exist.";
    return std::shared_ptr< SupervisionMode >{nullptr};
}

/// @brief Returns all the supervision mode belongs this fg.
/// @return all the supervision mode belongs this fg.
ara::core::Vector< std::shared_ptr< SupervisionMode > > FgSupervisionMode::GetAllSupervisionMode() noexcept
{
    ara::core::Vector< std::shared_ptr< SupervisionMode > > ret;
    for (auto const& element : supervisionMode_) {
        ret.push_back(element.second);
    }
    return ret;
}

/// @brief returns the state change string.
/// @param stateChange enum state change.
/// @return the state change string.
ara::core::StringView FgSupervisionMode::GetProcessStateChangeString(ProcessStateChange const& stateChange) noexcept
{
    if (stateChange == ProcessStateChange::kIdleToRunning) {
        return "IdleToRunning";
    }
    if (stateChange == ProcessStateChange::kRunningToIdle) {
        return "RunningToIdle";
    }
    if (stateChange == ProcessStateChange::kRestart) {
        return "Restart";
    }
    if (stateChange == ProcessStateChange::kKeepRunning) {
        return "KeepRunning";
    }
    return "Invalid";
}

/// @brief Update process pid and update its state change.
/// @param processFqn fqn of process.
/// @param newPid new pid of process.
void FgSupervisionMode::UpdateProcessPidAndStateChange(ara::core::String const& processFqn,
                                                       uint32_t const newPid) noexcept
{
    uint32_t oldPid{0U};
    if (processPidMap_.count(processFqn) > 0U) {
        oldPid = processPidMap_[processFqn];
    } else {
        LOG_ERROR << "process " << processFqn << " not exist in processPidMap_";
    }

    if ((oldPid == 0U) && (newPid > 0U)) {
        processStateChangeMap_[processFqn] = ProcessStateChange::kIdleToRunning;
    } else if ((oldPid > 0U) && (newPid == 0U)) {
        processStateChangeMap_[processFqn] = ProcessStateChange::kRunningToIdle;
    } else if ((oldPid > 0U) && (newPid > 0U) && (oldPid != newPid)) {
        processStateChangeMap_[processFqn] = ProcessStateChange::kRestart;
    } else if ((oldPid > 0U) && (newPid > 0U) && ((oldPid == newPid))) {
        processStateChangeMap_[processFqn] = ProcessStateChange::kKeepRunning;
    } else {
        processStateChangeMap_[processFqn] = ProcessStateChange::kInvalid;
    }
    LOG_DEBUG << "process:" << processFqn << ", old pid:" << oldPid << ", new pid:" << newPid
              << ", state change:" << GetProcessStateChangeString(processStateChangeMap_[processFqn]);

    processPidMap_[processFqn] = newPid;
}

ProcessStateChange FgSupervisionMode::GetProcessStateChange(ara::core::String const& processFqn)
{
    if (processStateChangeMap_.count(processFqn) == 0U) {
        LOG_DEBUG << "state change of process " << processFqn << " is invalid.";
        return ProcessStateChange::kInvalid;
    }
    return processStateChangeMap_[processFqn];
}

std::set< ara::core::String > FgSupervisionMode::GetProcess() const noexcept { return process_; }

}  // namespace internal
}  // namespace phm
}  // namespace ara