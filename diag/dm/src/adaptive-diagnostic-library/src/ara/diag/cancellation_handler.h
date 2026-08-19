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
/// @brief      This file provides the definitions of CancellationHandler and related types.
/// @details
/// @date       2021-11-23
/// @author     jiawei
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_DIAG_CANCELLATION_HANDLER_H_
#define ARA_DIAG_CANCELLATION_HANDLER_H_

#include <functional>
#include <memory>
namespace isoft {
namespace dm {
class CancellationEvent;
/// @brief CancellationHandlerFactory
class CancellationHandlerFactory;
}  // namespace dm
}  // namespace isoft
namespace ara {
namespace diag {

/// @brief CancellationHandler contains a shared state if the processing should be canceled
///
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
///
/// @traceid{SWS_DM_00608}@tracestatus{draft}
class CancellationHandler final
{
public:
    /// @brief Destructor of CancellationHandler cannot be used
    ///
    ~CancellationHandler();

    /// @brief Constructor of CancellationHandler cannot be used
    ///
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    ///
    /// @traceid{SWS_DM_00609}@tracestatus{draft}
    CancellationHandler() = delete;

    /// @brief Move constructor of CancellationHandler
    ///
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    ///
    /// @traceid{SWS_DM_00610}@tracestatus{draft}
    CancellationHandler(CancellationHandler&&) noexcept = default;

    /// @brief Copy constructor of CancellationHandler cannot be used
    ///
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    ///
    /// @traceid{SWS_DM_00611}@tracestatus{draft}
    CancellationHandler(CancellationHandler&) = delete;

    /// @brief Move assignment operator of CancellationHandler
    ///
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @return CancellationHandler&
    /// @traceid{SWS_DM_00612}@tracestatus{draft}
    CancellationHandler& operator=(CancellationHandler&&) noexcept = default;

    /// @brief Copy assignment operator of CancellationHandler cannot be used
    ///
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @return CancellationHandler&
    /// @traceid{SWS_DM_00613}@tracestatus{draft}
    CancellationHandler& operator=(CancellationHandler&) = delete;

    /// @brief Returns true in if the diagnostic service execution is cancelled in DM.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @return true
    /// @return false
    /// @throws on overflow
    ///
    /// @traceid{SWS_DM_00614}@tracestatus{draft}
    bool IsCanceled() const;

    /// @brief Regisering a notifier function which is called if the diagnostic service execution is canceled in DM.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] notifier Callback function
    /// @throws on overflow
    ///
    /// @traceid{SWS_DM_00615}@tracestatus{draft}
    void SetNotifier(std::function< void(void) > notifier);

private:
    /// @brief Constructor
    /// @param[in] event Conversation event
    explicit CancellationHandler(std::shared_ptr< isoft::dm::CancellationEvent > event) noexcept;

    /// @brief event_
    std::shared_ptr< isoft::dm::CancellationEvent > event_;
    friend class isoft::dm::CancellationHandlerFactory;
};
}  // namespace diag
}  // namespace ara

#endif  // ARA_DIAG_CANCELLATION_HANDLER_H_