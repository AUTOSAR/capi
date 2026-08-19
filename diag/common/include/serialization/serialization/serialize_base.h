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
/// @brief      Binding layer serialization basic header file
/// @details
/// @date       2024-11-13
/// @author     xuhua
/// @version    1.2.0
///
/// ================================================================

#ifndef __COM_NSOMEIP_SERIALIZED_BASE_H
#define __COM_NSOMEIP_SERIALIZED_BASE_H

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
#include "common_data_type.h"

/// @brief Namespace -- internal binding layer serialization
namespace isoft {
namespace serialize {
/// @brief Serialization error enumeration
enum ErrorCode
{
    kUnknownError = -1,  ///< Unknown error
    kInvalidWrite = -2,  ///< Invalid write
    kInvalidRead  = -3,  ///< Invalid read
    kInvalidPoint = -4,  ///< Invalid bit position
    kInvalidValue = -5   ///< Invalid value
};
/// @brief Byte order mode enumeration
enum class ByteOrder : uint8_t
{
    kMostSignificantByteFirst,  ///< Most significant byte first (big-endian)
    kMostSignificantByteLast,   ///< Least significant byte first (little-endian)
    kOpaque                     ///< Opaque (no conversion needed)
};
/// @brief Session handling mode enumeration
enum class SessionHandling : uint8_t
{
    kSessionHandlingActive,   ///< Enabled
    kSessionHandlingInactive  ///< Disabled
};

/// @brief String encoding mode enumeration
enum class StringEncoding : uint8_t
{
    kUTF8,  ///< UTF-8 encoding
    kUTF16  ///< UTF-16 encoding
};
/// @brief Conversion property type -- Default
struct TransformationProps
{
    /// @brief Get alignment @ref [TPS_MANI_03073]
    /// @details shall be either 8, 16, 32, 64, 128, or 256 @ref [constr_3356]
    /// @return uint16_t -- default no alignment @ref [SWS_CM_11263]
    static constexpr auto alignment() { return kInt16_8U; }  // NOLINT
    /// @brief Get byte order
    /// @return ByteOrder -- default kMostSignificantByteFirst @ref [SWS_CM_10270]
    /// [SWS_CM_10274] [SWS_CM_10276]
    static constexpr auto byteOrder() { return ByteOrder::kMostSignificantByteFirst; }  // NOLINT
    /// @brief Implement whether to serialize legacy string -- to be deprecated
    /// @return bool -- default false
    static constexpr auto implementsLegacyStringSerialization() { return bool{false}; }  // NOLINT
    /// @brief Get whether length field size is dynamic
    /// @return bool -- default false
    /// @retval true wire type 5-7 shall be used @ref [TPS_MANI_01186]
    /// @retval false wire type 4 shall be used @ref [TPS_MANI_01186]
    static constexpr auto isDynamicLengthFieldSize() { return bool{false}; }  // NOLINT
    /// @brief Get session handling mode
    /// @return SessionHandling -- default kSessionHandlingActive
    /// @retval kSessionHandlingActive The SOME/IP Transformer shall use session
    /// handling
    /// @retval kSessionHandlingInactive The SOME/IP Transformer doesn’t use
    /// session handling
    static constexpr auto sessionHandling() { return SessionHandling::kSessionHandlingActive; }  // NOLINT
    /// @brief Get sequence/associative map container length field placeholder @ref [TPS_MANI_03070]
    /// @details shall be either 0, 1, 2 or 4 @ref [constr_3353]
    /// @details 0 is only allowed to be used if a fixed size array @ref
    /// [constr_3447]
    /// @details in case of TLV, sizeof*LengthField shall have an identical value
    /// and greater than 0 @ref [constr_1628]
    /// @return uint8_t -- default 4 @ref [SWS_CM_00258] [SWS_CM_10258]
    static constexpr auto SizeofArrayLengthField() { return uint8_t{4}; }
    /// @brief Get sequence/associative map container length field placeholder (extended fixed-size array) @ref [TPS_MANI_03070]
    /// @details shall be either 0, 1, 2 or 4 @ref [constr_3353]
    /// @details 0 is only allowed to be used if a fixed size array @ref
    /// [constr_3447]
    /// @details in case of TLV, sizeof*LengthField shall have an identical value
    /// and greater than 0 @ref [constr_1628]
    /// @return uint8_t -- default 4 @ref [SWS_CM_00258] [SWS_CM_10258]
    static constexpr auto SizeofFixedArrayLengthField() { return uint8_t{4}; }
    /// @brief Get string length field placeholder @ref [TPS_MANI_03116]
    /// @details shall be either 0, 1, 2 or 4 @ref [constr_3372]
    /// @details in case of TLV, sizeof*LengthField shall have an identical value
    /// and greater than 0 @ref [constr_1628]
    /// @return uint8_t -- default 4 @ref [SWS_CM_10273] [SWS_CM_10275]
    static constexpr auto SizeofStringLengthField() { return uint8_t{4}; }  // NOLINT
    /// @brief Get structure length placeholder @ref [TPS_MANI_03071]
    /// @details shall be either 0, 1, 2 or 4 @ref [constr_3354]
    /// @details in case of TLV, sizeof*LengthField shall have an identical value
    /// and greater than 0 @ref [constr_1628]
    /// @return uint8_t -- default no length field @ref [SWS_CM_00253]
    /// [SWS_CM_00255]
    static constexpr auto SizeofStructLengthField() { return uint8_t{0}; }  // NOLINT
    /// @brief Get structure tag length placeholder @ref [TPS_MANI_03072]
    /// @details shall be either 0, 1, 2 or 4 @ref [constr_3355]
    /// @details in case of TLV, sizeof*LengthField shall have an identical value
    /// and greater than 0 @ref [constr_1628]
    /// @return uint8_t -- default 4
    static constexpr auto SzeofUnionLengthField() { return uint8_t{4}; }  // NOLINT
    /// @brief Get structure tag selector structure placeholder @ref [TPS_MANI_03074]
    /// @details shall be either 1, 2 or 4 @ref [constr_3357]
    /// @return uint8_t -- default 4
    static constexpr auto SizeofUnionTypeSelectorField() { return uint8_t{4}; }  // NOLINT
    /// @brief Get string encoding
    /// @return StringEncoding -- default kUTF8
    static constexpr auto stringEncoding() { return StringEncoding::kUTF8; }  // NOLINT
};
/// @brief Template type -- Length field type -- Default
/// @tparam size Number of bytes
template < uint8_t size >
struct LengthFieldType
{
    /// @brief Type alias -- Implementation type
    using type = size_t;  // NOLINT
};
/// @brief Template type -- Length field type -- 1 byte
template <>
struct LengthFieldType< 1 >
{
    /// @brief Type alias -- Implementation type
    using type = uint8_t;  // NOLINT
};
/// @brief Template type -- Length field type -- 2 bytes
template <>
struct LengthFieldType< 2 >
{
    /// @brief Type alias -- Implementation type
    using type = uint16_t;  // NOLINT
};
/// @brief Template type -- Length field type -- 4 bytes
template <>
struct LengthFieldType< 4 >
{
    /// @brief Type alias -- Implementation type
    using type = uint32_t;  // NOLINT
};
/// @brief Template type -- Length field type -- 8 bytes
template <>
struct LengthFieldType< kInt32_8 >
{
    /// @brief Type alias -- Implementation type
    using type = uint64_t;  // NOLINT
};
/// @brief Type alias -- Length field type
/// @tparam size Number of bytes
template < uint8_t size >
using length_field_t = typename LengthFieldType< size >::type;  // NOLINT

/// @brief Template type -- Determine if it is a basic (scalar) type
/// @tparam T Value type
template < typename T >
using IsScalar = std::is_scalar< T >;
/// @brief Template type -- Determine if it is a string type (no)
/// @tparam T Value type
template < typename T >
struct IsString : std::false_type
{
};
/// @brief Template type -- Determine if it is a string type (yes)
template <>
struct IsString< ara::core::String > : std::true_type
{
};
/// @brief Template type -- Determine if it is a structure (parameter list + enumerable members) type (no)
/// @tparam T Value type
/// @tparam Tagged A tag to enable SFINAE.
template < typename T, typename Tagged = void >
struct IsArgs : std::false_type
{
};
/// @brief Template type -- Determine if it is a structure (parameter list + enumerable members) type (yes)
template < typename T >
struct IsArgs< T, typename T::TagArgs > : std::true_type
{
};
/// @brief Template type -- Determine if it is a structure (parameter list + enumerable members) type (yes)
template < typename T >
struct IsArgs< T, typename T::IsArgsTag > : std::true_type
{
};
/// @brief Template type -- Determine if it is a structure (enumerable members) type (no)
/// @tparam T Value type
/// @tparam Tagged A tag to enable SFINAE.
template < typename T, typename Tagged = void >
struct IsEnumerable : std::false_type
{
};
/// @brief Template type -- Determine if it is a structure (enumerable members) type (yes)
template < typename T >
struct IsEnumerable< T, typename T::TagEnumerable > : std::true_type
{
};
/// @brief Template type -- Determine if it is a structure (enumerable members) type (yes)
template < typename T >
struct IsEnumerable< T, typename T::IsEnumerableTag > : std::true_type
{
};
/// @brief Template type -- Determine if it is a structure (with length field + enumerable members) type (yes)
template < typename T >
struct IsEnumerable< T, typename T::TagLengthField > : std::true_type
{
};
/// @brief Template type -- Determine if it is a structure (with length field + enumerable members) type (yes)
template < typename T >
struct IsEnumerable< T, typename T::IsLengthFieldTag > : std::true_type
{
};
/// @brief Template type --
/// Determine if it is a structure (with tag field + length field + enumerable members) type (no)
/// @tparam T Value type
/// @tparam Tagged A tag to enable SFINAE.
template < typename T, typename Tagged = void >
struct IsTLV : std::false_type
{
};
/// @brief Template type --
/// Determine if it is a structure (with tag field + length field + enumerable members) type (yes)
template < typename T >
struct IsTLV< T, typename T::TagTLV > : std::true_type
{
};
/// @brief Template type --
/// Determine if it is a structure (with tag field + length field + enumerable members) type (yes)
template < typename T >
struct IsTLV< T, typename T::IsTLVTag > : std::true_type
{
};
/// @brief Template type -- Determine if it is a structure (S2S conversion) type (no)
/// @tparam T Value type
/// @tparam Tagged A tag to enable SFINAE.
template < typename T, typename Tagged = void >
struct IsS2S : std::false_type
{
};
/// @brief Template type -- Determine if it is a structure (S2S conversion) type (yes)
template < typename T >
struct IsS2S< T, typename T::TagS2S > : std::true_type
{
};
/// @brief Template type -- Determine if it is a structure (S2S conversion) type (yes)
template < typename T >
struct IsS2S< T, typename T::IsS2STag > : std::true_type
{
};
/// @brief Template type -- Determine if it is a container type (no)
/// @tparam T Value type
/// @tparam C Container type
template < typename T, template < typename... > class C >
struct IsContainer : std::false_type
{
};
/// @brief Template type -- Determine if it is a container type (yes)
template < typename... T, template < typename... > class C >
struct IsContainer< C< T... >, C > : std::true_type
{
};
/// @brief Template type -- Determine if it is a sequence container type
/// @tparam T Value type
template < typename T >
using IsSequenceContainer
    = std::conditional_t< IsContainer< T, ara::core::Vector >::value || IsContainer< T, std::vector >::value,
                          std::true_type,
                          std::false_type >;
/// @brief Template type -- Determine if it is an associative map container type
/// @tparam T Value type
template < typename T >
using IsAssociativeContainer
    = std::conditional_t< IsContainer< T, ara::core::Map >::value || IsContainer< T, std::map >::value
                              || IsContainer< T, std::set >::value || IsContainer< T, std::unordered_map >::value
                              || IsContainer< T, std::unordered_set >::value,
                          std::true_type,
                          std::false_type >;
/// @brief Template type -- Determine whether extended fixed-size array length field can be extended (no)
/// @tparam T
template < typename T, typename = void >
struct EnableFixedArrayLengthField : std::false_type
{
};
/// @brief Template type -- Determine whether extended fixed-size array length field can be extended (yes)
/// @tparam T
template < typename T >
struct EnableFixedArrayLengthField<
    T,
    std::enable_if_t< std::is_same< uint8_t, decltype(T::SizeofFixedArrayLengthField()) >::value > > : std::true_type
{
};
/// @brief Template type -- Basic string (character + count)
/// @tparam TChar Data type
/// @tparam TSize Count type
template < typename TChar, typename TSize >
class BasicStringBorrow
{
public:
    /// @brief Type alias -- Data type
    using value_t = TChar;  // NOLINT
    /// @brief Type alias -- Count type
    using size_t = TSize;  // NOLINT
    /// @brief Constructor
    constexpr BasicStringBorrow() : data_(nullptr), size_(0) {}
    /// @brief Constructor
    /// @param[in] data Data pointer
    constexpr explicit BasicStringBorrow(value_t *data) : data_(data), size_(data ? strlen(data) : 0) {}
    /// @brief Constructor
    /// @param[in] data Data pointer
    /// @param[in] size Data count
    constexpr BasicStringBorrow(value_t *data, size_t size) : data_(data), size_(size) {}
    /// @brief Constructor
    /// @param[in] data Data pointer
    /// @param[in] size Data count
    constexpr BasicStringBorrow(value_t const *data, size_t size) : data_(const_cast< value_t * >(data)), size_(size) {}
    /// @brief Destructor
    ~BasicStringBorrow() = default;
    /// @brief Copy constructor
    /// @param other
    constexpr BasicStringBorrow(BasicStringBorrow const &other) = default;
    /// @brief Move constructor
    /// @param other
    constexpr BasicStringBorrow(BasicStringBorrow &&other) noexcept = default;
    /// @brief Copy assignment operator
    /// @param other
    /// @return BasicStringBorrow
    constexpr BasicStringBorrow &operator=(BasicStringBorrow const &other) = default;
    /// @brief Move assignment operator
    /// @param other
    /// @return BasicStringBorrow
    constexpr BasicStringBorrow &operator=(BasicStringBorrow &&other) noexcept = default;
    /// @brief Get data pointer
    /// @return Data pointer
    constexpr value_t *data() const { return data_; }  // NOLINT
    /// @brief Get data count
    /// @return Data count
    constexpr size_t size() const { return size_; }  // NOLINT
    /// @brief Get data begin pointer
    /// @return Data begin pointer
    constexpr value_t *begin() const { return data_; }  // NOLINT
    /// @brief Get data end pointer
    /// @return Data end pointer
    constexpr value_t *end() const { return data_ + size_; }  // NOLINT
    /// @brief Check if data is empty
    /// @return bool
    constexpr bool empty() const { return size_ == 0; }  // NOLINT
    /// @brief Convert to string
    /// @return Payload string
    constexpr auto ToString() const { return ara::core::StringView{reinterpret_cast< char * >(data_), size_}; }

private:
    /// @brief Data pointer
    value_t *data_;
    /// @brief Data count
    size_t size_;
};
/// @brief Type alias -- Raw string (pointer + length)
using StringBorrow = BasicStringBorrow< char, size_t >;
}  // namespace serialize
}  // namespace isoft
#endif
