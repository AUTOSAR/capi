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
/// @file       adaptive_application.h
/// @brief      Adaptive application
/// @details
/// @date       2024-08-15
/// @author     cuiyinli
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @export_level=/UCM/PackageManager
/// @module_path=/UCM/PackageManager
/// @interface_level=unit
/// @trace_id_sr=SR_UCM_00001
/// @unit_name=AdaptiveApplication
/// @unit_description=Adaptive application
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_PKGMGR_APP_ADAPTIVE_APPLICATION_H_
#define ARA_UCM_PKGMGR_APP_ADAPTIVE_APPLICATION_H_

#include <isoft/naicpp/global_evloop.h>
#include <isoft/naicpp/terminating_handler.h>

#include "util/noncopy.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief This class represents the Adaptive Application
///
/// see AUTOSAR_TR_Glossary.pdf
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10001
/// @trace_id_dd=DD_UCM_10001
/// @needwork = ad
/// @endcode
class AdaptiveApplication : NonCopyNonMove
{
public:
    /// @brief constructor
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10002
    /// @needwork = dda
    /// @endcode
    AdaptiveApplication() = default;
    /// @brief destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10003
    /// @needwork = dda
    /// @endcode
    virtual ~AdaptiveApplication() = default;

    /// @brief delete copy construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10004
    /// @needwork = dda
    /// @endcode
    AdaptiveApplication(AdaptiveApplication const& other) = delete;
    /// @brief delete copy asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10005
    /// @needwork = dda
    /// @endcode
    AdaptiveApplication& operator=(AdaptiveApplication const& other) = delete;
    /// @brief delete move construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10006
    /// @needwork = dda
    /// @endcode
    AdaptiveApplication(AdaptiveApplication&& other) = delete;
    /// @brief delete move asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10007
    /// @needwork = dda
    /// @endcode
    AdaptiveApplication& operator=(AdaptiveApplication&& other) = delete;

    /// @brief A method represents the lifecycle of adaptive application.
    /// Shall be called from main() function (return app.Execute())
    ///
    /// @return Execution status
    ///(could be used as a return value from main() function)
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10008
    /// @needwork = dda
    /// @endcode
    std::int32_t Execute() noexcept;  // PRQA S 2427

protected:
    /// @brief A method to be executed at initialize step.
    /// Default implementation simply returns true and does nothing
    /// Could be reimplemented for the specific application (if needed)
    ///
    /// @return Initialize success status (Was it successful or not)
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10009
    /// @needwork = dda
    /// @endcode
    virtual bool OnInitialize() noexcept;

    /// @brief A method to be executed at runtime.
    /// Shall be reimplemented for the specific application
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10010
    /// @needwork = dda
    /// @endcode
    virtual void Run() noexcept = 0;

    /// @brief A method to be executed at terminate step.
    /// Default implementation simply does nothing
    /// Could be reimplemented for the specific application (if needed)
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10011
    /// @needwork = dda
    /// @endcode
    virtual void OnTerminate() noexcept;

    /// @brief Waiting until SIG_TERM will be received.
    /// Could be called from Run() method for specific application
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10012
    /// @needwork = dda
    /// @endcode
    void _WaitUntilTermination() noexcept;

protected:
    /// @brief run a function with evloop
    /// @return Execution status
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10013
    /// @needwork = dda
    /// @endcode
    std::int32_t _Add2EvLoop2Exec(std::function< void() >&& fun) noexcept;  // PRQA S 2028,2427

protected:
    /// @brief main evloop
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10014
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< isoft::naicpp::EvLoop > mainLoop_;  // NOLINT

private:
    /// @brief Performs at initialize step actions.
    /// Calls OnInitialize() which could be reimplemented bu specific application
    ///
    /// @return Initialize success status (Was it successful or not)
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10015
    /// @needwork = dda
    /// @endcode
    bool _initialize() noexcept;

    /// @brief Performs actions that are required during app termination
    /// Calls OnTerminate() which could be reimplemented by specific app
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10016
    /// @needwork = dda
    /// @endcode
    void _terminate() noexcept;

private:
    /// @brief handler for termination with SIG_TERM
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10017
    /// @needwork = dda
    /// @endcode
    isoft::naicpp::TerminatingHandler terminatingHandler_;
};

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // ARA_UCM_PKGMGR_APP_ADAPTIVE_APPLICATION_H_
