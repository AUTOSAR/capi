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
/// @file       result.h
/// @brief      The definition and implementation of ara::core::Result<T, E> type (and its specialization for T=void) that contains a value of type T or an error of type E.
/// @details
/// @date       2022-06-20
/// @author     hanzhibo
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_CORE_RESULT_H_
#define ARA_CORE_RESULT_H_

#include <iostream>
#include <memory>
#include <system_error>
#include <type_traits>
#include <utility>

#include "ara/core/abort.h"
#include "ara/core/error_code.h"
#include "ara/core/optional.h"
#include "ara/core/variant.h"

namespace ara {
namespace core {

/// @brief This class is a type that contains either a value or an error.
///
/// @tparam T the type of value
/// @tparam E the type of error
///
/// @uptrace{SWS_CORE_00701, 7f8f4deeb58cb54dd14f852a09654f0a21e455e6}
template < typename T, typename E = ErrorCode >
class Result final
{
    ara::core::Variant< T, E > mData_;

    template < typename U, typename... Args >
    struct has_as_first_checker;

    template < typename U >
    struct has_as_first_checker< U > : std::false_type
    {
    };

    template < typename U, typename... Args >
    struct has_as_first_checker< U, U, Args... > : std::true_type
    {
    };

    template < typename U, typename A, typename... Args >
    struct has_as_first_checker< U, A, Args... > : std::false_type
    {
    };

    template < typename U, typename... Args >
    struct has_as_first  // NOLINT
        : public has_as_first_checker< typename std::remove_reference< U >::type,
                                       typename std::remove_reference< Args >::type... >
    {
    };

public:
    /// @uptrace{SWS_CORE_00711, bd1e9abffcc17f04343dab3c3fa55e65c20de7bc}
    using value_type = T;  // NOLINT

    /// @uptrace{SWS_CORE_00712, e51a93367c1b7c206fb3ad5bc0c9e989eaf27248}
    using error_type = E;  // NOLINT

    /// @uptrace{SWS_CORE_00731, 1660713acd8eb4a730654c713c3c29501a69f39e}
    static Result FromValue(T const& t) { return Result(t); }

    /// @uptrace{SWS_CORE_00732, e02fd637863a2d61455175d89d6c9ab50c8cf1f9}
    static Result FromValue(T&& t) { return Result(std::move(t)); }

    /// @uptrace{SWS_CORE_00733, b66440d78a3ce3f0cdf00a5d8ba158158d73a12c}
    template < typename... Args,
               typename = typename std::enable_if< true                                             //
                                                   && std::is_constructible< T, Args&&... >::value  //
                                                   && !has_as_first< T, Args... >::value            //
                                                   && !has_as_first< Result, Args... >::value       //
                                                   >::type                                          //
               >
    static Result FromValue(Args&&... args)
    {
        return Result(T{std::forward< Args >(args)...});
    }

    // ----------------------------------------

    /// @uptrace{SWS_CORE_00734, 166384c8fd64b4d74a1f2bddc9b975f9a927956a}
    static Result FromError(E const& e) { return Result(e); }

    /// @uptrace{SWS_CORE_00735, 1d6790688f3411cbc6040d8be27d2adbb017e58f}
    static Result FromError(E&& e) { return Result(std::move(e)); }

    /// @uptrace{SWS_CORE_00736, 5a2965683ae0f01844706c91056283355870c1c4}
    template < typename... Args,
               typename = typename std::enable_if< true                                             //
                                                   && std::is_constructible< E, Args&&... >::value  //
                                                   && !has_as_first< E, Args... >::value            //
                                                   && !has_as_first< Result, Args... >::value       //
                                                   >::type                                          //
               >
    static Result FromError(Args&&... args)
    {
        return Result(E{std::forward< Args >(args)...});
    }

    // ----------------------------------------

    /// @uptrace{SWS_CORE_00721, 65dfc6c0c1229dc57b2c4dc9c2cc9f1ce6b5147f}
    Result(T const& t) // NOLINT : Adding explicit here will make T => Result syntax extremely inconvenient
        : mData_(t)
    {
    }

    /// @uptrace{SWS_CORE_00722, 20bff429c9e5c79ae4e4d7ebb4e707e78d505424}
    explicit Result(T&& t) : mData_(std::move(t)) {}

    /// @uptrace{SWS_CORE_00723, 4e71a51886915bee88a84142a88f191b9af692fc}
    explicit Result(E const& e) : mData_(e) {}

    /// @uptrace{SWS_CORE_00724, eb14f62fb5b8c0f3766f99f333ffd1734da5716f}
    explicit Result(E&& e) : mData_(std::move(e)) {}

    /// @uptrace{SWS_CORE_00725, fd10125d250eb93b548c059aa7fdd3fa6c4a5307}
    Result(Result const& other) = default;

    /// @uptrace{SWS_CORE_00726, 60104d6ed6c104b6696daddf1a8e677725969382}
    Result(Result&& other) noexcept(
        std::is_nothrow_move_constructible< T >::value&& std::is_nothrow_move_constructible< E >::value)
        = default;

    /// @uptrace{SWS_CORE_00727, c7f1d10c90e0c4192da6d18aef3fc691a97bf7e5}
    ~Result() = default;

    /// @uptrace{SWS_CORE_00741, b030501271a04d3434e86495c1c6548681bd50d6}
    Result& operator=(Result const& other) = default;

