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
/// @file       serialize_base.h
/// @brief
/// @details
/// @date       2022-08-17
/// @author     mazelin
/// @version    1.2.0
///
/// ================================================================

#pragma once

#include <algorithm>
#include <set>
#include <unordered_map>
#include <unordered_set>

#include "ara/core/array.h"
#include "ara/core/error_code.h"
#include "ara/core/map.h"
#include "ara/core/optional.h"
#include "ara/core/string.h"
#include "ara/core/vector.h"

namespace isoft {
namespace serialize {
template < typename T >
using IsBase = std::is_scalar< T >;

template < typename T >
struct IsString : std::false_type
{
};
template <>
struct IsString< ara::core::String > : std::true_type
{
};

template < typename T, typename Tagged = void >
struct IsEnumerable : std::false_type
{
};
template < typename T >
struct IsEnumerable< T, typename T::TagEnumerable > : std::true_type
{
};
template < typename T >
struct IsEnumerable< T, typename T::IsEnumerableTag > : std::true_type
{
};

template < typename T, typename Tagged = void >
struct HasLengthField : std::false_type
{
};
template < typename T >
struct HasLengthField< T, typename T::TagLengthField > : std::true_type
{
};
template < typename T >
struct HasLengthField< T, typename T::IsLengthFieldTag > : std::true_type
{
};

template < typename T, typename Tagged = void >
struct IsTLV : std::false_type
{
};
template < typename T >
struct IsTLV< T, typename T::TagTLV > : std::true_type
{
};
template < typename T >
struct IsTLV< T, typename T::IsTLVTag > : std::true_type
{
};

template < typename T, template < typename... > class C >
struct IsContainer : std::false_type
{
};
template < typename... T, template < typename... > class C >
struct IsContainer< C< T... >, C > : std::true_type
{
};

template < typename T >
using IsSequenceContainer
    = std::conditional_t< IsContainer< T, ara::core::Vector >::value || IsContainer< T, std::vector >::value,
                          std::true_type,
                          std::false_type >;

template < typename T >
using IsAssociativeContainer
    = std::conditional_t< IsContainer< T, ara::core::Map >::value || IsContainer< T, std::map >::value
                              || IsContainer< T, std::set >::value || IsContainer< T, std::unordered_map >::value
                              || IsContainer< T, std::unordered_set >::value,
                          std::true_type,
                          std::false_type >;

template < typename TChar, typename TSize >
class BasicStringBorrow
{
public:
    using value_t = TChar;
    using size_t  = TSize;
    constexpr BasicStringBorrow() : data_(nullptr), size_(0) {}
    constexpr BasicStringBorrow(const BasicStringBorrow&) = default;
    constexpr BasicStringBorrow(value_t* data) : data_(data), size_(data ? strlen(data) : 0) {}
    constexpr BasicStringBorrow(value_t* data, size_t size) : data_(data), size_(size) {}
    constexpr BasicStringBorrow(const value_t* data, size_t size) : data_(const_cast< value_t* >(data)), size_(size) {}
    constexpr BasicStringBorrow& operator=(const BasicStringBorrow&) = default;
    constexpr value_t* data() const { return data_; }
    constexpr size_t size() const { return size_; }
    constexpr value_t* begin() const { return data_; }
    constexpr value_t* end() const { return data_ + size_; }
    constexpr bool empty() const { return size_ == 0; }

private:
    value_t* data_;
    size_t size_;
};
using StringBorrow = BasicStringBorrow< char, size_t >;
}  // namespace serialize
}  // namespace isoft
