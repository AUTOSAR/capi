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
/// @file       optional.h
/// @brief      the definition and implementation of ara::core::Optional that provides access to optional record elements of a Structure Implementation data type.
/// @details
/// @date       2022-06-27
/// @author     hanzhibo
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_CORE_OPTIONAL_H_
#define ARA_CORE_OPTIONAL_H_

#include <functional>
#include <initializer_list>
#include <memory>
#include <type_traits>
#include <utility>

#include "ara/core/exception.h"
#include "ara/core/utility.h"

namespace ara {
namespace core {

// optional for object types
template < class T >
class Optional;

// Disengaged state indicator
struct nullopt_t  // NOLINT
{
    struct placeholder  // NOLINT
    {
    };
    explicit constexpr nullopt_t(placeholder) {}
};
constexpr nullopt_t nullopt{nullopt_t::placeholder()};  // NOLINT

namespace  // unnamed  // NOLINT
{
template < typename T >
struct is_optional : std::false_type  // NOLINT
{
};
template < typename T >
struct is_optional< Optional< T > > : std::true_type
{
};

template < typename U, typename T >
using EnableIfConvertible = typename std::enable_if< std::is_convertible< U, T >::value >::type;
template < typename T >
using EnableIfCopyConstructible = typename std::enable_if< std::is_copy_constructible< T >::value >::type;
template < typename T >
using EnableIfMoveConstructible = typename std::enable_if< std::is_move_constructible< T >::value >::type;
template < typename T >
using EnableIfNotOptional = typename std::enable_if< !is_optional< typename std::decay< T >::type >::value >::type;
template < typename T >
using EnableIfLValueReference = typename std::enable_if< std::is_lvalue_reference< T >::value >::type;
template < typename T >
using EnableIfNotLValueReference = typename std::enable_if< !std::is_lvalue_reference< T >::value >::type;
template < typename T, typename... Args >
using EnableIfConstructible = typename std::enable_if< std::is_constructible< T, Args... >::value >::type;
}  // unnamed namespace

///
/// @brief this class provides access to optional record elements of a Structure Implementation data type.
///
/// @tparam T the type of optional values, i.e. values that may or may not be present.
///
/// @uptrace{SWS_CORE_01033, 49a318963aac542f6c9ad903c38b0f961663622e}
/// @uptrace{SWS_CORE_01030, bd5160c051d429126929750c1365878731204e8d}
/// @uptrace{SWS_CORE_01031, a75ea445a6b8c2e23ef93468cbc4779c6dfdc378}
///
template < class T >
class Optional
{
public:
    typedef T value_type;  // NOLINT

    // X.Y.4.1, constructors
    /**
     * @brief Default constructor
     */
    constexpr Optional() noexcept = default;
    constexpr Optional(nullopt_t) noexcept : bInit_{false}, ptrVal_{nullptr} {}  // NOLINT
    // Delete the predefined copy-constructor (for volatile, since we don't need it).
    // Doing so prevents another copy constructor from being implicitly declared.
    // This will allow us to SFINAE-in the copy-constructor when needed.
    Optional(const volatile Optional&) = delete;

    template < typename T_ = T, typename = EnableIfCopyConstructible< T_ > >  // NOLINT
    Optional(const Optional< T >& other) // NOLINT : explicit will cause many code compilation errors
        : bInit_(other.bInit_), ptrVal_(other.bInit_ ? new T(*other.ptrVal_) : nullptr)
    {
    }
    // Delete the predefined move-constructor (for volatile, since we don't need it).
    // Doing so prevents another move constructor from being implicitly declared.
    // This will allow us to SFINAE-in the move-constructor when needed.
    Optional(volatile Optional&&) = delete;

