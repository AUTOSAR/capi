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
/// @file       cancellation_handler_factory.h
/// @brief      This file provides definitions for CancellationHandlerFactory and related types.
/// @details
/// @date       2023-09-18
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_DIAG_API_CANCELLATION_HANDLER_FACTORY_H__
#define ARA_DIAG_API_CANCELLATION_HANDLER_FACTORY_H__

#include <ara/core/instance_specifier.h>
#include <ara/core/result.h>

#include "ara/diag/cancellation_handler.h"
#include "cancellation/cancellation_event_manager.h"
namespace isoft {
namespace dm {
/// @brief CancellationHandlerFactory contains a shared state if the processing
/// should be canceled
/// @vpublic
/// @traceid{SWS_DM_01340}@tracestatus{draft}
///
class CancellationHandlerFactory final
{
public:
    /// @brief create CancellationHandler object
    /// @param data shared data
    /// @return std::shared_ptr<CancellationHandler>
    static ara::diag::CancellationHandler Create(isoft::dm::CancellationEventManager &eventManager,
                                                 ara::diag::MetaInfo const &mi) noexcept
    {
        std::shared_ptr< isoft::dm::CancellationEvent > const eventPtr{eventManager.Get(mi)};
        return ara::diag::CancellationHandler(eventPtr);
    }

    /// @brief Default constructor of CancellationHandlerFactory
    /// @traceid{SWS_DM_01530}@tracestatus{draft}
    CancellationHandlerFactory() noexcept = default;
    /// @brief Default destructor
    /// @vprivate Vector component internal API
    /// @pre none
    /// @reentrant FALSE
    /// @traceid{SWS_DM_01531}@tracestatus{draft}
    ~CancellationHandlerFactory() noexcept = default;

    /// @brief Move constructs instance of class
    /// @param[out] other The other object
    /// @pre none
    /// @reentrant FALSE
    /// @traceid{SWS_DM_01532}@tracestatus{draft}
    CancellationHandlerFactory(CancellationHandlerFactory &&other) noexcept = delete;

    /// @brief Move assigns instance of class
    /// @param[out] other The other object
    /// @return Reference to self
    /// @pre none
    /// @reentrant FALSE
    /// @traceid{SWS_DM_01533}@tracestatus{draft}
    CancellationHandlerFactory &operator=(CancellationHandlerFactory &&other) &noexcept = delete;

    /// @brief Copy constructor of CancellationHandlerFactory cannot be used
    /// @pre none
    /// @reentrant FALSE
    /// @traceid{SWS_DM_01534}@tracestatus{draft}
    CancellationHandlerFactory(CancellationHandlerFactory const &) = delete;

    /// @brief Copy assignment operator of CancellationHandlerFactory cannot be
    /// used
    /// @return Reference to handler after assignment
    /// @pre none
    /// @reentrant FALSE
    /// @traceid{SWS_DM_01535}@tracestatus{draft}
    CancellationHandlerFactory &operator=(CancellationHandlerFactory const &) = delete;
};
}  // namespace dm
}  // namespace isoft

#endif  // ARA_DIAG_API_CANCELLATION_HANDLER_FACTORY_H__