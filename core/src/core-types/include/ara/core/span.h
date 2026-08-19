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
/// @file       span.h
/// @brief
/// @details
/// @date       2022-06-24
/// @author     hanzhibo
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_CORE_SPAN_H_
#define ARA_CORE_SPAN_H_

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iostream>
#include <limits>
#include <type_traits>

#include "ara/core/array.h"
#include "ara/core/utility.h"

namespace ara {
namespace core {

namespace internal {
//*/
constexpr static bool kDebugSpan = false;
/*/
constexpr static bool kDebugSpan = true;
//*/

template < typename... Args >
static inline void xtrace(Args const&... args) // NOLINT : Whether to keep type name
{
    if (kDebugSpan) {
        using expander = int[];  // NOLINT
        static_cast< void >(expander{0, (static_cast< void >(std::cerr << args), 0)...});
    }
}
}  // namespace internal

/// @uptrace{SWS_CORE_01901, 5a978dc68584b09c28bef3dbd67a1883f72982c4}
constexpr std::size_t dynamic_extent = std::numeric_limits< std::size_t >::max(); // NOLINT : Whether to keep type name

template < typename T, std::size_t Extent >  // NOLINT : keep template parameter naming style
class Span;

namespace internal {

// Re-implementation of std::remove_cvref from C++20.
template < typename T >
struct remove_cvref // NOLINT : Whether to keep type name
{
    using type = std::remove_cv_t< std::remove_reference_t< T > >;  // NOLINT
};

// Re-implementation of std::remove_cvref_t from C++20.
template < typename T >
using remove_cvref_t = typename remove_cvref< T >::type; // NOLINT : Whether to keep type name

template < typename T >
struct is_ara_core_span_checker : public std::false_type // NOLINT : Whether to keep type name
{
};

template < typename T, std::size_t Extent >  // NOLINT : keep template parameter naming style
struct is_ara_core_span_checker< core::Span< T, Extent > > : public std::true_type
{
};

template < typename T >
struct is_ara_core_span : public is_ara_core_span_checker< remove_cvref_t< T > > // NOLINT : Whether to keep type name
{
};

template < typename T >
struct is_ara_core_array_checker : public std::false_type // NOLINT : Whether to keep type name
{
};

template < typename T, std::size_t N >  // NOLINT : keep template parameter naming style
struct is_ara_core_array_checker< core::Array< T, N > > : public std::true_type
{
};

template < typename T >
struct is_ara_core_array : public is_ara_core_array_checker< remove_cvref_t< T > > // NOLINT : Whether to keep type name
{
};

template < typename, typename = size_t >
struct is_complete : std::false_type // NOLINT : Whether to keep type name
{
};

template < typename T >
struct is_complete< T, decltype(sizeof(T)) > : std::true_type
{
};

// Re-implementation of std::void_t from C++20.
// We assume that the compiler implements CWG defect 1558.
template < typename... >
using void_t = void; // NOLINT : Whether to keep type name

template < typename, typename, typename, typename = void >
struct is_suitable_container : public std::false_type // NOLINT : Whether to keep type name
{
};

template < typename C, typename ArgT, typename ElementType >
struct is_suitable_container< C,
                              ArgT,
                              ElementType,
                              void_t< //// NOLINT : Whether to keep type name
                                  std::enable_if_t< !internal::is_ara_core_span< C >::value >,   //
                                  std::enable_if_t< !internal::is_ara_core_array< C >::value >,  //
                                  std::enable_if_t< !std::is_array< C >::value >,                //
                                  decltype(core::data(std::declval< ArgT >())),                  //
                                  decltype(core::size(std::declval< ArgT >())),                  //
                                  std::enable_if_t< std::is_convertible<
                                      std::remove_pointer_t< decltype(core::data(std::declval< ArgT >())) > (*)[],
                                      ElementType (*)[] >::value >  //
                                  >                                 //
                              > : public std::true_type
{
};

}  // namespace internal

/// @brief A view over a contiguous sequence of objects
///
/// @tparam T the type of elements in the Span
/// @tparam Extent the extent to use for this Span
///
/// @uptrace{SWS_CORE_01900, 7151715d4f2a188d02ff51a32b28e99bebd1de69}
template < typename T, std::size_t Extent = dynamic_extent >  // NOLINT : keep template parameter naming style
class Span
{
public:
    /// @briefMove constructor
    /// @param[in] object Span&& object
    Span(Span&& object) noexcept = default;
    /// @brief Move operation
    /// @param[in] object Span&& object
    /// @return Span&
    Span& operator=(Span&& object) noexcept = default;

private:
    static_assert(std::is_object< T >::value, "Span cannot contain reference or void types");
    static_assert(!std::is_abstract< T >::value, "Span cannot contain abstract types");
    static_assert(internal::is_complete< T >::value, "Span cannot contain incomplete types");

