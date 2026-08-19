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
/// @file       cancellation_event.h
/// @brief      This file provides definitions for CancellationEvent and related types.
/// @details
/// @date       2022-08-02
/// @author     gaohuiming
/// @version    1.2.0
///
/// ================================================================

#ifndef ISOFT_DMD_CANCELLATION_EVENT_H__
#define ISOFT_DMD_CANCELLATION_EVENT_H__

#include <ara/core/instance_specifier.h>
#include <ara/core/result.h>
#include <ara/core/vector.h>

#include <mutex>

namespace isoft {
namespace dm {
class CancellationEventManager;

/// @brief CancellationEvent contains a shared state if the processing should be
/// canceled
/// @vpublic
/// @traceid{SWS_DM_01340}@tracestatus{draft}
///
class CancellationEvent final
{
public:
    /// @brief Default constructor of CancellationEvent
    /// @traceid{SWS_DM_01530}@tracestatus{draft}
    CancellationEvent() noexcept : id_{} {}

    /// @brief Default constructor of CancellationEvent
    /// @traceid{SWS_DM_01530}@tracestatus{draft}
    explicit CancellationEvent(ara::core::StringView const &id,
                               CancellationEventManager &cancellationEventManager) noexcept;
    /// @brief Default destructor
    /// @vprivate Vector component internal API
    /// @pre none
    /// @reentrant FALSE
    /// @traceid{SWS_DM_01531}@tracestatus{draft}
    ~CancellationEvent() noexcept = default;

    /// @brief Move constructs instance of class
    /// @param[out] other The other object
    /// @pre none
    /// @reentrant FALSE
    /// @traceid{SWS_DM_01532}@tracestatus{draft}
    CancellationEvent(CancellationEvent &&other) noexcept = delete;

    /// @brief Move assigns instance of class
    /// @param[out] other The other object
    /// @return Reference to self
    /// @pre none
    /// @reentrant FALSE
    /// @traceid{SWS_DM_01533}@tracestatus{draft}
    CancellationEvent &operator=(CancellationEvent &&other) &noexcept = delete;

    /// @brief Copy constructor of CancellationEvent cannot be used
    /// @pre none
    /// @reentrant FALSE
    /// @traceid{SWS_DM_01534}@tracestatus{draft}
    CancellationEvent(CancellationEvent const &) = delete;

    /// @brief Copy assignment operator of CancellationEvent cannot be used
    /// @return Reference to handler after assignment
    /// @pre none
    /// @reentrant FALSE
    /// @traceid{SWS_DM_01535}@tracestatus{draft}
    CancellationEvent &operator=(CancellationEvent const &) = delete;

    /// @brief Set event callback
    /// @param[in] notifier Callback
    /// @throws on overflow
    inline void SetNotifier(std::function< void(void) > &&notifier)
    {
        std::unique_lock< std::recursive_mutex > const lock{lock_};
        std::function< void(void) > const func{std::move(notifier)};
        notifierTable_.push_back(func);
    }

    inline void Notifier() noexcept
    {
        std::unique_lock< std::recursive_mutex > const lock{lock_};
        trigger_ = true;
        for (const auto &itr : notifierTable_) {
            itr();
        }
    }

    inline bool IsCancel() noexcept
    {
        std::unique_lock< std::recursive_mutex > const lock{lock_};
        return trigger_;
    }

    inline ara::core::StringView Id() const noexcept { return id_; }

    void Erase() noexcept;

private:
    /// @brief id_
    ara::core::StringView id_;

    /// @name lock_
    std::recursive_mutex lock_{};

    ara::core::Vector< std::function< void(void) > >
        /// @name notifier_
        notifierTable_{};

    bool trigger_{false};

    CancellationEventManager *cancellationEventManager_{};
};

}  // namespace dm
}  // namespace isoft

#endif  // ISOFT_DMD_CANCELLATION_EVENT_H__