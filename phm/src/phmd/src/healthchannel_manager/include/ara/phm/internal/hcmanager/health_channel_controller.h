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
/// @file       health_channel_controller.h
/// @brief      Manages resources in HealthChannelManager and provide interfaces for other modules.
/// @details
/// @date       2024-06-05
/// @author     wangyanlong
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/PlatformHealthManagement/HealthChannelManager
/// @unit_description=Manages resources in HealthChannelManager and provide interfaces for other modules.
/// @trace_id_sr=SR_PHM_01024,SR_PHM_01025,SR_PHM_01026,SR_PHM_01028,SR_PHM_01029,SR_PHM_01030,SR_PHM_01031
/// @unit_name=HealthChannelController
/// @interface_level=module
/// @endcode
///
/// ================================================================

#ifndef _ARA_PHM_INTERNAL_HEALTH_CHANNEL_CONTROLLER_H_
#define _ARA_PHM_INTERNAL_HEALTH_CHANNEL_CONTROLLER_H_

#include <ara/core/map.h>

#ifdef COMPILE_WITH_IAM
    #include <ara/iam/internal/grantquery/phm.h>
#endif

#include <functional>
#include <memory>

#include "ara/phm/internal/com/hc_server.h"
#include "ara/phm/internal/com/hca_client.h"
#include "ara/phm/internal/com/hcaext_server.h"
#include "ara/phm/internal/config_manager.h"
#include "ara/phm/internal/hcmanager/health_channel_entity.h"
#include "ara/phm/internal/hcmanager/health_channel_supervision.h"

namespace ara {
namespace phm {
namespace internal {

/// @brief Manages resources in HealthChannelManager and provide interfaces for other modules.
/// @trace_id_sr=SR_PHM_01024,SR_PHM_01025,SR_PHM_01026,SR_PHM_01028,SR_PHM_01029,SR_PHM_01030,SR_PHM_01031
/// @needwork = ad
class HealthChannelController final
{
public:
    /// @brief Creation of HealthChannelController, unique singleton.
    /// @return unique ptr of HealthChannelController.
    /// @trace_id_sr=SR_PHM_01024,SR_PHM_01025,SR_PHM_01026,SR_PHM_01028,SR_PHM_01029,SR_PHM_01030,SR_PHM_01031
    /// @needwork = ad
    static std::unique_ptr< HealthChannelController > GetInstanceUnique()
    {
        static std::unique_ptr< HealthChannelController > s_Instance{nullptr};
        static std::once_flag s_Flag{};
        std::call_once(s_Flag, []() { s_Instance.reset(new HealthChannelController()); });
        std::ignore = s_Flag;

        /// Exclusive singleton, only allowed to be obtained once
        assert(s_Instance);
        return std::move(s_Instance);
    }

    /// @brief Default deconstructor.
    /// @trace_id_sr=SR_PHM_01024,SR_PHM_01025,SR_PHM_01026,SR_PHM_01028,SR_PHM_01029,SR_PHM_01030,SR_PHM_01031
    /// @needwork = ad
    ~HealthChannelController() noexcept;

    /// @brief The copy constructor shall not be used.
    /// @param obj The object to be copied.
    /// @trace_id_sr=SR_PHM_01024,SR_PHM_01025,SR_PHM_01026,SR_PHM_01028,SR_PHM_01029,SR_PHM_01030,SR_PHM_01031
    /// @needwork = ad
    HealthChannelController(HealthChannelController& obj) = delete;

    /// @brief The move assignment shall not be used.
    /// @param obj The object to be copied.
    /// @return The copied object.
    /// @trace_id_sr=SR_PHM_01024,SR_PHM_01025,SR_PHM_01026,SR_PHM_01028,SR_PHM_01029,SR_PHM_01030,SR_PHM_01031
    /// @needwork = ad
    HealthChannelController& operator=(HealthChannelController const& obj) = delete;

