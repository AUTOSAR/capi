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
/// @file       i_communication_manager.h
/// @brief      Interface base class for communication management
/// @details
/// @date       2024-05-07
/// @author     lianglongxiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/StateManagement/Utils
/// @unit_name=ICommunicationManager
/// @interface_level=module
/// @unit_description=Interface base class for communication management
/// @trace_id_sr=SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008,
/// SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_06001, SR_SM_06002, SR_SM_06003, SR_SM_06004, SR_SM_07001, SR_SM_07002, SR_SM_09001, SR_SM_09008, SR_SM_10001,
/// SR_SM_08001
/// @endcode
///
/// ================================================================

#ifndef INC_I_MS_CONTROL_SERVER_H_
#define INC_I_MS_CONTROL_SERVER_H_

#include <functional>
#include <string>

#include "event.h"

namespace ara {
namespace sm {
namespace common {

/// @brief interface for all CommunicationManagers.
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008,
/// SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_06001, SR_SM_06002, SR_SM_06003, SR_SM_06004, SR_SM_07001, SR_SM_07002, SR_SM_09001, SR_SM_09008, SR_SM_10001,
/// SR_SM_08001
/// @trace_id_ad=AD_SM_08105
/// @trace_id_dd=DD_SM_08393
/// @needwork = ad
/// @endcode
class ICommunicationManager
{
public:
    /// @brief Constructor function
    /// @param name Communication manager name
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04005, SR_SM_04006, SR_SM_04007,
    /// SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_06001, SR_SM_06002, SR_SM_06003, SR_SM_06004, SR_SM_07001, SR_SM_07002, SR_SM_09001,
    /// SR_SM_09008, SR_SM_10001, SR_SM_08001
    /// @trace_id_ad=AD_SM_00485
    /// @trace_id_dd=DD_SM_00501
    /// @needwork = ad
    /// @endcode
    explicit ICommunicationManager(core::String name) noexcept : name_{std::move(name)} {}

    /// @brief Destructor function
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04005, SR_SM_04006, SR_SM_04007,
    /// SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_06001, SR_SM_06002, SR_SM_06003, SR_SM_06004, SR_SM_07001, SR_SM_07002, SR_SM_09001,
    /// SR_SM_09008, SR_SM_10001, SR_SM_08001
    /// @trace_id_ad=AD_SM_00486
    /// @trace_id_dd=DD_SM_00502
    /// @needwork = ad
    /// @endcode
    virtual ~ICommunicationManager() = default;

    /// @brief deleted copy constructor function
    /// @param other The ICommunicationManager instance to be copyed
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04005, SR_SM_04006, SR_SM_04007,
    /// SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_06001, SR_SM_06002, SR_SM_06003, SR_SM_06004, SR_SM_07001, SR_SM_07002, SR_SM_09001,
    /// SR_SM_09008, SR_SM_10001, SR_SM_08001
    /// @trace_id_ad=AD_SM_00487
    /// @trace_id_dd=DD_SM_00503
    /// @needwork = ad
    /// @endcode
    ICommunicationManager(ICommunicationManager const &other) = delete;

    /// @brief deleted copy assignment function
    /// @param other The ICommunicationManager instance to be copyed
    /// @return the assigned ICommunicationManager instance itself
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04005, SR_SM_04006, SR_SM_04007,
    /// SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_06001, SR_SM_06002, SR_SM_06003, SR_SM_06004, SR_SM_07001, SR_SM_07002, SR_SM_09001,
    /// SR_SM_09008, SR_SM_10001, SR_SM_08001
    /// @trace_id_ad=AD_SM_00488
    /// @trace_id_dd=DD_SM_00504
    /// @needwork = ad
    /// @endcode
    ICommunicationManager &operator=(ICommunicationManager const &other) = delete;

    /// @brief Move constructor function
    /// @param other The ICommunicationManager instance to be moved
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04005, SR_SM_04006, SR_SM_04007,
    /// SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_06001, SR_SM_06002, SR_SM_06003, SR_SM_06004, SR_SM_07001, SR_SM_07002, SR_SM_09001,
    /// SR_SM_09008, SR_SM_10001, SR_SM_08001
    /// @trace_id_ad=AD_SM_00489
    /// @trace_id_dd=DD_SM_00505
    /// @needwork = ad
    /// @endcode
    ICommunicationManager(ICommunicationManager &&other) = default;

