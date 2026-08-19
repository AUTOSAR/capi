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
/// @file       format.h
/// @brief      format header file
/// @details
/// @date       2022-01-06
/// @author     mazelin
/// @version    1.2.0
///
/// ================================================================

#pragma once

#include <cstdint>
#include <list>
#include <memory>
#include <set>
#include <tuple>
#include <unordered_map>
#include <unordered_set>

#include "ara/core/array.h"
#include "ara/core/map.h"
#include "ara/core/optional.h"
#include "ara/core/result.h"
#include "ara/core/string.h"
#include "ara/core/vector.h"

#define FmtNM              ara::com::internal::format
#define FmtMode            FmtNM::FormatMode
#define FmtStr(...)        FmtNM::Format< ara::core::String >{}(__VA_ARGS__)
#define GenKey(...)        FmtNM::KeyPrefix(), ##__VA_ARGS__, FmtNM::KeySuffix()
#define GenVal(...)        FmtNM::ValPrefix(), ##__VA_ARGS__, FmtNM::ValSuffix()
#define GenK2V0(k, ...)    GenKey(k), FmtNM::PairInfix(), GenVal(__VA_ARGS__)
#define GenK2V(...)        FmtNM::ArgsInfix(), GenK2V0(__VA_ARGS__)
#define GenArg0(v)         GenK2V0(#v, v)
#define GenArg(v)          FmtNM::ArgsInfix(), GenArg0(v)
#define GenMode(mode, ...) FmtNM::ModePrefix(mode), __VA_ARGS__, FmtNM::ModeSuffix()

