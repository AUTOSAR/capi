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
/// @file       callback_agent_arg.h
/// @brief
/// @details
/// @date       2022-06-24
/// @author     hanzhibo
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_CORE_INTERNAL_CALLBACK_AGENT_ARG_H_
#define ARA_CORE_INTERNAL_CALLBACK_AGENT_ARG_H_

#include <ara/core/internal/callback_agent.h>
#include <ara/core/promise.h>
#include <ara/core/result.h>

#include <cstdarg>
#include <memory>
#include <mutex>

namespace ara {
namespace core {
template < typename, typename >
class Future;
template < typename, typename >
class Promise;
namespace internal {
//********************************/
/// @brief Class PAgentReturn_Base_Arg maintains the action (Set, Get,Fire or Execute) on callback
// template <typename T_Func, typename ExecutorT>
template < typename T_Func, typename T_Args >
class PAgentReturn_Base_Arg : public IAgentCallback
{
public:
    /// @brief Copy constructor
    /// @param[in] object PAgentReturn_Base_Arg&
    PAgentReturn_Base_Arg(PAgentReturn_Base_Arg const &object) = default;
    /// @brief Copy the assignment function
    /// @param[in] object PAgentReturn_Base_Arg&
    /// @return PAgentReturn_Base_Arg&
    PAgentReturn_Base_Arg &operator=(PAgentReturn_Base_Arg const &object) = default;
    /// @briefMove constructor
    /// @param[in] object PAgentReturn_Base_Arg&& object
    PAgentReturn_Base_Arg(PAgentReturn_Base_Arg &&object) noexcept = default;
    /// @brief Move operation
    /// @param[in] object PAgentReturn_Base_Arg&& object
    /// @return PAgentReturn_Base_Arg&
    PAgentReturn_Base_Arg &operator=(PAgentReturn_Base_Arg &&object) noexcept = default;

public:
    /// @brief Return value type of callback function
    using T_FunReturn = typename std::result_of_t< T_Func(T_Args) >;

protected:
    std::function< T_FunReturn(T_Args) > pCallback_{nullptr};  // NOLINT
    T_Args &&args_;                                            // NOLINT

public:
    /// @brief Sets the callback. any callback that was set before is removed and replaced by the new callback.
    /// @param callback The callback to be set.
    /// @param args The param to be set.
    PAgentReturn_Base_Arg(T_Func &&callback, T_Args &&args)
        : pCallback_{std::forward< T_Func >(callback)}, args_{std::forward< T_Args >(args)}
    {
    }
    /// @brief
    ~PAgentReturn_Base_Arg() override = default;
    /// @brief Executes the stored callback.
    /// @note The callback is never exposed outside this object so that no one may have a dangling pointer to the stored
    /// callback.
    void FireCallback() override
    {
        if (false == _HasCallback()) {
            return;
        }
        _DoCallback();
    }
    /// @brief Returns if a callback was set previously.
    /// @return true if a callback is set, false otherwise.
    bool HasCallback() override { return _HasCallback(); }

protected:
    /// @brief Returns if a callback was set previously.
    /// @return true if a callback is set, false otherwise.
    bool _HasCallback() const { return static_cast< bool >(pCallback_); }
    /// @brief
    void _DoCallback()
    {
        pCallback_(std::forward< T_Args >(args_));
        return;
    }
};
//*********************/ //Template type: <T_Func, T_Args, E, T_FuncReturn>
template < typename T_Func, typename T_Args, typename E, typename TF >
class PAgentCallback_Arg : public PAgentReturn_Base_Arg< T_Func, T_Args >
{
private:
    /// @brief base class
    using T_Base = PAgentReturn_Base_Arg< T_Func, T_Args >;
    /// @brief Promise type
    using AgentPromise = T_Promise< typename T_Base::T_FunReturn, E >;

public:
    /// @brief
    using Ptr = std::shared_ptr< PAgentCallback_Arg< T_Func, T_Args, E, TF > >;

private:
    typename AgentPromise::type promise_;

public:
    PAgentCallback_Arg(T_Func &&callback, T_Args &&args)
        : T_Base(std::forward< T_Func >(callback), std::forward< T_Args >(args))
    {
    }
    /// @brief Executes the stored callback.
    void FireCallback() override
    {
        if (false == T_Base::_HasCallback()) {
            return;
        }
        typename T_Base::T_FunReturn resultCallback{T_Base::pCallback_(std::forward< T_Args >(T_Base::args_))};
        AgentPromise::SetResult(promise_, std::move(resultCallback));
        G_Printf("PAgentCallback_Arg = Default\n");
    }
    /// @brief Return State object associated with AgentCallback
    /// @return
    virtual typename State::Ptr GetPromiseStatePtr() { return promise_.GetStatePtr(); }
    /// @brief Return the Future from the stored Promise.
    /// @return
    auto GetFuture() { return promise_.get_future(); }
};
//*************/ //Template type: <T_Func, T_Args, E, Void>
template < typename T_Func, typename T_Args, typename E >
class PAgentCallback_Arg< T_Func,
                          T_Args,
                          E,
                          std::enable_if_t< std::is_void< std::result_of_t< T_Func(T_Args) > >::value > >
    : public PAgentReturn_Base_Arg< T_Func, T_Args >
{
private:
    /// @brief base class
    using T_Base = PAgentReturn_Base_Arg< T_Func, T_Args >;
    /// @brief
    using TF = std::enable_if_t< std::is_void< typename T_Base::T_FunReturn >::value >;

public:
    /// @brief
    using Ptr = std::shared_ptr< PAgentCallback_Arg< T_Func, T_Args, E, TF > >;

private:
    ara::core::Promise< void, E > promise_;

public:
    explicit PAgentCallback_Arg(T_Func &&callback, T_Args &&args)
        : T_Base(std::forward< T_Func >(callback), std::forward< T_Args >(args))
    {
    }
    /// @brief Executes the stored callback.
    void FireCallback() override
    {
        if (false == T_Base::_HasCallback()) {
            return;
        }
        T_Base::pCallback_(std::forward< T_Args >(T_Base::args_));
        promise_.SetResult(ara::core::Result< void >::FromValue());
        G_Printf("PAgentCallback_Arg = Void\n");
    }
    /// @brief Return State object associated with AgentCallback
    /// @return
    virtual typename State::Ptr GetPromiseStatePtr() { return promise_.GetStatePtr(); }
    /// @brief Return the Future from the stored Promise.
    /// @return
    auto GetFuture() { return promise_.get_future(); }
};
//********************************/
}  // namespace internal
}  // namespace core
}  // namespace ara

#endif /* ARA_CORE_INTERNAL_STATE_THEN_ARG_H_ */