    template < typename T_ = T, typename = EnableIfMoveConstructible< T_ > >  // NOLINT
    Optional(Optional< T >&& other) noexcept( // NOLINT : explicit will cause many code compilation errors
        std::is_nothrow_move_constructible< T >::value)
        : bInit_(other.bInit_), ptrVal_(std::move(other.ptrVal_))
    {
    }
    // Delete the predefined copy-constructor (for volatile, since we don't need it).
    // Doing so prevents another copy constructor from being implicitly declared.
    // This will allow us to SFINAE-in the copy-constructor when needed.
    Optional(const volatile T&) = delete;
    template < typename T_ = T, typename = EnableIfCopyConstructible< T_ > >  // NOLINT
    constexpr Optional(const T& v) // NOLINT : explicit will cause many code compilation errors
        : bInit_(true), ptrVal_(new T(v))
    {
    }
    // Delete the predefined move-constructor (for volatile, since we don't need it).
    // Doing so prevents another move constructor from being implicitly declared.
    // This will allow us to SFINAE-in the move-constructor when needed.
    Optional(volatile T&&) = delete;
    // template <typename T_ = T, typename = EnableIfMoveConstructible<T_>> modified by hanzhibo
    // constexpr Optional(T&& v)
    //     : bInit_(true)
    //     , ptrVal_(new T(std::move(v)))
    // {}
    // This constructor does not participate in overload resolution unless std::is_constructible_v<T, U&&> is true and
    // std::decay_t<U> (until C++20)std::remove_cvref_t<U> (since C++20) is neither std::in_place_t nor
    // std::optional<T>.
    template < typename U = T,
               typename   = typename std::enable_if< std::is_constructible< T, U&& >::value
                                                   && !std::is_same< typename std::decay< U >::type, in_place_t >::value
                                                   && !is_optional< typename std::decay< U >::type >::value >::type >
    constexpr Optional(U&& value) // NOLINT : explicit will cause many code compilation errors
        : bInit_(true), ptrVal_(new T(std::move(value)))
    {
    }

    template < class... Args, typename T_ = T, typename = EnableIfConstructible< T_, Args&&... > >  // NOLINT
    constexpr explicit Optional(in_place_t, Args&&... args)
        : bInit_(true), ptrVal_(new T(std::forward< Args >(args)...))
    {
    }
    template < class U,
               class... Args,
               typename T_ = T,  // NOLINT
               typename    = EnableIfConstructible< T_, std::initializer_list< U >, Args&&... > >
    constexpr explicit Optional(in_place_t, std::initializer_list< U > il, Args&&... args)
        : bInit_(true), ptrVal_(new T(il, std::forward< Args >(args)...))
    {
    }
    // X.Y.4.2, destructor
    ~Optional() = default;

    // X.Y.4.3, assignment
    Optional< T >& operator=(nullopt_t) noexcept
    {
        bInit_ = false;
        ptrVal_.reset(nullptr);
        return *this;
    }

    constexpr Optional< T >& operator=(const Optional< T >& rhs)  // NOLINT
    {
        bInit_ = rhs.bInit_;
        ptrVal_.reset(rhs.bInit_ ? new T(*rhs.ptrVal_) : nullptr);
        return *this;
    }

    constexpr Optional< T >& operator=(Optional< T >&& rhs) noexcept(
        std::is_nothrow_move_assignable< T >::value&& std::is_nothrow_move_constructible< T >::value)
    {
        bInit_ = rhs.bInit_;
        ptrVal_.reset(rhs.bInit_ ? new T(std::move(*rhs.ptrVal_)) : nullptr);
        return *this;
    }
    template < typename U = T,
               typename
               = typename std::enable_if< !is_optional< typename std::decay< U >::type >::value
                                          && std::is_constructible< T, U >::value && std::is_assignable< T&, U >::value
                                          && (!std::is_same< typename std::decay< U >::type, T >::value
                                              || !std::is_scalar< T >::value) >::type >
    Optional< T >& operator=(U&& rhs)
    {
        if (bInit_) {
            *ptrVal_ = std::forward< U >(rhs);
        } else {
            bInit_ = true;
            ptrVal_.reset(new T(std::forward< U >(rhs)));
        }
        return *this;
    }
    template < class... Args, typename T_ = T, typename = EnableIfConstructible< T_, Args&&... > >  // NOLINT
    T& emplace(Args&&... args)                                                                      // NOLINT
    {
        *this  = nullopt;
        bInit_ = true;
        ptrVal_.reset(new T(std::forward< Args >(args)...));
        return *ptrVal_;
    }
    template < class U,
               class... Args,
               typename T_ = T,  // NOLINT
               typename    = EnableIfConstructible< T_, std::initializer_list< U >, Args&&... > >
    T& emplace(std::initializer_list< U > il, Args&&... args)  // NOLINT
    {
        *this  = nullopt;
        bInit_ = true;
        ptrVal_.reset(new T(il, std::forward< Args >(args)...));
        return *ptrVal_;
    }