namespace ara {
namespace com {
namespace internal {
namespace format {
/// @brief FormatMode
enum class FormatMode : uint8_t
{
    kBlack,
    kRed,
    kGreen,
    kYellow,
    kBlue,
    kMagenta,
    kCyan,
    kWhite,
    kNone
};
/// @brief EnableColor
/// @param[in] enable
/// @return bool
bool EnableColor(bool enable) noexcept;
/// @brief KeyPrefix
/// @return char const*
char const* KeyPrefix() noexcept;
/// @brief KeySuffix
/// @return char const*
char const* KeySuffix() noexcept;
/// @brief ValPrefix
/// @return char const*
char const* ValPrefix() noexcept;
/// @brief ValSuffix
/// @return char const*
char const* ValSuffix() noexcept;
/// @brief PairInfix
/// @return char const*
char const* PairInfix() noexcept;
/// @brief ArgsInfix
/// @return char const*
char const* ArgsInfix() noexcept;
/// @brief ModePrefix
/// @param[in] mode
/// @return char const*
char const* ModePrefix(FormatMode mode) noexcept;
/// @brief ModeSuffix
/// @return char const*
char const* ModeSuffix() noexcept;

template < typename... >
using VoidType = void;
template < typename T, typename R, typename = void >
struct EnableStream : std::false_type
{
};
template < typename T, typename R >
struct EnableStream< T, R, VoidType< decltype(std::declval< R& >() << std::declval< T >()) > > : std::true_type
{
};
template < typename T, typename R, typename = void >
struct EnableByte : std::false_type
{
};
template < typename T, typename R >
struct EnableByte<
    T,
    R,
    VoidType< decltype(std::declval< R >().append(std::declval< T >().data(), std::declval< T >().size())) > >
    : std::true_type
{
};
template < typename R, typename V >
inline R& Append(R& r, V&& v, std::enable_if_t< EnableStream< V, R >::value >* = nullptr) noexcept
{
    return r << std::forward< V >(v);
}
template < typename R, typename V >
inline R& Append(R& r,
                 V&& v,
                 std::enable_if_t< !EnableStream< V, R >::value && !EnableByte< V, R >::value >* = nullptr) noexcept
{
    return r += v;
}
template < typename R, typename V >
inline R& Append(R& r,
                 V&& v,
                 std::enable_if_t< !EnableStream< V, R >::value && EnableByte< V, R >::value >* = nullptr) noexcept
{
    return r.append(v.data(), v.size());
}

/// @brief Formatter
/// @tparam R
/// @tparam V
template < typename R, typename V, typename = void >
struct Formatter;
/// @brief FormatToString
/// @tparam T
/// @param[in, out] r
/// @return R&
template < typename R >
inline R& FormatToString(R& r) noexcept
{
    return r;
}
/// @brief FormatToString
/// @tparam R
/// @tparam V
/// @param[in, out] r
/// @param[in] v
/// @return R&
template < typename R, typename V >
inline R& FormatToString(R& r, V const& v) noexcept
{
    return Formatter< R, V >::ToString(r, v);
}
/// @brief FormatToString
/// @tparam R
/// @tparam V
/// @tparam Vs
/// @param[in, out] r
/// @param[in] v
/// @param[in] vs
/// @return R&
template < typename R, typename V, typename... Vs >
inline R& FormatToString(R& r, V const& v, Vs&&... vs) noexcept
{
    return FormatToString(Formatter< R, V >::ToString(r, v), std::forward< Vs >(vs)...);
}
/// @brief Format
/// @tparam R
/// @tparam Vs
/// @param[in] vs
/// @return R
template < typename R >
struct Format
{
    R r;
    operator R&() noexcept { return r; }  // NOLINT -- design>[google-explicit-constructor]
    template < typename... Vs >
    inline Format& operator()(Vs&&... vs) noexcept
    {
        std::ignore = FormatToString(r, std::forward< Vs >(vs)...);
        return *this;
    }
};
/// @brief Formatter
/// @tparam R
template < typename R >
struct Formatter< R, Format< R > >
{
    /// @brief ToString
    /// @param[in, out] r
    /// @param[in] v
    /// @return R&
    static R& ToString(R& r, Format< R > const& v) noexcept { return FormatToString(r, v.r); }
};
/// @brief EnableToString
/// @tparam R
/// @tparam V
template < typename R, typename V, typename = void >
struct EnableToString : std::false_type
{
};
/// @brief EnableToString
/// @tparam R
/// @tparam V
template < typename R, typename V >
struct EnableToString<
    R,
    V,
    std::enable_if_t<
        std::is_same< R&, decltype(Formatter< R, V >::ToString(std::declval< R& >(), std::declval< V >())) >::value > >
    : std::true_type
{
};
/// @brief HasFunToString
/// @tparam R
/// @tparam V
template < typename R, typename V, typename = void >
struct HasFunToString : std::false_type
{
};
/// @brief HasFunToString
/// @tparam R
/// @tparam V
template < typename R, typename V >
struct HasFunToString<
    R,
    V,
    std::enable_if_t<
        !std::is_same< void, decltype(std::declval< V >().ToString()) >::value
        && std::is_same< R&, decltype(std::declval< R >().append(std::declval< V >().ToString())) >::value > >
    : std::true_type
{
};
/// @brief Formatter
/// @tparam R
/// @tparam V
template < typename R, typename V >
struct Formatter< R, V, std::enable_if_t< HasFunToString< R, V >::value > >
{
    /// @brief ToString
    /// @return
    /// @param[in, out] r
    /// @param[in] v
    static R& ToString(R& r, V const& v) noexcept { return Append(r, v.ToString()); }
};
/// @brief HasTagToString
/// @tparam T
template < typename T, typename Tagged = void >
struct HasTagToString : std::false_type
{
};
/// @brief HasTagToString
/// @tparam T
template < typename T >
struct HasTagToString< T, typename T::TagToString > : std::true_type
{
};
/// @brief Formatter
/// @tparam R
/// @tparam V
template < typename R, typename V >
struct Formatter< R, V, std::enable_if_t< HasTagToString< V >::value > >
{
    /// @brief ToString
    /// @param[in, out] r
    /// @param[in] v
    /// @return R&
    static R& ToString(R& r, V const& v) noexcept { return v.ToString(r); }
};
/// @brief HasTagEnumerable
/// @tparam T
template < typename T, typename Tagged = void >
struct HasTagEnumerable : std::false_type
{
};
/// @brief HasTagEnumerable
/// @tparam T
template < typename T >
struct HasTagEnumerable< T, typename T::TagArgs > : std::true_type
{
};
/// @brief HasTagEnumerable
/// @tparam T
template < typename T >
struct HasTagEnumerable< T, typename T::IsArgsTag > : std::true_type
{
};
/// @brief HasTagEnumerable
/// @tparam T
template < typename T >
struct HasTagEnumerable< T, typename T::TagEnumerable > : std::true_type
{
};
/// @brief HasTagEnumerable
/// @tparam T
template < typename T >
struct HasTagEnumerable< T, typename T::IsEnumerableTag > : std::true_type
{
};
/// @brief HasTagEnumerable
/// @tparam T
template < typename T >
struct HasTagEnumerable< T, typename T::TagLengthField > : std::true_type
{
};
/// @brief HasTagEnumerable
/// @tparam T
template < typename T >
struct HasTagEnumerable< T, typename T::IsLengthFieldTag > : std::true_type
{
};
/// @brief HasTagEnumerable
/// @tparam T
template < typename T >
struct HasTagEnumerable< T, typename T::TagTLV > : std::true_type
{
};
/// @brief HasTagEnumerable
/// @tparam T
template < typename T >
struct HasTagEnumerable< T, typename T::IsTLVTag > : std::true_type
{
};
/// @brief HasTagEnumerable
/// @tparam T
template < typename T >
struct HasTagEnumerable< T, typename T::TagS2S > : std::true_type
{
};
/// @brief HasTagEnumerable
/// @tparam T
template < typename T >
struct HasTagEnumerable< T, typename T::IsS2STag > : std::true_type
{
};
/// @brief Formatter
/// @tparam R
/// @tparam V
template < typename R, typename V >
struct Formatter<
    R,
    V,
    std::enable_if_t< HasTagEnumerable< V >::value && !HasTagToString< V >::value && !HasFunToString< R, V >::value > >
{
    /// @brief ToString
    /// @param[in, out] r
    /// @param[in] v
    /// @return R&
    static R& ToString(R& r, V const& v) noexcept
    {
        Caller caller{r};
        const_cast< V& >(v).enumerate(caller);
        return r;
    }
    /// @brief Caller
    struct Caller
    {
        /// @brief operator()
        /// @tparam Vs
        /// @param[in] vs
        template < typename... Vs >
        void operator()(Vs&&... vs) noexcept
        {
            if (++times > 1) {
                std::ignore = FormatToString(r, ArgsInfix());
            }
            FormatToString(r, GenKey(".", times), PairInfix());
            std::ignore = std::initializer_list< int32_t >{(FormatToString(r, GenVal(std::forward< Vs >(vs))), 0)...};
        }
        R& r;
        int32_t times{};
    };
};
/// @brief Formatter
/// @tparam R
/// @tparam V
template < typename R, typename V >
struct Formatter< R,
                  V,
                  std::enable_if_t< std::is_same< V, R >::value || std::is_same< V, std::string >::value
                                    || std::is_same< V, ara::core::String >::value
                                    || std::is_same< V, ara::core::StringView >::value > >
{
    /// @brief ToString
    /// @param[in, out] r
    /// @param[in] v
    /// @return R&
    static R& ToString(R& r, V const& v) noexcept { return Append(r, v); }
};
/// @brief Formatter
/// @tparam R
template < typename R >
struct Formatter< R, bool >
{
    /// @brief ToString
    /// @param[in, out] r
    /// @param[in] v
    /// @return R&
    static R& ToString(R& r, bool v) noexcept { return Append(r, v ? "<true>" : "<false>"); }
};
/// @brief Formatter
/// @tparam R
/// @tparam V
template < typename R, typename V >
struct Formatter< R, V, std::enable_if_t< std::is_enum< V >::value > >
{
    /// @brief ToString
    /// @param[in, out] r
    /// @param[in] v
    /// @return R&
    static R& ToString(R& r, V const& v) noexcept { return Append(r, std::to_string(static_cast< int32_t >(v))); }
};
/// @brief Formatter
/// @tparam R
/// @tparam V
template < typename R, typename V >
struct Formatter< R, V, std::enable_if_t< std::is_same< V, char >::value || std::is_same< V, int8_t >::value > >
{
    /// @brief ToString
    /// @param[in, out] r
    /// @param[in] v
    /// @return R&
    static R& ToString(R& r, V v) noexcept { return r += v; }
};
/// @brief Formatter
/// @tparam R
/// @tparam V
template < typename R, typename V >
struct Formatter< R,
                  V,
                  std::enable_if_t< std::is_same< V, int16_t >::value || std::is_same< V, int32_t >::value
                                    || std::is_same< V, int64_t >::value || std::is_same< V, float >::value
                                    || std::is_same< V, double >::value > >
{
    /// @brief ToString
    /// @param[in, out] r
    /// @param[in] v
    /// @return R&
    static R& ToString(R& r, V v) noexcept { return Append(r, std::to_string(v)); }
};
/// @brief Formatter
/// @tparam R
/// @tparam V
template < typename R, typename V >
struct Formatter< R,
                  V,
                  std::enable_if_t< std::is_same< V, uint8_t >::value || std::is_same< V, uint16_t >::value
                                    || std::is_same< V, uint32_t >::value || std::is_same< V, uint64_t >::value > >
{
    /// @brief ToString
    /// @param[in, out] r
    /// @param[in] v
    /// @return R&
    static R& ToString(R& r, V v) noexcept
    {
        auto constexpr kFormat{sizeof(V) == 1   ? "%u/0x%02x"
                               : sizeof(V) == 2 ? "%u/0x%04x"
                               : sizeof(V) == 4 ? "%u/0x%08x"
                                                : "%lu/0x%016lx"};
        auto constexpr kSize{sizeof(V) == 1   ? (3 + 3 + 2 + 1)
                             : sizeof(V) == 2 ? (5 + 3 + 4 + 1)
                             : sizeof(V) == 4 ? (10 + 3 + 8 + 1)
                                              : (20 + 3 + 16 + 1)};
        char buffer[kSize];
        snprintf(buffer, sizeof(buffer), kFormat, v, v);
        return Append(r, buffer);
    }
};
namespace internal {
/// @brief
template < typename T >
struct IsCArray : std::false_type
{
};
#if 0
/// @brief
template < typename T >
struct IsCArray< T[0] > : std::true_type
{
};
/// @brief
template < typename T >
struct IsCArray< T (&)[0] > : std::true_type
{
};
/// @brief
template < typename T >
struct IsCArray< T(&&)[0] > : std::true_type
{
};
#endif
/// @brief
template < typename T, size_t kN >
struct IsCArray< T[kN] > : std::true_type
{
};
/// @brief
template < typename T, size_t kN >
struct IsCArray< T (&)[kN] > : std::true_type
{
};
/// @brief
template < typename T, size_t kN >
struct IsCArray< T(&&)[kN] > : std::true_type
{
};
}  // namespace internal
/// @brief IsCArray
/// @tparam T
template < typename T >
struct IsCArray : internal::IsCArray< std::remove_cv_t< T > >
{
};
/// @brief Formatter
/// @tparam R
/// @tparam V
template < typename R, typename V >
struct Formatter< R, V, std::enable_if_t< IsCArray< V >::value > >
{
    /// @brief ToString
    /// @tparam U
    /// @param[in, out] r
    /// @param[in] v
    /// @return R&
    template < typename U = std::decay_t< decltype(*std::declval< V >()) > >
    static R& ToString(R& r, V const& v, std::enable_if_t< std::is_same< U, char >::value >* = nullptr) noexcept
    {
        constexpr auto kN{sizeof(v) / sizeof(v[0])};
        return kN > 0 ? Append(r, v) : FormatToString(r, GenK2V0("&", static_cast< int32_t >(kN)));
    }
    /// @brief ToString
    /// @tparam U
    /// @param[in, out] r
    /// @param[in] v
    /// @return R&
    template < typename U = std::decay_t< decltype(*std::declval< V >()) > >
    static R& ToString(R& r, V const& v, std::enable_if_t< !std::is_same< U, char >::value >* = nullptr) noexcept
    {
        constexpr auto kN{sizeof(v) / sizeof(v[0])};
        FormatToString(r, GenK2V0("&", static_cast< int32_t >(kN)));
        for (auto n{decltype(kN)(0)}; n < kN; ++n) {
            FormatToString(r, GenK2V(static_cast< int32_t >(n), v[n]));
        }
        return r;
    }
};
/// @brief IsArray
/// @tparam T
template < typename T >
struct IsArray : std::false_type
{
};
/// @brief IsArray
/// @tparam T
template < typename T, size_t kN >
struct IsArray< ara::core::Array< T, kN > > : std::true_type
{
};
/// @brief Formatter
/// @tparam R
/// @tparam V
template < typename R, typename V >
struct Formatter< R, V, std::enable_if_t< IsArray< V >::value > >
{
    /// @brief ToString
    /// @param[in, out] r
    /// @param[in] v
    /// @return R&
    static R& ToString(R& r, V const& v) noexcept
    {
        constexpr auto kN{sizeof(v) / sizeof(v[0])};
        FormatToString(r, GenK2V0("array", static_cast< int32_t >(kN)));
        for (auto n{decltype(kN)(0)}; n < kN; ++n) {
            FormatToString(r, GenK2V(static_cast< int32_t >(n), v[n]));
        }
        return r;
    }
};
namespace internal {
/// @brief
template < typename T >
struct IsCPointer : std::false_type
{
};
/// @brief
template < typename T >
struct IsCPointer< T* > : std::true_type
{
};
/// @brief
template < typename T >
struct IsCPointer< T*& > : std::true_type
{
};
/// @brief
template < typename T >
struct IsCPointer< T*&& > : std::true_type
{
};
/// @brief
template < typename T >
struct IsCPointer< T* const > : std::true_type
{
};
/// @brief
template < typename T >
struct IsCPointer< T* const& > : std::true_type
{
};
/// @brief
template < typename T >
struct IsCPointer< T* const&& > : std::true_type
{
};
}  // namespace internal
/// @brief IsCPointer
/// @tparam T
template < typename T >
struct IsCPointer : internal::IsCPointer< std::remove_cv_t< T > >
{
};
/// @brief Formatter
/// @tparam R
/// @tparam V
template < typename R, typename V >
struct Formatter< R, V, std::enable_if_t< IsCPointer< V >::value > >
{
    /// @brief ToString
    /// @tparam U
    /// @param[in, out] r
    /// @param[in] v
    /// @return R&
    template < typename U = std::decay_t< decltype(*std::declval< V >()) > >
    static R& ToString(R& r, V const& v, std::enable_if_t< std::is_same< U, char >::value >* = nullptr) noexcept
    {
        return v ? Append(r, v) : FormatToString(r, GenK2V0("*", "<nullptr>"));
    }
    /// @brief ToString
    /// @tparam U
    /// @param[in, out] r
    /// @param[in] v
    /// @return R&
    template < typename U = std::decay_t< decltype(*std::declval< V >()) > >
    static R& ToString(
        R& r,
        V const& v,
        std::enable_if_t< !std::is_same< U, char >::value && EnableToString< R, U >::value >* = nullptr) noexcept
    {
        return v ? FormatToString(r, GenK2V0("*", *v)) : FormatToString(r, GenK2V0("*", "<nullptr>"));
    }
    /// @brief ToString
    /// @tparam U
    /// @param[in, out] r
    /// @param[in] v
    /// @return R&
    template < typename U = std::decay_t< decltype(*std::declval< V >()) > >
    static R& ToString(
        R& r,
        V const& v,
        std::enable_if_t< !std::is_same< U, char >::value && !EnableToString< R, U >::value >* = nullptr) noexcept
    {
        return v ? FormatToString(r, GenK2V0("*", "<", reinterpret_cast< uintptr_t >(v), ">"))
                 : FormatToString(r, GenK2V0("*", "<nullptr>"));
    }
};
/// @brief Formatter
/// @tparam R
template < typename R >
struct Formatter< R, std::nullptr_t >
{
    /// @brief ToString
    /// @param[in, out] r
    /// @param[in] v
    /// @return R&
    static R& ToString(R& r, std::nullptr_t v) noexcept
    {
        std::ignore = v;
        return FormatToString(r, GenK2V0("*", "<nullptr>"));
    }
};
/// @brief IsContainer
/// @tparam T
/// @tparam C
template < typename T, template < typename... > class C >
struct IsContainer : std::false_type
{
};
/// @brief IsContainer
/// @tparam T
/// @tparam C
template < typename... T, template < typename... > class C >
struct IsContainer< C< T... >, C > : std::true_type
{
};
/// @brief Formatter
/// @tparam R
/// @tparam V
template < typename R, typename V >
struct Formatter< R,
                  V,
                  std::enable_if_t< IsContainer< V, std::weak_ptr >::value || IsContainer< V, std::shared_ptr >::value
                                    || IsContainer< V, std::unique_ptr >::value > >
{
    /// @brief ToString
    /// @param[in, out] r
    /// @param[in] v
    /// @return R&
    static R& ToString(R& r, V const& v) noexcept { return FormatToString(r, v.get()); }
};
/// @brief Formatter
/// @tparam R
/// @tparam V
template < typename R, typename V >
struct Formatter< R, V, std::enable_if_t< IsContainer< V, std::pair >::value > >
{
    /// @brief ToString
    /// @param[in, out] r
    /// @param[in] v
    /// @return R&
    static R& ToString(R& r, V const& v) noexcept { return FormatToString(r, GenK2V0(v.first, v.second)); }
};
/// @brief Formatter
/// @tparam R
/// @tparam V
template < typename R, typename V >
struct Formatter< R,
                  V,
                  std::enable_if_t< IsContainer< V, std::list >::value || IsContainer< V, std::vector >::value
                                    || IsContainer< V, ara::core::Vector >::value > >
{
    /// @brief ToString
    /// @param[in, out] r
    /// @param[in] v
    /// @return R&
    static R& ToString(R& r, V const& v) noexcept
    {
        if (IsContainer< V, std::list >::value) {
            std::ignore = FormatToString(r, GenK2V0("list", static_cast< int32_t >(v.size())));
        } else if (IsContainer< V, std::vector >::value || IsContainer< V, ara::core::Vector >::value) {
            std::ignore = FormatToString(r, GenK2V0("vector", static_cast< int32_t >(v.size())));
        }
        auto n{decltype(v.size())(0)};
        for (auto it{v.begin()}; it != v.end(); ++n, ++it) {
            std::ignore = FormatToString(r, GenK2V(static_cast< int32_t >(n), *it));
        }
        return r;
    }
};
/// @brief Formatter
/// @tparam R
/// @tparam V
template < typename R, typename V >
struct Formatter< R,
                  V,
                  std::enable_if_t< IsContainer< V, std::set >::value || IsContainer< V, std::unordered_set >::value > >
{
    /// @brief ToString
    /// @param[in, out] r
    /// @param[in] v
    /// @return R&
    static R& ToString(R& r, V const& v) noexcept
    {
        if (IsContainer< V, std::set >::value) {
            FormatToString(r, GenK2V0("set", static_cast< int32_t >(v.size())));
        } else if (IsContainer< V, std::unordered_set >::value) {
            FormatToString(r, GenK2V0("unordered_set", static_cast< int32_t >(v.size())));
        }
        for (auto it{v.begin()}; it != v.end(); ++it) {
            FormatToString(r, ArgsInfix(), GenVal(*it));
        }
        return r;
    }
};
/// @brief Formatter
/// @tparam R
/// @tparam V
template < typename R, typename V >
struct Formatter< R,
                  V,
                  std::enable_if_t< IsContainer< V, std::map >::value || IsContainer< V, ara::core::Map >::value
                                    || IsContainer< V, std::unordered_map >::value > >
{
    /// @brief ToString
    /// @param[in, out] r
    /// @param[in] v
    /// @return R&
    static R& ToString(R& r, V const& v) noexcept
    {
        if (IsContainer< V, std::map >::value || IsContainer< V, ara::core::Map >::value) {
            FormatToString(r, GenK2V0("map", static_cast< int32_t >(v.size())));
        } else if (IsContainer< V, std::unordered_map >::value) {
            FormatToString(r, GenK2V0("unordered_map", static_cast< int32_t >(v.size())));
        }
        for (auto it{v.begin()}; it != v.end(); ++it) {
            FormatToString(r, GenK2V(it->first, it->second));
        }
        return r;
    }
};
/// @brief Formatter
/// @tparam R
/// @tparam V
template < typename R, typename V >
struct Formatter< R, V, std::enable_if_t< IsContainer< V, ara::core::Optional >::value > >
{
    /// @brief ToString
    /// @param[in, out] r
    /// @param[in] v
    /// @return R&
    static R& ToString(R& r, V const& v) noexcept
    {
        return v ? FormatToString(r, GenK2V0("*", *v)) : FormatToString(r, GenK2V0("*", "<nullopt>"));
    }
};
/// @brief Formatter
/// @tparam R
/// @tparam V
template < typename R, typename V >
struct Formatter< R, V, std::enable_if_t< IsContainer< V, std::tuple >::value > >
{
    /// @brief ToString
    /// @tparam T
    /// @param[in, out] r
    /// @param[in] v
    /// @return R&
    template < typename... T >
    static R& ToString(R& r, std::tuple< T... > const& v) noexcept
    {
        constexpr auto kN{sizeof...(T)};
        FormatToString(r, GenK2V0("tuple", static_cast< int32_t >(kN)));
        if (kN > 0) {
            ToString(r, v, std::index_sequence_for< T... >{});
        }
        return r;
    }
    /// @brief ToString
    /// @tparam T
    /// @tparam kI
    /// @param[in, out] r
    /// @param[in] v`
    /// @return R&
    template < typename... T, size_t... kI >
    static void ToString(R& r, std::tuple< T... > const& v, std::index_sequence< kI... >) noexcept
    {
        std::ignore = std::initializer_list< int32_t >{
            (FormatToString(r, GenK2V(static_cast< int32_t >(kI), std::get< kI >(v))), 0)...};
    }
};
/// @brief Formatter
/// @tparam R
/// @tparam Ts
template < typename R, typename... Ts >
struct Formatter< R, ara::core::Variant< Ts... > >
{
    struct Visitor
    {
        template < typename T >
        void operator()(T const& v) noexcept
        {
            FormatToString(r, GenK2V0("variant", v));
        }
        R& r;
    };
    /// @brief ToString
    /// @param[in, out] r
    /// @param[in] v
    /// @return R&
    static R& ToString(R& r, ara::core::Variant< Ts... > const& v) noexcept
    {
        ara::core::visit(Visitor{r}, v);
        return r;
    }
};
/// @brief Formatter
/// @tparam R
template < typename R >
struct Formatter< R, ara::core::ErrorDomain >
{
    /// @brief ToString
    /// @param[in, out] r
    /// @param[in] v
    /// @return R&
    static R& ToString(R& r, ara::core::ErrorDomain const& v) noexcept
    {
        return FormatToString(r, GenK2V0(v.Id(), v.Name()));
    }
};
/// @brief Formatter
/// @tparam R
template < typename R >
struct Formatter< R, ara::core::ErrorCode >
{
    /// @brief ToString
    /// @param[in, out] r
    /// @param[in] v
    /// @return R&
    static R& ToString(R& r, ara::core::ErrorCode const& v) noexcept
    {
        return FormatToString(r, GenK2V0("error", v.Value()), GenK2V("message", v.Message()),
                              GenK2V("support data", v.SupportData()));
    }
};
/// @brief Formatter
/// @tparam R
/// @tparam T
/// @tparam E
template < typename R, typename T, typename E >
struct Formatter< R, ara::core::Result< T, E > >
{
    /// @brief ToString
    /// @tparam U
    /// @tparam c
    /// @param[in, out] r
    /// @param[in] v
    /// @return R&
    template < typename U = T, bool c = std::is_same< U, void >::value >
    static R& ToString(R& r, ara::core::Result< T, E > const& v, std::enable_if_t< !c >* = nullptr) noexcept
    {
        if (!v) {
            return FormatToString(r, GenK2V0("result", v.Error()));
        }
        return FormatToString(r, GenK2V0("result", v.Value()));
    }
    /// @brief ToString
    /// @tparam U
    /// @tparam c
    /// @param[in, out] r
    /// @param[in] v
    /// @return R&
    template < typename U = T, bool c = std::is_same< U, void >::value >
    static R& ToString(R& r, ara::core::Result< T, E > const& v, std::enable_if_t< c >* = nullptr) noexcept
    {
        if (!v) {
            return FormatToString(r, GenK2V0("result", v.Error()));
        }
        return FormatToString(r, GenK2V0("result", ""));
    }
};
}  // namespace format
}  // namespace internal
}  // namespace com
}  // namespace ara