    /// @uptrace{SWS_CORE_00742, 8a8653c286b4f9aa83d7876a553b9999cab19fc0}
    Result& operator=(Result&& other) noexcept(
        std::is_nothrow_move_constructible< T >::value&& std::is_nothrow_move_assignable< T >::value&&
            std::is_nothrow_move_constructible< E >::value&& std::is_nothrow_move_assignable< E >::value)
        = default;

    // ----------------------------------------

    /// @uptrace{SWS_CORE_00743, acf93b8f398d7bc9e5ceceae8cf49d9297960824}
    template < typename... Args >
    void EmplaceValue(Args&&... args)
    {
        // Unfortunately, Boost.Variant does not support emplace(), so we fall back to assignment here.
        mData_ = std::move(T(std::forward< Args >(args)...));
    }

    /// @uptrace{SWS_CORE_00744, 51defa1ed171c088fa256670830283290de26792}
    template < typename... Args >
    void EmplaceError(Args&&... args)
    {
        // Unfortunately, Boost.Variant does not support emplace(), so we fall back to assignment here.
        mData_ = std::move(E(std::forward< Args >(args)...));
    }

    /// @uptrace{SWS_CORE_00745, c01cc5aeb4ee3a3efa21dfcba711dc75d2c90ce4}
    void Swap(Result& other) noexcept(
        std::is_nothrow_move_constructible< T >::value&& std::is_nothrow_move_assignable< T >::value&&
            std::is_nothrow_move_constructible< E >::value&& std::is_nothrow_move_assignable< E >::value)
    {
        using std::swap;
        swap(mData_, other.mData_);
    }

    /// @uptrace{SWS_CORE_00751, 88f7537512c1e7f3c6192afe5067b2ff9a531cca}
    bool HasValue() const noexcept { return mData_.index() == 0; }

    /// @uptrace{SWS_CORE_00752, a5f40dc378a0d7f20ee96227070aa7f458b6e362}
    explicit operator bool() const noexcept { return HasValue(); }

    /// @uptrace{SWS_CORE_00753, dac11a084ebed96c6b053f6175b241a19237d9a0}
    T const& operator*() const& { return Value(); }

    /// @uptrace{SWS_CORE_00759, 9be18b648a6e8f3e55467ee41a5e2167f34ff02e}
    T&& operator*() && { return std::move(*this).Value(); }

    /// @uptrace{SWS_CORE_00754, 43f3ae005cf361a481701e815b94994c95287540}
    T const* operator->() const { return std::addressof(Value()); }

    /// @uptrace{SWS_CORE_00755, dc2048e3568964e5bcc2b6a8722405b79cfa05b0}
    T const& Value() const&
    {
        T const* ptr = ara::core::get_if< T >(&mData_);
        if (ptr) {
            return *ptr;
        }
        ara::core::Abort(
            "template <typename T, typename E = ErrorCode>Result T const& Value() const& called but NOT a value!\n");
    }

    /// @uptrace{SWS_CORE_00756, 8eb58942774c0b7d90af69f6bc88c053f4bbd7bb}
    T&& Value() &&
    {
        T* ptr = ara::core::get_if< T >(&mData_);
        if (ptr) {
            return std::move(*ptr);
        }
        ara::core::Abort(
            "template <typename T, typename E = ErrorCode>Result T&& Value() && called but NOT a value!\n");
    }

    /// @uptrace{SWS_CORE_00757, 08c30fe9353d8345c4c5578318210329a438c094}
    E const& Error() const&
    {
        E const* ptr = ara::core::get_if< E >(&mData_);
        if (ptr) {
            return *ptr;
        }
        ara::core::Abort(
            "template <typename T, typename E = ErrorCode>Result E const& Error() const& called but NOT an error!\n");
    }

    /// @uptrace{SWS_CORE_00758, 42bd8a9f9b88c9039f77741bec5b67b2522c14a1}
    E&& Error() &&
    {
        E* ptr = ara::core::get_if< E >(&mData_);
        if (ptr) {
            return std::move(*ptr);
        }
        ara::core::Abort(
            "template <typename T, typename E = ErrorCode>Result E&& Error() && called but NOT an error!\n");
    }

    /// @uptrace{SWS_CORE_00770, 958fb6071007038d0fb239aaf4fcd4228761c847}
    // Optional<T> Ok() const& noexcept // modified by hanzhibo
    Optional< T > Ok() const& { return HasValue() ? Optional< T >(Value()) : Optional< T >{nullopt}; }

    /// @uptrace{SWS_CORE_00771, 20b02a0bf4ab60b6b9bb4a0ec565a300e32886c5}
    // Optional<T> Ok() && noexcept // modified by hanzhibo
    Optional< T > Ok() && { return HasValue() ? Optional< T >(std::move(Value())) : Optional< T >(nullopt); }

    /// @uptrace{SWS_CORE_00772, e33b21e10177d12a29215f522689ac115603811a}
    // Optional<E> Err() const& noexcept // modified by hanzhibo
    Optional< E > Err() const& { return HasValue() ? nullopt : Optional< E >(Error()); }

    /// @uptrace{SWS_CORE_00773, dd36b61cacc409eb1686881fbd9af72dc9765b8d}
    // Optional<E> Err() && noexcept // modified by hanzhibo
    Optional< E > Err() && { return HasValue() ? nullopt : Optional< E >(std::move(Error())); }