    /// @brief The move constructor shall not be used.
    /// @param obj The object to be moved.
    /// @trace_id_sr=SR_PHM_01024,SR_PHM_01025,SR_PHM_01026,SR_PHM_01028,SR_PHM_01029,SR_PHM_01030,SR_PHM_01031
    /// @needwork = ad
    HealthChannelController(HealthChannelController&& obj) = delete;

    /// @brief The move assignment shall not be used.
    /// @param obj The object to be moved.
    /// @return the copied object.
    /// @trace_id_sr=SR_PHM_01024,SR_PHM_01025,SR_PHM_01026,SR_PHM_01028,SR_PHM_01029,SR_PHM_01030,SR_PHM_01031
    /// @needwork = ad
    HealthChannelController& operator=(HealthChannelController const&& obj) = delete;

    /// @brief Init health channel controller.
    /// @return 0, success
    /// @return < 0, fail
    /// @trace_id_sr=SR_PHM_01024,SR_PHM_01025,SR_PHM_01026,SR_PHM_01028,SR_PHM_01029,SR_PHM_01030,SR_PHM_01031
    /// @needwork = ad
    int32_t Init() noexcept;

    /// @brief Close com handles.
    /// @return 0, success
    /// @return < 0, fail
    /// @trace_id_sr=SR_PHM_01024,SR_PHM_01025,SR_PHM_01026,SR_PHM_01028,SR_PHM_01029,SR_PHM_01030,SR_PHM_01031
    /// @needwork = ad
    int32_t Stop() const noexcept;

    /// @brief Used by PhmContext to register a function, HealthChannelController can use this function to trigger
    /// watchdog.
    /// @param trigger a callback used by health channel controller to trigger watchdog
    /// @trace_id_sr=SR_PHM_01024,SR_PHM_01025,SR_PHM_01026,SR_PHM_01028,SR_PHM_01029,SR_PHM_01030,SR_PHM_01031
    /// @needwork = ad
    void SetWatchdogTriggerFunction(std::function< void() > const& trigger) noexcept;

private:
    /// @brief Create objects of HealthChannelEntity based on the conf from ConfigManager.
    /// @param healthChannelEntityConf the conf of health channel entity.
    /// @return ara::core::Vector<std::shared_ptr<HealthChannelEntity>> objects of health channel entity.
    /// @trace_id_sr=SR_PHM_01024,SR_PHM_01025,SR_PHM_01026,SR_PHM_01028,SR_PHM_01029,SR_PHM_01030,SR_PHM_01031
    /// @needwork = dda
    static ara::core::Vector< std::shared_ptr< HealthChannelEntity > > MakeHealthChannelEntity(
        ara::core::Vector< HealthChannelEntityConf > const& healthChannelEntityConf) noexcept;

    /// @brief Create objects of HealthChannelSupervision based on the conf from ConfigManager.
    /// @param healthChannelSupervisionConf the conf of health channel supervision.
    /// @return ara::core::Vector<std::shared_ptr<HealthChannelSupervision>> objects of health channel supervision.
    /// @trace_id_sr=SR_PHM_01024,SR_PHM_01025,SR_PHM_01026,SR_PHM_01028,SR_PHM_01029,SR_PHM_01030,SR_PHM_01031
    /// @needwork = dda
    ara::core::Vector< std::shared_ptr< HealthChannelSupervision > > _makeHealthChannelSupervision(
        ara::core::Vector< HealthChannelSupervisionConf > const& healthChannelSupervisionConf) noexcept;

    /// @brief Create handles used to communicate with AA、SM.
    /// @return 0, success; < 0, fail.
    /// @trace_id_sr=SR_PHM_01024,SR_PHM_01025,SR_PHM_01026,SR_PHM_01028,SR_PHM_01029,SR_PHM_01030,SR_PHM_01031
    /// @needwork = dda
    int32_t _makeCommunicationHandles();

private:
    /// @brief Destructor of HealthChannelController.
    /// @trace_id_sr=SR_PHM_01024,SR_PHM_01025,SR_PHM_01026,SR_PHM_01028,SR_PHM_01029,SR_PHM_01030,SR_PHM_01031
    /// @needwork = dda
    HealthChannelController() = default;

