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
/// @file       release_handler.h
/// @brief      This file provides the definitions of ReleaseHandler and related types.
/// @details
/// @date       2023-08-23
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_DIAG_RELEASE_HANDLER_H__
#define ARA_DIAG_RELEASE_HANDLER_H__

#include <ara/core/instance_specifier.h>
#include <ara/core/result.h>
#include <ara/core/span.h>
#include <ara/core/utility.h>

namespace isoft {
namespace dm {
namespace dis {
/// @brief ReleaseHandlerImpl
class ReleaseHandlerImpl;
class ReleaseHandlerFactory;
}  // namespace dis
}  // namespace dm
}  // namespace isoft

namespace ara {
namespace diag {

/// @brief ReleaseHandler contains a shared state if the processing should be canceled
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
/// @vpublic
/// @traceid{SWS_DM_01340}@tracestatus{draft}
///
class ReleaseHandler final
{
public:
    /// @brief Default constructor of ReleaseHandler cannot be used
    /// @traceid{SWS_DM_01530}@tracestatus{draft}
    ReleaseHandler() = delete;

    /// @brief Default destructor
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @vprivate Vector component internal API
    /// @pre -
    /// @reentrant FALSE
    /// @traceid{SWS_DM_01531}@tracestatus{draft}
    ~ReleaseHandler() noexcept = default;

    /// @brief Move constructs instance of class
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[out] other The other object
    /// @pre -
    /// @reentrant FALSE
    /// @traceid{SWS_DM_01532}@tracestatus{draft}
    ReleaseHandler(ReleaseHandler&& other) noexcept : impl_{other.impl_} {}

    /// @brief Move assigns instance of class
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[out] other The other object
    /// @return Reference to self
    /// @pre -
    /// @reentrant FALSE
    /// @traceid{SWS_DM_01533}@tracestatus{draft}
    ReleaseHandler& operator=(ReleaseHandler&& other) & noexcept
    {
        if (this != &other) {
            impl_ = other.impl_;
        }
        return *this;
    }

    /// @brief Copy constructor of ReleaseHandler cannot be used
    /// @pre -
    /// @reentrant FALSE
    /// @traceid{SWS_DM_01534}@tracestatus{draft}
    ReleaseHandler(ReleaseHandler const&) = delete;

    /// @brief Copy assignment operator of ReleaseHandler cannot be used
    /// @return Reference to handler after assignment
    /// @pre -
    /// @reentrant FALSE
    /// @traceid{SWS_DM_01535}@tracestatus{draft}
    ReleaseHandler& operator=(ReleaseHandler const&) = delete;

    /// @brief Reports whether the shared resource is no longer in use
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @return True in if the shared resource is no longer in use, False otherwise
    /// @pre -
    /// @reentrant FALSE
    /// @traceid{SWS_DM_01536}@tracestatus{draft}
    bool MayRelease() const noexcept;

    /// @brief Registering a notifier function which is called if the shared resource can be freed. A consecutive call
    /// of this method will overwrite the previous registered notifier.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] notifier  Notification function that is called upon releasing the shared resource
    /// @pre The passed in function do not throw any exceptions
    /// @pre The passed notifier must contain a callable object
    /// @reentrant FALSE
    /// @traceid{SWS_DM_01537}@tracestatus{draft}
    void SetNotifier(std::function< void(void) > notifier) noexcept;

private:
    /// @brief Default constructor of ReleaseHandler
    /// @traceid{SWS_DM_01530}@tracestatus{draft}
    explicit ReleaseHandler(std::shared_ptr< isoft::dm::dis::ReleaseHandlerImpl > impl) noexcept;
    std::shared_ptr< isoft::dm::dis::ReleaseHandlerImpl > impl_;
    friend isoft::dm::dis::ReleaseHandlerFactory;
};

}  // namespace diag
}  // namespace ara

#endif  // ARA_DIAG_RELEASE_HANDLER_H__