    /// @uptrace{SWS_CORE_00761, 06c01a572d6b02b06696e257a3f60eb9fc80d469}
    template < typename U >
    T ValueOr(U&& defaultValue) const&
    {
        return HasValue() ? Value() : static_cast< T >(std::forward< U >(defaultValue));
    }

    /// @uptrace{SWS_CORE_00762, 82968f267414bde13d380c6f6d73268a5bc72fcc}
    template < typename U >
    T ValueOr(U&& defaultValue) &&
    {
        return HasValue() ? std::move(Value()) : static_cast< T >(std::forward< U >(defaultValue));
    }

    /// @uptrace{SWS_CORE_00763, caa0bf91b5ef2538489b46e0db4094456db73cd3}
    template < typename G >
    E ErrorOr(G&& defaultError) const&
    {
        return HasValue() ? static_cast< E >(std::forward< G >(defaultError)) : Error();
    }

    /// @uptrace{SWS_CORE_00764, 95ad3970a4832818b5cb8e03bcd4037b80b8ea68}
    template < typename G >
    E ErrorOr(G&& defaultError) &&
    {
        return HasValue() ? static_cast< E >(std::forward< G >(defaultError)) : std::move(Error());
    }

    /// @uptrace{SWS_CORE_00765, aa7739be140a2f5e5884041f31a0494407f22ed7}
    template < typename G >
    bool CheckError(G&& error) const
    {
        return HasValue() ? false : (Error() == static_cast< E >(std::forward< G >(error)));
    }

#ifndef ARA_NO_EXCEPTIONS
    /// @uptrace{SWS_CORE_00766, 7de31741392cacdb51dcf11d4ec30a3afec57b86}
    T const& ValueOrThrow() const& noexcept(false)
    {
        T const* ptr = ara::core::get_if< T >(&mData_);
        if (ptr) {
            return *ptr;
        }

        E const* e = ara::core::get_if< E >(&mData_);
        e->ThrowAsException();
    }

    /// @uptrace{SWS_CORE_00769, 825f556b7e61f6e623b921961e5e113c80908db4}
    T&& ValueOrThrow() && noexcept(false)
    {
        T* ptr = ara::core::get_if< T >(&mData_);
        if (ptr) {
            return std::move(*ptr);
        }

        E* e = ara::core::get_if< E >(&mData_);
        e->ThrowAsException();
    }
#endif

    /// @uptrace{SWS_CORE_00767, e0273c20a94932322ec69e3986211def89410a69}
    template < typename F >
    T Resolve(F&& f) const
    {
        return HasValue() ? Value() : std::forward< F >(f)(Error());
    }

private:
    // Re-implementation of C++14's std::enable_if_t
    template < bool Condition, typename U = void >                      // NOLINT : keep template parameter naming style
    using enable_if_t = typename std::enable_if< Condition, U >::type;  // NOLINT

    // Re-implementation of C++14's std::result_of_t
    template < typename U >
    using result_of_t = typename std::result_of< U >::type;  // NOLINT

    /// @brief Trait that detects whether a type is a Result<...>
    template < typename U >
    struct is_result : std::false_type  // NOLINT
    {
    };

    template < typename U, typename G >
    struct is_result< Result< U, G > > : std::true_type  // NOLINT
    {
    };

    template < typename F >
    using CallableReturnsResult = enable_if_t< is_result< result_of_t< F(T const&) > >::value >;

    template < typename F >
    using CallableReturnsNoResult = enable_if_t< !is_result< result_of_t< F(T const&) > >::value >;

public:
    /// @uptrace{SWS_CORE_00768, 62c97cecf9c00813b6b8e73564a9b222ae0a910f}
    template < typename F, typename = CallableReturnsResult< F > >
    auto Bind(F&& f) const -> decltype(f(Value()))
    {
        using U = decltype(f(Value()));
        return HasValue() ? U{std::forward< F >(f)(Value())} : U(Error());
    }

    /// @uptrace{SWS_CORE_00768, 62c97cecf9c00813b6b8e73564a9b222ae0a910f}
    template < typename F, typename = CallableReturnsNoResult< F > >
    auto Bind(F&& f) const -> Result< decltype(f(Value())), E >
    {
        using U = decltype(f(Value()));
        using R = Result< U, E >;
        return HasValue() ? R{std::forward< F >(f)(Value())} : R(Error());
    }

