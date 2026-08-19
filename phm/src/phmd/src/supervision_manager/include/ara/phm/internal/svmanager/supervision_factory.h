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
/// @file       supervision_factory.h
/// @brief      used to create objects of supervision such as AliveSupervision、DeadlineSupervision.
/// @details
/// @date       2024-06-12
/// @author     wangyanlong
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/PlatformHealthManagement/SupervisionManager
/// @unit_description=used to create objects of supervision such as AliveSupervision、DeadlineSupervision.
/// @trace_id_sr=SR_PHM_01002,SR_PHM_01004,SR_PHM_01006,SR_PHM_01009,SR_PHM_010011,SR_PHM_010021
/// @unit_name=SupervisionFactory
/// @interface_level=none
/// @endcode
///
/// ================================================================

#ifndef ARA_PHM_INTERNAL_SVMANAGER_SUPERVISION_FACTORY_H_
#define ARA_PHM_INTERNAL_SVMANAGER_SUPERVISION_FACTORY_H_

#include <ara/core/vector.h>

#include "ara/phm/internal/config_struct.h"
#include "ara/phm/internal/svmanager/alive_supervision.h"
#include "ara/phm/internal/svmanager/base_supervision.h"
#include "ara/phm/internal/svmanager/deadline_supervision.h"
#include "ara/phm/internal/svmanager/fg_supervision_mode.h"
#include "ara/phm/internal/svmanager/global_supervision.h"
#include "ara/phm/internal/svmanager/local_supervision.h"
#include "ara/phm/internal/svmanager/logical_supervision.h"
#include "ara/phm/internal/svmanager/supervision_checkpoint.h"
#include "ara/phm/internal/svmanager/supervision_mode.h"

namespace ara {
namespace phm {
namespace internal {

/// @brief Used to create objects of supervision such as AliveSupervision、DeadlineSupervision.
/// @trace_id_sr=SR_PHM_01002,SR_PHM_01004,SR_PHM_01006,SR_PHM_01009,SR_PHM_010011,SR_PHM_010021
/// @needwork = ad
class SupervisionFactory final
{
public:
    /// @brief Creation of SupervisionFactory, unique singleton.
    /// @return unique ptr of SupervisionFactory.
    /// @trace_id_sr=SR_PHM_01002,SR_PHM_01004,SR_PHM_01006,SR_PHM_01009,SR_PHM_010011,SR_PHM_010021
    /// @needwork = ad
    static std::unique_ptr< SupervisionFactory > GetInstanceUnique()
    {
        static std::unique_ptr< SupervisionFactory > s_Instance{nullptr};
        static std::once_flag s_Flag{};
        std::call_once(s_Flag, []() { s_Instance.reset(new SupervisionFactory()); });
        std::ignore = s_Flag;

        /// Exclusive singleton, only allowed to be obtained once. On the second call, the instance is null
        assert(s_Instance);
        return std::move(s_Instance);
    }

    /// @brief Destructor.
    /// @throws QAC
    /// @trace_id_sr=SR_PHM_01002,SR_PHM_01004,SR_PHM_01006,SR_PHM_01009,SR_PHM_010011,SR_PHM_010021
    /// @needwork = ad
    ~SupervisionFactory() = default;

    /// @brief The copy constructor shall not be used.
    /// @param obj The object to be copied.
    /// @trace_id_sr=SR_PHM_01002,SR_PHM_01004,SR_PHM_01006,SR_PHM_01009,SR_PHM_010011,SR_PHM_010021
    /// @needwork = ad
    SupervisionFactory(SupervisionFactory& obj) = delete;

    /// @brief The copy assignment shall not be used.
    /// @param obj The object to be copied.
    /// @return The copied object.
    /// @trace_id_sr=SR_PHM_01002,SR_PHM_01004,SR_PHM_01006,SR_PHM_01009,SR_PHM_010011,SR_PHM_010021
    /// @needwork = ad
    SupervisionFactory& operator=(SupervisionFactory const& obj) = delete;

    /// @brief The move constructor shall not be used.
    /// @param obj The object to be moved.
    /// @trace_id_sr=SR_PHM_01002,SR_PHM_01004,SR_PHM_01006,SR_PHM_01009,SR_PHM_010011,SR_PHM_010021
    /// @needwork = ad
    SupervisionFactory(SupervisionFactory&& obj) = delete;

    /// @brief The move assignment shall not be used.
    /// @param obj The object to be moved.
    /// @return SupervisionFactory& the moved object.
    /// @trace_id_sr=SR_PHM_01002,SR_PHM_01004,SR_PHM_01006,SR_PHM_01009,SR_PHM_010011,SR_PHM_010021
    /// @needwork = ad
    SupervisionFactory& operator=(SupervisionFactory const&& obj) = delete;

    /// @brief Create SupervisionCheckpoint objects base on conf.
    /// @param checkpointConf conf of checkpoint.
    /// @return objects of SupervisionCheckpoint.
    /// @trace_id_sr=SR_PHM_01002,SR_PHM_01004,SR_PHM_01006,SR_PHM_01009,SR_PHM_010011,SR_PHM_010021
    /// @needwork = ad
    static ara::core::Vector< std::shared_ptr< SupervisionCheckpoint > > MakeCheckpoint(
        ara::core::Vector< CheckpointConf > const& checkpointConf) noexcept;

