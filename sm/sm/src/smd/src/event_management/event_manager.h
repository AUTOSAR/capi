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
/// @file       event_manager.h
/// @brief      Event management class
/// @details
/// @date       2024-05-03
/// @author     lianglongxiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/StateManagement/EventManagement
/// @unit_name=EventManager
/// @interface_level=module
/// @unit_description=Event management class
/// @trace_id_sr=SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008,
/// SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_06001, SR_SM_06002, SR_SM_06003, SR_SM_06004, SR_SM_07001, SR_SM_07002, SR_SM_08001, SR_SM_09001, SR_SM_09002,
/// SR_SM_09004, SR_SM_09006, SR_SM_09008, SR_SM_10001
/// @endcode
///
/// ================================================================

#ifndef EVENT_MANAGER_H_
#define EVENT_MANAGER_H_

#include <ara/core/map.h>
#include <ara/core/promise.h>
#include <ara/core/string.h>
#include <ara/core/vector.h>
#include <ara/log/logger.h>
#include <isoft/naicpp/terminating_handler.h>

#include <condition_variable>

#include "config/global_config.h"
#include "em_comm/em_comm_manager.h"
#include "event.h"
#include "fg_state_comm/fg_state_comm_manager.h"
#include "fg_state_notify_comm/fg_state_notify_comm_manager.h"
#include "i_communication_manager.h"
#include "shell_comm/shell_comm_manager.h"
#include "sm_state_comm/sm_state_comm_manager.h"
#include "state_machine_management/state_machine_manager.h"

#ifdef ARA_ENABLE_SM_ECU_STATE
    #include "ecu_state_comm/ecu_state_comm_manager.h"
#endif

#ifdef ARA_WITH_NM
    #include "nm_comm/nm_comm_manager.h"
#endif
#ifdef ARA_WITH_PHM
    #include "phm_comm/phm_comm_manager.h"
#endif
#ifdef ARA_WITH_DIAG
    #include "dm_comm/dm_comm_manager.h"
#endif
#ifdef ARA_WITH_UCM
    #include "ucm_comm/ucm_comm_manager.h"
#endif

namespace ara {
namespace sm {
namespace event_management {

/// @brief Event manager
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008,
/// SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_06001, SR_SM_06002, SR_SM_06003, SR_SM_06004, SR_SM_07001, SR_SM_07002, SR_SM_08001, SR_SM_09001, SR_SM_09002,
/// SR_SM_09004, SR_SM_09006, SR_SM_09008, SR_SM_10001
/// @trace_id_ad=AD_SM_08122
/// @trace_id_dd=DD_SM_08456
/// @needwork = ad
/// @endcode
class EventManager
{
public:
    /// @brief Constructor function
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04005, SR_SM_04006, SR_SM_04007,
    /// SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_06001, SR_SM_06002, SR_SM_06003, SR_SM_06004, SR_SM_07001, SR_SM_07002, SR_SM_08001,
    /// SR_SM_09001, SR_SM_09002, SR_SM_09004, SR_SM_09006, SR_SM_09008, SR_SM_10001
    /// @trace_id_ad=AD_SM_00617
    /// @trace_id_dd=DD_SM_00737
    /// @needwork = ad
    /// @endcode
    EventManager() noexcept;

    /// @brief Destructor function
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04005, SR_SM_04006, SR_SM_04007,
    /// SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_06001, SR_SM_06002, SR_SM_06003, SR_SM_06004, SR_SM_07001, SR_SM_07002, SR_SM_08001,
    /// SR_SM_09001, SR_SM_09002, SR_SM_09004, SR_SM_09006, SR_SM_09008, SR_SM_10001
    /// @trace_id_ad=AD_SM_00618
    /// @trace_id_dd=DD_SM_00738
    /// @needwork = ad
    /// @endcode
    ~EventManager() noexcept;

