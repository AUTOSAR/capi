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
/// @file       utility.h
/// @brief
/// @details
/// @date       2022-06-24
/// @author     hanzhibo
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_CORE_UTILITY_H_
#define ARA_CORE_UTILITY_H_

#include <unistd.h>

#include <climits>
#include <cstddef>
#include <cstring>
#include <initializer_list>
#include <iterator>

namespace ara {
namespace core {

namespace internal {

#ifdef __GNUC__
    #ifndef ATTR_PACKED
        #define APD_PACKED __attribute__((packed))
    #else
        // Do here what is necessary for achieving SWS_CORE_10101 for this compiler!
        #define APD_PACKED
    #endif
#endif

/// @brief A non-integral binary type
///
/// This is a class-based implementation. It fulfils all the requirements
/// but also allows other, non-conforming uses, in particular initialization
/// with brackets, e.g. "Byte b(42)". This is not possible with a
/// std::byte-based implementation and thus should not be used.
///
/// It is implementation-defined whether this type can be used for type aliasing
/// without triggering Undefined Behavior.
/// If in doubt, instruct your compiler to exhibit non-strict aliasing behavior
/// with something like gcc's -fno-strict-aliasing
///
/// @uptrace{SWS_CORE_10100, c4835bfcc8694ac668f6500152742490ab126ccf}
/// @uptrace{SWS_CORE_10101, e93630150dfb395016b4cef2f0beddf52a8feabc}
/// @uptrace{SWS_CORE_10105, 155b7a1d1d1534dbfbadbe8144531979479a2fe4}
class Byte final
{
private:
    /// @uptrace{SWS_CORE_10102, 61c9f71cb18b70fcfe6889cfc9a6add826c2f628}
    unsigned char mValue_;

public:
    /// @brief Default constructor
    ///
    /// This constructor deliberately does not initialize anything, so that
    /// default-initialization leaves the instance with indeterminate value.
    /// It also deliberately is "defaulted on first declaration", so that this
    /// class does NOT have a "user-provided" default constructor, which is
    /// necessary for achieving the desired equivalence to std::byte.
    ///
    /// @uptrace{SWS_CORE_10104, 813bf4034fc18ec7f048e75209707733b5e81ccf}
    Byte() noexcept = default;

    /// @brief Converting constructor from 'unsigned char'
    ///
    /// @uptrace{SWS_CORE_10103, 48ca98e8d9653f7c0a03bae8c5a0d66ae3923f33}
    /// @uptrace{SWS_CORE_10106, eca5113d2894171109c4f6cfcc477b21818904a7}
    constexpr explicit Byte(unsigned char value) noexcept : mValue_(value) {}

    // The compiler-generated special member functions are fine
    // and would be equivalent to these:
    //
    // constexpr byte(byte const& other) noexcept = default;
    // constexpr byte(byte&& other) noexcept = default;
    // constexpr byte& operator=(byte const& other) noexcept = default;
    // constexpr byte& operator=(byte&& other) noexcept = default;
    // ~byte() noexcept = default;

