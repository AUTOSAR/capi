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
/// @file       string.h
/// @brief      Interface to class ara::core::String
/// @details
/// @date       2022-06-24
/// @author     hanzhibo
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_CORE_STRING_H_
#define ARA_CORE_STRING_H_

#include <functional>
#include <string>
#include <type_traits>

#include "ara/core/string_view.h"

namespace ara {
namespace core {

namespace internal {
constexpr int32_t kDefaultBase{10};
// forward declare
template < typename CharT, typename Traits, typename Allocator >
class basic_string;

// declaration
template < typename CharT, typename Traits, typename Allocator >
inline int stoi  // NOLINT
    (const basic_string< CharT, Traits, Allocator >& str, size_t* pos = nullptr, int base = kDefaultBase);

template < typename CharT, typename Traits, typename Allocator >
inline long stol  // NOLINT
    (const basic_string< CharT, Traits, Allocator >& str, size_t* pos = nullptr, int base = kDefaultBase);

template < typename CharT, typename Traits, typename Allocator >
inline long long stoll  // NOLINT
    (const basic_string< CharT, Traits, Allocator >& str, size_t* pos = nullptr, int base = kDefaultBase);

template < typename CharT, typename Traits, typename Allocator >
inline unsigned long stoul  // NOLINT
    (const basic_string< CharT, Traits, Allocator >& str, size_t* pos = nullptr, int base = kDefaultBase);

template < typename CharT, typename Traits, typename Allocator >
inline unsigned long long stoull  // NOLINT
    (const basic_string< CharT, Traits, Allocator >& str, size_t* pos = nullptr, int base = kDefaultBase);

template < typename CharT, typename Traits, typename Allocator >
inline float stof(const basic_string< CharT, Traits, Allocator >& str, size_t* pos = nullptr);  // NOLINT

template < typename CharT, typename Traits, typename Allocator >
inline double stod(const basic_string< CharT, Traits, Allocator >& str, size_t* pos = nullptr);  // NOLINT

template < typename CharT, typename Traits, typename Allocator >
inline long double stold(const basic_string< CharT, Traits, Allocator >& str, size_t* pos = nullptr);  // NOLINT

template < typename CharT, typename Traits = std::char_traits< CharT >, typename Allocator = std::allocator< CharT > >
class basic_string  // NOLINT
{
    using internal_type = std::basic_string< CharT, Traits, Allocator >;  // NOLINT
    friend struct std::hash< basic_string< CharT, Traits, Allocator > >;  // NOLINT

    friend int stoi<>(const basic_string& str, size_t* pos, int base);                   // NOLINT
    friend long stol<>(const basic_string& str, size_t* pos, int base);                  // NOLINT
    friend long long stoll<>(const basic_string& str, size_t* pos, int base);            // NOLINT
    friend unsigned long stoul<>(const basic_string& str, size_t* pos, int base);        // NOLINT
    friend unsigned long long stoull<>(const basic_string& str, size_t* pos, int base);  // NOLINT
    friend float stof<>(const basic_string& str, size_t* pos);                           // NOLINT
    friend double stod<>(const basic_string& str, size_t* pos);                          // NOLINT
    friend long double stold<>(const basic_string& str, size_t* pos);                    // NOLINT

    internal_type value_;

public:
    using traits_type            = typename internal_type::traits_type;             // NOLINT
    using size_type              = typename internal_type::size_type;               // NOLINT
    using value_type             = typename internal_type::value_type;              // NOLINT
    using allocator_type         = typename internal_type::allocator_type;          // NOLINT
    using difference_type        = typename internal_type::difference_type;         // NOLINT
    using reference              = typename internal_type::reference;               // NOLINT
    using const_reference        = typename internal_type::const_reference;         // NOLINT
    using pointer                = typename internal_type::pointer;                 // NOLINT
    using const_pointer          = typename internal_type::const_pointer;           // NOLINT
    using iterator               = typename internal_type::iterator;                // NOLINT
    using const_iterator         = typename internal_type::const_iterator;          // NOLINT
    using reverse_iterator       = typename internal_type::reverse_iterator;        // NOLINT
    using const_reverse_iterator = typename internal_type::const_reverse_iterator;  // NOLINT

    static const size_type npos = internal_type::npos;  // NOLINT

    // modified by hanzhibo
    // explicit basic_string(Allocator const& a = Allocator())
    //     : value_(a)
    // {}

    /// added by hanzhibo begin
    basic_string() : basic_string(Allocator()) {}
    /// added by hanzhibo end

    explicit basic_string(Allocator const& a) : value_(a) {}

    // We want them for interoperability with std::string, so that we can support
    // assignment from std::string.

    explicit basic_string(internal_type const& other) : value_(other) {}

    explicit basic_string(internal_type&& other) noexcept : value_(std::move(other)) {}

    // Standard copy and move constructors.
    basic_string(basic_string const& other) : value_(other.value_) {}

    basic_string(basic_string const& other, Allocator const& a) : value_(other.value_, a) {}

    basic_string(basic_string&& other) noexcept : value_(std::move(other.value_)) {}

    basic_string(basic_string&& other, Allocator const& a) : value_(std::move(other.value_), a) {}

    /// @uptrace{SWS_CORE_03303, 5439cedb1e2306e2bdd167d9e1cf192100e930aa}
    // A helper type for avoiding boiler-plate.
    typedef basic_string_view< CharT, Traits > __sv_type;  // NOLINT

    template < typename _Tp, typename _Res >  // NOLINT
    using _If_sv                              // NOLINT
        = std::enable_if_t< std::__and_< std::is_convertible< const _Tp&, __sv_type >,
                                         std::__not_< std::is_convertible< const _Tp*, const basic_string* > >,
                                         std::__not_< std::is_convertible< const _Tp&, const CharT* > > >::value,
                            _Res >;

    // Allows an implicit conversion to __sv_type.
    static __sv_type _S_to_string_view(__sv_type __svt) noexcept  // NOLINT
    {
        return __svt;
    }

    template < typename T, typename = _If_sv< T, void > >
    basic_string(T const& t, size_type pos, size_type n, Allocator const& a = Allocator())
        : basic_string(_S_to_string_view(t).substr(pos, n), a)
    {
    }

    basic_string(size_type count, CharT ch, Allocator const& a = Allocator()) : value_(count, ch, a) {}

    /// added by hanzhibo begin
    basic_string(basic_string const& other, size_type pos, Allocator const& a = Allocator())
        : value_(other.value_, pos, a)
    {
    }
    /// added by hanzhibo end

    /// modified by hanzhibo begin
    // basic_string(basic_string const& other, size_type pos, size_type count = npos, Allocator const& a = Allocator())
    //     : value_(other.value_, pos, count, a) {}
    basic_string(basic_string const& other, size_type pos, size_type count, Allocator const& a = Allocator())
        : value_(other.value_, pos, count, a)
    {
    }
    /// modified by hanzhibo end

