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
/// @file       supervision_controller.h
/// @brief      Manages resources in SupervisionMager and provide interfaces for other modules.
/// @details
/// @date       2024-06-05
/// @author     wangyanlong
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/PlatformHealthManagement/SupervisionManager
/// @unit_description=Manages resources in SupervisionMager and provide interfaces for other modules.
/// @trace_id_sr=SR_PHM_01014,SR_PHM_01015,SR_PHM_01016,SR_PHM_01017,
/// SR_PHM_01027,SR_PHM_01029,SR_PHM_01030,SR_PHM_01031
/// @unit_name=SupervisionController
/// @interface_level=module
/// @endcode
///
/// ================================================================

#ifndef ARA_PHM_INTERNAL_SVMANAGER_SUPERVISION_CONTROLLER_H_
#define ARA_PHM_INTERNAL_SVMANAGER_SUPERVISION_CONTROLLER_H_

#include <ara/core/map.h>
#include <ara/core/string.h>

#ifdef COMPILE_WITH_IAM
    #include <ara/iam/internal/grantquery/phm.h>
#endif

#include <ara/exec/internal/find_process_client.h>
#include <ara/sm/internal/fg_state_notify_async_client.h>
#include <ara/sm/internal/fg_state_notify_error_domain.h>

#include <cassert>
#include <functional>
#include <memory>
#include <mutex>

#include "ara/phm/internal/com/ra_client.h"
#include "ara/phm/internal/com/raext_server.h"
#include "ara/phm/internal/com/sv_server.h"
#include "ara/phm/internal/config_manager.h"
#include "ara/phm/internal/svmanager/alive_supervision.h"
#include "ara/phm/internal/svmanager/base_supervision.h"
#include "ara/phm/internal/svmanager/deadline_supervision.h"
#include "ara/phm/internal/svmanager/fg_supervision_mode.h"
#include "ara/phm/internal/svmanager/global_supervision.h"
#include "ara/phm/internal/svmanager/local_supervision.h"
#include "ara/phm/internal/svmanager/logical_supervision.h"
#include "ara/phm/internal/svmanager/supervision_checkpoint.h"
#include "ara/phm/internal/svmanager/supervision_factory.h"
#include "ara/phm/internal/svmanager/supervision_mode.h"
#include "ara/phm/internal/types.h"

namespace ara {
namespace phm {
namespace internal {

/// @brief The client provided by SM to listen the state change of function
/// group.
/// @trace_id_sr=SR_PHM_01014,SR_PHM_01015,SR_PHM_01016,SR_PHM_01017,
/// @needwork = no
using FGStateNotifyAsyncClient = ara::sm::fg_state_notify_client::FGStateNotifyAsyncClient;

/// @brief function group name and its state.
/// @trace_id_sr=SR_PHM_01014,SR_PHM_01015,SR_PHM_01016,SR_PHM_01017,
/// @needwork = no
using FGStateType = ara::sm::fg_state_notify_client::FGStateType;

/// @brief Manages resources in SupervisionMager and provide interfaces for other modules.
/// @trace_id_sr=SR_PHM_01014,SR_PHM_01015,SR_PHM_01016,SR_PHM_01017,
/// @needwork = ad
class SupervisionController final
{
public:
    /// @brief Creation of SupervisionController, unique singleton.
    /// @return unique ptr of SupervisionController.
    /// @trace_id_sr=SR_PHM_01014,SR_PHM_01015,SR_PHM_01016,SR_PHM_01017,
    /// @needwork = ad
    static std::unique_ptr< SupervisionController > GetInstanceUnique()
    {
        static std::unique_ptr< SupervisionController > s_Instance{nullptr};
        static std::once_flag s_Flag{};
        std::call_once(s_Flag, []() { s_Instance.reset(new SupervisionController()); });
        std::ignore = s_Flag;

        /// Exclusive singleton, only allowed to be obtained once
        assert(s_Instance);
        return std::move(s_Instance);
    }

    /// @brief Destructor.
    /// @trace_id_sr=SR_PHM_01014,SR_PHM_01015,SR_PHM_01016,SR_PHM_01017,
    /// @needwork = ad
    ~SupervisionController() noexcept;

    /// @brief The copy constructor shall not be used.
    /// @param obj The object to be copied.
    /// @trace_id_sr=SR_PHM_01014,SR_PHM_01015,SR_PHM_01016,SR_PHM_01017,
    /// @needwork = ad
    SupervisionController(SupervisionController& obj) = delete;

