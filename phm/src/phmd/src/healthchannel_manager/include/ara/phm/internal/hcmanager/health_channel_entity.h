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
/// @file       health_channel_entity.h
/// @brief      represents a health channel.
/// @details
/// @date       2024-06-20
/// @author     wangyanlong
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/PlatformHealthManagement/HealthChannelManager
/// @unit_description=represents a health channel.
/// @trace_id_sr=SR_PHM_01024,SR_PHM_01025,SR_PHM_01026,SR_PHM_01028,SR_PHM_01029,SR_PHM_01030,SR_PHM_01031
/// @unit_name=HealthChannelEntity
/// @interface_level=unit
/// @endcode
///
/// ================================================================

#ifndef ARA_PHM_INTERNAL_HEALTH_CHANNEL_ENTITY_H_
#define ARA_PHM_INTERNAL_HEALTH_CHANNEL_ENTITY_H_

#include <ara/core/map.h>

#include "ara/phm/internal/hcmanager/health_channel_status.h"
#include "ara/phm/internal/phm_log.h"

namespace ara {
namespace phm {
namespace internal {

/// @brief Manages a health channel.
/// @trace_id_sr=SR_PHM_01024,SR_PHM_01025,SR_PHM_01026,SR_PHM_01028,SR_PHM_01029,SR_PHM_01030,SR_PHM_01031
/// @needwork = ad
class HealthChannelEntity final
{
public:
    /// @brief Creation of a HealthChannelEntity.
    /// @param name name of health channel.
    /// @param healthChannelStatus objects of health channel status belongs to health channel.
    /// @trace_id_sr=SR_PHM_01024,SR_PHM_01025,SR_PHM_01026,SR_PHM_01028,SR_PHM_01029,SR_PHM_01030,SR_PHM_01031
    /// @needwork = ad
    HealthChannelEntity(
        ara::core::String name,
        ara::core::Vector< std::shared_ptr< HealthChannelStatus > > const& healthChannelStatus) noexcept;

    /// @brief Destructor of HealthChannelEntity.
    /// @throws QAC
    /// @trace_id_sr=SR_PHM_01024,SR_PHM_01025,SR_PHM_01026,SR_PHM_01028,SR_PHM_01029,SR_PHM_01030,SR_PHM_01031
    /// @needwork = ad
    ~HealthChannelEntity() = default;

    /// @brief The copy assignment shall not be used.
    /// @param obj The object to be copied.
    /// @trace_id_sr=SR_PHM_01024,SR_PHM_01025,SR_PHM_01026,SR_PHM_01028,SR_PHM_01029,SR_PHM_01030,SR_PHM_01031
    /// @needwork = ad
    HealthChannelEntity(HealthChannelEntity& obj) = delete;

    /// @brief The copy assignment shall not be used.
    /// @param obj The object to be copied.
    /// @return The copied object.
    /// @trace_id_sr=SR_PHM_01024,SR_PHM_01025,SR_PHM_01026,SR_PHM_01028,SR_PHM_01029,SR_PHM_01030,SR_PHM_01031
    /// @needwork = ad
    HealthChannelEntity& operator=(HealthChannelEntity const& obj) = delete;

    /// @brief The move constructor shall not be used.
    /// @param obj The object to be moved.
    /// @trace_id_sr=SR_PHM_01024,SR_PHM_01025,SR_PHM_01026,SR_PHM_01028,SR_PHM_01029,SR_PHM_01030,SR_PHM_01031
    /// @needwork = ad
    HealthChannelEntity(HealthChannelEntity&& obj) = delete;

    /// @brief The move constructor shall not be used.
    /// @param obj The object to be moved.
    /// @return The moved object.
    /// @trace_id_sr=SR_PHM_01024,SR_PHM_01025,SR_PHM_01026,SR_PHM_01028,SR_PHM_01029,SR_PHM_01030,SR_PHM_01031
    /// @needwork = ad
    HealthChannelEntity& operator=(HealthChannelEntity const&& obj) = delete;

    /// @brief Returns HealthStatus of a health status id.
    /// @param status id of health status.
    /// @return Object of health status.
    /// @trace_id_sr=SR_PHM_01024,SR_PHM_01025,SR_PHM_01026,SR_PHM_01028,SR_PHM_01029,SR_PHM_01030,SR_PHM_01031
    /// @needwork = ad
    std::shared_ptr< HealthChannelStatus > GetHealthChannelStatus(HealthStatus const& status) noexcept;

    /// @brief Returns the name of health channel.
    /// @return the name of health channel.
    /// @trace_id_sr=SR_PHM_01024,SR_PHM_01025,SR_PHM_01026,SR_PHM_01028,SR_PHM_01029,SR_PHM_01030,SR_PHM_01031
    /// @needwork = ad
    ara::core::String GetName() const noexcept;

private:
    /// @brief name of health channel.
    /// @trace_id_sr=SR_PHM_01024,SR_PHM_01025,SR_PHM_01026,SR_PHM_01028,SR_PHM_01029,SR_PHM_01030,SR_PHM_01031
    /// @needwork = dda
    ara::core::String const kName;

    /// @brief container of HealthStatus belong to this health channel.
    /// @trace_id_sr=SR_PHM_01024,SR_PHM_01025,SR_PHM_01026,SR_PHM_01028,SR_PHM_01029,SR_PHM_01030,SR_PHM_01031
    /// @needwork = dda
    ara::core::Map< HealthStatus, std::shared_ptr< HealthChannelStatus > > healthStatus_;
};

}  // namespace internal
}  // namespace phm
}  // namespace ara

#endif