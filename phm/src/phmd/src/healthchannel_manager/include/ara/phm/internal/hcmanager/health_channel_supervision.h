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
/// @file       health_channel_supervision.h
/// @brief      maintains params used when a health status need recover.
/// @details
/// @date       2024-06-20
/// @author     wangyanlong
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/PlatformHealthManagement/HealthChannelManager
/// @unit_description=maintains params used when a health status need recover.
/// @trace_id_sr=SR_PHM_01024,SR_PHM_01025,SR_PHM_01026,SR_PHM_01028,SR_PHM_01029,SR_PHM_01030,SR_PHM_01031
/// @unit_name=HealthChannelSupervision
/// @interface_level=unit
/// @endcode
///
/// ================================================================

#ifndef ARA_PHM_INTERNAL_HEALTH_CHANNEL_SUPERVISION_H_
#define ARA_PHM_INTERNAL_HEALTH_CHANNEL_SUPERVISION_H_

#include "ara/phm/internal/config_struct.h"
#include "ara/phm/internal/hcmanager/health_channel_entity.h"

namespace ara {
namespace phm {
namespace internal {

/// @brief Manages a HealthChannelSupervision.
/// @trace_id_sr=SR_PHM_01024,SR_PHM_01025,SR_PHM_01026,SR_PHM_01028,SR_PHM_01029,SR_PHM_01030,SR_PHM_01031
/// @needwork = ad
class HealthChannelSupervision
{
public:
    /// @brief Creation of a HealthChannelSupervision
    /// @param conf conf of health channel supervision.
    /// @param healthChannelEntity health channel supervised by this health channel supervision.
    /// @trace_id_sr=SR_PHM_01024,SR_PHM_01025,SR_PHM_01026,SR_PHM_01028,SR_PHM_01029,SR_PHM_01030,SR_PHM_01031
    /// @needwork = ad
    HealthChannelSupervision(HealthChannelSupervisionConf const& conf,
                             std::shared_ptr< HealthChannelEntity > healthChannelEntity) noexcept;

    /// @brief Default deconstructor.
    /// @throws QAC
    /// @trace_id_sr=SR_PHM_01024,SR_PHM_01025,SR_PHM_01026,SR_PHM_01028,SR_PHM_01029,SR_PHM_01030,SR_PHM_01031
    /// @needwork = ad
    ~HealthChannelSupervision() = default;

    /// @brief The move constructor shall not be used.
    /// @param obj The object to be moved.
    /// @trace_id_sr=SR_PHM_01024,SR_PHM_01025,SR_PHM_01026,SR_PHM_01028,SR_PHM_01029,SR_PHM_01030,SR_PHM_01031
    /// @needwork = ad
    HealthChannelSupervision(HealthChannelSupervision&& obj) = delete;

    /// @brief The copy constructor shall not be used.
    /// @param obj The object to be copied.
    /// @trace_id_sr=SR_PHM_01024,SR_PHM_01025,SR_PHM_01026,SR_PHM_01028,SR_PHM_01029,SR_PHM_01030,SR_PHM_01031
    /// @needwork = ad
    HealthChannelSupervision(HealthChannelSupervision& obj) = delete;

    /// @brief The move assignment shall not be used.
    /// @param obj The object to be moved.
    /// @return the moved object.
    /// @trace_id_sr=SR_PHM_01024,SR_PHM_01025,SR_PHM_01026,SR_PHM_01028,SR_PHM_01029,SR_PHM_01030,SR_PHM_01031
    /// @needwork = ad
    HealthChannelSupervision& operator=(HealthChannelSupervision const&& obj) = delete;

    /// @brief The copy assignment shall not be used.
    /// @param obj The object to be copied.
    /// @return The copied object.
    /// @trace_id_sr=SR_PHM_01024,SR_PHM_01025,SR_PHM_01026,SR_PHM_01028,SR_PHM_01029,SR_PHM_01030,SR_PHM_01031
    /// @needwork = ad
    HealthChannelSupervision& operator=(HealthChannelSupervision const& obj) = delete;

    /// @brief Returns flag about whether health status need recover
    /// @param status id of health status
    /// @return true, health status need recover；false, health status does not need recover.
    /// @trace_id_sr=SR_PHM_01024,SR_PHM_01025,SR_PHM_01026,SR_PHM_01028,SR_PHM_01029,SR_PHM_01030,SR_PHM_01031
    /// @needwork = ad
    bool IsHealthStatusNeedRecover(uint32_t const& status) noexcept;

    /// @brief Returns recover timeout.
    /// @return recover timeout.
    /// @trace_id_sr=SR_PHM_01024,SR_PHM_01025,SR_PHM_01026,SR_PHM_01028,SR_PHM_01029,SR_PHM_01030,SR_PHM_01031
    /// @needwork = ad
    int32_t GetRecoveryNotificationTimeout() const noexcept;