    template < typename, typename >
    friend class Result;
};

/// @uptrace{SWS_CORE_00796, 9e605f8eb12fb3a920cd99c2e1840044bb70c249}
template < typename T, typename E >
inline void swap(Result< T, E >& lhs, Result< T, E >& rhs) noexcept(noexcept(lhs.Swap(rhs)))  // NOLINT
{
    lhs.Swap(rhs);
}

/// @uptrace{SWS_CORE_00780, 8aa19b459a02c2fb7d30d5cf08ee91579421a903}
template < typename T, typename E >
bool operator==(Result< T, E > const& lhs, Result< T, E > const& rhs)
{
    if (bool(lhs) != bool(rhs)) {
        return false;
    }
    if (lhs) {
        return lhs.Value() == rhs.Value();
    }
    return lhs.Error() == rhs.Error();
}

/// @uptrace{SWS_CORE_00781, 4eca0fd1697b3e7efda3e348dbf4337b27addff9}
template < typename T, typename E >
bool operator!=(Result< T, E > const& lhs, Result< T, E > const& rhs)
{
    if (bool(lhs) != bool(rhs)) {
        return true;
    }
    if (lhs) {
        return lhs.Value() != rhs.Value();
    }
    return lhs.Error() != rhs.Error();
}

/// @uptrace{SWS_CORE_00782, f5749b01752a82e7e522985e2a6ee117162c39fe}
template < typename T, typename E >
bool operator==(Result< T, E > const& lhs, T const& value)
{
    return lhs ? *lhs == value : false;
}

/// @uptrace{SWS_CORE_00783, 0f214a0f02b4bb3eda2f6cae0c0ad17bb3a3467b}
template < typename T, typename E >
bool operator==(T const& value, Result< T, E > const& rhs)
{
    return rhs ? value == *rhs : false;
}

/// @uptrace{SWS_CORE_00784, bb0ac03982b360dad5da59d1476bc31dce0ccbae}
template < typename T, typename E >
bool operator!=(Result< T, E > const& lhs, T const& value)
{
    return lhs ? *lhs != value : true;
}

/// @uptrace{SWS_CORE_00785, e8739e533a372478534fac83adddde3a60640a62}
template < typename T, typename E >
bool operator!=(T const& value, Result< T, E > const& rhs)
{
    return rhs ? value != *rhs : true;
}

/// @uptrace{SWS_CORE_00786, a3f4f56c1c6937e3712036bfe379ae9060608c9a}
template < typename T, typename E >
bool operator==(Result< T, E > const& lhs, E const& error)
{
    return lhs ? false : lhs.Error() == error;
}

/// @uptrace{SWS_CORE_00787, 12b659af3d377ffcb2291698578cf6ee110ca934}
template < typename T, typename E >
bool operator==(E const& error, Result< T, E > const& rhs)
{
    return rhs ? false : error == rhs.Error();
}

/// @uptrace{SWS_CORE_00788, 32e8ed0e646d9c8945d32fab05576c2876c0c5aa}
template < typename T, typename E >
bool operator!=(Result< T, E > const& lhs, E const& error)
{
    return lhs ? true : lhs.Error() != error;
}

/// @uptrace{SWS_CORE_00789, befe7f3a89bf21a52db027bce9b8720088f27dbc}
template < typename T, typename E >
bool operator!=(E const& error, Result< T, E > const& rhs)
{
    return rhs ? true : error != rhs.Error();
}

/// @brief Specialization for T&
///
/// @tparam T the type of value
/// @tparam E the type of error
///
template < typename T, typename E >
class Result< T&, E > final
{
    ara::core::Variant< std::reference_wrapper< T >, E > mData_;

    template < typename U, typename... Args >
    struct has_as_first_checker;

    template < typename U >
    struct has_as_first_checker< U > : std::false_type
    {
    };

    template < typename U, typename... Args >
    struct has_as_first_checker< U, U, Args... > : std::true_type
    {
    };

    template < typename U, typename A, typename... Args >
    struct has_as_first_checker< U, A, Args... > : std::false_type
    {
    };

    template < typename U, typename... Args >
    struct has_as_first  // NOLINT
        : public has_as_first_checker< typename std::remove_reference< U >::type,
                                       typename std::remove_reference< Args >::type... >
    {
    };

public:
    /// @uptrace{SWS_CORE_00711, bd1e9abffcc17f04343dab3c3fa55e65c20de7bc}
    using value_type = T;  // NOLINT

    /// @uptrace{SWS_CORE_00712, e51a93367c1b7c206fb3ad5bc0c9e989eaf27248}
    using error_type = E;  // NOLINT

    /// @uptrace{SWS_CORE_00731, 1660713acd8eb4a730654c713c3c29501a69f39e}
    static Result FromValue(T& t) { return Result(t); }

    /// @uptrace{SWS_CORE_00733, b66440d78a3ce3f0cdf00a5d8ba158158d73a12c}
    template < typename... Args,
               typename = typename std::enable_if< true                                             //
                                                   && std::is_constructible< T, Args&&... >::value  //
                                                   && !has_as_first< T, Args... >::value            //
                                                   && !has_as_first< Result, Args... >::value       //
                                                   >::type                                          //
               >
    static Result FromValue(Args&&... args)
    {
        return Result(T{std::forward< Args >(args)...});
    }

    // ----------------------------------------

    /// @uptrace{SWS_CORE_00734, 166384c8fd64b4d74a1f2bddc9b975f9a927956a}
    static Result FromError(E const& e) { return Result(e); }

    /// @uptrace{SWS_CORE_00735, 1d6790688f3411cbc6040d8be27d2adbb017e58f}
    static Result FromError(E&& e) { return Result(std::move(e)); }

    /// @uptrace{SWS_CORE_00736, 5a2965683ae0f01844706c91056283355870c1c4}
    template < typename... Args,
               typename = typename std::enable_if< true                                             //
                                                   && std::is_constructible< E, Args&&... >::value  //
                                                   && !has_as_first< E, Args... >::value            //
                                                   && !has_as_first< Result, Args... >::value       //
                                                   >::type                                          //
               >
    static Result FromError(Args&&... args)
    {
        return Result(E{std::forward< Args >(args)...});
    }

    // ----------------------------------------

    /// @uptrace{SWS_CORE_00721, 65dfc6c0c1229dc57b2c4dc9c2cc9f1ce6b5147f}
    explicit Result(T& t) : mData_(t) {}