    basic_string(CharT const* s, size_type count, Allocator const& a = Allocator()) : value_(s, count, a) {}

    basic_string( // NOLINT : Adding explicit here will make char * => String syntax extremely inconvenient
        CharT const* s,
        Allocator const& a = Allocator())
        : value_(s, a)
    {
    }

    template < class InputIt >
    basic_string(InputIt first, InputIt last, Allocator const& a = Allocator()) : value_(first, last, a)
    {
    }

    basic_string(std::initializer_list< CharT > ilist, Allocator const& a = Allocator()) : value_(ilist, a) {}

    /// @uptrace{SWS_CORE_03302, 95d82bbaeb6922d0fd357f65a26ebb642405c613}
    explicit basic_string(basic_string_view< CharT, Traits > sv, Allocator const& a = Allocator())
        : value_(sv.data(), sv.size(), a)
    {
    }

    // Standard copy and move assignment operators.
    basic_string& operator=(basic_string const& other) noexcept
    {
        if (&other == this) {
            return *this;
        }
        assign(other);
        return *this;
    }

    basic_string& operator=(basic_string&& other) noexcept
    {
        if (&other == this) {
            return *this;
        }
        assign(std::move(other));
        return *this;
    }

    basic_string& operator=(internal_type const& other)
    {
        assign(other);
        return *this;
    }

    basic_string& operator=(internal_type&& other)
    {
        assign(std::move(other));
        return *this;
    }

    basic_string& operator=(CharT const* s)
    {
        assign(s);
        return *this;
    }

    basic_string& operator=(CharT c)
    {
        assign(1, c);
        return *this;
    }

    basic_string& operator=(std::initializer_list< CharT > ilist)
    {
        assign(ilist);
        return *this;
    }

    /// @uptrace{SWS_CORE_03304, ad60314b5dee70b4b95edf55eb2ec38f6bcbfb08}
    basic_string& operator=(basic_string_view< CharT, Traits > sv) { return assign(sv); }

    basic_string& operator+=(basic_string const& other) { return append(other); }

    basic_string& operator+=(internal_type const& other) { return value_.append(other); }

    basic_string& operator+=(CharT c) { return append(static_cast< size_type >(1), c); }

    basic_string& operator+=(CharT const* s) { return append(s); }

    basic_string& operator+=(std::initializer_list< CharT > ilist) { return append(ilist); }

    /// @uptrace{SWS_CORE_03307, cd641ed84f53a3d1639f31ffc4c3f55de17f7821}
    basic_string& operator+=(basic_string_view< CharT, Traits > sv) { return append(sv); }

    basic_string& append(size_type count, CharT c)  // NOLINT
    {
        value_.append(count, c);
        return *this;
    }

    basic_string& append(basic_string const& other)  // NOLINT
    {
        value_.append(other.value_);
        return *this;
    }

    basic_string& append(basic_string const& other, size_type pos, size_type count)  // NOLINT
    {
        value_.append(other.value_, pos, count);
        return *this;
    }

    basic_string& append(internal_type const& other)  // NOLINT
    {
        value_.append(other);
        return *this;
    }

    basic_string& append(internal_type const& other, size_type pos, size_type count)  // NOLINT
    {
        value_.append(other, pos, count);
        return *this;
    }

    basic_string& append(CharT const* s, size_type pos, size_type count)  // NOLINT
    {
        value_.append(s + pos, count);
        return *this;
    }

    basic_string& append(CharT const* s, size_type count)  // NOLINT
    {
        value_.append(s, count);
        return *this;
    }

    basic_string& append(CharT const* s)  // NOLINT
    {
        value_.append(s);
        return *this;
    }

    template < class InputIt >
    basic_string& append(InputIt first, InputIt last)  // NOLINT
    {
        value_.append(first, last);
        return *this;
    }

    basic_string& append(std::initializer_list< CharT > ilist)  // NOLINT
    {
        value_.append(ilist);
        return *this;
    }

    /// @uptrace{SWS_CORE_03308, 906ac712f927da2f0fc23c317a4fd983cf00965b}
    basic_string& append(basic_string_view< CharT, Traits > sv)  // NOLINT
    {
        value_.append(sv.data(), sv.size());
        return *this;
    }

    /// @uptrace{SWS_CORE_03309, 883d737b4772037468a573e3ce78aff724e69443} _If_sv
    template < typename T, typename = _If_sv< T, void > >
    basic_string& append(T const& t, size_type pos, size_type n = npos)  // NOLINT
    {
        // TODO:: out_of_range if pos > sv.size()
        basic_string_view< CharT, Traits > const sv = t;
        size_type const rlen                        = std::min(n, size_type(sv.size() - pos));
        value_.append(sv.data() + pos, rlen);
        return *this;
    }

    basic_string& assign(size_type count, CharT c)  // NOLINT
    {
        value_.assign(count, c);
        return *this;
    }

    basic_string& assign(basic_string const& other)  // NOLINT
    {
        value_.assign(other.value_);
        return *this;
    }

    basic_string& assign(basic_string const& other, size_type pos, size_type count)  // NOLINT
    {
        value_.assign(other.value_, pos, count);
        return *this;
    }

    basic_string& assign(basic_string&& other)  // NOLINT
    {
        value_.assign(std::move(other.value_));
        return *this;
    }

    basic_string& assign(internal_type const& other)  // NOLINT
    {
        value_.assign(other);
        return *this;
    }

    basic_string& assign(internal_type const& other, size_type pos, size_type count)  // NOLINT
    {
        value_.assign(other, pos, count);
        return *this;
    }

    basic_string& assign(internal_type&& other)  // NOLINT
    {
        value_.assign(std::move(other));
        return *this;
    }

    basic_string& assign(CharT const* s, size_type pos, size_type count)  // NOLINT
    {
        value_.assign(s + pos, count);
        return *this;
    }

    basic_string& assign(CharT const* s, size_type count)  // NOLINT
    {
        value_.assign(s, count);
        return *this;
    }

    basic_string& assign(CharT const* s)  // NOLINT
    {
        value_.assign(s);
        return *this;
    }

    template < class InputIt >
    basic_string& assign(InputIt first, InputIt last)  // NOLINT
    {
        value_.assign(first, last);
        return *this;
    }

    basic_string& assign(std::initializer_list< CharT > ilist)  // NOLINT
    {
        value_.assign(ilist);
        return *this;
    }

    /// @uptrace{SWS_CORE_03305, 72498812fbc705c95ad33c79c699f1ff00b59eb1}
    basic_string& assign(basic_string_view< CharT, Traits > sv)  // NOLINT
    {
        value_.assign(sv.data(), sv.size());
        return *this;
    }

