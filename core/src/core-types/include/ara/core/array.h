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
/// @file       array.h
/// @brief      The definition and implementation of ara::core::array that represents a container which encapsulates fixed size arrays.
/// @details
/// @date       2022-06-22
/// @author     hanzhibo
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_CORE_ARRAY_H_
#define ARA_CORE_ARRAY_H_

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <tuple>
#include <type_traits>
#include <utility>

namespace ara {
namespace core {

namespace internal {

template < typename T, std::size_t N >  // NOLINT : keep template parameter naming style
struct ArrayHolder final
{
    using Type = T[N];

    static constexpr T const* ptr(Type const& data, std::size_t i) noexcept  // NOLINT
    {
        return &data[i];
    }

    static constexpr T* ptr(Type& data, std::size_t i) noexcept  // NOLINT
    {
        return &data[i];
    }
};

template < typename T >
struct ArrayHolder< T, 0 > final
{
    struct Type final
    {
    };

    static constexpr T const* ptr(Type const&, std::size_t) noexcept  // NOLINT
    {
        return nullptr;
    }

    static constexpr T* ptr(Type&, std::size_t) noexcept  // NOLINT
    {
        return nullptr;
    }
};

// This helper namespace basically re-implements C++17's std::is_nothrow_swappable.
// The key to this is that ADL has to be used for the swap() call within noexcept.
namespace adl {

using std::swap;

template < typename T >
struct is_nothrow_swappable  // NOLINT
    : std::integral_constant< bool, noexcept(swap(std::declval< T& >(), std::declval< T& >())) >
{
};

}  // namespace adl
}  // namespace internal

///
/// @brief Encapsulation of fixed size arrays
///
/// @tparam T the type of element in the array
/// @tparam N the number of elements in the array
/// @uptrace{SWS_CORE_01201, 826f673e2b9a8916eb342f5c439ac48317619cc2}
/// @uptrace{SWS_CORE_11200, 9927f7ee768de666e004648e58bbc9a2e77a2a99}
template < typename T, std::size_t N >  // NOLINT : keep template parameter naming style
class Array final
{
public:
    /// @uptrace{SWS_CORE_01210, 99cc123a2ce7304f05852d513ed91110ee505046}
    using reference = T&;  // NOLINT
    /// @uptrace{SWS_CORE_01211, 64e1716b399dc276e30d907c9d3ef171d5d1b359}
    using const_reference = T const&;  // NOLINT
    /// @uptrace{SWS_CORE_01212, 645ccc059c25c88b0d760c8f7fbd1e765c681591}
    using iterator = T*;  // NOLINT
    /// @uptrace{SWS_CORE_01213, e8bbe7baf14bcbf32d4ec3f50d52c4b634b19e6d}
    using const_iterator = T const*;  // NOLINT
    /// @uptrace{SWS_CORE_01214, 66005606ed669a96f11c8043830ebbeeac130a8b}
    using size_type = std::size_t;  // NOLINT
    /// @uptrace{SWS_CORE_01215, 862720e3bb55692213095d4566e83eaf00364296}
    using difference_type = std::ptrdiff_t;  // NOLINT
    /// @uptrace{SWS_CORE_01216, d1a0919796e82be0007dc0e8661cf88c780d95a8}
    using value_type = T;  // NOLINT
    /// @uptrace{SWS_CORE_01217, 790d3adf946dd4f1d79a595130ecfbda62eb97b1}
    using pointer = T*;  // NOLINT
    /// @uptrace{SWS_CORE_01218, ed786e23e9be33766d1adfae0e0f1bcee13cd1e2}
    using const_pointer = T const*;  // NOLINT
    /// @uptrace{SWS_CORE_01219, f77da4e85f4ba9476e39df9b67fd9de8d9f86c9c}
    using reverse_iterator = std::reverse_iterator< iterator >;  // NOLINT
    /// @uptrace{SWS_CORE_01220, a4da4d168b942db5bd4ce015817ffcf4a1cc3fc3}
    using const_reverse_iterator = std::reverse_iterator< const_iterator >;  // NOLINT

    // no explicit construct/copy/destroy for aggregate type

    // Need to support N==0, for which we have to introduce an indirection.
    using AH = internal::ArrayHolder< T, N >;
    typename AH::Type mData;

    /// @uptrace{SWS_CORE_01241, eac19149a10d181592bdc2be496413d76b5dab87}
    void fill(T const& u) { std::fill(begin(), end(), u); }  // NOLINT

    /// @uptrace{SWS_CORE_01242, 3ec9f9280e43a34846011a08d1e27f7b7b9d22e5}
    void swap(Array< T, N >& other) noexcept(internal::adl::is_nothrow_swappable< T >::value)  // NOLINT
    {
        std::swap_ranges(begin(), end(), other.begin());
    }