    /// @uptrace{SWS_CORE_00723, 4e71a51886915bee88a84142a88f191b9af692fc}
    explicit Result(E const& e) : mData_(e) {}

    /// @uptrace{SWS_CORE_00724, eb14f62fb5b8c0f3766f99f333ffd1734da5716f}
    explicit Result(E&& e) : mData_(std::move(e)) {}

    /// @uptrace{SWS_CORE_00725, fd10125d250eb93b548c059aa7fdd3fa6c4a5307}
    Result(Result const& other) = default;

    /// @uptrace{SWS_CORE_00726, 60104d6ed6c104b6696daddf1a8e677725969382}
    Result(Result&& other) noexcept(
        std::is_nothrow_move_constructible< T >::value&& std::is_nothrow_move_constructible< E >::value)
        = default;

    /// @uptrace{SWS_CORE_00727, c7f1d10c90e0c4192da6d18aef3fc691a97bf7e5}
    ~Result() = default;

    /// @uptrace{SWS_CORE_00741, b030501271a04d3434e86495c1c6548681bd50d6}
    Result& operator=(Result const& other) = default;

    /// @uptrace{SWS_CORE_00742, 8a8653c286b4f9aa83d7876a553b9999cab19fc0}
    Result& operator=(Result&& other) noexcept(
        std::is_nothrow_move_constructible< T >::value&& std::is_nothrow_move_assignable< T >::value&&
            std::is_nothrow_move_constructible< E >::value&& std::is_nothrow_move_assignable< E >::value)
        = default;

    // ----------------------------------------

    /// @uptrace{SWS_CORE_00743, acf93b8f398d7bc9e5ceceae8cf49d9297960824}
    template < typename... Args >
    void EmplaceValue(Args&&... args)
    {
        // Unfortunately, Boost.Variant does not support emplace(), so we fall back to assignment here.
        mData_ = std::move(T(std::forward< Args >(args)...));
    }

    /// @uptrace{SWS_CORE_00744, 51defa1ed171c088fa256670830283290de26792}
    template < typename... Args >
    void EmplaceError(Args&&... args)
    {
        // Unfortunately, Boost.Variant does not support emplace(), so we fall back to assignment here.
        mData_ = std::move(E(std::forward< Args >(args)...));
    }

    /// @uptrace{SWS_CORE_00745, c01cc5aeb4ee3a3efa21dfcba711dc75d2c90ce4}
    void Swap(Result& other) noexcept(
        std::is_nothrow_move_constructible< T >::value&& std::is_nothrow_move_assignable< T >::value&&
            std::is_nothrow_move_constructible< E >::value&& std::is_nothrow_move_assignable< E >::value)
    {
        using std::swap;
        swap(mData_, other.mData_);
    }

    /// @uptrace{SWS_CORE_00751, 88f7537512c1e7f3c6192afe5067b2ff9a531cca}
    bool HasValue() const noexcept { return mData_.index() == 0; }

    /// @uptrace{SWS_CORE_00752, a5f40dc378a0d7f20ee96227070aa7f458b6e362}
    explicit operator bool() const noexcept { return HasValue(); }

    /// @uptrace{SWS_CORE_00753, dac11a084ebed96c6b053f6175b241a19237d9a0}
    T const& operator*() const& { return Value(); }

    /// @uptrace{SWS_CORE_00754, 43f3ae005cf361a481701e815b94994c95287540}
    T const* operator->() const { return std::addressof(Value()); }

    /// @uptrace{SWS_CORE_00755, dc2048e3568964e5bcc2b6a8722405b79cfa05b0}
    T const& Value() const&
    {
        std::reference_wrapper< T > const* ptr = ara::core::get_if< std::reference_wrapper< T > >(&mData_);
        if (ptr) {
            return *ptr;
        }
        ara::core::Abort("template <typename T, typename E>Result T const& Value() const& called but NOT a value!\n");
    }

    /// @uptrace{SWS_CORE_00756, 8eb58942774c0b7d90af69f6bc88c053f4bbd7bb}
    T&& Value() &&
    {
        std::reference_wrapper< T >* ptr = ara::core::get_if< std::reference_wrapper< T > >(&mData_);
        if (ptr) {
            return std::move(ptr->get());
        }
        ara::core::Abort("template <typename T, typename E>Result T&& Value() && called but NOT a value!\n");
    }

    /// @uptrace{SWS_CORE_00757, 08c30fe9353d8345c4c5578318210329a438c094}
    E const& Error() const&
    {
        E const* ptr = ara::core::get_if< E >(&mData_);
        if (ptr) {
            return *ptr;
        }
        ara::core::Abort("template <typename T, typename E>Result E const& Error() const& called but NOT an error!\n");
    }

    /// @uptrace{SWS_CORE_00758, 42bd8a9f9b88c9039f77741bec5b67b2522c14a1}
    E&& Error() &&
    {
        E* ptr = ara::core::get_if< E >(&mData_);
        if (ptr) {
            return std::move(*ptr);
        }
        ara::core::Abort("template <typename T, typename E>Result E&& Error() && called but NOT an error!\n");
    }

    /// @uptrace{SWS_CORE_00770, 958fb6071007038d0fb239aaf4fcd4228761c847}
    // Optional<T> Ok() const& noexcept // modified by hanzhibo
    Optional< T > Ok() const& { return HasValue() ? Optional< T >(Value()) : nullopt; }