    /// @uptrace{SWS_CORE_03306, 290e65fb104bf1af6b979d9040a26878d2191ea9}
    template < typename T, typename = _If_sv< T, void > >
    basic_string& assign(T const& t, size_type pos, size_type n = npos)  // NOLINT
    {
        // TODO:: out_of_range if pos > sv.size()
        basic_string_view< CharT, Traits > const sv = t;
        size_type const rlen                        = std::min(n, size_type(value_.size() - pos));
        value_.assign(sv.data() + pos, rlen);
        return *this;
    }

    basic_string& insert(size_type index, size_type count, CharT ch)  // NOLINT
    {
        value_.insert(index, count, ch);
        return *this;
    }

    basic_string& insert(size_type index, CharT const* s)  // NOLINT
    {
        value_.insert(index, s);
        return *this;
    }

    basic_string& insert(size_type index, CharT const* s, size_type count)  // NOLINT
    {
        value_.insert(index, s, count);
        return *this;
    }

    basic_string& insert(size_type index, basic_string const& other)  // NOLINT
    {
        value_.insert(index, other.value_);
        return *this;
    }

    basic_string& insert(size_type index, basic_string const& other, size_type index_str, size_type count)  // NOLINT
    {
        value_.insert(index, other.value_, index_str, count);
        return *this;
    }

    iterator insert(const_iterator pos, CharT ch)  // NOLINT
    {
        return value_.insert(pos, ch);
    }

    iterator insert(const_iterator pos, size_type count, CharT ch)  // NOLINT
    {
        return value_.insert(pos, count, ch);
    }

    template < class InputIt >
    iterator insert(const_iterator pos, InputIt first, InputIt last)  // NOLINT
    {
        return value_.insert(pos, first, last);
    }

    iterator insert(const_iterator pos, std::initializer_list< CharT > ilist)  // NOLINT
    {
        return value_.insert(pos, ilist);
    }

    /// @uptrace{SWS_CORE_03310, fc248c792cb3071e59d1ecd371c09e94f59601a1}
    basic_string& insert(size_type pos, basic_string_view< CharT, Traits > sv)  // NOLINT
    {
        value_.insert(pos, sv.data(), sv.size());
        return *this;
    }

    /// @uptrace{SWS_CORE_03311, a1d46c4ba33417ddcce1a1b12a8e8c4e979c5217}
    template < typename T, typename = _If_sv< T, void > >
    basic_string& insert(size_type pos1, T const& t, size_type pos2, size_type n = npos)  // NOLINT
    {
        // TODO:: out_of_range if pos1 > size() or pos2 > sv.size()
        basic_string_view< CharT, Traits > sv = t;
        size_type const rlen                  = std::min(n, size_type(value_.size() - pos2));
        value_.insert(pos1, sv.data() + pos2, rlen);
        return *this;
    }

    basic_string& erase(size_type index = 0, size_type count = npos)  // NOLINT
    {
        value_.erase(index, count);
        return *this;
    }

    iterator erase(const_iterator position)  // NOLINT
    {
        return value_.erase(position);
    }

    iterator erase(const_iterator first, const_iterator last)  // NOLINT
    {
        return value_.erase(first, last);
    }

    basic_string& replace(size_type pos, size_type count, basic_string const& other)  // NOLINT
    {
        value_.replace(pos, count, other.value_);
        return *this;
    }

    basic_string& replace(const_iterator first, const_iterator last, basic_string const& other)  // NOLINT
    {
        value_.replace(first, last, other.value_);
        return *this;
    }

    basic_string& replace  // NOLINT
        (size_type pos, size_type count, basic_string const& other, size_type pos2, size_type count2)
    {
        value_.replace(pos, count, other.value_, pos2, count2);
        return *this;
    }

    template < class InputIt >
    basic_string& replace(const_iterator first, const_iterator last, InputIt first2, InputIt last2)  // NOLINT
    {
        value_.replace(first, last, first2, last2);
        return *this;
    }

    basic_string& replace(size_type pos, size_type count, CharT const* cstr, size_type count2)  // NOLINT
    {
        value_.replace(pos, count, cstr, count2);
        return *this;
    }

    basic_string& replace(const_iterator first, const_iterator last, CharT const* cstr, size_type count2)  // NOLINT
    {
        value_.replace(first, last, cstr, count2);
        return *this;
    }

    basic_string& replace(size_type pos, size_type count, CharT const* cstr)  // NOLINT
    {
        value_.replace(pos, count, cstr);
        return *this;
    }

    basic_string& replace(const_iterator first, const_iterator last, CharT const* cstr)  // NOLINT
    {
        value_.replace(first, last, cstr);
        return *this;
    }

    basic_string& replace(size_type pos, size_type count, size_type count2, CharT ch)  // NOLINT
    {
        value_.replace(pos, count, count2, ch);
        return *this;
    }

    basic_string& replace(const_iterator first, const_iterator last, size_type count2, CharT ch)  // NOLINT
    {
        value_.replace(first, last, count2, ch);
        return *this;
    }

    basic_string& replace(const_iterator first, const_iterator last, std::initializer_list< CharT > ilist)  // NOLINT
    {
        value_.replace(first, last, ilist);
        return *this;
    }

    /// @uptrace{SWS_CORE_03312, 90112f159e4785ac3748ed1611d66f671fcf2fc1}
    basic_string& replace(size_type pos1, size_type n1, basic_string_view< CharT, Traits > sv)  // NOLINT
    {
        value_.replace(pos1, n1, sv.data(), sv.size());
        return *this;
    }

    /// @uptrace{SWS_CORE_03313, fa0124ff816b93855d86f214e7202f8d209ef8a2}
    template < typename T, typename = _If_sv< T, void > >
    basic_string& replace(size_type pos1, size_type n1, T const& t, size_type pos2, size_type n2 = npos)  // NOLINT
    {
        // TODO:: out_of_range if pos1 > size() or pos2 > sv.size()
        basic_string_view< CharT, Traits > sv = t;
        size_type const rlen                  = std::min(n2, size_type(sv.size() - pos2));
        value_.replace(pos1, n1, sv.data() + pos2, rlen);
        return *this;
    }

    /// @uptrace{SWS_CORE_03314, 2813f152827b3dfc2426bb3f7c7983f7b8f4f0ab}
    basic_string& replace(const_iterator i1, const_iterator i2, basic_string_view< CharT, Traits > sv)  // NOLINT
    {
        return replace(static_cast< size_type >(i1 - value_.begin()), static_cast< size_type >(i2 - i1), sv);
    }

    /// @uptrace{SWS_CORE_03301, fa10567ea436d0c745b301f77170dd1f0ec031a9}
    operator basic_string_view< CharT, Traits >() const noexcept  // NOLINT
    {
        return basic_string_view< CharT, Traits >(value_.data(), value_.size());
    }

    size_type find(basic_string const& str, size_type pos = 0) const noexcept  // NOLINT
    {
        return value_.find(str.value_, pos);
    }

    size_type find(CharT const* s, size_type pos, size_type count) const  // NOLINT
    {
        return value_.find(s, pos, count);
    }

