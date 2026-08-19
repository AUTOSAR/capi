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
/// @file       fg_supervision_mode.h
/// @brief      A software unit represents a function group, include all states of this function group
/// @details
/// @date       2024-06-06
/// @author     wangyanlong
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/PlatformHealthManagement/SupervisionManager
/// @unit_description=A software unit represents a function group, include all states of this function group
/// and active superviisons in every state.
/// @trace_id_sr=SR_PHM_01021,SR_PHM_01022,SR_PHM_01023
/// @unit_name=FgSupervisionMode
/// @interface_level=unit
/// @endcode
///
/// ================================================================
///
/// and active superviisons in every state.
///
/// ================================================================

#ifndef ARA_PHM_INTERNAL_SVMANAGER_FG_SUPERVISION_MODE_H_
#define ARA_PHM_INTERNAL_SVMANAGER_FG_SUPERVISION_MODE_H_

#include <ara/core/map.h>
#include <ara/core/string.h>
#include <ara/core/string_view.h>
#include <ara/core/vector.h>

#include <memory>
#include <set>

#include "ara/phm/internal/svmanager/supervision_mode.h"

namespace ara {
namespace phm {
namespace internal {

/// @brief process state change.
/// @trace_id_sr=SR_PHM_01021,SR_PHM_01022,SR_PHM_01023
/// @needwork = ad
enum class ProcessStateChange : uint32_t
{
    /// @brief ilde to running.
    kIdleToRunning = 0,

    /// @brief running to idle.
    kRunningToIdle = 1,

    /// @brief reatsrt:running --> idle --> running
    kRestart = 2,

    /// @brief running --> running
    kKeepRunning = 3,

    /// @brief invalid
    kInvalid = 4
};

/// @brief FgSupervisionMode represents a function group, include all states of this function group
/// and active superviisons in every state.
/// @trace_id_sr=SR_PHM_01021,SR_PHM_01022,SR_PHM_01023
/// @needwork = ad
class FgSupervisionMode final
{
public:
    /// @brief Creation of a FgSupervisionMode.
    /// @param fgName name of function group.
    /// @param supervisionMode container of supervision mode.
    /// @trace_id_sr=SR_PHM_01021,SR_PHM_01022,SR_PHM_01023
    /// @needwork = ad
    FgSupervisionMode(ara::core::String const& fgName,
                      ara::core::Vector< std::shared_ptr< SupervisionMode > > supervisionMode) noexcept;

    /// @brief Default deconstructor.
    /// @throws QAC
    /// @trace_id_sr=SR_PHM_01021,SR_PHM_01022,SR_PHM_01023
    /// @needwork = ad
    ~FgSupervisionMode() = default;

    /// @brief The copy constructor shall not be used.
    /// @param obj The object to be copied.
    /// @trace_id_sr=SR_PHM_01021,SR_PHM_01022,SR_PHM_01023
    /// @needwork = ad
    FgSupervisionMode(FgSupervisionMode& obj) = delete;

    /// @brief The copy assignment shall not be used.
    /// @param obj The object to be copied.
    /// @return The copied object.
    /// @trace_id_sr=SR_PHM_01021,SR_PHM_01022,SR_PHM_01023
    /// @needwork = ad
    FgSupervisionMode& operator=(FgSupervisionMode const& obj) = delete;

    /// @brief The move constructor shall not be used.
    /// @param obj The object to be copied.
    /// @trace_id_sr=SR_PHM_01021,SR_PHM_01022,SR_PHM_01023
    /// @needwork = ad
    FgSupervisionMode(FgSupervisionMode&& obj) = delete;

    /// @brief The move assignment shall not be used.
    /// @param obj The object to be moved.
    /// @return The moved object.
    /// @trace_id_sr=SR_PHM_01021,SR_PHM_01022,SR_PHM_01023
    /// @needwork = ad
    FgSupervisionMode& operator=(FgSupervisionMode const&& obj) = delete;

    /// @brief Returns the name of function group.
    /// @return the name of function group.
    /// @trace_id_sr=SR_PHM_01021,SR_PHM_01022,SR_PHM_01023
    /// @needwork = ad
    ara::core::String GetFgName() const noexcept;