    /// @uptrace{SWS_CORE_00771, 20b02a0bf4ab60b6b9bb4a0ec565a300e32886c5}
    // Optional<T> Ok() && noexcept // modified by hanzhibo
    Optional< T > Ok() && { return HasValue() ? Optional< T >(std::move(Value())) : nullopt; }

    /// @uptrace{SWS_CORE_00772, e33b21e10177d12a29215f522689ac115603811a}
    // Optional<E> Err() const& noexcept // modified by hanzhibo
    Optional< E > Err() const& { return HasValue() ? nullopt : Optional< E >(Error()); }

    /// @uptrace{SWS_CORE_00773, dd36b61cacc409eb1686881fbd9af72dc9765b8d}
    // Optional<E> Err() && noexcept // modified by hanzhibo
    Optional< E > Err() && { return HasValue() ? nullopt : Optional< E >(std::move(Error())); }

    /// @uptrace{SWS_CORE_00761, 06c01a572d6b02b06696e257a3f60eb9fc80d469}
    template < typename U >
    T ValueOr(U&& defaultValue) const&
    {
        return HasValue() ? Value() : static_cast< T >(std::forward< U >(defaultValue));
    }

    /// @uptrace{SWS_CORE_00762, 82968f267414bde13d380c6f6d73268a5bc72fcc}
    template < typename U >
    T ValueOr(U&& defaultValue) &&
    {
        return HasValue() ? std::move(Value()) : static_cast< T >(std::forward< U >(defaultValue));
    }

    /// @uptrace{SWS_CORE_00763, caa0bf91b5ef2538489b46e0db4094456db73cd3}
    template < typename G >
    E ErrorOr(G&& defaultError) const&
    {
        return HasValue() ? static_cast< E >(std::forward< G >(defaultError)) : Error();
    }

    /// @uptrace{SWS_CORE_00764, 95ad3970a4832818b5cb8e03bcd4037b80b8ea68}
    template < typename G >
    E ErrorOr(G&& defaultError) &&
    {
        return HasValue() ? static_cast< E >(std::forward< G >(defaultError)) : std::move(Error());
    }

    /// @uptrace{SWS_CORE_00765, aa7739be140a2f5e5884041f31a0494407f22ed7}
    template < typename G >
    bool CheckError(G&& error) const
    {
        return HasValue() ? false : (Error() == static_cast< E >(std::forward< G >(error)));
    }

#ifndef ARA_NO_EXCEPTIONS
    /// @uptrace{SWS_CORE_00766, 7de31741392cacdb51dcf11d4ec30a3afec57b86}
    T const& ValueOrThrow() const& noexcept(false)
    {
        T const* ptr = ara::core::get_if< T >(&mData_);
        if (ptr) {
            return *ptr;
        }

        E const* e = ara::core::get_if< E >(&mData_);
        e->ThrowAsException();
    }
#endif

    /// @uptrace{SWS_CORE_00767, e0273c20a94932322ec69e3986211def89410a69}
    template < typename F >
    T Resolve(F&& f) const
    {
        return HasValue() ? Value() : std::forward< F >(f)(Error());
    }

private:
    // Re-implementation of C++14's std::enable_if_t
    template < bool Condition, typename U = void >                      // NOLINT : keep template parameter naming style
    using enable_if_t = typename std::enable_if< Condition, U >::type;  // NOLINT

    // Re-implementation of C++14's std::result_of_t
    template < typename U >
    using result_of_t = typename std::result_of< U >::type;  // NOLINT

    /// @brief Trait that detects whether a type is a Result<...>
    template < typename U >
    struct is_result : std::false_type  // NOLINT
    {
    };

    template < typename U, typename G >
    struct is_result< Result< U, G > > : std::true_type
    {
    };

    template < typename F >
    using CallableReturnsResult = enable_if_t< is_result< result_of_t< F(T const&) > >::value >;

    template < typename F >
    using CallableReturnsNoResult = enable_if_t< !is_result< result_of_t< F(T const&) > >::value >;

public:
    /// @uptrace{SWS_CORE_00768, 62c97cecf9c00813b6b8e73564a9b222ae0a910f}
    template < typename F, typename = CallableReturnsResult< F > >
    auto Bind(F&& f) const -> decltype(f(Value()))
    {
        using U = decltype(f(Value()));
        return HasValue() ? std::forward< F >(f)(Value()) : U(Error());
    }

    /// @uptrace{SWS_CORE_00768, 62c97cecf9c00813b6b8e73564a9b222ae0a910f}
    template < typename F, typename = CallableReturnsNoResult< F > >
    auto Bind(F&& f) const -> Result< decltype(f(Value())), E >
    {
        using U = decltype(f(Value()));
        using R = Result< U, E >;
        return HasValue() ? std::forward< F >(f)(Value()) : R(Error());
    }

    template < typename, typename >
    friend class Result;
};

/// @brief Specialization for void
///
/// @tparam E the type of error
///
/// @uptrace{SWS_CORE_00801, e096aad1345121e5ef2d7b65e0ba44dea7f7a806}
template < typename E >
class Result< void, E > final
{
    // Cannot use 'void' for variant, so define our own (default-constructible) dummy type instead.
    struct dummy  // NOLINT
    {
    };
    using T = dummy;
    ara::core::Variant< T, E > mData_;