    size_type find(CharT const* s, size_type pos = 0) const  // NOLINT
    {
        return value_.find(s, pos);
    }

    size_type find(CharT ch, size_type pos = 0) const noexcept  // NOLINT
    {
        return value_.find(ch, pos);
    }

    /// @uptrace{SWS_CORE_03315, 09066f994e73614993b9e8c52b10637893595c4b}
    size_type find(basic_string_view< CharT, Traits > sv, size_type pos = 0) const noexcept  // NOLINT
    {
        return value_.find(sv.data(), pos, sv.size());
    }

    size_type rfind(basic_string const& str, size_type pos = npos) const noexcept  // NOLINT
    {
        return value_.rfind(str.value_, pos);
    }

    size_type rfind(CharT const* s, size_type pos, size_type count) const  // NOLINT
    {
        return value_.rfind(s, pos, count);
    }

    size_type rfind(CharT const* s, size_type pos = npos) const  // NOLINT
    {
        return value_.rfind(s, pos);
    }

    size_type rfind(CharT ch, size_type pos = npos) const noexcept  // NOLINT
    {
        return value_.rfind(ch, pos);
    }

    /// @uptrace{SWS_CORE_03316, 66f1f43f469874331feea234e10291635392e24a}
    size_type rfind(basic_string_view< CharT, Traits > sv, size_type pos = npos) const noexcept  // NOLINT
    {
        return value_.rfind(sv.data(), pos, sv.size());
    }

    size_type find_first_of(CharT const* s, size_type pos, size_type count) const  // NOLINT
    {
        return value_.find_first_of(s, pos, count);
    }

    size_type find_first_of(CharT const* s, size_type pos = 0) const  // NOLINT
    {
        return value_.find_first_of(s, pos);
    }

    size_type find_first_of(CharT ch, size_type pos = 0) const noexcept  // NOLINT
    {
        return value_.find_first_of(ch, pos);
    }

    size_type find_first_of(basic_string const& other, size_type pos = 0) const noexcept  // NOLINT
    {
        return value_.find_first_of(other.value_, pos);
    }

    /// @uptrace{SWS_CORE_03317, 9d3f9612b34c56177d1548feb27f408ebb354827}
    size_type find_first_of(basic_string_view< CharT, Traits > sv, size_type pos = 0) const noexcept  // NOLINT
    {
        return value_.find_first_of(sv.data(), pos, sv.size());
    }

    constexpr size_type find_last_of(CharT const* s, size_type pos, size_type count) const  // NOLINT
    {
        return value_.find_last_of(s, pos, count);
    }

    size_type find_last_of(CharT const* s, size_type pos = npos) const  // NOLINT
    {
        return value_.find_last_of(s, pos);
    }

    size_type find_last_of(CharT ch, size_type pos = npos) const noexcept  // NOLINT
    {
        return value_.find_last_of(ch, pos);
    }

    constexpr size_type find_last_of(basic_string const& other, size_type pos = npos) const noexcept  // NOLINT
    {
        return value_.find_last_of(other.value_, pos);
    }

    /// @uptrace{SWS_CORE_03318, 33e4376d42c9f5771e7f6bf4d07c96cfc126d5d3}
    size_type find_last_of(basic_string_view< CharT, Traits > sv, size_type pos = npos) const noexcept  // NOLINT
    {
        return value_.find_last_of(sv.data(), pos, sv.size());
    }

    size_type find_first_not_of(CharT const* s, size_type pos, size_type count) const  // NOLINT
    {
        return value_.find_first_not_of(s, pos, count);
    }

    size_type find_first_not_of(CharT const* s, size_type pos = 0) const  // NOLINT
    {
        return value_.find_first_not_of(s, pos);
    }

    size_type find_first_not_of(CharT ch, size_type pos = 0) const noexcept  // NOLINT
    {
        return value_.find_first_not_of(ch, pos);
    }

    size_type find_first_not_of(basic_string const& other, size_type pos = 0) const noexcept  // NOLINT
    {
        return value_.find_first_not_of(other.value_, pos);
    }

    /// @uptrace{SWS_CORE_03319, 5f8b7afcabef9ac23eb1775b941d72198d4008c1}
    size_type find_first_not_of(basic_string_view< CharT, Traits > sv, size_type pos = 0) const noexcept  // NOLINT
    {
        return value_.find_first_not_of(sv.data(), pos, sv.size());
    }

    size_type find_last_not_of(CharT const* s, size_type pos, size_type count) const  // NOLINT
    {
        return value_.find_last_not_of(s, pos, count);
    }

    size_type find_last_not_of(CharT const* s, size_type pos = npos) const  // NOLINT
    {
        return value_.find_last_not_of(s, pos);
    }

    size_type find_last_not_of(CharT ch, size_type pos = npos) const noexcept  // NOLINT
    {
        return value_.find_last_not_of(ch, pos);
    }

    size_type find_last_not_of(basic_string const& other, size_type pos = npos) const noexcept  // NOLINT
    {
        return value_.find_last_not_of(other.value_, pos);
    }

    /// @uptrace{SWS_CORE_03320, 6cc9a7bd9a9374479db455df44be9263115e9cf1}
    size_type find_last_not_of(basic_string_view< CharT, Traits > sv, size_type pos = npos) const noexcept  // NOLINT
    {
        return value_.find_last_not_of(sv.data(), pos, sv.size());
    }

    int compare(basic_string const& other) const noexcept { return value_.compare(other.value_); }  // NOLINT

    int compare(size_type pos, size_type count, basic_string const& other) const  // NOLINT
    {
        return value_.compare(pos, count, other.value_);
    }

    int compare  // NOLINT
        (size_type pos1, size_type count1, basic_string const& other, size_type pos2, size_type count2) const
    {
        return value_.compare(pos1, count1, other.value_, pos2, count2);
    }

    int compare(internal_type const& other) const noexcept { return value_.compare(other); }  // NOLINT

    int compare(size_type pos, size_type count, internal_type const& other) const  // NOLINT
    {
        return value_.compare(pos, count, other);
    }

    int compare  // NOLINT
        (size_type pos1, size_type count1, internal_type const& other, size_type pos2, size_type count2) const
    {
        return value_.compare(pos1, count1, other, pos2, count2);
    }

    int compare(CharT const* s) const { return value_.compare(s); }  // NOLINT

    int compare(size_type pos1, size_type count1, CharT const* s) const  // NOLINT
    {
        return value_.compare(pos1, count1, s);
    }

    int compare(size_type pos1, size_type count1, CharT const* s, size_type pos2, size_type count2) const  // NOLINT
    {
        return value_.compare(pos1, count1, s + pos2, count2);
    }

    int compare(size_type pos1, size_type count1, CharT const* s, size_type count2) const  // NOLINT
    {
        return value_.compare(pos1, count1, s, count2);
    }

