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
/// @file       string_view.h
/// @brief
/// @details
/// @date       2022-06-24
/// @author     hanzhibo
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_CORE_STRING_VIEW_H_
#define ARA_CORE_STRING_VIEW_H_

#include <cstddef>
#include <iostream>
#include <iterator>
#include <limits>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>

// explicit include order
#include <bits/functional_hash.h>

#include "ara/core/abort.h"

namespace ara {
namespace core {
namespace internal {

#ifdef __GNUC__
    #define APD_GCC_CONSTEXPR constexpr
#else
    #define APD_GCC_CONSTEXPR
#endif

template < typename CharT, typename Traits = std::char_traits< CharT > >
class basic_string_view  // NOLINT
{
public:
    using traits_type            = Traits;                                   // NOLINT
    using value_type             = CharT;                                    // NOLINT
    using pointer                = CharT*;                                   // NOLINT
    using const_pointer          = CharT const*;                             // NOLINT
    using reference              = CharT&;                                   // NOLINT
    using const_reference        = CharT const&;                             // NOLINT
    using const_iterator         = CharT const*;                             // NOLINT
    using iterator               = const_iterator;                           // NOLINT
    using const_reverse_iterator = std::reverse_iterator< const_iterator >;  // NOLINT
    using reverse_iterator       = const_reverse_iterator;                   // NOLINT
    using size_type              = std::size_t;                              // NOLINT
    using difference_type        = std::ptrdiff_t;                           // NOLINT

    constexpr static size_type npos = size_type(-1);  // NOLINT

    // Traits::length() only becomes 'constexpr' with C++17, so we use our own implementation
    // that is always 'constexpr' as long as gcc is being used.
    APD_GCC_CONSTEXPR static size_type length(CharT const* text) noexcept  // NOLINT
    {
#ifdef __GNUC__
        return __builtin_strlen(text);
#else
        return Traits::length(text);
#endif
    }

    // Traits::compare() only becomes 'constexpr' with C++17, so we use our own implementation
    // that is always 'constexpr' as long as gcc is being used.
    APD_GCC_CONSTEXPR static int compare(CharT const* s1, CharT const* s2, size_t n) noexcept  // NOLINT
    {
#ifdef __GNUC__
        return __builtin_strncmp(s1, s2, n);
#else
        return Traits::compare(s1, s2, n);
#endif
    }

public:
    // 24.4.2.1, construction and assignment
    constexpr basic_string_view() noexcept  // NOLINT
        : basic_string_view(nullptr, 0)
    {
    }

    constexpr basic_string_view(const_pointer p, size_type count)  // NOLINT
        : mPtr_(p), mSize_(count)
    {
    }

    constexpr basic_string_view(const_pointer p) // NOLINT : Adding explicit here will make char * => StringView syntax extremely inconvenient
        : basic_string_view(p, p == nullptr ? 0 : length(p))
    {
    }

    constexpr basic_string_view(basic_string_view const& other) noexcept = default;

    // Not "constexpr" because that would make it also "const" on C++11 compilers.
    basic_string_view& operator=(basic_string_view const& other) noexcept = default;

    // 24.4.2.2, iterator support
    constexpr const_iterator begin() const noexcept { return mPtr_; }                                    // NOLINT
    constexpr const_iterator cbegin() const noexcept { return mPtr_; }                                   // NOLINT
    constexpr const_iterator end() const noexcept { return mPtr_ + mSize_; }                             // NOLINT
    constexpr const_iterator cend() const noexcept { return mPtr_ + mSize_; }                            // NOLINT
    constexpr const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }   // NOLINT
    constexpr const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(end()); }  // NOLINT
    constexpr const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }   // NOLINT
    constexpr const_reverse_iterator crend() const noexcept { return const_reverse_iterator(begin()); }  // NOLINT