    /// @brief deleted copy constructor function
    /// @param other The EventManager instance to be copyed
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04005, SR_SM_04006, SR_SM_04007,
    /// SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_06001, SR_SM_06002, SR_SM_06003, SR_SM_06004, SR_SM_07001, SR_SM_07002, SR_SM_08001,
    /// SR_SM_09001, SR_SM_09002, SR_SM_09004, SR_SM_09006, SR_SM_09008, SR_SM_10001
    /// @trace_id_ad=AD_SM_00619
    /// @trace_id_dd=DD_SM_00739
    /// @needwork = ad
    /// @endcode
    EventManager(EventManager const &other) = delete;

    /// @brief deleted copy assignment function
    /// @param other The EventManager instance to be copyed
    /// @return the assigned EventManager instance itself
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04005, SR_SM_04006, SR_SM_04007,
    /// SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_06001, SR_SM_06002, SR_SM_06003, SR_SM_06004, SR_SM_07001, SR_SM_07002, SR_SM_08001,
    /// SR_SM_09001, SR_SM_09002, SR_SM_09004, SR_SM_09006, SR_SM_09008, SR_SM_10001
    /// @trace_id_ad=AD_SM_00620
    /// @trace_id_dd=DD_SM_00740
    /// @needwork = ad
    /// @endcode
    EventManager &operator=(EventManager const &other) = delete;

    /// @brief Move constructor function
    /// @param other The EventManager instance to be moved
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04005, SR_SM_04006, SR_SM_04007,
    /// SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_06001, SR_SM_06002, SR_SM_06003, SR_SM_06004, SR_SM_07001, SR_SM_07002, SR_SM_08001,
    /// SR_SM_09001, SR_SM_09002, SR_SM_09004, SR_SM_09006, SR_SM_09008, SR_SM_10001
    /// @trace_id_ad=AD_SM_00621
    /// @trace_id_dd=DD_SM_00741
    /// @needwork = ad
    /// @endcode
    EventManager(EventManager &&other) = default;

    /// @brief Move assignment function
    /// @param other The EventManager instance to be moved
    /// @return the assigned EventManager instance itself
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04005, SR_SM_04006, SR_SM_04007,
    /// SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_06001, SR_SM_06002, SR_SM_06003, SR_SM_06004, SR_SM_07001, SR_SM_07002, SR_SM_08001,
    /// SR_SM_09001, SR_SM_09002, SR_SM_09004, SR_SM_09006, SR_SM_09008, SR_SM_10001
    /// @trace_id_ad=AD_SM_00622
    /// @trace_id_dd=DD_SM_00742
    /// @needwork = ad
    /// @endcode
    EventManager &operator=(EventManager &&other) = delete;

    /// @brief Initialize
    /// @return true - success
    /// @return false - failure
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04005, SR_SM_04006, SR_SM_04007,
    /// SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_06001, SR_SM_06002, SR_SM_06003, SR_SM_06004, SR_SM_07001, SR_SM_07002, SR_SM_08001,
    /// SR_SM_09001, SR_SM_09002, SR_SM_09004, SR_SM_09006, SR_SM_09008, SR_SM_10001
    /// @trace_id_ad=AD_SM_00623
    /// @trace_id_dd=DD_SM_00743
    /// @needwork = ad
    /// @endcode
    bool Init() noexcept;

    /// @brief Start event processing
    /// @return true - success
    /// @return false - failure
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04005, SR_SM_04006, SR_SM_04007,
    /// SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_06001, SR_SM_06002, SR_SM_06003, SR_SM_06004, SR_SM_07001, SR_SM_07002, SR_SM_08001,
    /// SR_SM_09001, SR_SM_09002, SR_SM_09004, SR_SM_09006, SR_SM_09008, SR_SM_10001
    /// @trace_id_ad=AD_SM_00624
    /// @trace_id_dd=DD_SM_00744
    /// @needwork = ad
    /// @endcode
    bool Run() noexcept;