    /// @brief The copy assignment shall not be used.
    /// @param obj The object to be copied.
    /// @return The copied object.
    /// @trace_id_sr=SR_PHM_01014,SR_PHM_01015,SR_PHM_01016,SR_PHM_01017,
    /// @needwork = ad
    SupervisionController& operator=(SupervisionController const& obj) = delete;

    /// @brief The move constructor shall not be used.
    /// @param obj The object to be moved.
    /// @trace_id_sr=SR_PHM_01014,SR_PHM_01015,SR_PHM_01016,SR_PHM_01017,
    /// @needwork = ad
    SupervisionController(SupervisionController&& obj) = delete;

    /// @brief The move assignment shall not be used.
    /// @param obj The object to be moved.
    /// @return SupervisionController& The moved object.
    /// @trace_id_sr=SR_PHM_01014,SR_PHM_01015,SR_PHM_01016,SR_PHM_01017,
    /// @needwork = ad
    SupervisionController& operator=(SupervisionController const&& obj) = delete;

    /// @brief Init supervision controller
    /// @return 0, success;< 0, fail
    /// @trace_id_sr=SR_PHM_01014,SR_PHM_01015,SR_PHM_01016,SR_PHM_01017,
    /// @needwork = ad
    int32_t Init() noexcept;

    /// @brief Stop supervision such as stop alive supervision, stop deadline
    /// supervision.
    /// @return 0, success;< 0, fail
    /// @trace_id_sr=SR_PHM_01014,SR_PHM_01015,SR_PHM_01016,SR_PHM_01017,
    /// @needwork = ad
    int32_t Stop() noexcept;

    /// @brief Used by PhmContext to register a function, SupervisionController can use this function to trigger
    /// watchdog.
    /// @param trigger a callback used by supervision controller to trigger watchdog
    /// @trace_id_sr=SR_PHM_01014,SR_PHM_01015,SR_PHM_01016,SR_PHM_01017,
    /// @needwork = ad
    void SetWatchdogTriggerFunction(std::function< void() > const& trigger) noexcept;

private:
    /// @brief Constructor.
    /// @trace_id_sr=SR_PHM_01014,SR_PHM_01015,SR_PHM_01016,SR_PHM_01017,
    /// @needwork = dda
    SupervisionController() = default;

    /// @brief Make and organize objects such as
    /// AliveSupervision、DeadlineSupervision.
    /// @return 0, success;< 0, fail
    /// @trace_id_sr=SR_PHM_01014,SR_PHM_01015,SR_PHM_01016,SR_PHM_01017,
    /// @needwork = dda
    int32_t _makeAndOrganizeSupervisionElement() noexcept;

    /// @brief To make checkpointBaseMap_.
    /// @trace_id_sr=SR_PHM_01014,SR_PHM_01015,SR_PHM_01016,SR_PHM_01017,
    /// @needwork = dda
    void _reOrganizeCheckpoint() noexcept;

    /// @brief to make localStatusMap_.
    /// @trace_id_sr=SR_PHM_01014,SR_PHM_01015,SR_PHM_01016,SR_PHM_01017,
    /// @needwork = dda
    void _reOrganizeLocalSupervision() noexcept;

    /// @brief to make globalStatusMap_.
    /// @trace_id_sr=SR_PHM_01014,SR_PHM_01015,SR_PHM_01016,SR_PHM_01017,
    /// @needwork = dda
    void _makeLocalGlobalMap() noexcept;

    /// @brief to make processBaseSupervisionMap_.
    /// @trace_id_sr=SR_PHM_01014,SR_PHM_01015,SR_PHM_01016,SR_PHM_01017,
    /// @needwork = dda
    void _makeProcessBaseSupervisionMap() noexcept;

    /// @brief Create handles used to communicate with AA、SM.
    /// @return 0, success; < 0, fail.
    /// @trace_id_sr=SR_PHM_01014,SR_PHM_01015,SR_PHM_01016,SR_PHM_01017,
    /// @needwork = dda
    int32_t _makeCommunicationHandles() noexcept;

    /// @brief Handle the event: process checkpoint, get local supervision status, get global supervision status.
    /// @throws QAC
    /// @param eventType type of event.
    /// @param specifierId instance specifier of SupervisedEntity.
    /// @param processId id of process who send the event.
    /// @param checkpointId id of checkpoint.
    /// @param processClusterAffiliation process cluster affiliation
    /// @param timestamp time stamp.
    /// @trace_id_sr=SR_PHM_01014,SR_PHM_01015,SR_PHM_01016,SR_PHM_01017,
    /// @needwork = dda
    void _supervisionComServerHandler(
        ara::phm::internal::svcom::EventType const eventType,
        Specifier const& specifierId,
        ProcessId const processId,
        CheckpointId const checkpointId,
        ara::phm::internal::svcom::ProcessClusterAffiliation const processClusterAffiliation,
        int64_t const timestamp);