    // 24.4.2.3, capacity
    constexpr size_type size() const noexcept { return mSize_; }    // NOLINT
    constexpr size_type length() const noexcept { return mSize_; }  // NOLINT
    constexpr size_type max_size() const noexcept                   // NOLINT
    {
        // TODO:: not for wchar!
        return std::numeric_limits< size_type >::max();
    }
    constexpr bool empty() const noexcept { return mSize_ == 0; }  // NOLINT

    // 24.4.2.4, element access
    constexpr const_reference operator[](size_type pos) const { return mPtr_[pos]; }
    constexpr const_reference at(size_type pos) const  // NOLINT
    {
        if (pos >= mSize_) {
#ifndef ARA_NO_EXCEPTIONS
            throw std::out_of_range("out of range");
#else
            ara::core::Abort("StringView at out of range");
#endif
        }
        return mPtr_[pos];
    }
    constexpr const_reference front() const { return mPtr_[0]; }          // NOLINT
    constexpr const_reference back() const { return mPtr_[mSize_ - 1]; }  // NOLINT
    constexpr const_pointer data() const noexcept { return mPtr_; }       // NOLINT

    // 24.4.2.5, modifiers
    // Not "constexpr" because that would make it also "const" on C++11 compilers.
    void remove_prefix(size_type n)  // NOLINT
    {
        mPtr_ += n;
        mSize_ -= n;
    }
    // Not "constexpr" because that would make it also "const" on C++11 compilers.
    void remove_suffix(size_type n) { mSize_ -= n; }  // NOLINT
    // Not "constexpr" because that would make it also "const" on C++11 compilers.
    void swap(basic_string_view& other) noexcept  // NOLINT
    {
        std::swap(mPtr_, other.mPtr_);
        std::swap(mSize_, other.mSize_);
    }

    // 24.4.2.6, operations
    size_type copy(CharT* dest, size_type count, size_type pos = 0) const  // NOLINT
    {
        if (pos > mSize_) {
#ifndef ARA_NO_EXCEPTIONS
            throw std::out_of_range("out of range");
#else
            ara::core::Abort("StringView copy out of range");
#endif
        }

        size_t const rcount = std::min(count, mSize_ - pos);
        std::copy(mPtr_ + pos, mPtr_ + pos + rcount, dest);
        return rcount;
    }
    constexpr basic_string_view substr(size_type pos = 0, size_type count = npos) const  // NOLINT
    {
        if (pos > mSize_) {
#ifndef ARA_NO_EXCEPTIONS
            throw std::out_of_range("out of range");
#else
            ara::core::Abort("StringView substr out of range");
#endif
        }

        size_t const rcount = std::min(count, mSize_ - pos);
        return basic_string_view(mPtr_ + pos, rcount);
    }
    constexpr int compare(basic_string_view v) const noexcept  // NOLINT
    {
        int ret = compare(mPtr_, v.mPtr_, std::min(mSize_, v.mSize_));
        if (ret == 0) {
            return (mSize_ < v.mSize_) ? -1 : (mSize_ > v.mSize_);
        }
        return ret;
    }
    constexpr int compare(size_type pos1, size_type count1, basic_string_view v) const  // NOLINT
    {
        return substr(pos1, count1).compare(v);
    }
    constexpr int compare(  // NOLINT
        size_type pos1,
        size_type count1,
        basic_string_view v,
        size_type pos2,
        size_type count2) const
    {
        return substr(pos1, count1).compare(v.substr(pos2, count2));
    }
    constexpr int compare(const_pointer s) const { return compare(basic_string_view(s)); }  // NOLINT
    constexpr int compare(size_type pos1, size_type count1, const_pointer s) const          // NOLINT
    {
        return substr(pos1, count1).compare(basic_string_view(s));
    }
    constexpr int compare(size_type pos1, size_type count1, const_pointer s, size_type count2) const  // NOLINT
    {
        return substr(pos1, count1).compare(basic_string_view(s, count2));
    }

