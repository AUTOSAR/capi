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
/// @file       health_channel_status.h
/// @brief      represents a status of health channel.
/// @details
/// @date       2024-06-20
/// @author     wangyanlong
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/PlatformHealthManagement/HealthChannelManager
/// @unit_description=represents a status of health channel.
/// @trace_id_sr=SR_PHM_01024,SR_PHM_01025,SR_PHM_01026,SR_PHM_01028,SR_PHM_01029,SR_PHM_01030,SR_PHM_01031
/// @unit_name=HealthChannelStatus
/// @interface_level=unit
/// @endcode
///
/// ================================================================

#ifndef ARA_PHM_INTERNAL_HEALTH_CHANNEL_STATUS_H_
#define ARA_PHM_INTERNAL_HEALTH_CHANNEL_STATUS_H_

#include <ara/core/string.h>

#include "ara/phm/internal/phm_log.h"
#include "ara/phm/internal/types.h"

namespace ara {
namespace phm {
namespace internal {

/// @brief Manages a health status of health channel.
/// @trace_id_sr=SR_PHM_01024,SR_PHM_01025,SR_PHM_01026,SR_PHM_01028,SR_PHM_01029,SR_PHM_01030,SR_PHM_01031
/// @needwork = ad
class HealthChannelStatus
{
public:
    /// @brief Creation of a HealthChannelStatus.
    /// @param name name of health status.
    /// @param id id of health status.
    /// @param trigger flag about whether this status need recover.
    /// @trace_id_sr=SR_PHM_01024,SR_PHM_01025,SR_PHM_01026,SR_PHM_01028,SR_PHM_01029,SR_PHM_01030,SR_PHM_01031
    /// @needwork = ad
    HealthChannelStatus(ara::core::String name, HealthStatus const id, bool const trigger) noexcept
        : kName{std::move(name)}, kStatusId{id}, kTrigger{trigger}
    {
        LOG_INFO << "create HealthChannelStatus, name:" << kName << ",id:" << kStatusId << ",trigger_:" << kTrigger;
    }

    /// @brief Default deconstructor.
    /// @throws QAC
    /// @trace_id_sr=SR_PHM_01024,SR_PHM_01025,SR_PHM_01026,SR_PHM_01028,SR_PHM_01029,SR_PHM_01030,SR_PHM_01031
    /// @needwork = ad
    ~HealthChannelStatus() = default;

    /// @brief The copy constructor shall not be used.
    /// @param obj The object to be copied.
    /// @trace_id_sr=SR_PHM_01024,SR_PHM_01025,SR_PHM_01026,SR_PHM_01028,SR_PHM_01029,SR_PHM_01030,SR_PHM_01031
    /// @needwork = ad
    HealthChannelStatus(HealthChannelStatus& obj) = delete;

    /// @brief The copy assignment shall not be used.
    /// @param obj The object to be copied.
    /// @return The copied object.
    /// @trace_id_sr=SR_PHM_01024,SR_PHM_01025,SR_PHM_01026,SR_PHM_01028,SR_PHM_01029,SR_PHM_01030,SR_PHM_01031
    /// @needwork = ad
    HealthChannelStatus& operator=(HealthChannelStatus const& obj) = delete;

    /// @brief The move constructor shall not be used.
    /// @param obj The moved object.
    /// @trace_id_sr=SR_PHM_01024,SR_PHM_01025,SR_PHM_01026,SR_PHM_01028,SR_PHM_01029,SR_PHM_01030,SR_PHM_01031
    /// @needwork = ad
    HealthChannelStatus(HealthChannelStatus&& obj) = delete;

    /// @brief The move assignment for HealthChannelStatus shall not be used.
    /// @param obj The object to be moved.
    /// @return The moved object.
    /// @trace_id_sr=SR_PHM_01024,SR_PHM_01025,SR_PHM_01026,SR_PHM_01028,SR_PHM_01029,SR_PHM_01030,SR_PHM_01031
    /// @needwork = ad
    HealthChannelStatus& operator=(HealthChannelStatus const&& obj) = delete;

    /// @brief Returns name of health status.
    /// @return name of health status.
    /// @return id of health status.
    /// @trace_id_sr=SR_PHM_01024,SR_PHM_01025,SR_PHM_01026,SR_PHM_01028,SR_PHM_01029,SR_PHM_01030,SR_PHM_01031
    /// @needwork = ad
    ara::core::String GetName() const noexcept { return kName; }

    /// @brief Returns id of health status.
    /// @return id of health status.
    /// @trace_id_sr=SR_PHM_01024,SR_PHM_01025,SR_PHM_01026,SR_PHM_01028,SR_PHM_01029,SR_PHM_01030,SR_PHM_01031
    /// @needwork = ad
    HealthStatus GetStatusId() const noexcept { return kStatusId; }

    /// @brief Returns flag about whether this status need recover.
    /// @return flag about whether this status need recover.
    /// @trace_id_sr=SR_PHM_01024,SR_PHM_01025,SR_PHM_01026,SR_PHM_01028,SR_PHM_01029,SR_PHM_01030,SR_PHM_01031
    /// @needwork = ad
    bool GetTrigger() const noexcept { return kTrigger; }

private:
    /// @brief name of health status.
    /// @trace_id_sr=SR_PHM_01024,SR_PHM_01025,SR_PHM_01026,SR_PHM_01028,SR_PHM_01029,SR_PHM_01030,SR_PHM_01031
    /// @needwork = dda
    ara::core::String const kName;

    /// @brief id of health status.
    /// @trace_id_sr=SR_PHM_01024,SR_PHM_01025,SR_PHM_01026,SR_PHM_01028,SR_PHM_01029,SR_PHM_01030,SR_PHM_01031
    /// @needwork = dda
    HealthStatus const kStatusId;

    /// @brief flag about whether this status need recover.
    /// @trace_id_sr=SR_PHM_01024,SR_PHM_01025,SR_PHM_01026,SR_PHM_01028,SR_PHM_01029,SR_PHM_01030,SR_PHM_01031
    /// @needwork = dda
    bool const kTrigger;
};

}  // namespace internal
}  // namespace phm
}  // namespace ara

#endif  // ARA_PHM_INTERNAL_HEALTH_CHANNEL_STATUS_H_