    /// @brief Registered to FGStateNotifyAsyncClient to listen change of all function group state in in initial
    /// process.
    /// @throws QAC
    /// @param res result.
    /// @param allFgState all function group state.
    /// @trace_id_sr=SR_PHM_01014,SR_PHM_01015,SR_PHM_01016,SR_PHM_01017,
    /// @needwork = dda
    void _initialFgStateReached(ara::core::Result< void > const& res,
                                ara::core::Vector< FGStateType > const& allFgState);

    /// @brief The handler of the result of FGStateNotifyAsyncClient::SubscribeFGState.
    /// @param res result.
    /// @trace_id_sr=SR_PHM_01014,SR_PHM_01015,SR_PHM_01016,SR_PHM_01017,
    /// @needwork = dda
    static void FgStateSubscribeResponseHandler(ara::core::Result< void > const& res) noexcept;

    /// @brief Registered to FGStateNotifyAsyncClient to listen change of one function group state
    /// @param fgState function group name and its state
    /// @trace_id_sr=SR_PHM_01014,SR_PHM_01015,SR_PHM_01016,SR_PHM_01017,
    /// @needwork = dda
    void _fgStateChanged(FGStateType const& fgState);

    /// @brief Registerd to GlobalSupervision to listen status change of GlobalSupervision
    /// @throws QAC
    /// @param globalSupervisionName name of global supervision.
    /// @param globalSupervisionStatus status of global supervision.
    /// @param baseSupervisionType type of base supervision.
    /// @param processName name of process.
    /// @param processClusterAffiliation process cluster affiliation.
    /// @trace_id_sr=SR_PHM_01014,SR_PHM_01015,SR_PHM_01016,SR_PHM_01017,
    /// @needwork = dda
    void _globalSupervisionStatusChanged(
        ara::core::String const& globalSupervisionName,
        GlobalSupervisionStatus const& globalSupervisionStatus,
        BaseSupervisionType const& baseSupervisionType,
        ara::core::String const& processName,
        ara::phm::internal::svcom::ProcessClusterAffiliation const processClusterAffiliation);

    /// @brief Check whether the checkpoint received by SupervisionController is legal
    /// @param eventType type of event from SupervisedEntity
    /// @param specifierId instance id of SupervisedEntity
    /// @param processId id of process who report checkpoint.
    /// @param checkpointId checkpoint id.
    /// @return true legal;false illegal.
    /// @trace_id_sr=SR_PHM_01014,SR_PHM_01015,SR_PHM_01016,SR_PHM_01017,
    /// @needwork = dda
    bool _checkAccessByIam(ara::phm::internal::svcom::EventType const eventType,
                           Specifier const& specifierId,
                           ProcessId const processId,
                           uint32_t const checkpointId) noexcept;

    /// @brief Switch supervision mode.
    /// @param fgName name of function group.
    /// @param fgState state of function group.
    /// @trace_id_sr=SR_PHM_01014,SR_PHM_01015,SR_PHM_01016,SR_PHM_01017,
    /// @needwork = dda
    void _switchSupervisionMode(ara::core::String const& fgName, ara::core::String const& fgState) noexcept;

    /// @brief A callback registered to recoveryActionExtServer_ to process Offer/StopOffer/GetGlobalSupervisionStatus
    /// from SM.
    /// @throws QAC
    /// @param instance the instance specifier of RecoveryAction.
    /// @param eventType offer/stopoffer
    /// @trace_id_sr=SR_PHM_01014,SR_PHM_01015,SR_PHM_01016,SR_PHM_01017,
    /// @needwork = dda
    void _recoveryActionExtServerHandler(ara::core::String const& instance,
                                         ara::phm::internal::raextcom::EventType const eventType);

    /// @brief To process the recovery result, this function is registered to the ara::phm::internal::racom::Client
    /// @throws QAC
    /// @param result result of recover.
    /// @param instance instance of RecoveryAction.
    /// @trace_id_sr=SR_PHM_01014,SR_PHM_01015,SR_PHM_01016,SR_PHM_01017,
    /// @needwork = dda
    void _recoveryResultHandler(ara::phm::internal::racom::RecoveryResult const result,
                                ara::core::String const& instance) const;

