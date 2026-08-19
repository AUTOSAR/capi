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
/// @file       callback_agent.h
/// @brief
/// @details
/// @date       2022-06-24
/// @author     hanzhibo
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_CORE_INTERNAL_CALLBACK_AGENT_H_
#define ARA_CORE_INTERNAL_CALLBACK_AGENT_H_

#include <ara/core/internal/state.h>
#include <ara/core/promise.h>
#include <ara/core/result.h>

#include <cstdarg>
#include <memory>

namespace ara {
namespace core {
template < typename, typename >
class Future;
template < typename, typename >
class Promise;
namespace internal {
// inline void G_Printf(const char* pchData) { printf("%s", pchData); }
inline void G_Printf(const char* pchData) { std::ignore = pchData; }
//********************************/
/// @brief Class PAgentReturn_Base maintains the action (Set, Get,Fire or Execute) on callback
// template <typename T_Func, typename ExecutorT>
template < typename T_Func >
class PAgentReturn_Base : public IAgentCallback
{
public:
    /// @brief Copy constructor
    /// @param[in] object PAgentReturn_Base&
    PAgentReturn_Base(PAgentReturn_Base const& object) = default;
    /// @brief Copy the assignment function
    /// @param[in] object PAgentReturn_Base&
    /// @return PAgentReturn_Base&
    PAgentReturn_Base& operator=(PAgentReturn_Base const& object) = default;
    /// @briefMove constructor
    /// @param[in] object PAgentReturn_Base&& object
    PAgentReturn_Base(PAgentReturn_Base&& object) noexcept = default;
    /// @brief Move operation
    /// @param[in] object PAgentReturn_Base&& object
    /// @return PAgentReturn_Base&
    PAgentReturn_Base& operator=(PAgentReturn_Base&& object) noexcept = default;

public:
    /// @brief
    using T_FunReturn = typename std::result_of_t< T_Func() >;

protected:
    std::function< T_FunReturn() > pCallback_{nullptr};  // NOLINT

public:
    /// @brief
    ~PAgentReturn_Base() override = default;

public:
    explicit PAgentReturn_Base(T_Func&& callback) : pCallback_{std::forward< T_Func >(callback)} {}
    void FireCallback() override
    {
        if (false == _HasCallback()) {
            return;
        }
        pCallback_();
    }
    /// @brief Returns if a callback was set previously.
    /// @return true if a callback is set, false otherwise.
    bool HasCallback() override { return _HasCallback(); }

protected:
    /// @brief Returns if a callback was set previously.
    /// @return true if a callback is set, false otherwise.
    bool _HasCallback() const { return static_cast< bool >(pCallback_); }
};
//********************************/
template < typename T, typename E >  // T_Data
class T_Promise final
{
public:
    using type = ara::core::Promise< T, E >;  // NOLINT
    static void SetResult(type& promist,
                          T&& result,
                          typename std::enable_if_t< !(std::is_void< T >::value) >* p = nullptr)
    {
        std::ignore = p;
        promist.SetResult(ara::core::Result< T, E >::FromValue(std::move(result)));
        G_Printf("T_Promise = Default\n");
    }
};
template < typename T, typename E >  // Result
class T_Promise< ara::core::Result< T, E >, E > final
{
public:
    using type = ara::core::Promise< T, E >;  // NOLINT
    static void SetResult(type& promist, ara::core::Result< T, E >&& result)
    {
        promist.SetResult(std::move(result));
        G_Printf("T_Promise = Result\n");
    }
};
template < typename T, typename E >  // Future
class T_Promise< ara::core::Future< T, E >, E > final
{
public:
    using type = ara::core::Promise< T, E >;  // NOLINT
    static void SetResult(type& promist, ara::core::Future< T, E >&& result)
    {
        promist.SetResult(std::move(std::move(result).GetResult()));
        G_Printf("T_Promise = Future\n");
    }
};
//**************************/ //Template type: <T_Func, E, T_FuncReturn>
template < typename T_Func, typename E, typename TF >
class PAgentCallback : public PAgentReturn_Base< T_Func >
{
private:
    /// @brief base class
    using T_Base = PAgentReturn_Base< T_Func >;
    /// @brief Promise type
    using AgentPromise = T_Promise< typename T_Base::T_FunReturn, E >;

public:
    /// @brief
    using Ptr = std::shared_ptr< PAgentCallback< T_Func, E, TF > >;

private:
    typename AgentPromise::type promise_;

public:
    explicit PAgentCallback(T_Func&& callback) : T_Base(std::forward< T_Func >(callback)) {}
    /// @brief Executes the stored callback.
    void FireCallback() override
    {
        if (false == T_Base::_HasCallback()) {
            return;
        }
        typename T_Base::T_FunReturn resultCallback{T_Base::pCallback_()};
        AgentPromise::SetResult(promise_, std::move(resultCallback));
        G_Printf("PAgentCallback = Default\n");
    }
    /// @brief Return State object associated with AgentCallback
    /// @return
    virtual typename State::Ptr GetPromiseStatePtr() { return promise_.GetStatePtr(); }

public:
    /// @brief Return the Future from the stored Promise.
    /// @return
    auto GetFuture() { return promise_.get_future(); }
};
//*************/ //Template type: <T_Func, E, Void>
template < typename T_Func, typename E >
class PAgentCallback< T_Func, E, std::enable_if_t< std::is_void< std::result_of_t< T_Func() > >::value > >
    : public PAgentReturn_Base< T_Func >
{
private:
    /// @brief base class
    using T_Base = PAgentReturn_Base< T_Func >;
    /// @brief
    using TF = std::enable_if_t< std::is_void< std::result_of_t< T_Func() > >::value >;

public:
    /// @brief
    using Ptr = std::shared_ptr< PAgentCallback< T_Func, E, TF > >;

private:
    ara::core::Promise< void, E > promise_;

public:
    explicit PAgentCallback(T_Func&& callback) : T_Base(std::forward< T_Func >(callback)) {}
    /// @brief Executes the stored callback.
    void FireCallback() override
    {
        if (false == T_Base::_HasCallback()) {
            return;
        }
        T_Base::pCallback_();
        promise_.SetResult(ara::core::Result< void >::FromValue());
        G_Printf("PAgentCallback = Void\n");
    }
    /// @brief Return State object associated with AgentCallback
    /// @return
    virtual typename State::Ptr GetPromiseStatePtr() { return promise_.GetStatePtr(); }

public:
    /// @brief Return the Future from the stored Promise.
    /// @return
    auto GetFuture() { return promise_.get_future(); }
};
//********************************/
}  // namespace internal
}  // namespace core
}  // namespace ara

#endif /* ARA_CORE_INTERNAL_STATE_THEN_H */