    constexpr size_type find(basic_string_view v, size_type pos = 0) const noexcept  // NOLINT
    {
        return find(v.data(), pos, v.size());
    }
    constexpr size_type find(CharT ch, size_type pos = 0) const noexcept  // NOLINT
    {
        return std::basic_string< CharT, Traits >(mPtr_, mSize_).find(ch, pos);
    }
    constexpr size_type find(const_pointer s, size_type pos, size_type count) const  // NOLINT
    {
        return std::basic_string< CharT, Traits >(mPtr_, mSize_).find(s, pos, count);
    }
    constexpr size_type find(const_pointer s, size_type pos = 0) const  // NOLINT
    {
        return find(s, pos, length(s));
    }

    constexpr size_type rfind(basic_string_view v, size_type pos = npos) const noexcept  // NOLINT
    {
        return rfind(v.data(), pos, v.size());
    }
    constexpr size_type rfind(CharT c, size_type pos = npos) const noexcept  // NOLINT
    {
        return std::basic_string< CharT, Traits >(mPtr_, mSize_).rfind(c, pos);
    }
    constexpr size_type rfind(const_pointer s, size_type pos, size_type count) const  // NOLINT
    {
        return std::basic_string< CharT, Traits >(mPtr_, mSize_).rfind(s, pos, count);
    }
    constexpr size_type rfind(const_pointer s, size_type pos = npos) const  // NOLINT
    {
        return rfind(s, pos, length(s));
    }

    constexpr size_type find_first_of(basic_string_view v, size_type pos = 0) const noexcept  // NOLINT
    {
        return find_first_of(v.data(), pos, v.size());
    }
    constexpr size_type find_first_of(CharT c, size_type pos = 0) const noexcept  // NOLINT
    {
        return std::basic_string< CharT, Traits >(mPtr_, mSize_).find_first_of(c, pos);
    }
    constexpr size_type find_first_of(const_pointer s, size_type pos, size_type count) const  // NOLINT
    {
        return std::basic_string< CharT, Traits >(mPtr_, mSize_).find_first_of(s, pos, count);
    }
    constexpr size_type find_first_of(const_pointer s, size_type pos = 0) const  // NOLINT
    {
        return find_first_of(s, pos, length(s));
    }

    constexpr size_type find_last_of(basic_string_view v, size_type pos = npos) const noexcept  // NOLINT
    {
        return find_last_of(v.data(), pos, v.size());
    }
    constexpr size_type find_last_of(CharT c, size_type pos = npos) const noexcept  // NOLINT
    {
        return std::basic_string< CharT, Traits >(mPtr_, mSize_).find_last_of(c, pos);
    }
    constexpr size_type find_last_of(const_pointer s, size_type pos, size_type count) const  // NOLINT
    {
        return std::basic_string< CharT, Traits >(mPtr_, mSize_).find_last_of(s, pos, count);
    }
    constexpr size_type find_last_of(const_pointer s, size_type pos = npos) const  // NOLINT
    {
        return find_last_of(s, pos, length(s));
    }

    constexpr size_type find_first_not_of(basic_string_view v, size_type pos = 0) const noexcept  // NOLINT
    {
        return find_first_not_of(v.data(), pos, v.size());
    }
    constexpr size_type find_first_not_of(CharT c, size_type pos = 0) const noexcept  // NOLINT
    {
        return std::basic_string< CharT, Traits >(mPtr_, mSize_).find_first_not_of(c, pos);
    }
    constexpr size_type find_first_not_of(const_pointer s, size_type pos, size_type count) const  // NOLINT
    {
        return std::basic_string< CharT, Traits >(mPtr_, mSize_).find_first_not_of(s, pos, count);
    }
    constexpr size_type find_first_not_of(const_pointer s, size_type pos = 0) const  // NOLINT
    {
        return find_first_not_of(s, pos, length(s));
    }