    /// @uptrace{SWS_CORE_03321, ea49434ff695f8a323fd8f81ae672cfd2a8e3226}
    int compare(basic_string_view< CharT, Traits > sv) const noexcept  // NOLINT
    {
        size_type const rlen = std::min(value_.size(), sv.size());
        int const ret        = Traits::compare(value_.data(), sv.data(), rlen);
        if (ret == 0) {
            return (value_.size() < sv.size()) ? -1 : value_.size() > sv.size();
        }
        return ret;
    }

    /// @uptrace{SWS_CORE_03322, 4db3b05bf8acbfe8fe13169d5519bcb50d9df868}
    int compare(size_type pos1, size_type n1, basic_string_view< CharT, Traits > sv) const  // NOLINT
    {
        return basic_string_view< CharT, Traits >(value_.data(), value_.size()).substr(pos1, n1).compare(sv);
    }

    /// @uptrace{SWS_CORE_03323, 50fa89de6b687ca8d8a25a428f33d6b9edae61a7}
    template < typename T, typename = _If_sv< T, void > >
    int compare(size_type pos1, size_type n1, T const& t, size_type pos2, size_type n2 = npos) const  // NOLINT
    {
        basic_string_view< CharT, Traits > const sv = t;
        return basic_string_view< CharT, Traits >(value_.data(), value_.size())
            .substr(pos1, n1)
            .compare(sv.substr(pos2, n2));
    }

    bool empty() const noexcept { return value_.empty(); }                // NOLINT
    size_type size() const noexcept { return value_.size(); }             // NOLINT
    size_type length() const noexcept { return value_.length(); }         // NOLINT
    size_type max_size() const noexcept { return value_.max_size(); }     // NOLINT
    size_type capacity() const noexcept { return value_.capacity(); }     // NOLINT
    void reserve(size_type newCap = 0) { value_.reserve(newCap); }        // NOLINT
    void resize(size_type count) { value_.resize(count); }                // NOLINT
    void resize(size_type count, CharT ch) { value_.resize(count, ch); }  // NOLINT
    void shrink_to_fit() { value_.shrink_to_fit(); }                      // NOLINT
    void clear() noexcept { value_.clear(); }                             // NOLINT
    CharT const* c_str() const noexcept { return value_.c_str(); }        // NOLINT
    CharT const* data() const noexcept { return value_.data(); }          // NOLINT

    /// added by hanzhibo begin
    reference at(size_type pos) { return value_.at(pos); }              // NOLINT
    const_reference at(size_type pos) const { return value_.at(pos); }  // NOLINT

    reference operator[](size_type pos) { return value_[pos]; }
    const_reference operator[](size_type pos) const { return value_[pos]; }
    /// added by hanzhibo end

    CharT& front() { return value_.front(); }              // NOLINT
    CharT const& front() const { return value_.front(); }  // NOLINT
    CharT& back() { return value_.back(); }                // NOLINT
    CharT const& back() const { return value_.back(); }    // NOLINT
    void push_back(CharT ch) { value_.push_back(ch); }     // NOLINT
    void pop_back() { value_.pop_back(); }                 // NOLINT

    basic_string substr(size_type pos = 0, size_type count = npos) const  // NOLINT
    {
        return basic_string{value_.substr(pos, count)};
    }

    // added by hanzhibo begin
    size_type copy(CharT* dest, size_type count, size_type pos = 0) const  // NOLINT
    {
        return value_.copy(dest, count, pos);
    }  // NOLINT
    // added by hanzhibo end

