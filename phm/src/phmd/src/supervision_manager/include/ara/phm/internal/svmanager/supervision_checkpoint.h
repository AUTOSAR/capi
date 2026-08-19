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
/// @file       supervision_checkpoint.h
/// @brief      Checkpoint, A point in the control flow of a Supervised Entity where the activity is reported.
/// @details
/// @date       2024-06-06
/// @author     wangyanlong
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/PlatformHealthManagement/SupervisionManager
/// @unit_description=Checkpoint, A point in the control flow of a Supervised Entity where the activity is reported.
/// @trace_id_sr=SR_PHM_01001
/// @unit_name=SupervisionCheckpoint
/// @interface_level=unit
/// @endcode
///
/// ================================================================

#ifndef ARA_PHM_INTERNAL_SVMANAGER_SUPERVISION_CHECKPOINT_H_
#define ARA_PHM_INTERNAL_SVMANAGER_SUPERVISION_CHECKPOINT_H_

#include <ara/core/string.h>

#include <memory>

#include "ara/phm/internal/com/sv_message.h"
#include "ara/phm/internal/config_struct.h"
#include "ara/phm/internal/types.h"

namespace ara {
namespace phm {
namespace internal {

/// @brief Manage supervision checkpoint.
/// @trace_id_sr=SR_PHM_01001
/// @needwork = ad
class SupervisionCheckpoint final
{
public:
    /// @brief Creation of a superviusion checkpoint.
    /// @param conf conf of a supervision checkpoint.
    /// @trace_id_sr=SR_PHM_01001
    /// @needwork = ad
    explicit SupervisionCheckpoint(CheckpointConf const& conf) noexcept;

    /// @brief Destructor of superviusion checkpoint.
    /// @throws QAC
    /// @trace_id_sr=SR_PHM_01001
    /// @needwork = ad
    ~SupervisionCheckpoint() = default;

    /// @brief The copy constructor shall not be used.
    /// @param obj The object to be copied
    /// @trace_id_sr=SR_PHM_01001
    /// @needwork = ad
    SupervisionCheckpoint(SupervisionCheckpoint& obj) = delete;

    /// @brief The move assignment shall not be used.
    /// @param obj The object to be copied.
    /// @return The copied object.
    /// @trace_id_sr=SR_PHM_01001
    /// @needwork = ad
    SupervisionCheckpoint& operator=(SupervisionCheckpoint const& obj) = delete;

    /// @brief The move constructor shall not be used.
    /// @param obj The object to be moved.
    /// @trace_id_sr=SR_PHM_01001
    /// @needwork = ad
    SupervisionCheckpoint(SupervisionCheckpoint&& obj) = delete;

    /// @brief The move assignment shall not be used.
    /// @param obj The object to be moved.
    /// @return the moved object.
    /// @trace_id_sr=SR_PHM_01001
    /// @needwork = ad
    SupervisionCheckpoint& operator=(SupervisionCheckpoint const&& obj) = delete;

    /// @brief == operator of SupervisionCheckpoint.
    /// @param left left object
    /// @param right right object
    /// @return true, equal;false, not equal
    /// @trace_id_sr=SR_PHM_01001
    /// @needwork = ad
    friend bool operator==(SupervisionCheckpoint const& left, SupervisionCheckpoint const& right) noexcept;

    /// @brief != operator of SupervisionCheckpoint.
    /// @param left left object
    /// @param right right object
    /// @return true, not equal;false, equal
    /// @trace_id_sr=SR_PHM_01001
    /// @needwork = ad
    friend bool operator!=(SupervisionCheckpoint const& left, SupervisionCheckpoint const& right) noexcept;

    /// @brief < operator of SupervisionCheckpoint.
    /// @param left left object
    /// @param right right object
    /// @return true, left < right;false, left >= right
    /// @trace_id_sr=SR_PHM_01001
    /// @needwork = ad
    friend bool operator<(SupervisionCheckpoint const& left, SupervisionCheckpoint const& right) noexcept;

    /// @brief Returns name of checkpoint.
    /// @return name of checkpoint.
    /// @trace_id_sr=SR_PHM_01001
    /// @needwork = ad
    ara::core::String GetShortName() const noexcept;

    /// @brief Returns identifier of checkpoint.
    /// @return identifier of checkpoint.
    /// @trace_id_sr=SR_PHM_01001
    /// @needwork = ad
    Specifier GetIdentifier() const noexcept;

    /// @brief Returns checkpoint id.
    /// @return checkpoint id.
    /// @trace_id_sr=SR_PHM_01001
    /// @needwork = ad
    uint32_t GetCheckpointId() const noexcept;

    /// @brief Returns the name of process checkpoint belongs to.
    /// @return the name of process checkpoint belongs to.
    /// @trace_id_sr=SR_PHM_01001
    /// @needwork = ad
    ara::core::String GetProcessName() const noexcept;

    /// @brief Returns the fqn of checkpoint.
    /// @return the fqn of checkpoint.
    /// @trace_id_sr=SR_PHM_01001
    /// @needwork = ad
    ara::core::String GetFqn() const noexcept;

    /// @brief set process cluster affiliation.
    /// @param processClusterAffiliation process cluster affiliation.
    /// @trace_id_sr=SR_PHM_01001
    /// @needwork = no
    void SetProcessClusterAffiliation(
        ara::phm::internal::svcom::ProcessClusterAffiliation const processClusterAffiliation) noexcept;

    /// @brief returns process cluster affiliation.
    /// @return process cluster affiliation.
    /// @trace_id_sr=SR_PHM_01001
    /// @needwork = no
    ara::phm::internal::svcom::ProcessClusterAffiliation GetProcessClusterAffiliation() const noexcept;

private:
    /// @brief name of checkpoint.
    /// @trace_id_sr=SR_PHM_01001
    /// @needwork = dda
    ara::core::String const kShortName;

    /// @brief identifier of checkpoint.
    /// @trace_id_sr=SR_PHM_01001
    /// @needwork = dda
    Specifier const kIdentifier;

    /// @brief fqn of checkpoint.
    /// @trace_id_sr=SR_PHM_01001
    /// @needwork = dda
    ara::core::String const kFqn;

    /// @brief id of checkpoint.
    /// @trace_id_sr=SR_PHM_01001
    /// @needwork = dda
    uint32_t const kCheckpointId;

    /// @brief the name of process this checkpoint belongs to.
    /// @trace_id_sr=SR_PHM_01001
    /// @needwork = dda
    ara::core::String const kProcessName;

    /// @brief process cluster affiliation.
    /// @trace_id_sr=SR_PHM_01001
    /// @needwork = no
    ara::phm::internal::svcom::ProcessClusterAffiliation processClusterAffiliation_;
};

}  // namespace internal
}  // namespace phm
}  // namespace ara

#endif  // ARA_PHM_INTERNAL_SVMANAGER_SUPERVISION_CHECKPOINT_H_