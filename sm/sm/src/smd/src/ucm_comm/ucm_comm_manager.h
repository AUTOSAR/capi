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
/// @file       ucm_comm_manager.h
/// @brief      =Management class for communication with UCM
/// @details
/// @date       2024-06-26
/// @author     lianglongxiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/StateManagement/UCMComm
/// @unit_name=UCMCommManager
/// @interface_level=module
/// @unit_description=Management class for communication with UCM
/// @trace_id_sr=SR_SM_04001, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009
/// @endcode
///
/// ================================================================

#ifndef UCM_STATE_COMM_MANAGER_H_
#define UCM_STATE_COMM_MANAGER_H_

#include <ara/core/map.h>

#include <functional>
#include <memory>
#include <string>

#include "i_communication_manager.h"
#include "update_request_impl.h"

namespace ara {
namespace sm {
namespace ucm_comm {

/// @brief The class for managing communication with UCM for updating configuration machine state
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_04001, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009
/// @trace_id_ad=AD_SM_08119
/// @trace_id_dd=DD_SM_08445
/// @needwork = ad
/// @endcode
class UCMCommManager : public common::ICommunicationManager
{
public:
    /// @brief Constructor function
    /// @param name CommManager name
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_04001, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009
    /// @trace_id_ad=AD_SM_00587
    /// @trace_id_dd=DD_SM_00703
    /// @needwork = ad
    /// @endcode
    explicit UCMCommManager(core::String const &name) noexcept;

    /// @brief Destructor function
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_04001, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009
    /// @trace_id_ad=AD_SM_00588
    /// @trace_id_dd=DD_SM_00704
    /// @needwork = ad
    /// @endcode
    ~UCMCommManager() noexcept override;

    /// @brief deleted copy constructor function
    /// @param other The UCMCommManager instance to be copyed
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_04001, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009
    /// @trace_id_ad=AD_SM_00589
    /// @trace_id_dd=DD_SM_00705
    /// @needwork = ad
    /// @endcode
    UCMCommManager(UCMCommManager const &other) = delete;

    /// @brief deleted copy assignment function
    /// @param other The UCMCommManager instance to be copyed
    /// @return the assigned UCMCommManager instance itself
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_04001, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009
    /// @trace_id_ad=AD_SM_00590
    /// @trace_id_dd=DD_SM_00706
    /// @needwork = ad
    /// @endcode
    UCMCommManager &operator=(UCMCommManager const &other) = delete;

    /// @brief Move constructor function
    /// @param other The UCMCommManager instance to be moved
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_04001, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009
    /// @trace_id_ad=AD_SM_00591
    /// @trace_id_dd=DD_SM_00707
    /// @needwork = ad
    /// @endcode
    UCMCommManager(UCMCommManager &&other) = default;

    /// @brief Move assignment function
    /// @param other The UCMCommManager instance to be moved
    /// @return the assigned UCMCommManager instance itself
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_04001, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009
    /// @trace_id_ad=AD_SM_00592
    /// @trace_id_dd=DD_SM_00708
    /// @needwork = ad
    /// @endcode
    UCMCommManager &operator=(UCMCommManager &&other) = delete;

    /// @brief Initialize
    /// @return true - success
    /// @return false - failure
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_04001, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009
    /// @trace_id_ad=AD_SM_00593
    /// @trace_id_dd=DD_SM_00709
    /// @needwork = ad
    /// @endcode
    bool Init() noexcept override;

    /// @brief Start accepting requests
    /// @return true - success
    /// @return false - failure
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_04001, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009
    /// @trace_id_ad=AD_SM_00594
    /// @trace_id_dd=DD_SM_00710
    /// @needwork = ad
    /// @endcode
    bool Start() noexcept override;

    /// @brief Stop accepting requests
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_04001, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009
    /// @trace_id_ad=AD_SM_00595
    /// @trace_id_dd=DD_SM_00711
    /// @needwork = ad
    /// @endcode
    void Stop() noexcept override;

    /// @brief Register the callback function for appending events
    /// @param appendEventHandler Callback function for appending events
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_04001, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009
    /// @trace_id_ad=AD_SM_00596
    /// @trace_id_dd=DD_SM_00712
    /// @needwork = ad
    /// @endcode
    void RegisterAppendEventHandler(std::function< void(common::Event &&) > appendEventHandler) noexcept override;

private:
    /// @brief UpdateRequestImpl communication server Instance
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_04001, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08446
    /// @needwork = dda
    /// @endcode
    std::unique_ptr< UpdateRequestImpl > updateRequestInstance_{nullptr};

    /// @brief Whether it has started
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_04001, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08447
    /// @needwork = dda
    /// @endcode
    bool started_{false};

    /// @brief Logger instance
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_04001, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08448
    /// @needwork = dda
    /// @endcode
    log::Logger &log_{
        log::CreateLogger((core::StringView{"#CMG"}), (core::StringView{"Communication Manager context"}))};
};
}  // namespace ucm_comm
}  // namespace sm
}  // namespace ara

#endif  // TRIGGER_OUT_FUNCTION_GROUP_CONTROL_SERVER_H_
