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
/// @file       state.h
/// @brief
/// @details
/// @date       2022-06-24
/// @author     hanzhibo
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_CORE_INTERNAL_STATE_H_
#define ARA_CORE_INTERNAL_STATE_H_

#include <memory>
#include <mutex>

namespace ara {
namespace core {
namespace internal {
class State;
//********************************/
class IAgentCallback
{
public:
    IAgentCallback() = default;
    /// @brief Copy constructor
    /// @param[in] object IAgentCallback&
    IAgentCallback(IAgentCallback const& object) = default;
    /// @brief Copy the assignment function
    /// @param[in] object IAgentCallback&
    /// @return IAgentCallback&
    IAgentCallback& operator=(IAgentCallback const& object) = default;
    /// @briefMove constructor
    /// @param[in] object IAgentCallback&& object
    IAgentCallback(IAgentCallback&& object) = default;
    /// @brief Move operation
    /// @param[in] object IAgentCallback&& object
    /// @return IAgentCallback&
    IAgentCallback& operator=(IAgentCallback&& object) = default;

public:
    typedef std::shared_ptr< IAgentCallback > Ptr;
    virtual ~IAgentCallback() = default;
    /// @brief Executes the stored callback.
    /// @note The callback is never exposed outside this object so that no one may have a dangling pointer to the stored
    /// callback.
    virtual void FireCallback() = 0;
    /// @brief Returns if a callback was set previously.
    /// @return true if a callback is set, false otherwise.
    virtual bool HasCallback() = 0;
    /// @brief Return State object associated with AgentCallback
    /// @return
    virtual typename std::shared_ptr< State > GetPromiseStatePtr() = 0;
};
//********************************/
/// @brief Class State maintains the action (Set, Get,Fire or Execute) on callback
/// @private
class State final
{
public:
    typedef std::shared_ptr< State > Ptr;

protected:
    std::mutex mutex_{};                           // NOLINT
    IAgentCallback::Ptr pAgentCallback_{nullptr};  // NOLINT
    State::Ptr pStateNext_{nullptr};               // NOLINT
    /// @brief Flag indicating whether callback has cleanup callback called by Future destructor
    bool bCbClearFlag_{false};  // NOLINT

protected:
    int32_t nBusyCount_{0};  // NOLINT
    // int32_t nID_{0};
    // static int32_t nID_index;

public:
    /// @brief
    State()
    {
        _addBusyCount();
        // nID_ = nID_index++;
        // printf("State = %d\n", nID_);
    }
    /// @brief
    State(State const& other) : pAgentCallback_{other.pAgentCallback_}
    {
        _addBusyCount();
        // nID_ = nID_index++;
        // printf("State = %d\n", nID_);
    }
    /// @brief
    State& operator=(State const& other)
    {
        if (&other == this) {
            return *this;
        }
        pAgentCallback_ = other.pAgentCallback_;
        pStateNext_     = other.pStateNext_;
        nBusyCount_     = other.nBusyCount_;
        return *this;
    }
    /// @brief
    State(State&& other) = delete;
    /// @brief
    State& operator=(State&&) = delete;

    /// @brief
    ~State() = default;
    /// @brief
    /// @param pCallback
    void AttachCallback(IAgentCallback::Ptr const& pCallback)
    {
        pAgentCallback_ = pCallback;
        if (pCallback) {
            BuildStateLink(pCallback->GetPromiseStatePtr());
        }
    }
    /// @brief create callback chain
    /// @param pNextState
    void BuildStateLink(State::Ptr const& pNextState)
    {
        // Set the last callback saved by previous then as the parent callback of current callback
        if (pAgentCallback_) {
            SetStateNext(pNextState);
        }
    }
    /// @brief Executes the stored callback.
    /// @note The callback is never exposed outside this object so that no one may have a dangling pointer to the stored
    /// callback.
    void FireCallback()
    {
        std::unique_lock< std::mutex > lock(mutex_);
        if (_HasCallback()) {
            // printf("State::FireCallback = %d\n", nID_);
            pAgentCallback_->FireCallback();
        }
    }
    /// @brief Executes the stored callback without mutex lock.
    /// @note As the name suggests, this is a version which is not doing the mutex lock when running callback.
    void FireCallback_NoLock()
    {
        if (_HasCallback()) {
            // printf("State::FireCallback_NoLock = %d\n", nID_);
            pAgentCallback_->FireCallback();
        }
    }
    /// @brief Returns if a callback was set previously.
    /// @return true if a callback is set, false otherwise.
    bool HasCallback()
    {
        std::unique_lock< std::mutex > lock(mutex_);
        return _HasCallback();
    }
    /// @brief reset data
    /// @return
    void SetCallbackClearFlag()
    {
        _subBusyCount();
        if (_isNeedClear()) {
            if (false == IsHaveAliveState()) {
                bCbClearFlag_ = true;
                // printf("State::AttachCallback(nullptr) = %d\n", nID_);
            }
        }
    }
    void SetStateNext(State::Ptr const& pStateNext) { pStateNext_ = pStateNext; }
    bool IsHaveAliveState() const
    {
        State::Ptr pNext = pStateNext_;
        while (pNext) {
            if (false == pNext->_isNeedClear()) {
                return true;
            }
            pNext = pNext->pStateNext_;
        }
        return false;
    }

private:
    /// @brief Check if resource count is 0
    inline bool _isNeedClear() const { return nBusyCount_ <= 0; }
    /// @brief Decrement resource count
    inline void _addBusyCount() { nBusyCount_ += 1; }
    /// @brief Decrement resource count
    inline void _subBusyCount() { nBusyCount_ -= 1; }

private:
    bool _HasCallback() const
    {
        if (!pAgentCallback_) {
            return false;
        }
        if (false == pAgentCallback_->HasCallback()) {
            return false;
        }
        return !bCbClearFlag_;
    }
};
//********************************/
} /* namespace internal */
}  // namespace core
}  // namespace ara

#endif /* ARA_CORE_INTERNAL_STATE_H */
