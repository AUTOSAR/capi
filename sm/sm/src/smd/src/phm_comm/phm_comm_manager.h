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
/// @file       phm_comm_manager.h
/// @brief      Management class for communication with Platform Health Management, implementing error recovery functionality
/// @details
/// @date       2024-05-07
/// @author     lianglongxiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/StateManagement/PHMComm
/// @unit_name=PHMCommManager
/// @interface_level=module
/// @unit_description=Management class for communication with Platform Health Management, implementing error recovery functionality
/// @trace_id_sr=SR_SM_08001
/// @endcode
///
/// ================================================================

#ifndef PHM_COMM_MANAGER_H_
#define PHM_COMM_MANAGER_H_

#include <memory>

#include "engine_recovery_action.h"
#include "hc_testunit_health_chanel_action.h"
#include "i_communication_manager.h"
#include "sv_testunit_recovery_action.h"
#include "tyre_pressure_health_chanel_action.h"
#include "vol_health_chanel_action.h"
#include "wheel_recovery_action.h"

namespace ara {
namespace sm {
namespace phm_comm {

/// @brief The class for managing communication with PHM for error recovery
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_08001
/// @trace_id_ad=AD_SM_08043
/// @trace_id_dd=DD_SM_08114
/// @needwork = ad
/// @endcode
class PHMCommManager : public common::ICommunicationManager
{
public:
    /// @brief Constructor function
    /// @param name CommManager name
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_08001
    /// @trace_id_ad=AD_SM_00095
    /// @trace_id_dd=DD_SM_00095
    /// @needwork = ad
    /// @endcode
    explicit PHMCommManager(core::String const &name) noexcept;

    /// @brief Destructor function
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_08001
    /// @trace_id_ad=AD_SM_00096
    /// @trace_id_dd=DD_SM_00096
    /// @needwork = ad
    /// @endcode
    ~PHMCommManager() noexcept override;

    /// @brief deleted copy constructor function
    /// @param other The PHMCommManager instance to be copyed
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_08001
    /// @trace_id_ad=AD_SM_00097
    /// @trace_id_dd=DD_SM_00097
    /// @needwork = ad
    /// @endcode
    PHMCommManager(PHMCommManager const &other) = delete;

    /// @brief deleted copy assignment function
    /// @param other The PHMCommManager instance to be copyed
    /// @return the assigned PHMCommManager instance itself
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_08001
    /// @trace_id_ad=AD_SM_00098
    /// @trace_id_dd=DD_SM_00098
    /// @needwork = ad
    /// @endcode
    PHMCommManager &operator=(PHMCommManager const &other) = delete;

    /// @brief Move constructor function
    /// @param other The PHMCommManager instance to be moved
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_08001
    /// @trace_id_ad=AD_SM_00099
    /// @trace_id_dd=DD_SM_00099
    /// @needwork = ad
    /// @endcode
    PHMCommManager(PHMCommManager &&other) noexcept;

    /// @brief Move assignment function
    /// @param other The PHMCommManager instance to be moved
    /// @return the assigned PHMCommManager instance itself
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_08001
    /// @trace_id_ad=AD_SM_00100
    /// @trace_id_dd=DD_SM_00100
    /// @needwork = ad
    /// @endcode
    PHMCommManager &operator=(PHMCommManager &&other) = delete;

    /// @brief Initialize
    /// @return true - success
    /// @return false - failure
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_08001
    /// @trace_id_ad=AD_SM_00101
    /// @trace_id_dd=DD_SM_00101
    /// @needwork = ad
    /// @endcode
    bool Init() noexcept override;

    /// @brief Start accepting requests
    /// @return true - success
    /// @return false - failure
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_08001
    /// @trace_id_ad=AD_SM_00102
    /// @trace_id_dd=DD_SM_00102
    /// @needwork = ad
    /// @endcode
    bool Start() noexcept override;

    /// @brief Stop accepting requests
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_08001
    /// @trace_id_ad=AD_SM_00103
    /// @trace_id_dd=DD_SM_00103
    /// @needwork = ad
    /// @endcode
    void Stop() noexcept override;

    /// @brief Register the callback function for appending events
    /// @param appendEventHandler Callback function for appending events
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_08001
    /// @trace_id_ad=AD_SM_00104
    /// @trace_id_dd=DD_SM_00104
    /// @needwork = ad
    /// @endcode
    void RegisterAppendEventHandler(std::function< void(common::Event &&) > appendEventHandler) noexcept override;

private:
    /// @brief Whether it has started
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_08001
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08115
    /// @needwork = dda
    /// @endcode
    bool started_{false};

    /// @brief Voltage health channel
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_08001
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08116
    /// @needwork = dda
    /// @endcode
    std::unique_ptr< VolHealthChannelAction > volHealthChannelAction_{nullptr};

    /// @brief Test unit health channel
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_08001
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08117
    /// @needwork = dda
    /// @endcode
    std::unique_ptr< HcTestUnitHealthChannelAction > hcTestUnitHealthChannelAction_{nullptr};

    /// @brief Tire pressure health channel
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_08001
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08118
    /// @needwork = dda
    /// @endcode
    std::unique_ptr< TyrePressureHealthChannelAction > tyrePressureHealthChannelAction_{nullptr};

    /// @brief Engine monitoring recovery
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_08001
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08119
    /// @needwork = dda
    /// @endcode
    std::unique_ptr< EngineRecoveryAction > engineRecoveryAction_{nullptr};

    /// @brief Test unit monitoring recovery
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_08001
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08120
    /// @needwork = dda
    /// @endcode
    std::unique_ptr< SvTestUnitRecoveryAction > svTestUnitRecoveryAction_{nullptr};

    /// @brief Wheel speed monitoring recovery
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_08001
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08121
    /// @needwork = dda
    /// @endcode
    std::unique_ptr< WheelRecoveryAction > wheelRecoveryAction_{nullptr};

    /// @brief Logger instance
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_08001
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08122
    /// @needwork = dda
    /// @endcode
    log::Logger &log_;
};

}  // namespace phm_comm
}  // namespace sm
}  // namespace ara

#endif  // TRIGGER_OUT_FUNCTION_GROUP_CONTROL_SERVER_H_