    /// @brief A callback registered to healthChannelServer_ to process health status
    /// @throws QAC
    /// @param specifierId identifier of health channel.
    /// @param processId process id who report health status.
    /// @param healthStatusId id of health status.
    /// @trace_id_sr=SR_PHM_01024,SR_PHM_01025,SR_PHM_01026,SR_PHM_01028,SR_PHM_01029,SR_PHM_01030,SR_PHM_01031
    /// @needwork = dda
    void _healthStatusHandler(Specifier const& specifierId,
                              ProcessId const processId,
                              HealthStatus const healthStatusId);

    /// @brief A callback registered to healthChannelActionExtServer_ to process Offer/StopOffer from SM.
    /// @throws QAC
    /// @param instance the instance specifier of HealthChannelAction.
    /// @param eventType offer or stopoffer.
    /// @trace_id_sr=SR_PHM_01024,SR_PHM_01025,SR_PHM_01026,SR_PHM_01028,SR_PHM_01029,SR_PHM_01030,SR_PHM_01031
    /// @needwork = dda
    void _healthChannelExtServerHandler(ara::core::String const& instance,
                                        ara::phm::internal::hcaextcom::EventType const eventType);

    /// @brief To process the recovery result, this function is registered to healthChannelActionClient_.
    /// @throws QAC
    /// @param result result of recover.
    /// @param instance instance of HealthChannelAction.
    /// @trace_id_sr=SR_PHM_01024,SR_PHM_01025,SR_PHM_01026,SR_PHM_01028,SR_PHM_01029,SR_PHM_01030,SR_PHM_01031
    /// @needwork = dda
    void _recoveryResultHandler(ara::phm::internal::hcacom::RecoveryResult const result,
                                ara::core::String const& instance) const;

private:
    /// @brief Check whether the process can report this health status.
    /// @param processId process id who report health status.
    /// @param healthStatusId id of health status.
    /// @return true, check passed; false，check not passed.
    /// @trace_id_sr=SR_PHM_01024,SR_PHM_01025,SR_PHM_01026,SR_PHM_01028,SR_PHM_01029,SR_PHM_01030,SR_PHM_01031
    /// @needwork = dda
    bool _checkAccessByIam(ProcessId const processId, HealthStatus const healthStatusId) const noexcept;

private:
    /// @brief used to get conf of health channel.
    /// @trace_id_sr=SR_PHM_01024,SR_PHM_01025,SR_PHM_01026,SR_PHM_01028,SR_PHM_01029,SR_PHM_01030,SR_PHM_01031
    /// @needwork = dda
    std::shared_ptr< ConfigManager > configManager_;

    /// @brief Communication server used to receive health status from AA.
    /// @trace_id_sr=SR_PHM_01024,SR_PHM_01025,SR_PHM_01026,SR_PHM_01028,SR_PHM_01029,SR_PHM_01030,SR_PHM_01031
    /// @needwork = dda
    std::unique_ptr< ara::phm::internal::hccom::Server > healthChannelServer_;

    /// @brief used to notify SM when a health status need recovery.
    /// @trace_id_sr=SR_PHM_01024,SR_PHM_01025,SR_PHM_01026,SR_PHM_01028,SR_PHM_01029,SR_PHM_01030,SR_PHM_01031
    /// @needwork = dda
    std::unique_ptr< ara::phm::internal::hcacom::Client > healthChannelActionClient_;