    constexpr size_type find_last_not_of(basic_string_view v, size_type pos = npos) const noexcept  // NOLINT
    {
        return find_last_not_of(v.data(), pos, v.size());
    }
    constexpr size_type find_last_not_of(CharT c, size_type pos = npos) const noexcept  // NOLINT
    {
        return std::basic_string< CharT, Traits >(mPtr_, mSize_).find_last_not_of(c, pos);
    }
    constexpr size_type find_last_not_of(const_pointer s, size_type pos, size_type count) const  // NOLINT
    {
        return std::basic_string< CharT, Traits >(mPtr_, mSize_).find_last_not_of(s, pos, count);
    }
    constexpr size_type find_last_not_of(const_pointer s, size_type pos = npos) const  // NOLINT
    {
        return find_last_not_of(s, pos, length(s));
    }

private:
    CharT const* mPtr_{nullptr};
    size_type mSize_{0U};
};

// 24.4.4, inserters and extractors
template < typename CharT, typename Traits >
// inline std::basic_ostream<CharT, Traits>& operator<<(std::basic_ostream<CharT, Traits>& os,
//                                                      basic_string_view<CharT, Traits> const& v) {
inline std::basic_ostream< CharT, Traits >& operator<<(std::basic_ostream< CharT, Traits >& os,
                                                       basic_string_view< CharT, Traits > v)
{  // modified by hanzhibo
    typename std::basic_ostream< CharT, Traits >::sentry s(os);
    if (s) {
        // In case the argument's width does not fit into a 'streamsize', truncate it.
        std::size_t const maxSize    = static_cast< std::size_t >(std::numeric_limits< std::streamsize >::max());
        std::size_t const outputSize = (v.size() <= maxSize) ? v.size() : maxSize;

        std::streamsize const vsize = static_cast< std::streamsize >(outputSize);
        std::streamsize const width = os.width();
        if (width > vsize) {
            // Stream width is wider than text - must left- or right-align.
            auto fill = [&os](std::streamsize num) {
                CharT const ch = os.fill();
                auto rdbuf     = os.rdbuf();
                while (num--) {
                    std::ostream::int_type const ret = rdbuf->sputc(ch);
                    if (Traits::eq_int_type(ret, Traits::eof())) {
                        os.setstate(std::ios_base::badbit);
                        break;
                    }
                }
            };

            bool const isLeft = ((os.flags() & std::ios_base::adjustfield) == std::ios_base::left);
            if (!isLeft) {
                fill(width - vsize);
            }

            if (os.good()) {
                os.write(v.data(), vsize);
            }

            if (isLeft && os.good()) {
                fill(width - vsize);
            }
        } else {
            // Text is as long or longer than the defined stream width - just output the whole thing.
            os.write(v.data(), vsize);
        }
    }
    return os;
}

// 24.4.3, non-member comparison functions,
//         incl. additional overloads

// Helper type for forcing template argument deduction to proceed based on
// only one of the arguments to a comparison operator. See n3766 for details.
template < typename T >
using Identity = typename std::decay< T >::type;

template < typename CharT, typename Traits >
constexpr bool operator==(basic_string_view< CharT, Traits > lhs, basic_string_view< CharT, Traits > rhs) noexcept
{
    return lhs.compare(rhs) == 0;
}
template < typename CharT, typename Traits >
constexpr bool operator==(basic_string_view< CharT, Traits > lhs,
                          Identity< basic_string_view< CharT, Traits > > rhs) noexcept
{
    return lhs.compare(rhs) == 0;
}
template < typename CharT, typename Traits >
constexpr bool operator==(Identity< basic_string_view< CharT, Traits > > lhs,
                          basic_string_view< CharT, Traits > rhs) noexcept
{
    return lhs.compare(rhs) == 0;
}