    // iterators:

    /// @uptrace{SWS_CORE_01250, 05fa9f0e93c0a4aecdb1a6e986b53c6a9222c435}
    iterator begin() noexcept { return AH::ptr(mData, 0); }  // NOLINT

    /// @uptrace{SWS_CORE_01251, 4edeb024b94dc16b53cc3dc5ad5aad4f5045352f}
    const_iterator begin() const noexcept { return AH::ptr(mData, 0); }  // NOLINT

    /// @uptrace{SWS_CORE_01252, 0b6d2df05d38a28557f4abd28acd140b114bd72a}
    iterator end() noexcept { return AH::ptr(mData, N); }  // NOLINT

    /// @uptrace{SWS_CORE_01253, 11c75c8ec3a10d0d26a4c0523894dd82ec791b1f}
    const_iterator end() const noexcept { return AH::ptr(mData, N); }  // NOLINT

    /// @uptrace{SWS_CORE_01254, 967c33d84ed477f2dfaea3ec87e3d8ed3a1b2f1d}
    reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }  // NOLINT

    /// @uptrace{SWS_CORE_01255, b58fd176629a66142e9a412b9623e30a724b6c2c}
    const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }  // NOLINT

    /// @uptrace{SWS_CORE_01256, 1c5fc2054472422c3420264aa9a9daf3d40e2221}
    reverse_iterator rend() noexcept { return reverse_iterator(begin()); }  // NOLINT

    /// @uptrace{SWS_CORE_01257, 29d1f1e7d3c08700b9085db78e0ed6014fb4e32b}
    const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }  // NOLINT

    /// @uptrace{SWS_CORE_01258, 1a38c53968c3dfd676ca06d371aaebd2400be692}
    const_iterator cbegin() const noexcept { return begin(); }  // NOLINT

    /// @uptrace{SWS_CORE_01259, 4904be7c515eb447a04a3bc188b215a0cc984d94}
    const_iterator cend() const noexcept { return end(); }  // NOLINT

    /// @uptrace{SWS_CORE_01260, 3592cd5fb249c95ec772dce88735ca0a586d6751}
    const_reverse_iterator crbegin() const noexcept { return rbegin(); }  // NOLINT

    /// @uptrace{SWS_CORE_01261, 14071188d25e30c70b8994c7955490499d94004b}
    const_reverse_iterator crend() const noexcept { return rend(); }  // NOLINT

    // capacity:

    /// @uptrace{SWS_CORE_01262, 1fd44d767a3d97462e2ceaac7cecbc8c4114c186}
    constexpr size_type size() const noexcept { return N; }  // NOLINT

    /// @uptrace{SWS_CORE_01263, 67c931c82bf7427bc3613ed52a8873a68afbf86e}
    constexpr size_type max_size() const noexcept { return N; }  // NOLINT

    /// @uptrace{SWS_CORE_01264, 08b8bba17a0eec62ed5e60d364a0958e7c8a443d}
    constexpr bool empty() const noexcept { return N == 0; }  // NOLINT

    // element access:

    /// @uptrace{SWS_CORE_01265, f9dab130da7e587035f2b42916d388490f5631a2}
    reference operator[](size_type n) { return *AH::ptr(mData, n); }

    /// @uptrace{SWS_CORE_01266, 8005067b2a67fa9eecb77921ec454d75e021bd91}
    constexpr const_reference operator[](size_type n) const { return *AH::ptr(mData, n); }

    /// @uptrace{SWS_CORE_01267, 5bee9645aeb6015b978fcac3a06330d5273479c7}
    reference front() { return *AH::ptr(mData, 0); }  // NOLINT

    /// @uptrace{SWS_CORE_01268, a202b052a1039d1c1342dd8d2e76eb9178642933}
    constexpr const_reference front() const { return *AH::ptr(mData, 0); }  // NOLINT

    /// @uptrace{SWS_CORE_01269, ba8a526975945b7cacfc918ad420141f8d8247c9}
    reference back() { return *AH::ptr(mData, N - 1); }  // NOLINT

    /// @uptrace{SWS_CORE_01270, 45292bfbb161ea950b9c2bbf81ef88e098b411b9}
    constexpr const_reference back() const { return *AH::ptr(mData, N - 1); }  // NOLINT

    /// @uptrace{SWS_CORE_01271, bf276c6bf30b2df34a16ebfd7ee1d5e60837e5d8}
    pointer data() noexcept { return AH::ptr(mData, 0); }  // NOLINT

    /// @uptrace{SWS_CORE_01272, f6fadbaa07cf208c8f70358420d5659b1e00c1ec}
    const_pointer data() const noexcept { return AH::ptr(mData, 0); }  // NOLINT
};

