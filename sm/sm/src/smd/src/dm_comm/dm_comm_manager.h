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
/// @file       dm_comm_manager.h
/// @brief      Management class for communication with Diagnostic Management
/// @details
/// @date       2024-04-30
/// @author     lianglongxiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/StateManagement/DMComm
/// @unit_name=DMCommManager
/// @interface_level=module
/// @unit_description=Management class for communication with Diagnostic Management
/// @trace_id_sr=SR_SM_03002
/// @endcode
///
/// ================================================================

#ifndef DM_COMM_MANAGER_H_
#define DM_COMM_MANAGER_H_

#include <ara/core/string.h>

#include <functional>
#include <memory>

#include "ecu_reset_request_impl.h"
#include "i_communication_manager.h"

namespace ara {
namespace sm {
namespace dm_comm {

/// @brief The class for managing communication with diagnostic management
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_03002
/// @trace_id_ad=AD_SM_08056
/// @trace_id_dd=DD_SM_08145
/// @needwork = ad
/// @endcode
class DMCommManager : public common::ICommunicationManager
{
public:
    /// @brief Constructor function
    /// @param name CommManager name
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_03002
    /// @trace_id_ad=AD_SM_00137
    /// @trace_id_dd=DD_SM_00137
    /// @needwork = ad
    /// @endcode
    explicit DMCommManager(core::String const &name) noexcept;

    /// @brief Destructor function
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_03002
    /// @trace_id_ad=AD_SM_00138
    /// @trace_id_dd=DD_SM_00138
    /// @needwork = ad
    /// @endcode
    ~DMCommManager() noexcept override;

    /// @brief deleted copy constructor function
    /// @param other The DMCommManager instance to be copyed
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_03002
    /// @trace_id_ad=AD_SM_00139
    /// @trace_id_dd=DD_SM_00139
    /// @needwork = ad
    /// @endcode
    DMCommManager(DMCommManager const &other) = delete;

    /// @brief deleted copy assignment function
    /// @param other The DMCommManager instance to be copyed
    /// @return the assigned DMCommManager instance itself
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_03002
    /// @trace_id_ad=AD_SM_00140
    /// @trace_id_dd=DD_SM_00140
    /// @needwork = ad
    /// @endcode
    DMCommManager &operator=(DMCommManager const &other) = delete;

    /// @brief Move constructor function
    /// @param other The DMCommManager instance to be moved
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_03002
    /// @trace_id_ad=AD_SM_00141
    /// @trace_id_dd=DD_SM_00141
    /// @needwork = ad
    /// @endcode
    DMCommManager(DMCommManager &&other) = default;

    /// @brief Move assignment function
    /// @param other The DMCommManager instance to be moved
    /// @return the assigned DMCommManager instance itself
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_03002
    /// @trace_id_ad=AD_SM_00142
    /// @trace_id_dd=DD_SM_00142
    /// @needwork = ad
    /// @endcode
    DMCommManager &operator=(DMCommManager &&other) = delete;

    /// @brief Initialize
    /// @return true - success
    /// @return false - failure
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_03002
    /// @trace_id_ad=AD_SM_00143
    /// @trace_id_dd=DD_SM_00143
    /// @needwork = ad
    /// @endcode
    bool Init() noexcept override;

    /// @brief Start accepting requests
    /// @return true - success
    /// @return false - failure
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_03002
    /// @trace_id_ad=AD_SM_00144
    /// @trace_id_dd=DD_SM_00144
    /// @needwork = ad
    /// @endcode
    bool Start() noexcept override;

    /// @brief Stop accepting requests
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_03002
    /// @trace_id_ad=AD_SM_00145
    /// @trace_id_dd=DD_SM_00145
    /// @needwork = ad
    /// @endcode
    void Stop() noexcept override;

    /// @brief Register the callback function for appending events
    /// @param appendEventHandler Callback function for appending events
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_03002
    /// @trace_id_ad=AD_SM_00146
    /// @trace_id_dd=DD_SM_00146
    /// @needwork = ad
    /// @endcode
    void RegisterAppendEventHandler(std::function< void(common::Event &&) > appendEventHandler) noexcept override;

private:
    /// @brief Logger instance
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_03002
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08146
    /// @needwork = dda
    /// @endcode
    log::Logger &log_;

    /// @brief Whether it has started
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_03002
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08147
    /// @needwork = dda
    /// @endcode
    bool started_;

    /// @brief EcuResetRequest communication server Instance
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_03002
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08148
    /// @needwork = dda
    /// @endcode
    std::unique_ptr< EcuResetRequestImpl > ecuResetRequestInstance_;
};
}  // namespace dm_comm
}  // namespace sm
}  // namespace ara

#endif  // TRIGGER_OUT_FUNCTION_GROUP_CONTROL_SERVER_H_