template < typename CharT, typename Traits >
constexpr bool operator!=(basic_string_view< CharT, Traits > lhs, basic_string_view< CharT, Traits > rhs) noexcept
{
    return lhs.compare(rhs) != 0;
}
template < typename CharT, typename Traits >
constexpr bool operator!=(basic_string_view< CharT, Traits > lhs,
                          Identity< basic_string_view< CharT, Traits > > rhs) noexcept
{
    return lhs.compare(rhs) != 0;
}
template < typename CharT, typename Traits >
constexpr bool operator!=(Identity< basic_string_view< CharT, Traits > > lhs,
                          basic_string_view< CharT, Traits > rhs) noexcept
{
    return lhs.compare(rhs) != 0;
}

template < typename CharT, typename Traits >
constexpr bool operator<(basic_string_view< CharT, Traits > lhs, basic_string_view< CharT, Traits > rhs) noexcept
{
    return lhs.compare(rhs) < 0;
}
template < typename CharT, typename Traits >
constexpr bool operator<(basic_string_view< CharT, Traits > lhs,
                         Identity< basic_string_view< CharT, Traits > > rhs) noexcept
{
    return lhs.compare(rhs) < 0;
}
template < typename CharT, typename Traits >
constexpr bool operator<(Identity< basic_string_view< CharT, Traits > > lhs,
                         basic_string_view< CharT, Traits > rhs) noexcept
{
    return lhs.compare(rhs) < 0;
}

template < typename CharT, typename Traits >
constexpr bool operator>(basic_string_view< CharT, Traits > lhs, basic_string_view< CharT, Traits > rhs) noexcept
{
    return lhs.compare(rhs) > 0;
}
template < typename CharT, typename Traits >
constexpr bool operator>(basic_string_view< CharT, Traits > lhs,
                         Identity< basic_string_view< CharT, Traits > > rhs) noexcept
{
    return lhs.compare(rhs) > 0;
}
template < typename CharT, typename Traits >
constexpr bool operator>(Identity< basic_string_view< CharT, Traits > > lhs,
                         basic_string_view< CharT, Traits > rhs) noexcept
{
    return lhs.compare(rhs) > 0;
}

template < typename CharT, typename Traits >
constexpr bool operator<=(basic_string_view< CharT, Traits > lhs, basic_string_view< CharT, Traits > rhs) noexcept
{
    return lhs.compare(rhs) <= 0;
}
template < typename CharT, typename Traits >
constexpr bool operator<=(basic_string_view< CharT, Traits > lhs,
                          Identity< basic_string_view< CharT, Traits > > rhs) noexcept
{
    return lhs.compare(rhs) <= 0;
}
template < typename CharT, typename Traits >
constexpr bool operator<=(Identity< basic_string_view< CharT, Traits > > lhs,
                          basic_string_view< CharT, Traits > rhs) noexcept
{
    return lhs.compare(rhs) <= 0;
}

template < typename CharT, typename Traits >
constexpr bool operator>=(basic_string_view< CharT, Traits > lhs, basic_string_view< CharT, Traits > rhs) noexcept
{
    return lhs.compare(rhs) >= 0;
}
template < typename CharT, typename Traits >
constexpr bool operator>=(basic_string_view< CharT, Traits > lhs,
                          Identity< basic_string_view< CharT, Traits > > rhs) noexcept
{
    return lhs.compare(rhs) >= 0;
}
template < typename CharT, typename Traits >
constexpr bool operator>=(Identity< basic_string_view< CharT, Traits > > lhs,
                          basic_string_view< CharT, Traits > rhs) noexcept
{
    return lhs.compare(rhs) >= 0;
}

#undef APD_GCC_CONSTEXPR

}  // namespace internal

/// @brief A read-only view over a contiguous sequence of characters.
///
/// @uptrace{SWS_CORE_02001, 478dd068ec59c6b41b44c4d028ee87a059c53499}
using StringView = internal::basic_string_view< char >;

/// added by hanzhibo begin
#ifdef _GLIBCXX_USE_WCHAR_T
using WStringView = internal::basic_string_view< wchar_t >;
#endif
// #ifdef _GLIBCXX_USE_CHAR8_T
//   using U8StringView = internal::basic_string_view<char8_t>;
// #endif
using U16StringView = internal::basic_string_view< char16_t >;
using U32StringView = internal::basic_string_view< char32_t >;