    /// @brief Stop event processing
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04005, SR_SM_04006, SR_SM_04007,
    /// SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_06001, SR_SM_06002, SR_SM_06003, SR_SM_06004, SR_SM_07001, SR_SM_07002, SR_SM_08001,
    /// SR_SM_09001, SR_SM_09002, SR_SM_09004, SR_SM_09006, SR_SM_09008, SR_SM_10001
    /// @trace_id_ad=AD_SM_00625
    /// @trace_id_dd=DD_SM_00745
    /// @needwork = ad
    /// @endcode
    void Stop() noexcept;

    /// @brief Add event request
    /// @param event  Event request
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04005, SR_SM_04006, SR_SM_04007,
    /// SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_06001, SR_SM_06002, SR_SM_06003, SR_SM_06004, SR_SM_07001, SR_SM_07002, SR_SM_08001,
    /// SR_SM_09001, SR_SM_09002, SR_SM_09004, SR_SM_09006, SR_SM_09008, SR_SM_10001
    /// @trace_id_ad=AD_SM_00627
    /// @trace_id_dd=DD_SM_00747
    /// @needwork = ad
    /// @endcode
    void AppendEvent(common::Event &&event) noexcept;

    /// @brief Get the execution error related to the specified function group
    /// @param fgName Function group name
    /// @return Set result
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr= SR_SM_06004, SR_SM_08001, SR_SM_09002
    /// @trace_id_ad=AD_SM_00628
    /// @trace_id_dd=DD_SM_00748
    /// @needwork = ad
    /// @endcode
    core::Result< exec::ExecutionErrorEvent > GetExecutionError(core::String const &fgName) noexcept;

private:
    /// @brief Add event request
    /// @param event  Event request
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04005, SR_SM_04006, SR_SM_04007,
    /// SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_06001, SR_SM_06002, SR_SM_06003, SR_SM_06004, SR_SM_07001, SR_SM_07002, SR_SM_08001,
    /// SR_SM_09001, SR_SM_09002, SR_SM_09004, SR_SM_09006, SR_SM_09008, SR_SM_10001
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_80000
    /// @needwork = dda
    /// @endcode
    void _appendEventForEVLoop(common::Event &&event) noexcept;

    /// @brief Get the initial machine state transition result
    /// @return true Transition successful
    /// @return false Transition failed
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04005, SR_SM_04006, SR_SM_04007,
    /// SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_06001, SR_SM_06002, SR_SM_06003, SR_SM_06004, SR_SM_07001, SR_SM_07002, SR_SM_08001,
    /// SR_SM_09001, SR_SM_09002, SR_SM_09004, SR_SM_09006, SR_SM_09008, SR_SM_10001
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08457
    /// @needwork = dda
    /// @endcode
    bool _getInitialMachineStateTransitionResult() noexcept;

    /// @brief Logger instance
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04005, SR_SM_04006, SR_SM_04007,
    /// SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_06001, SR_SM_06002, SR_SM_06003, SR_SM_06004, SR_SM_07001, SR_SM_07002, SR_SM_08001,
    /// SR_SM_09001, SR_SM_09002, SR_SM_09004, SR_SM_09006, SR_SM_09008, SR_SM_10001
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08458
    /// @needwork = dda
    /// @endcode
    log::Logger &log_;

    /// @brief List of all communication managers
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04005, SR_SM_04006, SR_SM_04007,
    /// SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_06001, SR_SM_06002, SR_SM_06003, SR_SM_06004, SR_SM_07001, SR_SM_07002, SR_SM_08001,
    /// SR_SM_09001, SR_SM_09002, SR_SM_09004, SR_SM_09006, SR_SM_09008, SR_SM_10001
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08459
    /// @needwork = dda
    /// @endcode
    core::Vector< std::shared_ptr< common::ICommunicationManager > > commManagers_;

    /// @brief FGStateCommManager instance
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_01001, SR_SM_01002, SR_SM_01003
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08460
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< fg_state_comm::FGStateCommManager > fgStateCommManager_;