    /// @brief switch supervision mode of base supervision.
    /// @param baseSupervisionFqn fqn of base supervision.
    /// @param stateChange state change of process.
    /// @trace_id_sr=SR_PHM_01014,SR_PHM_01015,SR_PHM_01016,SR_PHM_01017,
    /// @needwork = dda
    void _switchBaseSupervisionMode(ara::core::String const& baseSupervisionFqn,
                                    ProcessStateChange const& stateChange) noexcept;

    /// @brief returns the string of event type from supervised entity.
    /// @param eventType event type.
    /// @return the string of event type from supervised entity.
    /// @trace_id_sr=SR_PHM_01014,SR_PHM_01015,SR_PHM_01016,SR_PHM_01017,
    /// @needwork = dda
    static ara::core::StringView GetSupervisionComEventTypeString(
        ara::phm::internal::svcom::EventType const& eventType) noexcept;

private:
    /// @brief config manager.
    /// @trace_id_sr=SR_PHM_01014,SR_PHM_01015,SR_PHM_01016,SR_PHM_01017,
    /// @needwork = dda
    std::shared_ptr< ConfigManager > configManager_;

    /// @brief superviusion server.
    /// @trace_id_sr=SR_PHM_01014,SR_PHM_01015,SR_PHM_01016,SR_PHM_01017,
    /// @needwork = dda
    std::unique_ptr< ara::phm::internal::svcom::Server > supervisionServer_;

    /// @brief recovery client.
    /// @trace_id_sr=SR_PHM_01014,SR_PHM_01015,SR_PHM_01016,SR_PHM_01017,
    /// @needwork = dda
    std::unique_ptr< ara::phm::internal::racom::Client > recoveryActionClient_;

    /// @brief Used to process Offer/StopOffer/GetGlobalSupervisionStatus request from SM.
    /// @trace_id_sr=SR_PHM_01014,SR_PHM_01015,SR_PHM_01016,SR_PHM_01017,
    /// @needwork = dda
    std::unique_ptr< ara::phm::internal::raextcom::Server > recoveryActionExtServer_;

    /// @brief fg state client.
    /// @trace_id_sr=SR_PHM_01014,SR_PHM_01015,SR_PHM_01016,SR_PHM_01017,
    /// @needwork = dda
    FGStateNotifyAsyncClient* fgStateClient_{nullptr};

#ifdef COMPILE_WITH_IAM
    /// @brief iam handle.
    /// @trace_id_sr=SR_PHM_01014,SR_PHM_01015,SR_PHM_01016,SR_PHM_01017,
    /// @needwork = dda
    std::unique_ptr< ara::iam::internal::grant::IAMGrantPhmQuery > iamHandle_;
#endif

    /// @brief When SM recovery fails, or EM/SM is abnormal, trigger the watchdog through this function.
    /// @trace_id_sr=SR_PHM_01014,SR_PHM_01015,SR_PHM_01016,SR_PHM_01017,
    /// @needwork = dda
    std::function< void() > watchdogTriggerFuntion_;

    /// @brief <fqn of checkpoint, Checkpoint>
    /// @trace_id_sr=SR_PHM_01014,SR_PHM_01015,SR_PHM_01016,SR_PHM_01017,
    /// @needwork = dda
    ara::core::Map< ara::core::String, std::shared_ptr< SupervisionCheckpoint > > checkpoint_;

    /// @brief <identifier + id, checkpoint>, when processing a checkpoint, find the corresponding checkpoint based on instance and checkpoint id
    /// @trace_id_sr=SR_PHM_01014,SR_PHM_01015,SR_PHM_01016,SR_PHM_01017,
    /// @needwork = dda
    ara::core::Map< ara::core::String, std::shared_ptr< SupervisionCheckpoint > > checkpointIdMap_;

    /// @brief <fqn of base supervision, BaseSupervision>
    /// @trace_id_sr=SR_PHM_01014,SR_PHM_01015,SR_PHM_01016,SR_PHM_01017,
    /// @needwork = dda
    ara::core::Map< ara::core::String, std::shared_ptr< BaseSupervision > > baseSupervision_;

    /// @brief <identifier of, <checkpoint id, vector<supervision>>>, used when processing checkpoints
    /// @trace_id_sr=SR_PHM_01014,SR_PHM_01015,SR_PHM_01016,SR_PHM_01017,
    /// @needwork = dda
    ara::core::Map< Specifier, std::map< CheckpointId, ara::core::Vector< std::shared_ptr< BaseSupervision > > > >
        checkpointBaseMap_;