    /// @brief Create AliveSupervision objects base on conf.
    /// @param aliveSupervisionConf conf of alive supervision.
    /// @param checkpoint The container of checkpoint, find checkpoint referenced by AliveSupervision in it.
    /// @return objects of AliveSupervision.
    /// @trace_id_sr=SR_PHM_01002,SR_PHM_01004,SR_PHM_01006,SR_PHM_01009,SR_PHM_010011,SR_PHM_010021
    /// @needwork = ad
    static ara::core::Vector< std::shared_ptr< AliveSupervision > > MakeAliveSupervision(
        ara::core::Vector< AliveSupervisionConf > const& aliveSupervisionConf,
        ara::core::Map< ara::core::String, std::shared_ptr< SupervisionCheckpoint > >& checkpoint) noexcept;

    /// @brief Create DeadlineSupervision objects base on conf.
    /// @param deadlineSupervisionConf conf of deadline supervision.
    /// @param checkpoint The container of checkpoint, find checkpoint referenced by DeadlineSupervision in it.
    /// @return objects of DeadlineSupervision.
    /// @trace_id_sr=SR_PHM_01002,SR_PHM_01004,SR_PHM_01006,SR_PHM_01009,SR_PHM_010011,SR_PHM_010021
    /// @needwork = ad
    static ara::core::Vector< std::shared_ptr< DeadlineSupervision > > MakeDeadlineSupervision(
        ara::core::Vector< DeadlineSupervisionConf > const& deadlineSupervisionConf,
        ara::core::Map< ara::core::String, std::shared_ptr< SupervisionCheckpoint > >& checkpoint) noexcept;

    /// @brief Create LogicalSupervision objects base on LogicalSupervisionConf.
    /// @param logicalSupervisionConf conf of logical supervision
    /// @param checkpoint The container of checkpoint, find checkpoint referenced by LogicalSupervision in it.
    /// @return objects of LogicalSupervision.
    /// @trace_id_sr=SR_PHM_01002,SR_PHM_01004,SR_PHM_01006,SR_PHM_01009,SR_PHM_010011,SR_PHM_010021
    /// @needwork = ad
    static ara::core::Vector< std::shared_ptr< LogicalSupervision > > MakeLogicalSupervision(
        ara::core::Vector< LogicalSupervisionConf > const& logicalSupervisionConf,
        ara::core::Map< ara::core::String, std::shared_ptr< SupervisionCheckpoint > >& checkpoint) noexcept;

    /// @brief  Create LocalSupervision objects base on conf.
    /// @param localSupervisionConf conf of local supervision.
    /// @param baseSupervision The container of base supervision, find base supervision referenced by
    /// local supervision in it.
    /// @param baseLocalMap key is fqn of base supervision, value is local supervision.
    /// @return objects of LocalSupervision.
    /// @trace_id_sr=SR_PHM_01002,SR_PHM_01004,SR_PHM_01006,SR_PHM_01009,SR_PHM_010011,SR_PHM_010021
    /// @needwork = ad
    static ara::core::Vector< std::shared_ptr< LocalSupervision > > MakeLocalSupervision(
        ara::core::Vector< LocalSupervisionConf > const& localSupervisionConf,
        ara::core::Map< ara::core::String, std::shared_ptr< BaseSupervision > >& baseSupervision,
        ara::core::Map< ara::core::String, ara::core::Vector< std::shared_ptr< LocalSupervision > > >&
            baseLocalMap) noexcept;

    /// @brief Create GlobalSupervision objects base on GlobalSupervisionConf.
    /// @param globalSupervisionConf conf of global supervision.
    /// @param localSupervision The container of local supervision, find local supervision referenced by
    /// global supervision in it.
    /// @return objects of GlobalSupervision.
    /// @trace_id_sr=SR_PHM_01002,SR_PHM_01004,SR_PHM_01006,SR_PHM_01009,SR_PHM_010011,SR_PHM_010021
    /// @needwork = ad
    static ara::core::Vector< std::shared_ptr< GlobalSupervision > > MakeGlobalSupervision(
        ara::core::Vector< GlobalSupervisionConf > const& globalSupervisionConf,
        ara::core::Map< ara::core::String, std::shared_ptr< LocalSupervision > >& localSupervision) noexcept;

    /// @brief Create FgSupervisionMode objects base on FgSupervisionModeConf.
    /// @param fgSupervisionModeConf conf of fg supervision mode
    /// @return FgSupervisionMode objects
    /// @trace_id_sr=SR_PHM_01002,SR_PHM_01004,SR_PHM_01006,SR_PHM_01009,SR_PHM_010011,SR_PHM_010021
    /// @needwork = ad
    static ara::core::Vector< std::shared_ptr< FgSupervisionMode > > MakeSupervisionMode(
        ara::core::Vector< FgSupervisionModeConf > const& fgSupervisionModeConf) noexcept;

private:
    /// @brief Default constructor.
    /// @trace_id_sr=SR_PHM_01002,SR_PHM_01004,SR_PHM_01006,SR_PHM_01009,SR_PHM_010011,SR_PHM_010021
    /// @needwork = dda
    SupervisionFactory() = default;
};

}  // namespace internal
}  // namespace phm
}  // namespace ara

#endif  // ARA_PHM_INTERNAL_SVMANAGER_SUPERVISION_FACTORY_H_