    template < typename U >
    using type_of_data = decltype(ara::core::data(std::declval< U >())); // NOLINT : Whether to keep type name

public:
    /// @uptrace{SWS_CORE_01911, 9b2cdcf63413605203c3a8d24c244d62599a7f50}
    using element_type = T; // NOLINT : Whether to keep type name
    /// @uptrace{SWS_CORE_01912, ff20db8cef8d0aee6ba11bc4a956407461eaad3e}
    using value_type = typename std::remove_cv< element_type >::type; // NOLINT : Whether to keep type name
    /// @uptrace{SWS_CORE_01913, e838611b4ebf55ad5e50063622721e095ffc51b7}
    using index_type = std::size_t; // NOLINT : Whether to keep type name
    /// @uptrace{SWS_CORE_01914, a89984e5cecfc137583516ba87d34f14547c1c38}
    using difference_type = std::ptrdiff_t; // NOLINT : Whether to keep type name
    /// @uptrace{SWS_CORE_01915, c6e83ee971c0ca24dfeb1a357a97492c12e711d3}
    using pointer = element_type*; // NOLINT : Whether to keep type name
    /// @uptrace{SWS_CORE_01916, 02ae7d03a1894cb45b041884185f06978ddc6506}
    using reference = element_type&; // NOLINT : Whether to keep type name
    /// @uptrace{SWS_CORE_01917, 9d7a306fec946207c66bde310c9c36df71d5ab8b}
    using iterator = element_type*; // NOLINT : Whether to keep type name
    /// @uptrace{SWS_CORE_01918, 1f3881a685524c5df97d28a2be375d1b359a2c5b}
    using const_iterator = element_type const*; // NOLINT : Whether to keep type name
    /// @uptrace{SWS_CORE_01919, 6a56b3b68967f14146b3c49ea133bd24783a87f7}
    using reverse_iterator = std::reverse_iterator< iterator >; // NOLINT : Whether to keep type name
    /// @uptrace{SWS_CORE_01920, e1666b1db43a0fbab5147cf8d15d7f4b33a80519}
    using const_reverse_iterator = std::reverse_iterator< const_iterator >; // NOLINT : Whether to keep type name

    // Not in C++ standard, but we need it for GMock's IsSize()
    /// @uptrace{SWS_CORE_01921, 23eac803ae0603e7cf770797ecd91d20a494b79d}
    using size_type = index_type; // NOLINT : Whether to keep type name

    /// @uptrace{SWS_CORE_01931, 077342bdb0a315fa9a2c595cbaa04a675b62c355}
    static constexpr index_type extent = Extent; // NOLINT : Whether to keep type name

    // 21.7.3.2, constructors, copy, and assignment

    /// @uptrace{SWS_CORE_01941, 7ef037598c4d81f99162ad2d027f816f10a6f06b}
    template < typename U = void,
               typename
               = typename std::enable_if< Extent == dynamic_extent || Extent == 0, // NOLINT : Whether to keep type name
                                          U >::type >
    constexpr Span() noexcept : mData_(nullptr)
    {
        internal::xtrace("__ Span()\n");
    }

    /// @uptrace{SWS_CORE_01942, 5fe4fa6decd43116effa3e814c5c7fcf3eedb4e6}
    constexpr Span(pointer ptr, index_type count) : mData_(ptr), mExtent_(count)
    {
        internal::xtrace("__ Span(T*, size_t)\n");
        internal::xtrace("____ ptr = ", ptr, ", count = ", count, " (ex = ", Extent, ")\n");
        if (Extent != dynamic_extent && count != Extent) {
            assert(!"inconsistent extent");
        }
    }

    /// @uptrace{SWS_CORE_01943, b3d4a54f3a4ebe82a4b98665565626fc65e421ff}
    constexpr Span(pointer firstElem, pointer lastElem)
        : Span(firstElem, static_cast< std::size_t >(lastElem - firstElem))
    {
        internal::xtrace("__ Span(T*, T*)\n");
        assert(std::distance(firstElem, lastElem) >= 0);
    }