    /// @brief <fqn of local supervision, LocalSupervision>
    /// @trace_id_sr=SR_PHM_01014,SR_PHM_01015,SR_PHM_01016,SR_PHM_01017,
    /// @needwork = dda
    ara::core::Map< ara::core::String, std::shared_ptr< LocalSupervision > > localSupervision_;

    /// @brief <fqn of base supervision, vector of local supervision>
    /// @trace_id_sr=SR_PHM_01014,SR_PHM_01015,SR_PHM_01016,SR_PHM_01017,
    /// @needwork = dda
    ara::core::Map< ara::core::String, ara::core::Vector< std::shared_ptr< LocalSupervision > > > baseLocalMap_;

    /// @brief <fqn of global supervision, GlobalSupervision>
    /// The shortName of global supervision is fqn
    /// @trace_id_sr=SR_PHM_01014,SR_PHM_01015,SR_PHM_01016,SR_PHM_01017,
    /// @needwork = dda
    ara::core::Map< ara::core::String, std::shared_ptr< GlobalSupervision > > globalSupervision_;

    /// @brief <checkpoint identifier,LocalSupervision>
    /// @trace_id_sr=SR_PHM_01014,SR_PHM_01015,SR_PHM_01016,SR_PHM_01017,
    /// @needwork = dda
    ara::core::Map< ara::core::String, std::shared_ptr< LocalSupervision > > localStatusMap_;

    /// @brief <LocalSupervision, GlobalSupervision>,
    /// Used by SupervisedEntity to obtain GlobalSupervisionStatus, one local
    /// supervision belongs to only one global supervision.
    /// @trace_id_sr=SR_PHM_01014,SR_PHM_01015,SR_PHM_01016,SR_PHM_01017,
    /// @needwork = dda
    ara::core::Map< std::shared_ptr< LocalSupervision >, std::shared_ptr< GlobalSupervision > > globalStatusMap_;

    /// @brief <fg name, FgSupervisionMode>
    /// @trace_id_sr=SR_PHM_01014,SR_PHM_01015,SR_PHM_01016,SR_PHM_01017,
    /// @needwork = dda
    ara::core::Map< ara::core::String, std::shared_ptr< FgSupervisionMode > > fgSupervisionMode_;

    /// @brief <process name, FgSupervisionMode>
    /// Find the function group where the process belongs based on the process, FgSupervisionMode obtains the process error code based on the current state
    /// @trace_id_sr=SR_PHM_01014,SR_PHM_01015,SR_PHM_01016,SR_PHM_01017,
    /// @needwork = dda
    ara::core::Map< ara::core::String, std::shared_ptr< FgSupervisionMode > > processSupervisionModeMap_;

    /// @brief <process fqn, base supervision fqn>
    /// @todo Using string for everything, is it a waste of memory?
    /// @trace_id_sr=SR_PHM_01014,SR_PHM_01015,SR_PHM_01016,SR_PHM_01017,
    /// @needwork = dda
    ara::core::Map< ara::core::String, std::set< ara::core::String > > processBaseSupervisionMap_;

    /// @brief <health channel id, instance specifier>.
    /// @trace_id_sr=SR_PHM_01014,SR_PHM_01015,SR_PHM_01016,SR_PHM_01017,
    /// @needwork = dda
    ara::core::Map< ara::core::String, ara::core::String > recoveryActionMap_;

    /// @brief <instance specifier, health channel id>.
    /// @trace_id_sr=SR_PHM_01014,SR_PHM_01015,SR_PHM_01016,SR_PHM_01017,
    /// @needwork = dda
    ara::core::Map< ara::core::String, ara::core::String > recoveryActionReverseMap_;

    /// @brief supervision mode lock.
    /// Supervision mode switching and checkpoint processing, the two are synchronized
    /// @trace_id_sr=SR_PHM_01014,SR_PHM_01015,SR_PHM_01016,SR_PHM_01017,
    /// @needwork = dda
    std::mutex supervisionModeLock_;

    /// @brief used to find pid by fqn.
    /// @trace_id_sr=SR_PHM_01014,SR_PHM_01015,SR_PHM_01016,SR_PHM_01017,
    /// @needwork = dda
    std::unique_ptr< ara::exec::internal::FindProcessClient > findProcessClient_;
};

}  // namespace internal
}  // namespace phm
}  // namespace ara

#endif  // ARA_PHM_INTERNAL_SVMANAGER_SUPERVISION_CONTROLLER_H_