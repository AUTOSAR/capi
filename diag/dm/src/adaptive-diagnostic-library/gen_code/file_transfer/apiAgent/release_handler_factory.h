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
/// @file       release_handler_factory.h
/// @brief      This file provides the definitions of ReleaseHandlerFactory and related types.
/// @details
/// @date       2023-08-23
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================

#ifndef __APIAGENT_RELEASEHANDLERFACTORY_H__
#define __APIAGENT_RELEASEHANDLERFACTORY_H__

#include <ara/core/instance_specifier.h>
#include <ara/core/result.h>

#include "ara/diag/release_handler.h"
#include "release_handler_impl.h"
namespace isoft {
namespace dm {
namespace dis {

/// @brief ReleaseHandlerFactory contains a shared state if the processing should be canceled
/// @vpublic
/// @traceid{SWS_DM_01340}@tracestatus{draft}
///
class ReleaseHandlerFactory final
{
public:
    /// @brief create ReleaseHandler object
    /// @param data shared data
    /// @return std::shared_ptr<ReleaseHandler>
    static ara::diag::ReleaseHandler Create(std::shared_ptr< ReleaseHandlerImpl >& impl)
    {
        return {ara::diag::ReleaseHandler{impl}};
    }
    /// @brief Default constructor of ReleaseHandlerFactory
    /// @traceid{SWS_DM_01530}@tracestatus{draft}
    ReleaseHandlerFactory() noexcept = default;
    /// @brief Default destructor
    /// @vprivate Vector component internal API
    /// @pre -
    /// @reentrant FALSE
    /// @traceid{SWS_DM_01531}@tracestatus{draft}
    ~ReleaseHandlerFactory() noexcept = default;

    /// @brief Move constructs instance of class
    /// @param[out] other The other object
    /// @pre -
    /// @reentrant FALSE
    /// @traceid{SWS_DM_01532}@tracestatus{draft}
    ReleaseHandlerFactory(ReleaseHandlerFactory&& other) noexcept = delete;

    /// @brief Move assigns instance of class
    /// @param[out] other The other object
    /// @return Reference to self
    /// @pre -
    /// @reentrant FALSE
    /// @traceid{SWS_DM_01533}@tracestatus{draft}
    ReleaseHandlerFactory& operator=(ReleaseHandlerFactory&& other) & noexcept = delete;

    /// @brief Copy constructor of ReleaseHandlerFactory cannot be used
    /// @pre -
    /// @reentrant FALSE
    /// @traceid{SWS_DM_01534}@tracestatus{draft}
    ReleaseHandlerFactory(ReleaseHandlerFactory const&) = delete;

    /// @brief Copy assignment operator of ReleaseHandlerFactory cannot be used
    /// @return Reference to handler after assignment
    /// @pre -
    /// @reentrant FALSE
    /// @traceid{SWS_DM_01535}@tracestatus{draft}
    ReleaseHandlerFactory& operator=(ReleaseHandlerFactory const&) = delete;
};
}  // namespace dis
}  // namespace dm
}  // namespace isoft

#endif  // __APIAGENT_RELEASEHANDLERFACTORY_H__