    /// @uptrace{SWS_CORE_01944, 3c7bc43ca57f85a834bb22c9f3b15083c4e1fbb4}
    template <
        std::size_t N,  // NOLINT : keep template parameter naming style
        typename = typename std::enable_if<
            (Extent == dynamic_extent || Extent == N)
            && std::is_convertible< typename std::remove_pointer< type_of_data< element_type (&)[N] > >::type (*)[],
                                    element_type (*)[] >::value >::type >
    constexpr explicit Span(element_type (&arr)[N]) noexcept : Span(std::addressof(arr[0]), N)
    {
        internal::xtrace("__ Span(raw)\n");
    }

    /// @uptrace{SWS_CORE_01945, e6b1b5dfb2615f51a1dba8d8d82617fd94efd1fb}
    template < std::size_t N,  // NOLINT : keep template parameter naming style
               typename = typename std::enable_if<
                   (Extent == dynamic_extent || Extent == N)
                   && std::is_convertible< typename std::remove_pointer<
                                               decltype(std::declval< Array< value_type, N > >().data()) >::type (*)[],
                                           element_type (*)[] >::value >::type >
    constexpr explicit Span(Array< value_type, N >& arr) noexcept : Span(arr.data(), N)
    {
        internal::xtrace("__ Span(Array&)\n");
    }

    /// @uptrace{SWS_CORE_01946, 83fa19ccb1ba74e19a03a809dfc5e9f5a1f498b0}
    template <
        std::size_t N,  // NOLINT : keep template parameter naming style
        typename = typename std::enable_if<
            (Extent == dynamic_extent || Extent == N)
            && std::is_convertible< typename std::remove_pointer<
                                        decltype(std::declval< Array< value_type, N > const >().data()) >::type (*)[],
                                    element_type (*)[] >::value >::type >
    constexpr explicit Span(Array< value_type, N > const& arr) noexcept : Span(arr.data(), N)
    {
        internal::xtrace("__ Span(Array const&)\n");
    }

    /// @uptrace{SWS_CORE_01947, e9e33554bfd9ac85d5d2d9c63e08c06dac7dcaf8}
    template < typename Container,
               typename = typename std::enable_if<
                   internal::is_suitable_container< Container, Container&, element_type >::value >::type >
    constexpr explicit Span(Container& cont) : Span(core::data(cont), core::size(cont))
    {
        internal::xtrace("__ Span(Container&)\n");
        assert(std::distance(core::data(cont), core::data(cont) + core::size(cont)) >= 0);
    }

    /// @uptrace{SWS_CORE_01948, e1f2c122e9e37a660821a0ef308fed9e0accfb34}
    template < typename Container,
               typename = typename std::enable_if<
                   internal::is_suitable_container< Container, Container const&, element_type >::value >::type >
    constexpr explicit Span(Container const& cont) : Span(core::data(cont), core::size(cont))
    {
        internal::xtrace("__ Span(Container const&)\n");
        assert(std::distance(core::data(cont), core::data(cont) + core::size(cont)) >= 0);
    }

    /// @uptrace{SWS_CORE_01949, ce05d9fef4f4829e28a20cb3ba262fa144b89e3b}
    constexpr Span(Span const& other) noexcept = default;

    /// @brief Converting constructor
    ///
    /// This ctor allows assignment of a cv-Span<> from a normal Span<>,
    /// and also of a dynamic_extent-Span<> from a static extent-one.
    ///
    /// @uptrace{SWS_CORE_01950, d3c7bf4125f77e014a7527e0f2a58102a3ed9085}
    template < typename U,
               std::size_t N,  // NOLINT : keep template parameter naming style
               typename
               = typename std::enable_if< (Extent == dynamic_extent || Extent == N)
                                          && std::is_convertible< U (*)[], element_type (*)[] >::value >::type* >
    constexpr explicit Span(Span< U, N > const& s) noexcept : Span(s.data(), s.size())
    {
        internal::xtrace("__ converting ctor from N ", N, "\n");
    }

    /// @uptrace{SWS_CORE_01951, d22236f6dee6fa7fc365e3a3f85dc9a5edd8d639}
    ~Span() noexcept = default;

    // Not "constexpr" because that would make it also "const" on C++11 compilers.
    /// @uptrace{SWS_CORE_01952, cdc7d029e555580cfc5e9ab5fa0e04740f922c4d}
    Span& operator=(Span const& other) noexcept = default;