    /// @brief Used to process Offer/StopOffer request from SM.
    /// @trace_id_sr=SR_PHM_01024,SR_PHM_01025,SR_PHM_01026,SR_PHM_01028,SR_PHM_01029,SR_PHM_01030,SR_PHM_01031
    /// @needwork = dda
    std::unique_ptr< ara::phm::internal::hcaextcom::Server > healthChannelActionExtServer_;

#ifdef COMPILE_WITH_IAM
    /// @brief iam handle.
    /// @trace_id_sr=SR_PHM_01024,SR_PHM_01025,SR_PHM_01026,SR_PHM_01028,SR_PHM_01029,SR_PHM_01030,SR_PHM_01031
    /// @needwork = dda
    std::unique_ptr< ara::iam::internal::grant::IAMGrantPhmQuery > iamHandle_;
#endif

    /// @brief When recovery failed, use this function to trigger the watchdog.
    /// @trace_id_sr=SR_PHM_01024,SR_PHM_01025,SR_PHM_01026,SR_PHM_01028,SR_PHM_01029,SR_PHM_01030,SR_PHM_01031
    /// @needwork = dda
    std::function< void() > watchdogTriggerFuntion_;

    /// @brief <health channel name, HealthChannelEntity>.
    /// @trace_id_sr=SR_PHM_01024,SR_PHM_01025,SR_PHM_01026,SR_PHM_01028,SR_PHM_01029,SR_PHM_01030,SR_PHM_01031
    /// @needwork = dda
    ara::core::Map< ara::core::String, std::shared_ptr< HealthChannelEntity > > healthChannelEntity_;

    /// @brief <identifier, HealthChannelSupervision>.
    /// @trace_id_sr=SR_PHM_01024,SR_PHM_01025,SR_PHM_01026,SR_PHM_01028,SR_PHM_01029,SR_PHM_01030,SR_PHM_01031
    /// @needwork = dda
    ara::core::Map< ara::core::String, std::shared_ptr< HealthChannelSupervision > > healthChannelSupervision_;

    /// @brief <health channel interface, HealthChannelSupervision>.
    /// @trace_id_sr=SR_PHM_01024,SR_PHM_01025,SR_PHM_01026,SR_PHM_01028,SR_PHM_01029,SR_PHM_01030,SR_PHM_01031
    /// @needwork = dda
    ara::core::Map< ara::core::String, std::shared_ptr< HealthChannelSupervision > > healthChannelSupervisionExt_;

    /// "recovery_notification_to_pport_prototype_mapping": [
    /// 		{
    /// 			"instance_specifier": "smd/state_manager/TestUnitHealthSupervisionRecoveryNotification",
    /// 			"healthChannelId":
    /// "/AUTOSAR/PlatformHealthManagement/deployment_phm_motive_testunit/healthChannelSupervision_testunit"
    /// 		},
    /// 		{
    /// 			"instance_specifier": "smd/state_manager/TestUnitHealthChannelRecoveryNotification",
    /// 			"healthChannelId":
    /// "/AUTOSAR/PlatformHealthManagement/deployment_phm_motive_testunit/healthChannelExternalStatus_testunit"
    /// 		}
    /// 	]
    /// @brief <health channel id, instance specifier>
    /// @trace_id_sr=SR_PHM_01024,SR_PHM_01025,SR_PHM_01026,SR_PHM_01028,SR_PHM_01029,SR_PHM_01030,SR_PHM_01031
    /// @needwork = dda
    ara::core::Map< ara::core::String, ara::core::String > recoveryActionMap_;

    /// @brief <instance specifier, health channel id>
    /// @trace_id_sr=SR_PHM_01024,SR_PHM_01025,SR_PHM_01026,SR_PHM_01028,SR_PHM_01029,SR_PHM_01030,SR_PHM_01031
    /// @needwork = dda
    ara::core::Map< ara::core::String, ara::core::String > recoveryActionReverseMap_;
};

}  // namespace internal
}  // namespace phm
}  // namespace ara

#endif  // _ARA_PHM_INTERNAL_HEALTH_CHANNEL_CONTROLLER_H_