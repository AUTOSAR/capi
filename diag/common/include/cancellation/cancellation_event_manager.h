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
/// @file       cancellation_event_manager.h
/// @brief      This file provides definitions for CancellationEventManagerManager and related types.
/// @details
/// @date       2023-09-18
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================

#ifndef ISOFT_DM_CANCELLATION_EVENT_MANAGER_H__
#define ISOFT_DM_CANCELLATION_EVENT_MANAGER_H__
#include <ara/core/map.h>
#include <ara/diag/meta_info.h>

#include <memory>
#include <mutex>

#include "cancellation_event.h"
namespace isoft {
namespace dm {

/// @brief CancellationEventManager contains a shared state if the processing
/// should be canceled
/// @vpublic
/// @traceid{SWS_DM_01340}@tracestatus{draft}
///
class CancellationEventManager final
{
public:
    /// @brief Default constructor of CancellationEventManager
    /// @traceid{SWS_DM_01530}@tracestatus{draft}
    CancellationEventManager() = default;
    /// @brief Default destructor
    /// @vprivate Vector component internal API
    /// @pre none
    /// @reentrant FALSE
    /// @traceid{SWS_DM_01531}@tracestatus{draft}
    ~CancellationEventManager() noexcept = default;

    /// @brief Move constructs instance of class
    /// @param[out] other The other object
    /// @pre none
    /// @reentrant FALSE
    /// @traceid{SWS_DM_01532}@tracestatus{draft}
    CancellationEventManager(CancellationEventManager &&other) noexcept = default;

    /// @brief Move assigns instance of class
    /// @param[out] other The other object
    /// @return Reference to self
    /// @pre none
    /// @reentrant FALSE
    /// @traceid{SWS_DM_01533}@tracestatus{draft}
    CancellationEventManager &operator=(CancellationEventManager &&other) &noexcept = default;

    /// @brief Copy constructor of CancellationEventManager cannot be used
    /// @pre none
    /// @reentrant FALSE
    /// @traceid{SWS_DM_01534}@tracestatus{draft}
    CancellationEventManager(CancellationEventManager const &) = delete;

    /// @brief Copy assignment operator of CancellationEventManager cannot be used
    /// @return Reference to handler after assignment
    /// @pre none
    /// @reentrant FALSE
    /// @traceid{SWS_DM_01535}@tracestatus{draft}
    CancellationEventManager &operator=(CancellationEventManager const &) = delete;

    /// @brief Get CancellationEvent object
    /// @param metaInfo Conversation information
    /// @return CancellationEvent object
    std::shared_ptr< CancellationEvent > Get(ara::diag::MetaInfo const &metaInfo) noexcept
    {
        ara::core::Optional< ara::core::StringView > sa{metaInfo.GetValue(ara::core::StringView("kSA"))};
        if (!sa.has_value()) {
            return {nullptr};
        }
        std::unique_lock< std::mutex > const lock{*resultLockPtr_};
        ara::core::Map< ara::core::StringView, std::shared_ptr< CancellationEvent > >::iterator const it{
            cancellationEventMap_.find(*sa)};
        if (it != cancellationEventMap_.end()) {
            return it->second;
        }
        std::shared_ptr< CancellationEvent > cancellationEventPtr{std::make_shared< CancellationEvent >(*sa, *this)};

        cancellationEventMap_[*sa] = cancellationEventPtr;
        return cancellationEventPtr;
    }

    /// @brief Throw and return the object, call this function when Event is triggered.
    /// @param metaInfo Conversation information
    /// @return CancellationEvent object
    std::shared_ptr< CancellationEvent > Erase(ara::diag::MetaInfo const &metaInfo) noexcept
    {
        ara::core::Optional< ara::core::StringView > sa{metaInfo.GetValue(ara::core::StringView("kSA"))};
        if (!sa.has_value()) {
            return {nullptr};
        }
        return Erase(*sa);
    }

    /// @brief Throw and return the object, call this function when Event is triggered.
    /// @param metaInfo Conversation information
    /// @return CancellationEvent object
    std::shared_ptr< CancellationEvent > Erase(std::shared_ptr< CancellationEvent > &eventPtr) noexcept
    {
        ara::core::StringView const id{eventPtr->Id()};
        return Erase(id);
    }

    /// @brief Throw and return the object, call this function when Event is triggered.
    /// @param metaInfo Conversation information
    /// @return CancellationEvent object
    std::shared_ptr< CancellationEvent > Erase(ara::core::StringView const &id) noexcept
    {
        ara::core::Map< ara::core::StringView, std::shared_ptr< CancellationEvent > >::iterator const it{
            cancellationEventMap_.find(id)};
        if (it != cancellationEventMap_.end()) {
            std::unique_lock< std::mutex > const lock{*resultLockPtr_};
            std::shared_ptr< CancellationEvent > cancellationEventPtr{it->second};
            std::ignore = cancellationEventMap_.erase(it);
            return cancellationEventPtr;
        }
        return {nullptr};
    }

private:
    std::shared_ptr< std::mutex > resultLockPtr_{std::make_shared< std::mutex >()};
    ara::core::Map< ara::core::StringView, std::shared_ptr< CancellationEvent > > cancellationEventMap_;
};
}  // namespace dm
}  // namespace isoft

#endif  // ISOFT_DM_CANCELLATION_EVENT_MANAGER_H__