    /// @brief ShellCommManager instance
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_10001
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08461
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< shell_comm::ShellCommManager > shellCommManager_;

    /// @brief EMCommManager instance
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_06001, SR_SM_06002, SR_SM_06003, SR_SM_06004
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08462
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< em_comm::EMCommManager > emCommManager_;
#ifdef ARA_WITH_NM
    /// @brief NMCommManager instance
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_05003, SR_SM_05004
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08463
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< nm_comm::NMCommManager > nmCommManager_;
#endif
#ifdef ARA_WITH_DIAG
    /// @brief DMCommManager instance
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_03002
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08464
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< dm_comm::DMCommManager > dmCommManager_;
#endif
#ifdef ARA_WITH_PHM
    /// @brief PHMCommManager instance
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_08001
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08465
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< phm_comm::PHMCommManager > phmCommManager_;
#endif
#ifdef ARA_WITH_UCM
    /// @brief UCMCommManager instance
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr= SR_SM_04001, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08466
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< ucm_comm::UCMCommManager > ucmCommManager_;
#endif
#ifdef ARA_ENABLE_SM_ECU_STATE
    /// @brief EcuStateCommManager instance
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_07001, SR_SM_07002
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08467
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< ecu_state_comm::EcuStateCommManager > ecuStateCommManager_;
#endif
    /// @brief FGStateNotifyCommManager instance
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_02001, SR_SM_02002
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08468
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< fg_state_notify_comm::FGStateNotifyCommManager > fgStateNotifyCommManager_;

    /// @brief SMStateCommManager instance
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_09001, SR_SM_09008
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08469
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< sm_state_comm::SMStateCommManager > smStateCommManager_;

    /// @brief StateMachineManager instance
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr= SR_SM_09001, SR_SM_09002, SR_SM_09004, SR_SM_09006, SR_SM_09008
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08470
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< state_machine_management::StateMachineManager > stateMachineManager_;

    /// @brief Global configuration instance, used to obtain configuration information
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_05003, SR_SM_05004, SR_SM_09001, SR_SM_09002, SR_SM_09004, SR_SM_09006, SR_SM_09008
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08471
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< config::GlobalConfig > globalConfigInstance_;

    /// @brief Whether it has started
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04005, SR_SM_04006, SR_SM_04007,
    /// SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_06001, SR_SM_06002, SR_SM_06003, SR_SM_06004, SR_SM_07001, SR_SM_07002, SR_SM_08001,
    /// SR_SM_09001, SR_SM_09002, SR_SM_09004, SR_SM_09006, SR_SM_09008, SR_SM_10001
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08472
    /// @needwork = dda
    /// @endcode
    bool started_{false};

    /// @brief Used to capture SIGTERM signal
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04005, SR_SM_04006, SR_SM_04007,
    /// SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_06001, SR_SM_06002, SR_SM_06003, SR_SM_06004, SR_SM_07001, SR_SM_07002, SR_SM_08001,
    /// SR_SM_09001, SR_SM_09002, SR_SM_09004, SR_SM_09006, SR_SM_09008, SR_SM_10001
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08495
    /// @needwork = dda
    /// @endcode
    isoft::naicpp::TerminatingHandler terminatingHandler_;

    /// @brief Used to block the main thread
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04005, SR_SM_04006, SR_SM_04007,
    /// SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_06001, SR_SM_06002, SR_SM_06003, SR_SM_06004, SR_SM_07001, SR_SM_07002, SR_SM_08001,
    /// SR_SM_09001, SR_SM_09002, SR_SM_09004, SR_SM_09006, SR_SM_09008, SR_SM_10001
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08496
    /// @needwork = dda
    /// @endcode
    core::Promise< void > promise_;

#ifdef ARA_TEST_SHELL
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    std::shared_ptr< isoft::naicpp::EvNodeTimer > innerTimer_;
#endif
};

}  // namespace event_management
}  // namespace sm
}  // namespace ara

#endif