    /// @brief Conversion operator to 'unsigned char'
    ///
    /// @uptrace{SWS_CORE_10107, 2fa27011261714412aefaff19748402e4bc40c4b}
    /// @uptrace{SWS_CORE_10108, 19a5f8ea09d3c3b807f73fc71d5da287845709dc}
    constexpr explicit operator unsigned char() const noexcept { return mValue_; }
} APD_PACKED;

/// @uptrace{SWS_CORE_10109, 38763df3b9556ea433a56be3cf79be73b08f0419}
constexpr bool operator==(Byte b1, Byte b2) noexcept
{
    return (static_cast< unsigned char >(b1) == static_cast< unsigned char >(b2));
}

/// @uptrace{SWS_CORE_10110, 2bf6cd7c7240d21252b078817d4b27029e24a1ec}
constexpr bool operator!=(Byte b1, Byte b2) noexcept
{
    return (static_cast< unsigned char >(b1) != static_cast< unsigned char >(b2));
}

}  // namespace internal

#if __cplusplus < 201703L
/// @uptrace{SWS_CORE_04200, a916c07ac2f79979f0759eabbc6af01d6dae54ef}
using Byte = internal::Byte;
#else
/// @uptrace{SWS_CORE_04200, a916c07ac2f79979f0759eabbc6af01d6dae54ef}
using Byte = std::byte;
#endif

// -------------------------------------------------------------------------
// Re-implementation of std::void_t from C++20.
// We assume that the compiler implements CWG defect 1558.
template < typename... >
using void_t = void;  // NOLINT

/// @uptrace{SWS_CORE_04011, 03993507b51b8e857c97c53f7799bb880d666227}
struct in_place_t  // NOLINT
{
    /// @uptrace{SWS_CORE_04012, c8900cce603253740e3fec2c155982ba5e2c5dac}
    explicit in_place_t() = default;
};

/// @uptrace{SWS_CORE_04013, 9580ac8ee9f1feda95dc64dd51303f65bb96e7df}
//  constexpr  in_place_t in_place{};
constexpr in_place_t in_place{}; // Remove inline for cross-compilation pass // NOLINT

/// @uptrace{SWS_CORE_04021, 834985a92e677cc4f17de0470a42f39c4c3f77b3}
template < typename T >
struct in_place_type_t  // NOLINT
{
    /// @uptrace{SWS_CORE_04022, a00d750e98f4e36d4bdb2ff2b001ae60651aac30}
    explicit in_place_type_t() = default;
};

/// @uptrace{SWS_CORE_04023, d2cb0959ef2b73f6bff740b21843e67a01cb1810}
template < typename T >
//  constexpr  in_place_type_t<T> in_place_type{};
constexpr in_place_type_t< T > in_place_type{}; // Remove inline for cross-compilation pass // NOLINT

/// @uptrace{SWS_CORE_04031, e31ae61996c38ffe5ff754fa1c234a2746764cc5}
template < std::size_t I >  // NOLINT : keep template parameter naming style
struct in_place_index_t     // NOLINT
{
    /// @uptrace{SWS_CORE_04032, ca29dc431e15f136622521622aaa789b00970aec}
    explicit in_place_index_t() = default;
};

/// @uptrace{SWS_CORE_04033, 4599e8ecfdf087e03ae7aa0506f21907ae42a8da}
template < std::size_t I >  // NOLINT : keep template parameter naming style
//  constexpr  in_place_index_t<I> in_place_index{};
constexpr in_place_index_t< I > in_place_index{}; // Remove inline for cross-compilation pass // NOLINT

// -------------------------------------------------------------------------

#if (__cpp_lib_nonmember_container_access - 0) >= 201411
using std::data;
using std::empty;
using std::size;
#else
// pre C++17

/// @brief Return a pointer to the block of memory that contains the elements of a container.
/// @tparam Container  a type with a data() method
/// @param c  an instance of Container
/// @returns a pointer to the first element of the container
///
/// @uptrace{SWS_CORE_04110, 6fed34991cc95e4dab1457009359ab8bc9d3b9b5}
template < typename Container >
constexpr auto data(Container& c) -> decltype(c.data())  // NOLINT
{
    return c.data();
}

/// @brief Return a const_pointer to the block of memory that contains the elements of a container.
/// @tparam Container  a type with a data() method
/// @param c  an instance of Container
/// @returns a pointer to the first element of the container
///
/// @uptrace{SWS_CORE_04111, 98350bc3b3650119a72530ab50fc159f7afc8845}
template < typename Container >
constexpr auto data(Container const& c) -> decltype(c.data())  // NOLINT
{
    return c.data();
}

/// @brief Return a pointer to the block of memory that contains the elements of a raw array.
/// @tparam T  the type of array elements
/// @tparam N  the number of elements in the array
/// @param array  reference to a raw array
/// @returns a pointer to the first element of the array
///
/// @uptrace{SWS_CORE_04112, 94b837811726938c8e3e3a40b913cda5adddba7c}
template < typename T, std::size_t N >     // NOLINT : keep template parameter naming style
constexpr T* data(T (&array)[N]) noexcept  // NOLINT
{
    return array;
}

/// @brief Return a pointer to the block of memory that contains the elements of a std::initializer_list.
/// @tparam E  the type of elements in the std::initializer_list
/// @param il  the std::initializer_list
/// @returns a pointer to the first element of the std::initializer_list
///
/// @uptrace{SWS_CORE_04113, 5c238ab2eeb1f29c26270e2803eac58fa05f189a}
template < typename E >
constexpr E const* data(std::initializer_list< E > il) noexcept  // NOLINT
{
    return il.begin();
}

/// @brief Return the size of a container.
/// @tparam Container  a type with a data() method
/// @param c  an instance of Container
/// @returns the size of the container
///
/// @uptrace{SWS_CORE_04120, bfed5262fd72083fcbe55b98daa92d1e26a0ea84}
template < typename Container >
constexpr auto size(Container const& c) -> decltype(c.size())  // NOLINT
{
    return c.size();
}

/// @brief Return the size of a raw array.
/// @tparam T  the type of array elements
/// @tparam N  the number of elements in the array
/// @param array  reference to a raw array
/// @returns the size of the array, i.e. N
///
/// @uptrace{SWS_CORE_04121, 36c87a2c17ceedf03b55124698a09756844bafbf}
template < typename T, std::size_t N >                    // NOLINT : keep template parameter naming style
constexpr std::size_t size(T const (&array)[N]) noexcept  // NOLINT
{
    static_cast< void >(array);
    return N;
}

/// @brief Return whether the given container is empty.
/// @tparam Container  a type with a empty() method
/// @param c  an instance of Container
/// @returns true if the container is empty, false otherwise
///
/// @uptrace{SWS_CORE_04130, cbf1b388d05dca6d46fad794a961e00e4b8102cc}
template < typename Container >
constexpr auto empty(Container const& c) -> decltype(c.empty())  // NOLINT
{
    return c.empty();
}

/// @brief Return whether the given raw array is empty.
///
/// As raw arrays cannot have zero elements in C++, this function
/// always returns false.
///
/// @tparam T  the type of array elements
/// @tparam N  the number of elements in the array
/// @param array  the raw array
/// @returns false
///
/// @uptrace{SWS_CORE_04131, 6f0ff6f54a515bd3679b4dde9a9432f0bdfd415a}
template < typename T, std::size_t N >              // NOLINT : keep template parameter naming style
constexpr bool empty(T const (&array)[N]) noexcept  // NOLINT
{
    static_cast< void >(array);
    return false;
}

/// @brief Return whether the given std::initializer_list is empty.
/// @tparam E  the type of elements in the std::initializer_list
/// @param il  the std::initializer_list
/// @returns true if the std::initializer_list is empty, false otherwise
///
/// @uptrace{SWS_CORE_04132, 7f52da25d9f08b8aca6e1f08b3350d0b7a777dfd}
template < typename E >
constexpr bool empty(std::initializer_list< E > il) noexcept  // NOLINT
{
    return il.size() == 0;
}
#endif

}  // namespace core
}  // namespace ara

#endif  // ARA_CORE_UTILITY_H_