    void reset() noexcept  // NOLINT
    {
        bInit_ = false;
        ptrVal_.reset(nullptr);
    }

    // X.Y.4.4, swap
    template < typename T_ = T, typename = EnableIfMoveConstructible< T_ > >                           // NOLINT
    void swap(Optional< T >& rhs) noexcept(std::is_nothrow_move_constructible< T >::value&& noexcept(  // NOLINT
        std::swap(std::declval< T& >(), std::declval< T& >())))
    {
        std::swap(this->ptrVal_, rhs.ptrVal_);
        std::swap(bInit_, rhs.bInit_);
    }

    // X.Y.4.5, observers
    constexpr T const* operator->() const noexcept { return ptrVal_.get(); }
    constexpr T* operator->() noexcept { return ptrVal_.get(); }
    constexpr T const& operator*() const& noexcept { return *ptrVal_; }
    constexpr T& operator*() & noexcept { return *ptrVal_; }
    constexpr T const&& operator*() const&& noexcept { return std::move(*ptrVal_); }
    constexpr T&& operator*() && noexcept { return std::move(*ptrVal_); }

    constexpr bool has_value() const noexcept { return bInit_; }  // NOLINT

    constexpr explicit operator bool() const noexcept { return bInit_; }

    template < class U,
               typename T_ = T,  // NOLINT
               typename    = EnableIfCopyConstructible< T_ >,
               typename    = EnableIfConvertible< U&&, T_ > >
    constexpr T value_or(U&& v) const&  // NOLINT
    {
        return bool(*this) ? **this : static_cast< T >(std::forward< U >(v));
    }
    template < class U,
               typename T_ = T,  // NOLINT
               typename    = EnableIfMoveConstructible< T_ >,
               typename    = EnableIfConvertible< U&&, T_ > >
    constexpr T value_or(U&& v) &&  // NOLINT
    {
        return bool(*this) ? std::move(**this) : static_cast< T >(std::forward< U >(v));
    }

private:
    bool bInit_{false};
    std::unique_ptr< T > ptrVal_{nullptr};
};

// Relational operators
template < class T, class U >
constexpr bool operator==(const Optional< T >& x, const Optional< U >& y)
{
    return bool(x) != bool(y) ? false : (!bool(x) ? true : *x == *y);
}
template < class T, class U >
constexpr bool operator!=(const Optional< T >& x, const Optional< U >& y)
{
    return !(x == y);
}
template < class T, class U >
constexpr bool operator<(const Optional< T >& x, const Optional< U >& y)
{
    return !bool(y) ? false : (!bool(x) ? true : std::less< T >{}(*x, *y));
}
template < class T, class U >
constexpr bool operator>(const Optional< T >& x, const Optional< U >& y)
{
    return !(x < y) && !(x == y);
}
template < class T, class U >
constexpr bool operator<=(const Optional< T >& x, const Optional< U >& y)
{
    return (x < y) || (x == y);
}
template < class T, class U >
constexpr bool operator>=(const Optional< T >& x, const Optional< U >& y)
{
    return !(x < y);
}

// Comparison with nullopt
template < class T >
constexpr bool operator==(const Optional< T >& x, nullopt_t) noexcept
{
    return !bool(x);
}
template < class T >
constexpr bool operator==(nullopt_t, const Optional< T >& x) noexcept
{
    return !bool(x);
}
template < class T >
constexpr bool operator!=(const Optional< T >& x, nullopt_t) noexcept
{
    return !(x == nullopt);
}
template < class T >
constexpr bool operator!=(nullopt_t, const Optional< T >& x) noexcept
{
    return !(nullopt == x);
}
template < class T >
constexpr bool operator<(const Optional< T >&, nullopt_t) noexcept
{
    return false;
}
template < class T >
constexpr bool operator<(nullopt_t, const Optional< T >& x) noexcept
{
    return bool(x);
}
template < class T >
constexpr bool operator<=(const Optional< T >& x, nullopt_t) noexcept
{
    return (x < nullopt) || (x == nullopt);
}
template < class T >
constexpr bool operator<=(nullopt_t, const Optional< T >& x) noexcept
{
    return (nullopt < x) || (nullopt == x);
}
template < class T >
constexpr bool operator>(const Optional< T >& x, nullopt_t) noexcept
{
    return !(x < nullopt) && !(x == nullopt);
}
template < class T >
constexpr bool operator>(nullopt_t, const Optional< T >& x) noexcept
{
    return !(nullopt < x) && !(nullopt == x);
}
template < class T >
constexpr bool operator>=(const Optional< T >& x, nullopt_t) noexcept
{
    return !(x < nullopt);
}
template < class T >
constexpr bool operator>=(nullopt_t, const Optional< T >& x) noexcept
{
    return !(nullopt < x);
}

// Comparison with a value
template < class T, class U >
constexpr bool operator==(const Optional< T >& x, const U& v)
{
    return bool(x) ? *x == v : false;
}
template < class T, class U >
constexpr bool operator==(const T& v, const Optional< U >& x)
{
    return bool(x) ? v == *x : false;
}
template < class T, class U >
constexpr bool operator!=(const Optional< T >& x, const U& v)
{
    return !(x == v);
}
template < class T, class U >
constexpr bool operator!=(const T& v, const Optional< U >& x)
{
    return !(v == x);
}
template < class T, class U >
constexpr bool operator<(const Optional< T >& x, const U& v)
{
    return bool(x) ? std::less< T >{}(*x, v) : true;
}
template < class T, class U >
constexpr bool operator<(const T& v, const Optional< U >& x)
{
    return bool(x) ? std::less< T >{}(v, *x) : false;
}
template < class T, class U >
constexpr bool operator<=(const Optional< T >& x, const U& v)
{
    return (x < v) || (x == v);
}
template < class T, class U >
constexpr bool operator<=(const T& v, const Optional< U >& x)
{
    return (v < x) || (v == x);
}
template < class T, class U >
constexpr bool operator>(const Optional< T >& x, const U& v)
{
    return !(x < v) && !(x == v);
}
template < class T, class U >
constexpr bool operator>(const T& v, const Optional< U >& x)
{
    return !(v < x) && !(v == x);
}
template < class T, class U >
constexpr bool operator>=(const Optional< T >& x, const U& v)
{
    return !(x < v);
}
template < class T, class U >
constexpr bool operator>=(const T& v, const Optional< U >& x)
{
    return !(v < x);
}

// Specialized algorithms
/// @uptrace{SWS_CORE_01096, 1335307084b1b634ac04324d692d49cc66d61333}
template < class T >
void swap(Optional< T >& x, Optional< T >& y) noexcept(noexcept(x.swap(y)))  // NOLINT
{
    x.swap(y);
}
template < class T, typename = EnableIfNotLValueReference< T > >
constexpr Optional< typename std::decay< T >::type > make_optional(T&& v)  // NOLINT
{
    return Optional< typename std::decay< T >::type >(std::forward< T >(v));
}
template < class T, typename = EnableIfLValueReference< T > >
constexpr Optional< typename std::decay< T >::type > make_optional(  // NOLINT
    typename std::remove_reference< T >::type&& v)
{
    return Optional< typename std::decay< T >::type >(std::forward< typename std::remove_reference< T >::type >(v));
}

// added by hanzhibo
// template< class T, class... Args >
// constexpr Optional<T> make_optional( Args&&... args )
// {
//     return Optional<T>(in_place, std::forward<Args>(args)...);
// }
template < class T, class U, class... Args >
constexpr Optional< T > make_optional(std::initializer_list< U > il, Args&&... args)  // NOLINT
{
    return Optional< T >(in_place, il, std::forward< Args >(args)...);
}

// hash support
template < class T >
struct hash;
template < class T >
struct hash< Optional< T > >
{
    std::size_t operator()(const Optional< T >& o) const
    {
        if (bool(o)) {
            return std::hash< T >()(*o);
        }
        return 0;
    }
};

}  // namespace core
}  // namespace ara

#endif  // ARA_CORE_OPTIONAL_H_