    /// @brief Move assignment function
    /// @param other The ICommunicationManager instance to be moved
    /// @return the assigned ICommunicationManager instance itself
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04005, SR_SM_04006, SR_SM_04007,
    /// SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_06001, SR_SM_06002, SR_SM_06003, SR_SM_06004, SR_SM_07001, SR_SM_07002, SR_SM_09001,
    /// SR_SM_09008, SR_SM_10001, SR_SM_08001
    /// @trace_id_ad=AD_SM_00490
    /// @trace_id_dd=DD_SM_00506
    /// @needwork = ad
    /// @endcode
    ICommunicationManager &operator=(ICommunicationManager &&other) = delete;

    /// @brief Get the communication manager name
    /// @return Communication manager name
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04005, SR_SM_04006, SR_SM_04007,
    /// SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_06001, SR_SM_06002, SR_SM_06003, SR_SM_06004, SR_SM_07001, SR_SM_07002, SR_SM_09001,
    /// SR_SM_09008, SR_SM_10001, SR_SM_08001
    /// @trace_id_ad=AD_SM_00491
    /// @trace_id_dd=DD_SM_00507
    /// @needwork = ad
    /// @endcode
    core::String const Name() const noexcept { return name_; }

    /// @brief Initialize
    /// @return true - success
    /// @return false - failure
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04005, SR_SM_04006, SR_SM_04007,
    /// SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_06001, SR_SM_06002, SR_SM_06003, SR_SM_06004, SR_SM_07001, SR_SM_07002, SR_SM_09001,
    /// SR_SM_09008, SR_SM_10001, SR_SM_08001
    /// @trace_id_ad=AD_SM_00492
    /// @trace_id_dd=DD_SM_00508
    /// @needwork = ad
    /// @endcode
    virtual bool Init() noexcept = 0;

    /// @brief start to work
    /// @return true - success
    /// @return false - failure
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04005, SR_SM_04006, SR_SM_04007,
    /// SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_06001, SR_SM_06002, SR_SM_06003, SR_SM_06004, SR_SM_07001, SR_SM_07002, SR_SM_09001,
    /// SR_SM_09008, SR_SM_10001, SR_SM_08001
    /// @trace_id_ad=AD_SM_00493
    /// @trace_id_dd=DD_SM_00509
    /// @needwork = ad
    /// @endcode
    virtual bool Start() noexcept = 0;

    /// @brief  Stop working.
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04005, SR_SM_04006, SR_SM_04007,
    /// SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_06001, SR_SM_06002, SR_SM_06003, SR_SM_06004, SR_SM_07001, SR_SM_07002, SR_SM_09001,
    /// SR_SM_09008, SR_SM_10001, SR_SM_08001
    /// @trace_id_ad=AD_SM_00494
    /// @trace_id_dd=DD_SM_00510
    /// @needwork = ad
    /// @endcode
    virtual void Stop() noexcept = 0;

    /// @brief Register the callback function for appending events
    /// @param appendEventHandler Callback function for appending events
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04005, SR_SM_04006, SR_SM_04007,
    /// SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_06001, SR_SM_06002, SR_SM_06003, SR_SM_06004, SR_SM_07001, SR_SM_07002, SR_SM_09001,
    /// SR_SM_09008, SR_SM_10001, SR_SM_08001
    /// @trace_id_ad=AD_SM_00495
    /// @trace_id_dd=DD_SM_00511
    /// @needwork = ad
    /// @endcode
    virtual void RegisterAppendEventHandler(std::function< void(common::Event &&) > appendEventHandler) noexcept = 0;

private:
    /// @brief Communication manager name
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_01001, SR_SM_01002, SR_SM_01003, SR_SM_02001, SR_SM_02002, SR_SM_03002, SR_SM_04001, SR_SM_04005, SR_SM_04006, SR_SM_04007,
    /// SR_SM_04008, SR_SM_04009, SR_SM_05003, SR_SM_05004, SR_SM_06001, SR_SM_06002, SR_SM_06003, SR_SM_06004, SR_SM_07001, SR_SM_07002, SR_SM_09001,
    /// SR_SM_09008, SR_SM_10001, SR_SM_08001
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08394
    /// @needwork = dda
    /// @endcode
    core::String name_;
};

}  // namespace common
}  // namespace sm
}  // namespace ara

#endif  // INC_I_MS_CONTROL_SERVER_H_
