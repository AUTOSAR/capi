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
/// @file       cancellation_state.h
/// @brief      This file provides cancellation_state functionality
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
/// @trace_id_sr=edb12ecd-2fac-4699-bae7-328631dcb14b
/// @unit_name=CancellationState
/// @unit_description=This file provides cancellation_state functionality
/// @endcode
///
/// ================================================================

#ifndef ISOFT_UDS_SERVER_CANCELLATIONSTATE_H_
#define ISOFT_UDS_SERVER_CANCELLATIONSTATE_H_

#include <atomic>
#include <functional>
#include <memory>
#include <vector>
namespace isoft {
namespace uds {
namespace server {

/// @code{.isoft}
///
/// export_level=/Diagnostics
///
/// @trace_id_sws={SWS_DM_00608}@tracestatus{draft}
/// @endcode

/// @brief Cancellation state
/// @code{.isoft}
/// @note None
/// @interface_level=unit
/// @needwork=ad
/// @trace_id_sws={SWS_DM_00609}@tracestatus{draft}
/// @endcode
class CancellationState final
{
public:
    /// @brief Constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00889
    /// @trace_id_dd=DD_UDS_02003
    /// @needwork = ad
    /// @endcode
    CancellationState();

    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00890
    /// @trace_id_dd=DD_UDS_02004
    /// @needwork = ad
    /// @endcode
    ~CancellationState() noexcept = default;

    /// @code{.isoft}
    ///
    /// export_level=/Diagnostics
    ///
    /// @trace_id_sws={SWS_DM_00610}@tracestatus{draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00879
    /// @trace_id_dd=DD_UDS_00859
    /// @needwork = ad
    /// @endcode
    CancellationState(CancellationState&&) noexcept = delete;

    /// @code{.isoft}
    ///
    /// export_level=/Diagnostics
    ///
    /// @trace_id_sws={SWS_DM_00611}@tracestatus{draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00880
    /// @trace_id_dd=DD_UDS_00860
    /// @needwork = ad
    /// @endcode
    CancellationState(CancellationState&) = delete;

    /// @return CancellationHandler&
    /// @code{.isoft}
    ///
    /// export_level=/Diagnostics
    /// @trace_id_sws={SWS_DM_00612}@tracestatus{draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00881
    /// @trace_id_dd=DD_UDS_00861
    /// @needwork = ad
    /// @endcode
    CancellationState& operator=(CancellationState&&) noexcept = delete;

    /// @return CancellationHandler&
    /// @code{.isoft}
    ///
    /// export_level=/Diagnostics
    /// @trace_id_sws={SWS_DM_00613}@tracestatus{draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00882
    /// @trace_id_dd=DD_UDS_00862
    /// @needwork = ad
    /// @endcode
    CancellationState& operator=(CancellationState&) = delete;

    /// @brief Determine whether it has been cancelled
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00891
    /// @trace_id_dd=DD_UDS_02005
    /// @needwork = ad
    /// @endcode
    bool IsCanceled() const;

    /// @brief Add cancellation notification function
    /// @param[in] notifier Callback function
    /// @throws on overflow
    ///
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=ad
    /// @trace_id_sws={SWS_DM_00616}@tracestatus{draft}
    /// @endcode
    void AddNotifier(std::function< void(void) > const& notifier);

    /// @brief Execute cancellation notification function
    /// @throws on overflow
    ///
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=ad
    /// @trace_id_sws={SWS_DM_00617}@tracestatus{draft}
    /// @endcode
    void DoCanceled() noexcept;

private:
    /// @brief Cancellation notification function table
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @needwork=ad
    /// @trace_id_sws={SWS_DM_00618}@tracestatus{draft}
    /// @endcode
    std::vector< std::function< void(void) > > notifierTable_;

    /// @brief Cancellation state
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @needwork=ad
    /// @trace_id_sws={SWS_DM_00619}@tracestatus{draft}
    /// @endcode
    std::atomic_bool cancelState_{false};
};

}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  // ISOFT_UDS_SERVER_CANCELLATIONSTATE_H_