    /// @brief Returns current state of function group.
    /// @return current state of function group.
    /// @trace_id_sr=SR_PHM_01021,SR_PHM_01022,SR_PHM_01023
    /// @needwork = ad
    ara::core::String GetFgState() const noexcept;

    /// @brief Set stae of function group.
    /// @param state new state of function group.
    /// @trace_id_sr=SR_PHM_01021,SR_PHM_01022,SR_PHM_01023
    /// @needwork = ad
    void SetState(ara::core::String const& state) noexcept;

    /// @brief ara::core::String const& processFqn
    /// @param processFqn fqn of process.
    /// @return  state change os this process.
    /// @trace_id_sr=SR_PHM_01021,SR_PHM_01022,SR_PHM_01023
    /// @needwork = ad
    ProcessStateChange GetProcessStateChange(ara::core::String const& processFqn);

    /// @brief Returns current supervision mode.
    /// @param state state of function group.
    /// @return current supervision mode.
    /// @trace_id_sr=SR_PHM_01021,SR_PHM_01022,SR_PHM_01023
    /// @needwork = ad
    std::shared_ptr< SupervisionMode > GetSupervisionMode(ara::core::String const& state) noexcept;

    /// @brief Returns all the supervision mode belongs this fg.
    /// @return all the supervision mode belongs this fg.
    /// @trace_id_sr=SR_PHM_01021,SR_PHM_01022,SR_PHM_01023
    /// @needwork = ad
    ara::core::Vector< std::shared_ptr< SupervisionMode > > GetAllSupervisionMode() noexcept;

    /// @brief returns the processes belongs to this function group.
    /// @return returns the processes belongs to this function group.
    /// @trace_id_sr=SR_PHM_01021,SR_PHM_01022,SR_PHM_01023
    /// @needwork = ad
    std::set< ara::core::String > GetProcess() const noexcept;

    /// @brief Update process pid and update its state change.
    /// @param processFqn fqn of process.
    /// @param newPid new pid of process.
    /// @trace_id_sr=SR_PHM_01021,SR_PHM_01022,SR_PHM_01023
    /// @needwork = ad
    void UpdateProcessPidAndStateChange(ara::core::String const& processFqn, uint32_t const newPid) noexcept;

private:
    /// @brief returns the state change string.
    /// @param stateChange enum state change.
    /// @return the state change string.
    /// @trace_id_sr=SR_PHM_01021,SR_PHM_01022,SR_PHM_01023
    /// @needwork = dda
    static ara::core::StringView GetProcessStateChangeString(ProcessStateChange const& stateChange) noexcept;

private:
    /// @brief name of function group.
    /// @trace_id_sr=SR_PHM_01021,SR_PHM_01022,SR_PHM_01023
    /// @needwork = dda
    ara::core::String const kFgName;

    /// @brief current state of function group.
    /// @trace_id_sr=SR_PHM_01021,SR_PHM_01022,SR_PHM_01023
    /// @needwork = dda
    ara::core::String fgState_;

    /// @brief container of SupervisionMode <fg state, supervision mode>.
    /// @trace_id_sr=SR_PHM_01021,SR_PHM_01022,SR_PHM_01023
    /// @needwork = dda
    ara::core::Map< ara::core::String, std::shared_ptr< SupervisionMode > > supervisionMode_;

    /// @brief <process fqn, pid>
    /// @trace_id_sr=SR_PHM_01021,SR_PHM_01022,SR_PHM_01023
    /// @needwork = dda
    ara::core::Map< ara::core::String, uint32_t > processPidMap_;

    /// @brief processs fqn of this function group.
    /// @trace_id_sr=SR_PHM_01021,SR_PHM_01022,SR_PHM_01023
    /// @needwork = dda
    std::set< ara::core::String > process_;

    /// @brief <process fqn, state change of process>
    /// @todo Use StringView to save memory
    /// @trace_id_sr=SR_PHM_01021,SR_PHM_01022,SR_PHM_01023
    /// @needwork = dda
    ara::core::Map< ara::core::String, ProcessStateChange > processStateChangeMap_;
};

}  // namespace internal
}  // namespace phm
}  // namespace ara

#endif  // ARA_PHM_INTERNAL_SVMANAGER_FG_SUPERVISION_MODE_H_