    // copied from template <typename T, typename E = ErrorCode> class Result by hanzhibo
    template < typename U, typename... Args >  // NOLINT
    struct has_as_first_checker;               // NOLINT

    template < typename U >                             // NOLINT
    struct has_as_first_checker< U > : std::false_type  // NOLINT
    {
    };

    template < typename U, typename... Args >                      // NOLINT
    struct has_as_first_checker< U, U, Args... > : std::true_type  // NOLINT
    {
    };

    template < typename U, typename A, typename... Args >           // NOLINT
    struct has_as_first_checker< U, A, Args... > : std::false_type  // NOLINT
    {
    };

    template < typename U, typename... Args >  // NOLINT
    struct has_as_first                        // NOLINT
        : public has_as_first_checker< typename std::remove_reference< U >::type,
                                       typename std::remove_reference< Args >::type... >
    {
    };

public:
    /// @uptrace{SWS_CORE_00811, f6a843b85d9778b9cb6d79c4484024bbfc909862}
    using value_type = void;  // NOLINT

    /// @uptrace{SWS_CORE_00812, 196ad9420c30b0de0f2bb5d6753cf6d7e96cad56}
    using error_type = E;  // NOLINT

    /// @uptrace{SWS_CORE_00831, 3b4050ed430716b099d17d4e1e629cc6282a54a8}
    static Result FromValue() noexcept { return Result(); }

    /// @uptrace{SWS_CORE_00834, c743395146d3f17b7ebcd330e603555f0d9616cf}
    static Result FromError(E const& e) { return Result(e); }

    /// @uptrace{SWS_CORE_00835, 6c9d3f802af692ecff7ad4651b07aa254de4aa49}
    static Result FromError(E&& e) { return Result(std::move(e)); }

    /// @uptrace{SWS_CORE_00836, 163f64fb050eff5c48d2095427752d2ce529a3e4}
    // template <typename... Args> // modified by hanzhibo
    template < typename... Args,
               typename = typename std::enable_if< true                                             //
                                                   && std::is_constructible< E, Args&&... >::value  //
                                                   && !has_as_first< E, Args... >::value            //
                                                   && !has_as_first< Result, Args... >::value       //
                                                   >::type                                          //
               >
    static Result FromError(Args&&... args)
    {
        return Result(E{std::forward< Args >(args)...});
    }

    // ----------------------------------------

    /// @uptrace{SWS_CORE_00821, 0d35548dc13b0d3a1cd7c5e2571645a9c3c44f14}
    Result() noexcept : mData_(T{}) {}

    /// @uptrace{SWS_CORE_00823, 21d22dd80d2b8c111246323b201cb7f54dc301cb}
    explicit Result(E const& e) : mData_(e) {}

    /// @uptrace{SWS_CORE_00824, 05eb92516d4a083a4a37aaaedc24bdaece257702}
    explicit Result(E&& e) : mData_(std::move(e)) {}

    /// @uptrace{SWS_CORE_00825, 82d1fe992a0fd40c706f5a30c834da87c4e660f6}
    Result(Result const& other) = default;

    /// @uptrace{SWS_CORE_00826, 32d58af2d832df6fcbe22c66316c9a4cd8b5284e}
    Result(Result&& other) noexcept(std::is_nothrow_move_constructible< E >::value) = default;

    /// @uptrace{SWS_CORE_00827, f3a000bb97c4427b7ea6faa5921bc67414c63254}
    ~Result() noexcept = default;

    /// @uptrace{SWS_CORE_00841, 304dc7d3fe7df09e1f222291e434223aac67f6df}
    Result& operator=(Result const& other) = default;

    /// @uptrace{SWS_CORE_00842, 68e33f279b2e5dfb796e2ceb58cf316384e3f0ae}
    Result& operator=(Result&& other) noexcept(
        std::is_nothrow_move_constructible< E >::value&& std::is_nothrow_move_assignable< E >::value)
        = default;

    // ----------------------------------------

    /// @uptrace{SWS_CORE_00843, c66a3ec6bf172a13b3a321a6a353de9ce5750485}
    template < typename... Args >
    void EmplaceValue(Args&&... args) noexcept
    {
        mData_ = T(args...);
    }

    /// @uptrace{SWS_CORE_00844, fa465953c44477c2e18667ffdfe972bbe03bc09a}
    template < typename... Args >
    void EmplaceError(Args&&... args)
    {
        // Unfortunately, Boost.Variant does not support emplace(), so we fall back to assignment here.
        mData_ = std::move(E(std::forward< Args >(args)...));
    }

    /// @uptrace{SWS_CORE_00845, 792309b5cd1002476634c1dceac547c99e2863e2}
    void Swap(Result& other) noexcept(
        std::is_nothrow_move_constructible< E >::value&& std::is_nothrow_move_assignable< E >::value)
    {
        using std::swap;
        swap(mData_, other.mData_);
    }

    /// @uptrace{SWS_CORE_00851, f745d15786863d9c55f954fad88cd08c580ccecd}
    bool HasValue() const noexcept { return mData_.index() == 0; }

    /// @uptrace{SWS_CORE_00852, 68dcdc94c708c29d7c2f0ccf799486710eeca120}
    explicit operator bool() const noexcept { return HasValue(); }

    /// @uptrace{SWS_CORE_00853, b23d8cdce799ab0a2ee7e137b0af245bcfd821f9}
    void operator*() const { return Value(); }

