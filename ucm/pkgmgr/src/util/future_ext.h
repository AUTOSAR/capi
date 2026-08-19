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
/// @file       future_ext.h
/// @brief      future extension
/// @details
/// @date       2024-08-15
/// @author     cuiyinli
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @export_level=/UCM/Utils
/// @module_path=/UCM/Utils
/// @interface_level=module
/// @trace_id_sr=SR_UCM_00001
/// @unit_name=FutureExt
/// @unit_description=future extension
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_PKGMGR_UTIL_FUTURE_EXT_H_
#define ARA_UCM_PKGMGR_UTIL_FUTURE_EXT_H_

#include <functional>

#include "common/alias.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief This class merge promise and future in packaged task
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10321
/// @trace_id_dd=DD_UCM_10742
/// @needwork = ad
/// @endcode
template < typename R >
class PackagedTask
{
public:
    /// @brief constructor
    /// @param f
    /// @param args
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10743
    /// @needwork = dda
    /// @endcode
    template < typename F, typename... Args >
    explicit PackagedTask(F&& f, Args&&... args)
        : f_{std::move(std::bind(std::forward< F >(f), std::forward< Args >(args)...))}, p_{}
    {
    }

    /// @brief destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10744
    /// @needwork = dda
    /// @endcode
    virtual ~PackagedTask() = default;

    /// @brief delete copy construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10745
    /// @needwork = dda
    /// @endcode
    PackagedTask(PackagedTask const& other) = delete;
    /// @brief delete copy asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10746
    /// @needwork = dda
    /// @endcode
    PackagedTask& operator=(PackagedTask const& other) = delete;

    /// @brief move construct
    /// @param other other class object
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10747
    /// @needwork = dda
    /// @endcode
    PackagedTask(PackagedTask&& other) = delete;
    /// PackagedTask(PackagedTask&& other) {.
    ///    f_ = std::move(other.f_);
    ///    p_ = std::move(other.p_);
    ///}

    /// @brief move asign
    /// @param other other class object
    /// @return ref
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10748
    /// @needwork = dda
    /// @endcode
    PackagedTask& operator=(PackagedTask&& other) = delete;
    ///PackagedTask& operator=(PackagedTask&& other) {
    ///    if (this != &other) {
    ///        f_ = std::move(other.f_);
    ///        p_ = std::move(other.p_);
    ///    }
    ///    return *this;
    ///}

    /// @brief operator()
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10749
    /// @needwork = dda
    /// @endcode
    void operator()() { on_call(); }

    /// @brief on_call
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10750
    /// @needwork = dda
    /// @endcode
    virtual void on_call() = 0;  // NOLINT

    /// @brief get_future
    /// @return future
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10751
    /// @needwork = dda
    /// @endcode
    AraFuture< R > get_future()  // NOLINT
    {
        return std::move(p_.get_future());
    }

protected:
    /// @brief function
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10752
    /// @needwork = dda
    /// @endcode
    std::function< AraResult< R >() > f_;  // NOLINT
    /// @brief Promise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10753
    /// @needwork = dda
    /// @endcode
    AraPromise< R > p_;  // NOLINT
};

/// @brief PackagedTaskReturnValue
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10321
/// @trace_id_dd=DD_UCM_10754
/// @needwork = dd
/// @endcode
template < typename R >
class PackagedTaskReturnValue : public PackagedTask< R >
{
public:
    /// @brief constructor
    /// @param f
    /// @param args
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10755
    /// @needwork = dda
    /// @endcode
    template < typename F, typename... Args >
    explicit PackagedTaskReturnValue(F&& f, Args&&... args)
        : PackagedTask< R >{std::forward< F >(f), std::forward< Args >(args)...}
    {
    }

    /// @brief on_call
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10756
    /// @needwork = dda
    /// @endcode
    void on_call() override  // NOLINT
    {
        AraResult< R > const ret{std::move(this->f_())};
        if (ret.HasValue()) {
            this->p_.set_value(ret.Value());
        } else {
            this->p_.SetError(ret.Error());
        }
    }
};