    void swap(basic_string& other) { value_.swap(other.value_); }                 // NOLINT
    iterator begin() noexcept { return value_.begin(); }                          // NOLINT
    const_iterator begin() const noexcept { return value_.begin(); }              // NOLINT
    const_iterator cbegin() const noexcept { return value_.cbegin(); }            // NOLINT
    iterator end() noexcept { return value_.end(); }                              // NOLINT
    const_iterator end() const noexcept { return value_.end(); }                  // NOLINT
    const_iterator cend() const noexcept { return value_.cend(); }                // NOLINT
    reverse_iterator rbegin() noexcept { return value_.rbegin(); }                // NOLINT
    const_reverse_iterator rbegin() const noexcept { return value_.rbegin(); }    // NOLINT
    const_reverse_iterator crbegin() const noexcept { return value_.crbegin(); }  // NOLINT
    reverse_iterator rend() noexcept { return value_.rend(); }                    // NOLINT
    const_reverse_iterator rend() const noexcept { return value_.rend(); }        // NOLINT
    const_reverse_iterator crend() const noexcept { return value_.crend(); }      // NOLINT
    allocator_type get_allocator() const { return value_.get_allocator(); }       // NOLINT
};

template < typename CharT, typename Traits, typename Allocator >
basic_string< CharT, Traits, Allocator > operator+(basic_string< CharT, Traits, Allocator > const& lhs,
                                                   basic_string< CharT, Traits, Allocator > const& rhs)
{
    return basic_string< CharT, Traits, Allocator >(lhs).append(rhs);
}

template < typename CharT, typename Traits, typename Allocator >
basic_string< CharT, Traits, Allocator > operator+(basic_string< CharT, Traits, Allocator > const& lhs,
                                                   CharT const* rhs)
{
    return basic_string< CharT, Traits, Allocator >(lhs).append(rhs);
}

/// added by hanzhibo begin
template < typename CharT, typename Traits, typename Allocator >
basic_string< CharT, Traits, Allocator > operator+(basic_string< CharT, Traits, Allocator > const& lhs, CharT rhs)
{
    return basic_string< CharT, Traits, Allocator >(lhs).append(1, rhs);
}

template < typename CharT, typename Traits, typename Allocator >
basic_string< CharT, Traits, Allocator > operator+(CharT lhs, basic_string< CharT, Traits, Allocator > const& rhs)
{
    typedef basic_string< CharT, Traits, Allocator > __string_type;  // NOLINT
    typedef typename __string_type::size_type __size_type;           // NOLINT
    __string_type __str;                                             // NOLINT
    const __size_type __len = rhs.size();                            // NOLINT
    __str.reserve(__len + 1);
    __str.append(__size_type(1), lhs);
    __str.append(rhs);
    return __str;
}

template < typename CharT, typename Traits, typename Allocator >
basic_string< CharT, Traits, Allocator > operator+(basic_string< CharT, Traits, Allocator >&& lhs,
                                                   basic_string< CharT, Traits, Allocator >&& rhs)
{
    return std::move(lhs.append(rhs));
}

template < typename CharT, typename Traits, typename Allocator >
basic_string< CharT, Traits, Allocator > operator+(basic_string< CharT, Traits, Allocator >&& lhs,
                                                   basic_string< CharT, Traits, Allocator > const& rhs)
{
    return std::move(lhs.append(rhs));
}

template < typename CharT, typename Traits, typename Allocator >
basic_string< CharT, Traits, Allocator > operator+(basic_string< CharT, Traits, Allocator >&& lhs, CharT const* rhs)
{
    return std::move(lhs.append(rhs));
}

template < typename CharT, typename Traits, typename Allocator >
basic_string< CharT, Traits, Allocator > operator+(basic_string< CharT, Traits, Allocator >&& lhs, CharT rhs)
{
    return std::move(lhs.append(1, rhs));
}

template < typename CharT, typename Traits, typename Allocator >
basic_string< CharT, Traits, Allocator > operator+(basic_string< CharT, Traits, Allocator > const& lhs,
                                                   basic_string< CharT, Traits, Allocator >&& rhs)
{
    return std::move(rhs.insert(0, lhs));
}

template < typename CharT, typename Traits, typename Allocator >
basic_string< CharT, Traits, Allocator > operator+(CharT const* lhs, basic_string< CharT, Traits, Allocator >&& rhs)
{
    return std::move(rhs.insert(0, lhs));
}

template < typename CharT, typename Traits, typename Allocator >
basic_string< CharT, Traits, Allocator > operator+(CharT lhs, basic_string< CharT, Traits, Allocator >&& rhs)
{
    return std::move(rhs.insert(0, 1, lhs));
}
/// added by hanzhibo end

template < typename CharT, typename Traits, typename Allocator >
basic_string< CharT, Traits, Allocator > operator+(CharT const* lhs,
                                                   basic_string< CharT, Traits, Allocator > const& rhs)
{
    return basic_string< CharT, Traits, Allocator >(lhs).append(rhs);
}

template < typename CharT, typename Traits, typename Allocator >
basic_string< CharT, Traits, Allocator > operator+(std::basic_string< CharT, Traits, Allocator > const& lhs,
                                                   basic_string< CharT, Traits, Allocator > const& rhs)
{
    return basic_string< CharT, Traits, Allocator >(lhs).append(rhs);
}

template < typename CharT, typename Traits, typename Allocator >
basic_string< CharT, Traits, Allocator > operator+(basic_string< CharT, Traits, Allocator > const& lhs,
                                                   std::basic_string< CharT, Traits, Allocator > const& rhs)
{
    return basic_string< CharT, Traits, Allocator >(lhs).append(rhs);
}

template < typename CharT, typename Traits, typename Allocator >
bool operator==(basic_string< CharT, Traits, Allocator > const& lhs,
                basic_string< CharT, Traits, Allocator > const& rhs) noexcept
{
    return lhs.compare(rhs) == 0;
}

template < typename CharT, typename Traits, typename Allocator >
bool operator!=(basic_string< CharT, Traits, Allocator > const& lhs,
                basic_string< CharT, Traits, Allocator > const& rhs) noexcept
{
    return !(lhs == rhs);
}

template < typename CharT, typename Traits, typename Allocator >
bool operator<(basic_string< CharT, Traits, Allocator > const& lhs,
               basic_string< CharT, Traits, Allocator > const& rhs) noexcept
{
    return lhs.compare(rhs) < 0;
}

template < typename CharT, typename Traits, typename Allocator >
bool operator<=(basic_string< CharT, Traits, Allocator > const& lhs,
                basic_string< CharT, Traits, Allocator > const& rhs) noexcept
{
    return lhs.compare(rhs) <= 0;
}

template < typename CharT, typename Traits, typename Allocator >
bool operator>(basic_string< CharT, Traits, Allocator > const& lhs,
               basic_string< CharT, Traits, Allocator > const& rhs) noexcept
{
    return lhs.compare(rhs) > 0;
}

template < typename CharT, typename Traits, typename Allocator >
bool operator>=(basic_string< CharT, Traits, Allocator > const& lhs,
                basic_string< CharT, Traits, Allocator > const& rhs) noexcept
{
    return lhs.compare(rhs) >= 0;
}

/// added by hanzhibo begin
template < typename CharT, typename Traits, typename Allocator >
void swap(basic_string< CharT, Traits, Allocator >& lhs, basic_string< CharT, Traits, Allocator >& rhs)  // NOLINT
{
    lhs.swap(rhs);
}
/// added by hanzhibo end

template < typename CharT, typename Traits, typename Allocator >
bool operator==(basic_string< CharT, Traits, Allocator > const& lhs,
                std::basic_string< CharT, Traits, Allocator > const& rhs)
{
    return lhs.compare(rhs) == 0;
}

template < typename CharT, typename Traits, typename Allocator >
bool operator!=(basic_string< CharT, Traits, Allocator > const& lhs,
                std::basic_string< CharT, Traits, Allocator > const& rhs)
{
    return !(lhs == rhs);
}

template < typename CharT, typename Traits, typename Allocator >
bool operator<(basic_string< CharT, Traits, Allocator > const& lhs,
               std::basic_string< CharT, Traits, Allocator > const& rhs)
{
    return lhs.compare(rhs) < 0;
}

template < typename CharT, typename Traits, typename Allocator >
bool operator<=(basic_string< CharT, Traits, Allocator > const& lhs,
                std::basic_string< CharT, Traits, Allocator > const& rhs)
{
    return lhs.compare(rhs) <= 0;
}

template < typename CharT, typename Traits, typename Allocator >
bool operator>(basic_string< CharT, Traits, Allocator > const& lhs,
               std::basic_string< CharT, Traits, Allocator > const& rhs)
{
    return lhs.compare(rhs) > 0;
}

template < typename CharT, typename Traits, typename Allocator >
bool operator>=(basic_string< CharT, Traits, Allocator > const& lhs,
                std::basic_string< CharT, Traits, Allocator > const& rhs)
{
    return lhs.compare(rhs) >= 0;
}

template < typename CharT, typename Traits, typename Allocator >
bool operator==(std::basic_string< CharT, Traits, Allocator > const& lhs,
                basic_string< CharT, Traits, Allocator > const& rhs)
{
    return rhs.compare(lhs) == 0;
}

template < typename CharT, typename Traits, typename Allocator >
bool operator!=(std::basic_string< CharT, Traits, Allocator > const& lhs,
                basic_string< CharT, Traits, Allocator > const& rhs)
{
    return !(lhs == rhs);
}

template < typename CharT, typename Traits, typename Allocator >
bool operator<(std::basic_string< CharT, Traits, Allocator > const& lhs,
               basic_string< CharT, Traits, Allocator > const& rhs)
{
    return rhs.compare(lhs) > 0;
}

template < typename CharT, typename Traits, typename Allocator >
bool operator<=(std::basic_string< CharT, Traits, Allocator > const& lhs,
                basic_string< CharT, Traits, Allocator > const& rhs)
{
    return rhs.compare(lhs) <= 0;
}

template < typename CharT, typename Traits, typename Allocator >
bool operator>(std::basic_string< CharT, Traits, Allocator > const& lhs,
               basic_string< CharT, Traits, Allocator > const& rhs)
{
    return rhs.compare(lhs) < 0;
}

template < typename CharT, typename Traits, typename Allocator >
bool operator>=(std::basic_string< CharT, Traits, Allocator > const& lhs,
                basic_string< CharT, Traits, Allocator > const& rhs)
{
    return rhs.compare(lhs) >= 0;
}

template < typename CharT, typename Traits, typename Allocator >
bool operator==(CharT const* lhs, basic_string< CharT, Traits, Allocator > const& rhs)
{
    return rhs.compare(lhs) == 0;
}

template < typename CharT, typename Traits, typename Allocator >
bool operator==(basic_string< CharT, Traits, Allocator > const& lhs, CharT const* rhs)
{
    return lhs.compare(rhs) == 0;
}

template < typename CharT, typename Traits, typename Allocator >
bool operator!=(CharT const* lhs, basic_string< CharT, Traits, Allocator > const& rhs)
{
    return !(lhs == rhs);
}

template < typename CharT, typename Traits, typename Allocator >
bool operator!=(basic_string< CharT, Traits, Allocator > const& lhs, CharT const* rhs)
{
    return !(lhs == rhs);
}

template < typename CharT, typename Traits, typename Allocator >
bool operator<(CharT const* lhs, basic_string< CharT, Traits, Allocator > const& rhs)
{
    return rhs.compare(lhs) > 0;
}

template < typename CharT, typename Traits, typename Allocator >
bool operator<(basic_string< CharT, Traits, Allocator > const& lhs, CharT const* rhs)
{
    return lhs.compare(rhs) < 0;
}

template < typename CharT, typename Traits, typename Allocator >
bool operator<=(CharT const* lhs, basic_string< CharT, Traits, Allocator > const& rhs)
{
    return rhs.compare(lhs) >= 0;
}

template < typename CharT, typename Traits, typename Allocator >
bool operator<=(basic_string< CharT, Traits, Allocator > const& lhs, CharT const* rhs)
{
    return lhs.compare(rhs) <= 0;
}

template < typename CharT, typename Traits, typename Allocator >
bool operator>(CharT const* lhs, basic_string< CharT, Traits, Allocator > const& rhs)
{
    return rhs.compare(lhs) < 0;
}

template < typename CharT, typename Traits, typename Allocator >
bool operator>(basic_string< CharT, Traits, Allocator > const& lhs, CharT const* rhs)
{
    return lhs.compare(rhs) > 0;
}

template < typename CharT, typename Traits, typename Allocator >
bool operator>=(CharT const* lhs, basic_string< CharT, Traits, Allocator > const& rhs)
{
    return rhs.compare(lhs) <= 0;
}

template < typename CharT, typename Traits, typename Allocator >
bool operator>=(basic_string< CharT, Traits, Allocator > const& lhs, CharT const* rhs)
{
    return lhs.compare(rhs) >= 0;
}

template < class CharT, class Traits, class Allocator >
std::basic_ostream< CharT, Traits >& operator<<(std::basic_ostream< CharT, Traits >& os,
                                                basic_string< CharT, Traits, Allocator > const& str)
{
    return os << str.c_str();
}

template < class CharT, class Traits, class Allocator >
std::basic_istream< CharT, Traits >& operator>>(std::basic_istream< CharT, Traits >& is,
                                                basic_string< CharT, Traits, Allocator >& str)
{
    std::string value;
    is >> value;
    basic_string< CharT, Traits, Allocator >(value).swap(str);
    return is;
}

/// added by hanzhibo begin
template <>
inline int stoi(const basic_string< char >& str, size_t* pos, int base)  // NOLINT
{
    return std::stoi(str.value_, pos, base);
}

template <>
inline long stol(const basic_string< char >& str, size_t* pos, int base)  // NOLINT
{
    return std::stol(str.value_, pos, base);
}

template <>
inline long long stoll(const basic_string< char >& str, size_t* pos, int base)  // NOLINT
{
    return std::stoll(str.value_, pos, base);
}

template <>
inline unsigned long stoul(const basic_string< char >& str, size_t* pos, int base)  // NOLINT
{
    return std::stoul(str.value_, pos, base);
}

template <>
inline unsigned long long stoull(const basic_string< char >& str, size_t* pos, int base)  // NOLINT
{
    return std::stoull(str.value_, pos, base);
}

template <>
inline float stof(const basic_string< char >& str, size_t* pos)  // NOLINT
{
    return std::stof(str.value_, pos);
}

template <>
inline double stod(const basic_string< char >& str, size_t* pos)  // NOLINT
{
    return std::stod(str.value_, pos);
}

template <>
inline long double stold(const basic_string< char >& str, size_t* pos)  // NOLINT
{
    return std::stold(str.value_, pos);
}

#ifdef _GLIBCXX_USE_WCHAR_T
template <>
inline int stoi(const basic_string< wchar_t >& str, size_t* pos, int base)  // NOLINT
{
    return std::stoi(str.value_, pos, base);
}

template <>
inline long stol(const basic_string< wchar_t >& str, size_t* pos, int base)  // NOLINT
{
    return std::stol(str.value_, pos, base);
}

template <>
inline long long stoll(const basic_string< wchar_t >& str, size_t* pos, int base)  // NOLINT
{
    return std::stoll(str.value_, pos, base);
}

template <>
inline unsigned long stoul(const basic_string< wchar_t >& str, size_t* pos, int base)  // NOLINT
{
    return std::stoul(str.value_, pos, base);
}

template <>
inline unsigned long long stoull(const basic_string< wchar_t >& str, size_t* pos, int base)  // NOLINT
{
    return std::stoull(str.value_, pos, base);
}

template <>
inline float stof(const basic_string< wchar_t >& str, size_t* pos)  // NOLINT
{
    return std::stof(str.value_, pos);
}

template <>
inline double stod(const basic_string< wchar_t >& str, size_t* pos)  // NOLINT
{
    return std::stod(str.value_, pos);
}

template <>
inline long double stold(const basic_string< wchar_t >& str, size_t* pos)  // NOLINT
{
    return std::stold(str.value_, pos);
}
#endif
/// added by hanzhibo end

}  // namespace internal

/// @brief Representation of a contiguous sequence of narrow characters with custom allocator
///
/// @uptrace{SWS_CORE_03000, 2850bea732bf47e0de077ec71500baed26843ec6}
template < typename Allocator = std::allocator< char > >
using BasicString = internal::basic_string< char, std::char_traits< char >, Allocator >;

/// @brief Representation of a contiguous sequence of narrow characters
///
/// @uptrace{SWS_CORE_03001, 39f52021c5eccf3b69ca605ceab0cad93e37dce0}
using String = BasicString<>;

/// added by hanzhibo begin
#ifdef _GLIBCXX_USE_WCHAR_T
/// A string of @c wchar_t
typedef internal::basic_string< wchar_t > WString;  // NOLINT
#endif

// #ifdef _GLIBCXX_USE_CHAR8_T
//   /// A string of @c char8_t
//   typedef internal::basic_string<char8_t> U8String;
// #endif

#if __cplusplus >= 201103L
/// A string of @c char16_t
typedef internal::basic_string< char16_t > U16String;

/// A string of @c char32_t
typedef internal::basic_string< char32_t > U32String;
#endif
/// added by hanzhibo end

/// @brief Add overload of swap() for BasicString.
///
/// @param lhs  the first argument of the swap invocation
/// @param rhs  the second argument of the swap invocation
///
/// @uptrace{SWS_CORE_03296, a5fd3ddfd4e062be337f40e2fde1f9564fb6d460}
template < typename Allocator >
void swap(BasicString< Allocator >& lhs, BasicString< Allocator >& rhs)  // NOLINT
{
    lhs.swap(rhs);
}

template < class CharT, class Traits, class Allocator >
std::basic_istream< CharT, Traits >& getline  // NOLINT
    (std::basic_istream< CharT, Traits >& input, internal::basic_string< CharT, Traits, Allocator >& s, CharT delim)
{
    std::basic_string< CharT, Traits, Allocator > line;
    std::basic_istream< CharT, Traits >& result = std::getline(input, line, delim);
    s.assign(line.c_str());
    return result;
}

template < class CharT, class Traits, class Allocator >
std::basic_istream< CharT, Traits >& getline  // NOLINT
    (std::basic_istream< CharT, Traits >&& input, internal::basic_string< CharT, Traits, Allocator >& s, CharT delim)
{
    std::basic_string< CharT, Traits, Allocator > line;
    std::basic_istream< CharT, Traits >& result = std::getline(input, line, delim);
    s.assign(line.c_str());
    return result;
}

template < class CharT, class Traits, class Allocator >
std::basic_istream< CharT, Traits >& getline  // NOLINT
    (std::basic_istream< CharT, Traits >& input, internal::basic_string< CharT, Traits, Allocator >& s)
{
    std::basic_string< CharT, Traits, Allocator > line;
    std::basic_istream< CharT, Traits >& result = std::getline(input, line);
    s.assign(line.c_str());
    return result;
}

template < class CharT, class Traits, class Allocator >
std::basic_istream< CharT, Traits >& getline  // NOLINT
    (std::basic_istream< CharT, Traits >&& input, internal::basic_string< CharT, Traits, Allocator >& s)
{
    std::basic_string< CharT, Traits, Allocator > line;
    std::basic_istream< CharT, Traits >& result = std::getline(input, line);
    s.assign(line.c_str());
    return result;
}

inline String to_string(int value) { return String{std::to_string(value)}; }                 // NOLINT
inline String to_string(long value) { return String{std::to_string(value)}; }                // NOLINT
inline String to_string(long long value) { return String{std::to_string(value)}; }           // NOLINT
inline String to_string(unsigned value) { return String{std::to_string(value)}; }            // NOLINT
inline String to_string(unsigned long value) { return String{std::to_string(value)}; }       // NOLINT
inline String to_string(unsigned long long value) { return String{std::to_string(value)}; }  // NOLINT
inline String to_string(float value) { return String{std::to_string(value)}; }               // NOLINT
inline String to_string(double value) { return String{std::to_string(value)}; }              // NOLINT
inline String to_string(long double value) { return String{std::to_string(value)}; }         // NOLINT

#ifdef _GLIBCXX_USE_WCHAR_T
inline WString to_wstring(int value) { return WString{std::to_wstring(value)}; }                 // NOLINT
inline WString to_wstring(long value) { return WString{std::to_wstring(value)}; }                // NOLINT
inline WString to_wstring(long long value) { return WString{std::to_wstring(value)}; }           // NOLINT
inline WString to_wstring(unsigned value) { return WString{std::to_wstring(value)}; }            // NOLINT
inline WString to_wstring(unsigned long value) { return WString{std::to_wstring(value)}; }       // NOLINT
inline WString to_wstring(unsigned long long value) { return WString{std::to_wstring(value)}; }  // NOLINT
inline WString to_wstring(float value) { return WString{std::to_wstring(value)}; }               // NOLINT
inline WString to_wstring(double value) { return WString{std::to_wstring(value)}; }              // NOLINT
inline WString to_wstring(long double value) { return WString{std::to_wstring(value)}; }         // NOLINT
#endif

}  // namespace core
}  // namespace ara