    // 21.7.3.3, subviews

    /// @uptrace{SWS_CORE_01961, 2b0c339aab947b676af78a491a19ce56ba03affd}
    template < std::size_t Count >                       // NOLINT : keep template parameter naming style
    constexpr Span< element_type, Count > first() const  // NOLINT
    {
        assert(Count <= size());

        return {mData_, Count};
    }

    /// @uptrace{SWS_CORE_01962, c9f0a11e9d0a86d08ef398fee31e5c008e28dc68}
    constexpr Span< element_type, dynamic_extent > first(index_type count) const  // NOLINT
    {
        assert(count <= size());

        return {mData_, count};
    }

    /// @uptrace{SWS_CORE_01963, fa570c393f2749f0344997d6dabfe592c6e998a3}
    template < std::size_t Count >                      // NOLINT : keep template parameter naming style
    constexpr Span< element_type, Count > last() const  // NOLINT
    {
        assert(Count <= size());

        return {mData_ + (size() - Count), Count};
    }

    /// @uptrace{SWS_CORE_01964, 29c030b850f9b22be34ea44918ceb583840b2f79}
    constexpr Span< element_type, dynamic_extent > last(index_type count) const  // NOLINT
    {
        assert(count <= size());

        return {mData_ + (size() - count), count};
    }

    /// @uptrace{SWS_CORE_01965, 124d4bfc20bad4bb683574b06165c3cbf3005027}
    template < std::size_t Offset, std::size_t Count = dynamic_extent >  // NOLINT : keep template parameter naming style
    constexpr auto subspan() const                                       // NOLINT
        -> Span< element_type,
                 (Count != dynamic_extent) ? Count : (Extent != dynamic_extent ? Extent - Offset : dynamic_extent) >
    {
        assert(Offset <= size());
        assert(Count == dynamic_extent || (Offset + Count <= size()));

        constexpr index_type E  // NOLINT
            = (Count != dynamic_extent) ? Count : (Extent != dynamic_extent ? Extent - Offset : dynamic_extent);

        return Span< element_type, E >(mData_ + Offset, Count != dynamic_extent ? Count : size() - Offset);
    }

    /// @uptrace{SWS_CORE_01966, 2158cac4976f3774fa759d79c19173c6fc07ed16}
    constexpr Span< element_type, dynamic_extent > subspan(  // NOLINT
        index_type offset,
        index_type count = dynamic_extent) const
    {
        assert(offset <= size());
        assert(count == dynamic_extent || (offset + count <= size()));

        return {mData_ + offset, count == dynamic_extent ? size() - offset : count};
    }

    // 21.7.3.4, observers

    /// @uptrace{SWS_CORE_01967, fa64f1d2168280ca8553c44bafae9fb6ad4f228c}
    constexpr index_type size() const noexcept { return mExtent_; }  // NOLINT

    /// @uptrace{SWS_CORE_01968, f56a378a133106629b8781e9a6fe978b8c86d665}
    constexpr index_type size_bytes() const noexcept { return mExtent_ * sizeof(T); }  // NOLINT

    /// @uptrace{SWS_CORE_01969, 421483a41083ae4ef6f7424a3f21b835b925df5c}
    constexpr bool empty() const noexcept { return mExtent_ == 0; }  // NOLINT

    // 21.7.3.5, element access

    /// @uptrace{SWS_CORE_01970, ca839baa5abd09cdf75931c8ab5e94078ab97341}
    constexpr reference operator[](index_type idx) const { return mData_[idx]; }

    /// @uptrace{SWS_CORE_01971, 081b61cd0155c2002a0a48bb62af4f36f6d42dd2}
    constexpr pointer data() const noexcept { return mData_; }  // NOLINT

    // 21.7.3.6, iterator support

    /// @uptrace{SWS_CORE_01972, 4db77cf1ff8b075a9e7a4e26c63b3ca09eeaae6e}
    constexpr iterator begin() const noexcept { return &mData_[0]; }  // NOLINT

    /// @uptrace{SWS_CORE_01973, 54a1f40bfbb6866e32ae32fa9f3ff1c220847946}
    constexpr iterator end() const noexcept { return &mData_[mExtent_]; }  // NOLINT