/// @brief PackagedTaskReturnVoid
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10321
/// @trace_id_dd=DD_UCM_10757
/// @needwork = dd
/// @endcode
class PackagedTaskReturnVoid : public PackagedTask< void >
{
public:
    /// @brief constructor
    /// @param f
    /// @param args
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10758
    /// @needwork = dda
    /// @endcode
    template < typename F, typename... Args >
    explicit PackagedTaskReturnVoid(F&& f, Args&&... args)
        : PackagedTask< void >{std::forward< F >(f), std::forward< Args >(args)...}
    {
    }

    /// @brief on_call
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10759
    /// @needwork = dda
    /// @endcode
    void on_call() override
    {
        AraResultVoid const ret{this->f_()};
        if (ret.HasValue()) {
            this->p_.set_value();
        } else {
            this->p_.SetError(ret.Error());
        }
    }
};

/// @brief sync call function directly
/// F return is AraResult<R>
/// @param f
/// @param args
/// @return future
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10405
/// @trace_id_dd=DD_UCM_10760
/// @needwork = ad
/// @endcode
template < typename R, typename F, typename... Args >
AraFuture< R > SyncCall(F&& f, Args&&... args)
{
    PackagedTaskReturnValue< R > task{std::forward< F >(f), std::forward< Args >(args)...};
    AraFuture< R > fu{std::move(task.get_future())};
    task();
    return fu;
}

/// @brief async call function in thread
/// @param f
/// @param args
/// @return future
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10405
/// @trace_id_dd=DD_UCM_10761
/// @needwork = dd
/// @endcode
template < typename R, typename F, typename... Args >
AraFuture< R > ASyncCall(F&& f, Args&&... args)
{
    std::unique_ptr< PackagedTask< R > > task{
        std::make_unique< PackagedTaskReturnValue< R > >(std::forward< F >(f), std::forward< Args >(args)...)};
    AraFuture< R > fu{std::move(task->get_future())};

    std::thread th{[t = std::move(task)]() { (*t)(); }};
    th.detach();

    return fu;
}

/// @brief sync call function directly, the function return void
/// @param f
/// @param args
/// @return future
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10405
/// @trace_id_dd=DD_UCM_10762
/// @needwork = dd
/// @endcode
template < typename F, typename... Args >
AraFutureVoid SyncCallVoid(F&& f, Args&&... args)
{
    PackagedTaskReturnVoid task{std::forward< F >(f), std::forward< Args >(args)...};
    AraFutureVoid fu{task.get_future()};
    task();
    return fu;
}

/// @brief async call function in thread, the function return void
/// @param f
/// @param args
/// @return future
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10405
/// @trace_id_dd=DD_UCM_10763
/// @needwork = dd
/// @endcode
template < typename F, typename... Args >
AraFutureVoid ASyncCallVoid(F&& f, Args&&... args)
{
    std::unique_ptr< PackagedTask< void > > task{
        std::make_unique< PackagedTaskReturnVoid >(std::forward< F >(f), std::forward< Args >(args)...)};
    AraFutureVoid fu{task->get_future()};

    std::thread th{[t = std::move(task)]() { (*t)(); }};
    th.detach();

    return fu;
}

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara

/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define ASYNC_CALL_BEGIN(type)   return ASyncCall<type>([&]() noexcept -> AraResult<type> {
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define ASYNC_CALL_END()                                                                                               \
    })

/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define ASYNC_CALL_VOID_BEGIN()  return ASyncCallVoid([&]() noexcept {
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define ASYNC_CALL_VOID_BEGIN_WITH_FUNC(funcName)                                                                      \
    return ASyncCallVoid([&, funcName]() noexcept {
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define ASYNC_CALL_VOID_BEGIN_WITH_FUNC_ID(funcName, transferId)                                                       \
    return ASyncCallVoid([&, funcName, transferId]() noexcept {
#endif  // ARA_UCM_PKGMGR_UTIL_FUTURE_EXT_H_