namespace std {

/// @brief Specialization of std::hash for ara::core::String
template <>
struct hash< ara::core::String >
{
    using result_type = std::size_t;  // NOLINT

    /// @brief Function call operator to retrieve the hash value of ara::core::String.
    ///
    /// By providing this, ara::core::String can be used in std::unordered_map and std::unordered_set.
    ///
    /// @param k  the ara::core::String to be hashed.
    /// @returns the hash value
    result_type operator()(ara::core::String const& s) const noexcept
    {
        using std::hash;
        return hash< std::string >()(s.value_);
    }
};

/// added by hanzhibo begin
#ifdef _GLIBCXX_USE_WCHAR_T
/// @brief Specialization of std::hash for ara::core::WString
template <>
struct hash< ara::core::WString >
{
    using result_type = std::size_t;  // NOLINT

    /// @brief Function call operator to retrieve the hash value of ara::core::WString.
    ///
    /// By providing this, ara::core::WString can be used in std::unordered_map and std::unordered_set.
    ///
    /// @param k  the ara::core::WString to be hashed.
    /// @returns the hash value
    result_type operator()(ara::core::WString const& s) const noexcept
    {
        using std::hash;
        return hash< std::wstring >()(s.value_);
    }
};
#endif

#if __cplusplus >= 201103L
/// @brief Specialization of std::hash for ara::core::U16String
template <>
struct hash< ara::core::U16String >
{
    using result_type = std::size_t;  // NOLINT

    /// @brief Function call operator to retrieve the hash value of ara::core::U16String.
    ///
    /// By providing this, ara::core::U16String can be used in std::unordered_map and std::unordered_set.
    ///
    /// @param k  the ara::core::U16String to be hashed.
    /// @returns the hash value
    result_type operator()(ara::core::U16String const& s) const noexcept
    {
        using std::hash;
        return hash< std::u16string >()(s.value_);
    }
};

/// @brief Specialization of std::hash for ara::core::U32String
template <>
struct hash< ara::core::U32String >
{
    using result_type = std::size_t;  // NOLINT

    /// @brief Function call operator to retrieve the hash value of ara::core::U32String.
    ///
    /// By providing this, ara::core::U32String can be used in std::unordered_map and std::unordered_set.
    ///
    /// @param k  the ara::core::U32String to be hashed.
    /// @returns the hash value
    result_type operator()(ara::core::U32String const& s) const noexcept
    {
        using std::hash;
        return hash< std::u32string >()(s.value_);
    }
};
#endif
/// added by hanzhibo end

}  // namespace std

#endif  // ARA_CORE_STRING_H_
