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
/// @file       fg_state_notify_comm_manager.h
/// @brief      Management class for communication with PHM and IDSM, implementing function group state notification
/// @details
/// @date       2024-05-03
/// @author     lianglongxiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/StateManagement/FGStateNotifyComm
/// @unit_name=FGStateNotifyCommManager
/// @interface_level=module
/// @unit_description=Management class for communication with PHM and IDSM, implementing function group state notification
/// @trace_id_sr=SR_SM_02001, SR_SM_02002
/// @endcode
///
/// ================================================================

#ifndef FG_STATE_NOTIFY_COMM_MANAGER_H_
#define FG_STATE_NOTIFY_COMM_MANAGER_H_

#include <functional>
#include <memory>
#include <string>

#include "i_communication_manager.h"
#include "server.h"

// non-generated code
namespace ara {
namespace sm {
namespace fg_state_notify_comm {

/// @brief This class is responsible for communicating with phm and idsm, for obtaining and notifying function group states
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_02001, SR_SM_02002
/// @trace_id_ad=AD_SM_08093
/// @trace_id_dd=DD_SM_08336
/// @needwork = ad
/// @endcode
class FGStateNotifyCommManager : public common::ICommunicationManager
{
public:
    /// @brief Constructor function
    /// @param name CommManager name
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_02001, SR_SM_02002
    /// @trace_id_ad=AD_SM_00348
    /// @trace_id_dd=DD_SM_00364
    /// @needwork = ad
    /// @endcode
    explicit FGStateNotifyCommManager(core::String const &name) noexcept;

    /// @brief Destructor function
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_02001, SR_SM_02002
    /// @trace_id_ad=AD_SM_00349
    /// @trace_id_dd=DD_SM_00365
    /// @needwork = ad
    /// @endcode
    ~FGStateNotifyCommManager() noexcept override;

    /// @brief deleted copy constructor function
    /// @param other The FGStateNotifyCommManager instance to be copyed
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_02001, SR_SM_02002
    /// @trace_id_ad=AD_SM_00350
    /// @trace_id_dd=DD_SM_00366
    /// @needwork = ad
    /// @endcode
    FGStateNotifyCommManager(FGStateNotifyCommManager const &other) = delete;

    /// @brief deleted copy assignment function
    /// @param other The FGStateNotifyCommManager instance to be copyed
    /// @return the assigned FGStateNotifyCommManager instance itself
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_02001, SR_SM_02002
    /// @trace_id_ad=AD_SM_00351
    /// @trace_id_dd=DD_SM_00367
    /// @needwork = ad
    /// @endcode
    FGStateNotifyCommManager &operator=(FGStateNotifyCommManager const &other) = delete;

    /// @brief Move constructor function
    /// @param other The FGStateNotifyCommManager instance to be moved
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_02001, SR_SM_02002
    /// @trace_id_ad=AD_SM_00352
    /// @trace_id_dd=DD_SM_00368
    /// @needwork = ad
    /// @endcode
    FGStateNotifyCommManager(FGStateNotifyCommManager &&other) = default;

    /// @brief Move assignment function
    /// @param other The FGStateNotifyCommManager instance to be moved
    /// @return the assigned FGStateNotifyCommManager instance itself
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_02001, SR_SM_02002
    /// @trace_id_ad=AD_SM_00353
    /// @trace_id_dd=DD_SM_00369
    /// @needwork = ad
    /// @endcode
    FGStateNotifyCommManager &operator=(FGStateNotifyCommManager &&other) = delete;

    /// @brief Initialize
    /// @return true - success
    /// @return false - failure
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_02001, SR_SM_02002
    /// @trace_id_ad=AD_SM_00354
    /// @trace_id_dd=DD_SM_00370
    /// @needwork = ad
    /// @endcode
    bool Init() noexcept override;

    /// @brief Start accepting requests
    /// @return true - success
    /// @return false - failure
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_02001, SR_SM_02002
    /// @trace_id_ad=AD_SM_00355
    /// @trace_id_dd=DD_SM_00371
    /// @needwork = ad
    /// @endcode
    bool Start() noexcept override;

    /// @brief Stop accepting requests
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_02001, SR_SM_02002
    /// @trace_id_ad=AD_SM_00356
    /// @trace_id_dd=DD_SM_00372
    /// @needwork = ad
    /// @endcode
    void Stop() noexcept override;

    /// @brief Register the callback function for appending events
    /// @param appendEventHandler Callback function for appending events
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_02001, SR_SM_02002
    /// @trace_id_ad=AD_SM_00357
    /// @trace_id_dd=DD_SM_00373
    /// @needwork = ad
    /// @endcode
    void RegisterAppendEventHandler(std::function< void(common::Event &&) > appendEventHandler) noexcept override;

    /// @brief Notify function group state update
    /// @param fgFQN Function group name
    /// @param fgState Function group state
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_02002
    /// @trace_id_ad=AD_SM_00358
    /// @trace_id_dd=DD_SM_00374
    /// @needwork = ad
    /// @endcode
    void NotifyFGStateUpdate(core::String const &fgFQN, core::String const &fgState) const noexcept;

private:
    /// @brief FGStateNotifyComm communication server Instance
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_02001, SR_SM_02002
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08337
    /// @needwork = dda
    /// @endcode
    std::unique_ptr< fg_state_ipc::Server > serverInstance_{nullptr};

    /// @brief Whether it has started
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_02001, SR_SM_02002
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08338
    /// @needwork = dda
    /// @endcode
    bool started_{false};

    /// @brief Logger instance
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_02001, SR_SM_02002
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08339
    /// @needwork = dda
    /// @endcode
    log::Logger &log_{
        log::CreateLogger((core::StringView{"#CMG"}), (core::StringView{"FGStateNotifyCommManager context"}))};
};
}  // namespace fg_state_notify_comm
}  // namespace sm
}  // namespace ara

#endif  // FG_STATE_NOTIFY_COMM_MANAGER_H_