// comparison operators:

/// @uptrace{SWS_CORE_01290, 1ede443b8054005c0d7b21134ab36fe8115caf3a}
template < typename T, std::size_t N >  // NOLINT : keep template parameter naming style
bool operator==(Array< T, N > const& lhs, Array< T, N > const& rhs)
{
    return std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

/// @uptrace{SWS_CORE_01291, bfb156a873c713730105a0f21ae21641e866c63b}
template < typename T, std::size_t N >  // NOLINT : keep template parameter naming style
bool operator!=(Array< T, N > const& lhs, Array< T, N > const& rhs)
{
    return !operator==(lhs, rhs);
}

/// @uptrace{SWS_CORE_01292, fd5afb8a8dfab40dae51c13b100e0443e5f5fede}
template < typename T, std::size_t N >  // NOLINT : keep template parameter naming style
bool operator<(Array< T, N > const& lhs, Array< T, N > const& rhs)
{
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

/// @uptrace{SWS_CORE_01293, 1064e65555efb8342b6241072aff576fb7611154}
template < typename T, std::size_t N >  // NOLINT : keep template parameter naming style
bool operator>(Array< T, N > const& lhs, Array< T, N > const& rhs)
{
    return rhs < lhs;
}

/// @uptrace{SWS_CORE_01294, 1f293cc0bfec264653ad3edd051b5236deea7fd5}
template < typename T, std::size_t N >  // NOLINT : keep template parameter naming style
bool operator<=(Array< T, N > const& lhs, Array< T, N > const& rhs)
{
    return !(rhs < lhs);
}

/// @uptrace{SWS_CORE_01295, ef1446eaf775d6a6d677e2aef26861c420766683}
template < typename T, std::size_t N >  // NOLINT : keep template parameter naming style
bool operator>=(Array< T, N > const& lhs, Array< T, N > const& rhs)
{
    return !(lhs < rhs);
}

/// @uptrace{SWS_CORE_01296, 1e5f0f8dbe6ae2312a3ffaeb3745bcff53b47462}
template < typename T, std::size_t N >  // NOLINT : keep template parameter naming style
void swap(Array< T, N >& lhs, Array< T, N >& rhs) noexcept(noexcept(lhs.swap(rhs)))  // NOLINT
{
    lhs.swap(rhs);
}

// tuple interface (1/2)
// The C++14 standard prohibits extension of the std namespace with new overloads,
// so we need to put them into our own namespace and rely on ADL to have them found.

/// @uptrace{SWS_CORE_01282, 543f30cfb739abddc6aff2b5b77c00b578de02ec}
template < std::size_t I, typename T, std::size_t N >  // NOLINT : keep template parameter naming style
constexpr T& get(Array< T, N >& a) noexcept            // NOLINT
{
    static_assert(I < N, "index out of bounds");
    return a[I];
}

/// @uptrace{SWS_CORE_01283, ea9f3761570f3cfec1e8dd1c3bb1cf19c2c05ba5}
template < std::size_t I, typename T, std::size_t N >  // NOLINT : keep template parameter naming style
constexpr T&& get(Array< T, N >&& a) noexcept          // NOLINT
{
    return std::move(get< I >(a));
}

/// @uptrace{SWS_CORE_01284, d14cf2d8994ab793e3125f3148deadd66800ee5f}
template < std::size_t I, typename T, std::size_t N >    // NOLINT : keep template parameter naming style
constexpr T const& get(Array< T, N > const& a) noexcept  // NOLINT
{
    static_assert(I < N, "index out of bounds");
    return a[I];
}

}  // namespace core
}  // namespace ara

// tuple interface (2/2)
// Specialization of these symbols in the std namespace is permitted in C++14 by 17.6.4.2.1 [namespace.std],
// because they depend on a user-defined type (i.e. ara::core::Array).

namespace std {

/// @uptrace{SWS_CORE_01280, b13f10477dda0d7751cab89cadb627fe0ca09213}
template < typename T, size_t N >  // NOLINT : keep template parameter naming style
struct tuple_size< ara::core::Array< T, N > > : integral_constant< size_t, N >
{
};

/// @uptrace{SWS_CORE_01281, 797a28a86ed0b0161d2e426e1e4a4e6abcbc908a}
template < size_t I, typename T, size_t N >  // NOLINT : keep template parameter naming style
struct tuple_element< I, ara::core::Array< T, N > >
{
    static_assert(I < N, "index out of bounds");

    /// @uptrace{SWS_CORE_01285, 2270cad2101f6c862d2c2b8dea0ed32674189a72}
    using type = T;  // NOLINT
};

}  // namespace std

#endif  // ARA_CORE_ARRAY_H_
