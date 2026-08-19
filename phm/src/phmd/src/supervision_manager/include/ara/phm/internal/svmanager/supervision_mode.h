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
/// @file       supervision_mode.h
/// @brief      A supervision mode represents a state of function group and activated base supervisions in this state.
/// @details
/// @date       2024-06-06
/// @author     wangyanlong
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/PlatformHealthManagement/SupervisionManager
/// @unit_description=A supervision mode represents a state of function group and activated base supervisions in this state.
/// @trace_id_sr=SR_PHM_01021,SR_PHM_01022,SR_PHM_01023
/// @unit_name=SupervisionMode
/// @interface_level=unit
/// @endcode
///
/// ================================================================

#ifndef ARA_PHM_INTERNAL_SVMANAGER_SUPERVISION_MODE_H_
#define ARA_PHM_INTERNAL_SVMANAGER_SUPERVISION_MODE_H_

#include <ara/core/map.h>
#include <ara/core/string.h>
#include <ara/core/vector.h>

#include <set>

namespace ara {
namespace phm {
namespace internal {

/// @brief Manages supervision mode.
/// @trace_id_sr=SR_PHM_01021,SR_PHM_01022,SR_PHM_01023
/// @needwork = ad
class SupervisionMode final
{
public:
    /// @brief Constructor.
    /// @param fgName name of function group.
    /// @param fgState state of function group.
    /// @param baseSupervisionFqn fqns of base supervision activated in this supervision mode.
    /// @param processInfos process info in this function group state.
    /// @trace_id_sr=SR_PHM_01021,SR_PHM_01022,SR_PHM_01023
    /// @needwork = ad
    SupervisionMode(ara::core::String fgName,
                    ara::core::String fgState,
                    ara::core::Vector< ara::core::String > const& baseSupervisionFqn,
                    ara::core::Map< ara::core::String, int32_t > const& processInfos) noexcept;

    /// @brief Destructor.
    /// @throws QAC
    /// @trace_id_sr=SR_PHM_01021,SR_PHM_01022,SR_PHM_01023
    /// @needwork = ad
    ~SupervisionMode() = default;

    /// @brief The copy constructor shall not be used.
    /// @param obj The object to be copied.
    /// @trace_id_sr=SR_PHM_01021,SR_PHM_01022,SR_PHM_01023
    /// @needwork = ad
    SupervisionMode(SupervisionMode& obj) = delete;

    /// @brief The copy assignment shall not be used.
    /// @param obj The object to be copied.
    /// @return The copied object.
    /// @trace_id_sr=SR_PHM_01021,SR_PHM_01022,SR_PHM_01023
    /// @needwork = ad
    SupervisionMode& operator=(SupervisionMode const& obj) = delete;

    /// @brief The move constructor shall not be used.
    /// @param obj The object to be moved.
    /// @trace_id_sr=SR_PHM_01021,SR_PHM_01022,SR_PHM_01023
    /// @needwork = ad
    SupervisionMode(SupervisionMode&& obj) = delete;

    /// @brief The move assignment shall not be used.
    /// @param obj The object to be moved.
    /// @return The moved object.
    /// @trace_id_sr=SR_PHM_01021,SR_PHM_01022,SR_PHM_01023
    /// @needwork = ad
    SupervisionMode& operator=(SupervisionMode const&& obj) = delete;

    /// @brief Returns fqn of supervisions activated in this supervison mode.
    /// @return fqn of supervisions activated in this supervison mode.
    /// @trace_id_sr=SR_PHM_01021,SR_PHM_01022,SR_PHM_01023
    /// @needwork = ad
    std::set< ara::core::String > GetSupervisions() const noexcept;

    /// @brief Returns execution error code of process running in this supervision mode.
    /// @param processName name of process running in this supervision mode.
    /// @return execution error code of process running in this supervision mode.
    /// @trace_id_sr=SR_PHM_01021,SR_PHM_01022,SR_PHM_01023
    /// @needwork = ad
    int32_t GetProcessExecutionError(ara::core::String const& processName) noexcept;

    /// @brief Returns state of function group of supervision mode.
    /// @return state of function group of supervision mode.
    /// @trace_id_sr=SR_PHM_01021,SR_PHM_01022,SR_PHM_01023
    /// @needwork = ad
    ara::core::String GetFgState() const noexcept;

    /// @brief Returns the process info of this supervision mode.
    /// @return the process info of this supervision mode.
    /// @trace_id_sr=SR_PHM_01021,SR_PHM_01022,SR_PHM_01023
    /// @needwork = ad
    ara::core::Map< ara::core::String, int32_t > GetProcessInfo() const noexcept;

private:
    /// @brief name of function group.
    /// @trace_id_sr=SR_PHM_01021,SR_PHM_01022,SR_PHM_01023
    /// @needwork = dda
    ara::core::String const kFgName;

    /// @brief state of function group.
    /// @trace_id_sr=SR_PHM_01021,SR_PHM_01022,SR_PHM_01023
    /// @needwork = dda
    ara::core::String const kFgState;

    /// @brief fqn of active supervisions in this function group state.
    // @todo
    /// @trace_id_sr=SR_PHM_01021,SR_PHM_01022,SR_PHM_01023
    /// @needwork = dda
    std::set< ara::core::String > supervisionsFqn_;

    /// @brief process info in this function group state.
    /// @trace_id_sr=SR_PHM_01021,SR_PHM_01022,SR_PHM_01023
    /// @needwork = dda
    ara::core::Map< ara::core::String, int32_t > processInfo_;
};

}  // namespace internal
}  // namespace phm
}  // namespace ara

#endif  // ARA_PHM_INTERNAL_SVMANAGER_SUPERVISION_MODE_H_