    /// @uptrace{SWS_CORE_01974, 5f720c1b8cd97a74e727d6e3007e736b41c72226}
    constexpr const_iterator cbegin() const noexcept { return &mData_[0]; }  // NOLINT

    /// @uptrace{SWS_CORE_01975, f39667fa8ec33386f7afd7fc6876ad5cfc9d32d9}
    constexpr const_iterator cend() const noexcept { return &mData_[mExtent_]; }  // NOLINT

    /// @uptrace{SWS_CORE_01976, c7a3dea366c5f402eb32521a0754687d3741ff5b}
    constexpr reverse_iterator rbegin() const noexcept { return reverse_iterator(end()); }  // NOLINT

    /// @uptrace{SWS_CORE_01977, b73b8a9693a2c4e1a90cc151b7af255e420017a7}
    constexpr reverse_iterator rend() const noexcept { return reverse_iterator(begin()); }  // NOLINT

    /// @uptrace{SWS_CORE_01978, 19bd59c265ce5e73fb682fa4f0ce1cd7018d37cc}
    constexpr const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(end()); }  // NOLINT

    /// @uptrace{SWS_CORE_01979, f87db44bdd57c7760d801f49b517ed82dccf88d0}
    constexpr const_reverse_iterator crend() const noexcept { return const_reverse_iterator(begin()); }  // NOLINT

private:
    pointer mData_{nullptr};
    index_type mExtent_{0U};
};

#if (__cpp_deduction_guides - 0) >= 201703L
template < typename T, std::size_t N >
Span(T (&)[N]) -> Span< T, N >;

template < typename T, std::size_t N >
Span(Array< T, N >&) -> Span< T, N >;

template < typename T, std::size_t N >
Span(Array< T, N > const&) -> Span< T const, N >;

template < typename Container >
Span(Container&) -> Span< typename Container::value_type >;

template < typename Container >
Span(Container const&) -> Span< const typename Container::value_type >;
#endif  // C++17 deduction guides

/// @uptrace{SWS_CORE_01980, aa8d6fc960bc5039b12be62f6a985ea6661d1cbc}
template < typename ElementType, std::size_t Extent >  // NOLINT : keep template parameter naming style
Span< Byte const, Extent == dynamic_extent ? dynamic_extent : sizeof(ElementType) * Extent > as_bytes(  // NOLINT
    Span< ElementType, Extent > s) noexcept
{
    return {reinterpret_cast< Byte const* >(s.data()), s.size_bytes()};
}

/// @uptrace{SWS_CORE_01981, d0c22361d2ecdda7f6f1393f9edcc1fe0b14500b}
template < typename ElementType, std::size_t Extent >  // NOLINT : keep template parameter naming style
Span< Byte, Extent == dynamic_extent ? dynamic_extent : sizeof(ElementType) * Extent > as_writable_bytes(  // NOLINT
    Span< ElementType, Extent > s) noexcept
{
    return {reinterpret_cast< Byte* >(s.data()), s.size_bytes()};
}

/// @uptrace{SWS_CORE_01990, f88d74b25661b4f9d27811c316ee23dff2ba1b22}
template < typename T >
constexpr Span< T > MakeSpan(T* ptr, typename Span< T >::index_type count)
{
    return Span< T >(ptr, count);
}

/// @uptrace{SWS_CORE_01991, 7af755718849322d9a436697443e10939960fb91}
template < typename T >
constexpr Span< T > MakeSpan(T* firstElem, T* lastElem)
{
    return Span< T >(firstElem, lastElem);
}

/// @uptrace{SWS_CORE_01992, 6a02f37a317300a25bc17a10816779b0fee18351}
template < typename T, std::size_t N >  // NOLINT : keep template parameter naming style
constexpr Span< T, N > MakeSpan(T (&arr)[N]) noexcept
{
    return Span< T, N >(arr);
}

/// @uptrace{SWS_CORE_01993, 139dac5093ff6a192d3e31cf1e617dc27c80ed3e}
template < typename Container >
constexpr Span< typename Container::value_type > MakeSpan(Container& cont)
{
    return Span< typename Container::value_type >(cont);
}

/// @uptrace{SWS_CORE_01994, 487485f06a870635a859b6aab59ebc0b54b255d7}
template < typename Container >
constexpr Span< typename Container::value_type const > MakeSpan(Container const& cont)
{
    return Span< typename Container::value_type const >(cont);
}

}  // namespace core
}  // namespace ara

#endif  // ARA_CORE_SPAN_H_
