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
/// @file       shell_comm_manager.h
/// @brief      The class for managing communication with ShellApp for changing/getting function group state and state machine state
/// @details
/// @date       2024-05-07
/// @author     lianglongxiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/StateManagement/ShellComm
/// @unit_name=ShellCommManager
/// @interface_level=module
/// @unit_description=The class for managing communication with ShellApp for changing/getting function group state and state machine state
/// @trace_id_sr=SR_SM_10001
/// @endcode
///
/// ================================================================

#ifndef SHELL_COMM_MANAGER_H_
#define SHELL_COMM_MANAGER_H_

#include "ara/log/logger.h"
#include "i_communication_manager.h"
#include "shell_request_impl.h"

namespace ara {
namespace sm {
namespace shell_comm {

/// @brief The class for managing communication with SHellApp for changing/getting function group state and state machine state
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_10001
/// @trace_id_ad=AD_SM_08121
/// @trace_id_dd=DD_SM_08452
/// @needwork = ad
/// @endcode
class ShellCommManager : public common::ICommunicationManager
{
public:
    /// @brief Constructor function
    /// @param name CommManager name
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_10001
    /// @trace_id_ad=AD_SM_00607
    /// @trace_id_dd=DD_SM_00727
    /// @needwork = ad
    /// @endcode
    explicit ShellCommManager(core::String const &name) noexcept;

    /// @brief Destructor function
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_10001
    /// @trace_id_ad=AD_SM_00608
    /// @trace_id_dd=DD_SM_00728
    /// @needwork = ad
    /// @endcode
    ~ShellCommManager() noexcept override;

    /// @brief deleted copy constructor function
    /// @param other The ShellCommManager instance to be copyed
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_10001
    /// @trace_id_ad=AD_SM_00609
    /// @trace_id_dd=DD_SM_00729
    /// @needwork = ad
    /// @endcode
    ShellCommManager(ShellCommManager const &other) = delete;

    /// @brief deleted copy assignment function
    /// @param other The ShellCommManager instance to be copyed
    /// @return the assigned ShellCommManager instance itself
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_10001
    /// @trace_id_ad=AD_SM_00610
    /// @trace_id_dd=DD_SM_00730
    /// @needwork = ad
    /// @endcode
    ShellCommManager &operator=(ShellCommManager const &other) = delete;

    /// @brief Move constructor function
    /// @param other The ShellCommManager instance to be moved
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_10001
    /// @trace_id_ad=AD_SM_00611
    /// @trace_id_dd=DD_SM_00731
    /// @needwork = ad
    /// @endcode
    ShellCommManager(ShellCommManager &&other) = default;

    /// @brief Move assignment function
    /// @param other The ShellCommManager instance to be moved
    /// @return the assigned ShellCommManager instance itself
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_10001
    /// @trace_id_ad=AD_SM_00612
    /// @trace_id_dd=DD_SM_00732
    /// @needwork = ad
    /// @endcode
    ShellCommManager &operator=(ShellCommManager &&other) = delete;

    /// @brief Initialize
    /// @return true - success
    /// @return false - failure
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_10001
    /// @trace_id_ad=AD_SM_00613
    /// @trace_id_dd=DD_SM_00733
    /// @needwork = ad
    /// @endcode
    bool Init() noexcept override;

    /// @brief Start accepting requests
    /// @return true - success
    /// @return false - failure
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_10001
    /// @trace_id_ad=AD_SM_00614
    /// @trace_id_dd=DD_SM_00734
    /// @needwork = ad
    /// @endcode
    bool Start() noexcept override;

    /// @brief Stop accepting requests
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_10001
    /// @trace_id_ad=AD_SM_00615
    /// @trace_id_dd=DD_SM_00735
    /// @needwork = ad
    /// @endcode
    void Stop() noexcept override;

    /// @brief Register the callback function for appending events
    /// @param appendEventHandler Callback function for appending events
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_10001
    /// @trace_id_ad=AD_SM_00616
    /// @trace_id_dd=DD_SM_00736
    /// @needwork = ad
    /// @endcode
    void RegisterAppendEventHandler(std::function< void(common::Event &&) > appendEventHandler) noexcept override;

    /// @code{.isoft}
    /// @interface_level=module
    /// @needwork = no
    /// @endcode
    void NotifySMStateUpdate(core::String const &smFQN, core::String const &smState) const noexcept;

private:
    /// @brief ShellRequestImpl communication server Instance
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_10001
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08453
    /// @needwork = dda
    /// @endcode
    std::unique_ptr< ShellRequestImpl > shellRequestInstance_{nullptr};

    /// @brief Whether it has started
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_10001
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08454
    /// @needwork = dda
    /// @endcode
    bool started_{false};

    /// @brief Logger instance
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_10001
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08455
    /// @needwork = dda
    /// @endcode
    log::Logger &log_{log::CreateLogger((core::StringView{"#CMG"}), (core::StringView{"ShellCommManager context"}))};
};
}  // namespace shell_comm
}  // namespace sm
}  // namespace ara

#endif  // SHELL_COMM_MANAGER_H_
