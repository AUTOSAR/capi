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
/// @file       fg_state_comm_manager.h
/// @brief      Management class for function group state communication
/// @details
/// @date       2024-05-03
/// @author     lianglongxiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/StateManagement/FGStateComm
/// @unit_name=FGStateCommManager
/// @interface_level=module
/// @unit_description=Management class for function group state communication
/// @trace_id_sr=SR_SM_01001, SR_SM_01002, SR_SM_01003
/// @endcode
///
/// ================================================================

#ifndef FG_STATE_COMM_MANAGER_H_
#define FG_STATE_COMM_MANAGER_H_

#include <ara/core/map.h>

#include <functional>
#include <memory>
#include <string>

#include "config/global_config.h"
#include "fg_state_comm/trigger_fg_state_server.h"
#include "i_communication_manager.h"

namespace ara {
namespace sm {
namespace fg_state_comm {

/// @brief The class for managing communication with AA for changing/getting/being notified the function group state
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_01001, SR_SM_01002, SR_SM_01003
/// @trace_id_ad=AD_SM_08114
/// @trace_id_dd=DD_SM_08431
/// @needwork = ad
/// @endcode
class FGStateCommManager : public common::ICommunicationManager
{
public:
    /// @brief Constructor function
    /// @param name CommManager name
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_01001, SR_SM_01002, SR_SM_01003
    /// @trace_id_ad=AD_SM_00548
    /// @trace_id_dd=DD_SM_00664
    /// @needwork = ad
    /// @endcode
    explicit FGStateCommManager(core::String const &name) noexcept;

    /// @brief Destructor function
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_01001, SR_SM_01002, SR_SM_01003
    /// @trace_id_ad=AD_SM_00549
    /// @trace_id_dd=DD_SM_00665
    /// @needwork = ad
    /// @endcode
    ~FGStateCommManager() noexcept override;

    /// @brief deleted copy constructor function
    /// @param other The FGStateCommManager instance to be copyed
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_01001, SR_SM_01002, SR_SM_01003
    /// @trace_id_ad=AD_SM_00550
    /// @trace_id_dd=DD_SM_00666
    /// @needwork = ad
    /// @endcode
    FGStateCommManager(FGStateCommManager const &other) = delete;

    /// @brief deleted copy assignment function
    /// @param other The FGStateCommManager instance to be copyed
    /// @return the assigned FGStateCommManager instance itself
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_01001, SR_SM_01002, SR_SM_01003
    /// @trace_id_ad=AD_SM_00551
    /// @trace_id_dd=DD_SM_00667
    /// @needwork = ad
    /// @endcode
    FGStateCommManager &operator=(FGStateCommManager const &other) = delete;

    /// @brief Move constructor function
    /// @param other The FGStateCommManager instance to be moved
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_01001, SR_SM_01002, SR_SM_01003
    /// @trace_id_ad=AD_SM_00552
    /// @trace_id_dd=DD_SM_00668
    /// @needwork = ad
    /// @endcode
    FGStateCommManager(FGStateCommManager &&other) = default;

    /// @brief Move assignment function
    /// @param other The FGStateCommManager instance to be moved
    /// @return the assigned FGStateCommManager instance itself
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_01001, SR_SM_01002, SR_SM_01003
    /// @trace_id_ad=AD_SM_00553
    /// @trace_id_dd=DD_SM_00669
    /// @needwork = ad
    /// @endcode
    FGStateCommManager &operator=(FGStateCommManager &&other) = delete;

    /// @brief Initialize
    /// @return true - success
    /// @return false - failure
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_01001, SR_SM_01002, SR_SM_01003
    /// @trace_id_ad=AD_SM_00554
    /// @trace_id_dd=DD_SM_00670
    /// @needwork = ad
    /// @endcode
    bool Init() noexcept override;

    /// @brief Start accepting requests
    /// @return true - success
    /// @return false - failure
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_01001, SR_SM_01002, SR_SM_01003
    /// @trace_id_ad=AD_SM_00555
    /// @trace_id_dd=DD_SM_00671
    /// @needwork = ad
    /// @endcode
    bool Start() noexcept override;

    /// @brief Stop accepting requests
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_01001, SR_SM_01002, SR_SM_01003
    /// @trace_id_ad=AD_SM_00556
    /// @trace_id_dd=DD_SM_00672
    /// @needwork = ad
    /// @endcode
    void Stop() noexcept override;

    /// @brief Register the callback function for appending events
    /// @param appendEventHandler Callback function for appending events
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_01001, SR_SM_01002, SR_SM_01003
    /// @trace_id_ad=AD_SM_00557
    /// @trace_id_dd=DD_SM_00673
    /// @needwork = ad
    /// @endcode
    void RegisterAppendEventHandler(std::function< void(common::Event &&) > appendEventHandler) noexcept override;

    /// @brief Notify function group state update
    /// @param fgFQN Function group name
    /// @param fgState Function group state
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_01002, SR_SM_01003
    /// @trace_id_ad=AD_SM_00558
    /// @trace_id_dd=DD_SM_00674
    /// @needwork = ad
    /// @endcode
    void NotifyFGStateUpdate(core::String const &fgFQN, core::String const &fgState) const noexcept;

    /// @brief Set the global configuration instance
    /// @param globalConfigInstance Global configuration instance
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_01001, SR_SM_01002, SR_SM_01003
    /// @trace_id_ad=AD_SM_00559
    /// @trace_id_dd=DD_SM_00675
    /// @needwork = ad
    /// @endcode
    void SetGlobalConfigInstance(std::shared_ptr< config::GlobalConfig > const &globalConfigInstance) noexcept;

private:
    /// @brief Global configuration instance, used to obtain function group service configuration information
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_01001, SR_SM_01002, SR_SM_01003
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08432
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< config::GlobalConfig > globalConfigInstance_{};

    /// @brief Set of function group state service instances
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_01001, SR_SM_01002, SR_SM_01003
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08433
    /// @needwork = dda
    /// @endcode
    core::Map< core::String, std::unique_ptr< TriggerFGStateServer > > triggerFGStateServers_;

    /// @brief Whether it has started
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_01001, SR_SM_01002, SR_SM_01003
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08434
    /// @needwork = dda
    /// @endcode
    bool started_;

    /// @brief Logger instance
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_01001, SR_SM_01002, SR_SM_01003
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08435
    /// @needwork = dda
    /// @endcode
    log::Logger &log_;
};

}  // namespace fg_state_comm
}  // namespace sm
}  // namespace ara

#endif  // TRIGGER_OUT_FUNCTION_GROUP_CONTROL_SERVER_H_