    /// @brief Returns max retry.
    /// @return max retry.
    /// @trace_id_sr=SR_PHM_01024,SR_PHM_01025,SR_PHM_01026,SR_PHM_01028,SR_PHM_01029,SR_PHM_01030,SR_PHM_01031
    /// @needwork = ad
    uint32_t GetRecoveryNotificationRetry() const noexcept;

    /// @brief Returns the name of health channel supervision.
    /// @return the name of health channel supervision.
    /// @trace_id_sr=SR_PHM_01024,SR_PHM_01025,SR_PHM_01026,SR_PHM_01028,SR_PHM_01029,SR_PHM_01030,SR_PHM_01031
    /// @needwork = ad
    ara::core::String GetName() const noexcept;

    /// @brief Returns the instance specifier of health channel.
    /// @return the instance specifier of health channel.
    /// @trace_id_sr=SR_PHM_01024,SR_PHM_01025,SR_PHM_01026,SR_PHM_01028,SR_PHM_01029,SR_PHM_01030,SR_PHM_01031
    /// @needwork = ad
    ara::core::String GetHcInterfaceIdentifier() const noexcept;

    /// TODO This needs to be modified later because the meaning is unclear
    /// @brief Returns the meta mode identifier.
    /// @return the meta mode identifier.
    /// @trace_id_sr=SR_PHM_01024,SR_PHM_01025,SR_PHM_01026,SR_PHM_01028,SR_PHM_01029,SR_PHM_01030,SR_PHM_01031
    /// @needwork = ad
    ara::core::String GetMetaModelIdentifier() const noexcept;

    /// @brief Returns the flag of whether this supervision is offereed.
    /// @return the flag of whether this supervision is offereed.
    /// @trace_id_sr=SR_PHM_01024,SR_PHM_01025,SR_PHM_01026,SR_PHM_01028,SR_PHM_01029,SR_PHM_01030,SR_PHM_01031
    /// @needwork = ad
    bool IsOffered() const noexcept;

    /// @brief Set the flag of whether this supervision is offereed.
    /// @param offer true, offered; false not offered.
    /// @trace_id_sr=SR_PHM_01024,SR_PHM_01025,SR_PHM_01026,SR_PHM_01028,SR_PHM_01029,SR_PHM_01030,SR_PHM_01031
    /// @needwork = ad
    void SetOffer(bool const offer) noexcept;

private:
    /// @brief name of HealthChannelSupervision.
    /// @trace_id_sr=SR_PHM_01024,SR_PHM_01025,SR_PHM_01026,SR_PHM_01028,SR_PHM_01029,SR_PHM_01030,SR_PHM_01031
    /// @needwork = dda
    ara::core::String const kName;

    /// @brief id of health channel supervision, instance specifier of HealthChannel.
    /// @trace_id_sr=SR_PHM_01024,SR_PHM_01025,SR_PHM_01026,SR_PHM_01028,SR_PHM_01029,SR_PHM_01030,SR_PHM_01031
    /// @needwork = dda
    ara::core::String const kHcInterfaceIdentifier;

    /// @brief id for HealthChannelManager to com with HealthChannelAction.
    /// @trace_id_sr=SR_PHM_01024,SR_PHM_01025,SR_PHM_01026,SR_PHM_01028,SR_PHM_01029,SR_PHM_01030,SR_PHM_01031
    /// @needwork = dda
    ara::core::String const kMetaModelIdentifier;

    /// @brief recover timeout.
    /// @trace_id_sr=SR_PHM_01024,SR_PHM_01025,SR_PHM_01026,SR_PHM_01028,SR_PHM_01029,SR_PHM_01030,SR_PHM_01031
    /// @needwork = dda
    int32_t const kRecoveryNotificationTimeout;

    /// @brief max retry of recover.
    /// @trace_id_sr=SR_PHM_01024,SR_PHM_01025,SR_PHM_01026,SR_PHM_01028,SR_PHM_01029,SR_PHM_01030,SR_PHM_01031
    /// @needwork = dda
    uint32_t const kRecoveryNotificationRetry;

    /// @brief health channel supervised by this health channel supervision.
    /// @trace_id_sr=SR_PHM_01024,SR_PHM_01025,SR_PHM_01026,SR_PHM_01028,SR_PHM_01029,SR_PHM_01030,SR_PHM_01031
    /// @needwork = dda
    std::shared_ptr< HealthChannelEntity > healthChannelEntity_;

    /// @brief flag about whether this supervision is offered, true, offered; false not offered.
    /// @trace_id_sr=SR_PHM_01024,SR_PHM_01025,SR_PHM_01026,SR_PHM_01028,SR_PHM_01029,SR_PHM_01030,SR_PHM_01031
    /// @needwork = dda
    bool offered_;
};

}  // namespace internal
}  // namespace phm
}  // namespace ara

#endif