    /// @uptrace{SWS_CORE_00855, 3d6e7f286b37086138411d86f15be92cf435e62c}
    void Value() const
    {
        T const* ptr = ara::core::get_if< T >(&mData_);
        if (ptr) {
            return;
        }
        ara::core::Abort("template <typename E>Result<void, E> void Value() const called but NOT a (void) value!\n");
    }

    /// @uptrace{SWS_CORE_00857, 245272b3d10000ad3f32d8641a84c79e7dc51060}
    E const& Error() const&
    {
        E const* ptr = ara::core::get_if< E >(&mData_);
        if (ptr) {
            return *ptr;
        }
        ara::core::Abort("template <typename E>Result<void, E> E const& Error() const& called but NOT an error!\n");
    }

    /// @uptrace{SWS_CORE_00858, a9452d4815bd7eef97f26604ae50363ef707b719}
    E&& Error() &&
    {
        E* ptr = ara::core::get_if< E >(&mData_);
        if (ptr) {
            return std::move(*ptr);
        }
        ara::core::Abort("template <typename E>Result<void, E> E&& Error() && called but NOT an error!\n");
    }

    /// @uptrace{SWS_CORE_00868, cff668f836dd658dde543d309e2dabeee1f5dfb2}
    Optional< E > Err() const&  // noexcept
    {
        return HasValue() ? nullopt : Optional< E >(Error());
    }

    /// @uptrace{SWS_CORE_00869, f8b76bf1faefb7937244348081ba24f3305f108d}
    Optional< E > Err() &&  // noexcept
    {
        return HasValue() ? nullopt : Optional< E >(std::move(Error()));
    }

    /// @uptrace{SWS_CORE_00861, 8f9aaa93ae4d5fa9b960c722592f01420483e582}
    template < typename U >
    void ValueOr(U&& defaultValue) const
    {
        return HasValue() ? Value() : static_cast< void >(std::forward< U >(defaultValue));
    }

    /// @uptrace{SWS_CORE_00863, 6977f06e1f4dc4331d87749ff2033f0a7be44b44}
    template < typename G >
    E ErrorOr(G&& defaultError) const&
    {
        return HasValue() ? static_cast< E >(std::forward< G >(defaultError)) : Error();
    }

    /// @uptrace{SWS_CORE_00864, f163c3ce70a531559f24541ecd01dae7e45b6da3}
    template < typename G >
    E ErrorOr(G&& defaultError) &&
    {
        return HasValue() ? static_cast< E >(std::forward< G >(defaultError)) : std::move(Error());
    }

    /// @uptrace{SWS_CORE_00865, 4ceefa409ecbd05250256126adebea8ea08c4828}
    template < typename G >
    bool CheckError(G&& error) const
    {
        return HasValue() ? false : (Error() == static_cast< E >(std::forward< G >(error)));
    }

#ifndef ARA_NO_EXCEPTIONS
    /// @uptrace{SWS_CORE_00866, ded926bd75940b0d0c0a17510fd7723917cc6752}
    void ValueOrThrow() const noexcept(false)
    {
        T const* ptr = ara::core::get_if< T >(&mData_);
        if (ptr) {
            return;
        }

        E const* e = ara::core::get_if< E >(&mData_);
        e->ThrowAsException();
    }
#endif

    /// @uptrace{SWS_CORE_00867, c52ed05827340a3d53812d462b5cbb74788f0c8e}
    template < typename F >
    void Resolve(F&& f) const
    {
        return HasValue() ? Value() : std::forward< F >(f)(Error());
    }

private:
    // Re-implementation of C++14's std::enable_if_t
    template < bool Condition, typename U = void >                      // NOLINT : keep template parameter naming style
    using enable_if_t = typename std::enable_if< Condition, U >::type;  // NOLINT

    // Re-implementation of C++14's std::result_of_t
    template < typename U >
    using result_of_t = typename std::result_of< U >::type;  // NOLINT

    /// @brief Trait that detects whether a type is a Result<...>
    template < typename U >
    struct is_result : std::false_type  // NOLINT
    {
    };

    template < typename U, typename G >
    struct is_result< Result< U, G > > : std::true_type
    {
    };

    template < typename F >
    using CallableReturnsResult = enable_if_t< is_result< result_of_t< F() > >::value >;

    template < typename F >
    using CallableReturnsNoResult = enable_if_t< !is_result< result_of_t< F() > >::value >;

public:
    /// @uptrace{SWS_CORE_00870, 2e52f559b48982f891c166b25bef5f27b4699ca3}
    template < typename F, typename = CallableReturnsResult< F > >
    auto Bind(F&& f) const -> decltype(f())
    {
        using U = decltype(f());
        return HasValue() ? std::forward< F >(f)() : U(Error());
    }

    /// @uptrace{SWS_CORE_00870, 2e52f559b48982f891c166b25bef5f27b4699ca3}
    template < typename F, typename = CallableReturnsNoResult< F > >
    auto Bind(F&& f) const -> Result< decltype(f()), E >
    {
        using U = decltype(f());
        using R = Result< U, E >;
        return HasValue() ? R{std::forward< F >(f)()} : R(Error());
    }

    template < typename, typename >
    friend class Result;
};

}  // namespace core
}  // namespace ara

#endif  // ARA_CORE_RESULT_H_
