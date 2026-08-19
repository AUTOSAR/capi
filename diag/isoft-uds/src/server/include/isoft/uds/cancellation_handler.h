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
/// @file       cancellation_handler.h
/// @brief      This file provides cancellation_handler functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/isoft/uds
/// @interface_level=component
/// @trace_id_sr=2b766c9e-e488-41f6-b62e-c9d31918fcb9
/// @unit_name=CancellationHandler
/// @unit_description=This file provides cancellation_handler functionality
/// @endcode
///
/// ================================================================

#ifndef ARA_DIAG_CANCELLATION_HANDLER_H_
#define ARA_DIAG_CANCELLATION_HANDLER_H_
#include <cstdint>
#include <functional>
#include <memory>

#include "isoft/uds/cancellation_state.h"

namespace isoft {
namespace uds {
namespace server {

/// @code{.isoft}
///
/// export_level=/Diagnostics
///
/// @trace_id_sws={SWS_DM_00608}@tracestatus{draft}
/// @endcode

/// @brief Cancellation handler
/// @code{.isoft}
/// @note None
/// @interface_level=unit
/// @needwork=ad
/// @endcode
class CancellationHandler final
{
public:
    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00958
    /// @trace_id_dd=DD_UDS_02072
    /// @needwork = ad
    /// @endcode
    ~CancellationHandler() noexcept = default;

    /// @code{.isoft}
    ///
    /// export_level=/Diagnostics
    ///
    /// @trace_id_sws={SWS_DM_00609}@tracestatus{draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00883
    /// @trace_id_dd=DD_UDS_00888
    /// @needwork = ad
    /// @endcode
    CancellationHandler() = delete;

    /// @brief Move constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00959
    /// @trace_id_dd=DD_UDS_02073
    /// @needwork = ad
    /// @endcode
    CancellationHandler(CancellationHandler&&) noexcept = default;

    /// @code{.isoft}
    ///
    /// export_level=/Diagnostics
    ///
    /// @trace_id_sws={SWS_DM_00611}@tracestatus{draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00884
    /// @trace_id_dd=DD_UDS_00889
    /// @needwork = ad
    /// @endcode
    CancellationHandler(CancellationHandler&) = delete;

    /// @brief Move assignment operator
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00960
    /// @trace_id_dd=DD_UDS_02074
    /// @needwork = ad
    /// @endcode
    CancellationHandler& operator=(CancellationHandler&&) noexcept = default;

    /// @return CancellationHandler&
    /// @code{.isoft}
    ///
    /// export_level=/Diagnostics
    /// @trace_id_sws={SWS_DM_00613}@tracestatus{draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00885
    /// @trace_id_dd=DD_UDS_00890
    /// @needwork = ad
    /// @endcode
    CancellationHandler& operator=(CancellationHandler&) = delete;

    /// @brief Determine whether it has been cancelled
    /// @return true
    /// @return false
    /// @throws on overflow
    ///
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=ad
    /// @trace_id_sws={SWS_DM_00614}@tracestatus{draft}
    /// @endcode
    bool IsCanceled() const;

    /// @brief Set cancellation function
    /// @param[in] notifier Callback function
    /// @throws on overflow
    ///
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=ad
    /// @trace_id_sws={SWS_DM_00616}@tracestatus{draft}
    /// @endcode
    void SetNotifier(std::function< void(void) > const& notifier);

    /// @brief Constructor
    /// @param[in] state Cancellation state
    /// @throws on overflow
    ///
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=ad
    /// @trace_id_sws={SWS_DM_00612}@tracestatus{draft}
    /// @endcode
    explicit CancellationHandler(std::shared_ptr< CancellationState > state) noexcept;

private:
    /// @brief Lifetime pointer
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @needwork=ad
    /// @trace_id_sws={SWS_DM_00617}@tracestatus{draft}
    /// @endcode
    std::shared_ptr< std::uint8_t > lifePtr_;
    /// @brief Cancellation state pointer
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @needwork=ad
    /// @trace_id_sws={SWS_DM_00618}@tracestatus{draft}
    /// @endcode
    std::shared_ptr< CancellationState > statePtr_;
};

}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  // ARA_DIAG_CANCELLATION_HANDLER_H_