// [string.view.hash], hash support:

template < typename _Tp >  // NOLINT
struct hash;

template < typename _Hash >  // NOLINT
struct __is_fast_hash;

template <>
struct hash< StringView > : public std::__hash_base< size_t, StringView >
{
    size_t operator()(const StringView& __str) const noexcept  // NOLINT
    {
        return std::_Hash_impl::hash(__str.data(), __str.length());
    }
};

template <>
struct __is_fast_hash< hash< StringView > > : std::false_type
{
};

#ifdef _GLIBCXX_USE_WCHAR_T
template <>
struct hash< WStringView > : public std::__hash_base< size_t, WStringView >
{
    size_t operator()(const WStringView& __s) const noexcept  // NOLINT
    {
        return std::_Hash_impl::hash(__s.data(), __s.length() * sizeof(wchar_t));
    }
};

template <>
struct __is_fast_hash< hash< WStringView > > : std::false_type
{
};
#endif

// #ifdef _GLIBCXX_USE_CHAR8_T
//   template<>
//     struct hash<U8StringView>
//     : public std::__hash_base<size_t, U8StringView>
//     {
//       size_t
//       operator()(const U8StringView& __str) const noexcept
//       { return std::_Hash_impl::hash(__str.data(), __str.length()); }
//     };

//   template<>
//     struct __is_fast_hash<hash<U8StringView>> : std::false_type
//     { };
// #endif

template <>
struct hash< U16StringView > : public std::__hash_base< size_t, U16StringView >
{
    size_t operator()(const U16StringView& __s) const noexcept  // NOLINT
    {
        return std::_Hash_impl::hash(__s.data(), __s.length() * sizeof(char16_t));
    }
};

template <>
struct __is_fast_hash< hash< U16StringView > > : std::false_type
{
};

template <>
struct hash< U32StringView > : public std::__hash_base< size_t, U32StringView >
{
    size_t operator()(const U32StringView& __s) const noexcept  // NOLINT
    {
        return std::_Hash_impl::hash(__s.data(), __s.length() * sizeof(char32_t));
    }
};

template <>
struct __is_fast_hash< hash< U32StringView > > : std::false_type
{
};

inline namespace literals {
inline namespace string_view_literals {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wliteral-suffix"
inline constexpr internal::basic_string_view< char > operator""sv(const char* __str,      // NOLINT
                                                                  size_t __len) noexcept  // NOLINT
{
    return internal::basic_string_view< char >{__str, __len};
}

#ifdef _GLIBCXX_USE_WCHAR_T
inline constexpr internal::basic_string_view< wchar_t > operator""sv(const wchar_t* __str,   // NOLINT
                                                                     size_t __len) noexcept  // NOLINT
{
    return internal::basic_string_view< wchar_t >{__str, __len};
}
#endif

// #ifdef _GLIBCXX_USE_CHAR8_T
//     inline constexpr internal::basic_string_view<char8_t>
//     operator""sv(const char8_t* __str, size_t __len) noexcept
//     { return internal::basic_string_view<char8_t>{__str, __len}; }
// #endif

inline constexpr internal::basic_string_view< char16_t > operator""sv(const char16_t* __str,  // NOLINT
                                                                      size_t __len) noexcept  // NOLINT
{
    return internal::basic_string_view< char16_t >{__str, __len};
}

inline constexpr internal::basic_string_view< char32_t > operator""sv(const char32_t* __str,  // NOLINT
                                                                      size_t __len) noexcept  // NOLINT
{
    return internal::basic_string_view< char32_t >{__str, __len};
}

#pragma GCC diagnostic pop
}  // namespace string_view_literals
}  // namespace literals
/// added by hanzhibo end

}  // namespace core
}  // namespace ara

#endif  // ARA_CORE_STRING_VIEW_H_
