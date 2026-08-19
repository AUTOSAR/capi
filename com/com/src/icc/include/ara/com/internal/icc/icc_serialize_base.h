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
/// @file       icc_serialize_base.h
/// @brief      Binding layer serialization basic header file
/// @details
/// @date       2021-12-31
/// @author     mazelin
/// @version    1.2.0
///
/// ================================================================

#ifndef __COM_ICC_SERIALIZED_BASE_H
#define __COM_ICC_SERIALIZED_BASE_H

#include <algorithm>
#include <set>
#include <unordered_map>
#include <unordered_set>

#include "ara/core/array.h"
#include "ara/core/error_code.h"
#include "ara/core/map.h"
#include "ara/core/optional.h"
#include "ara/core/string.h"
#include "ara/core/variant.h"
#include "ara/core/vector.h"

/// @brief Namespace -- internal binding layer serialization
namespace ara {
namespace com {
namespace internal {
namespace icc {
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
/// @brief Alignment bit occupation enumeration
enum class AlignmentSize : uint16_t
{
    kBit8   = 1 * CHAR_BIT,
    kBit16  = 2 * CHAR_BIT,
    kBit32  = 4 * CHAR_BIT,
    kBit64  = 8 * CHAR_BIT,
    kBit128 = 16 * CHAR_BIT,
    kBit256 = 32 * CHAR_BIT,
};
/// @brief Length field byte occupation enumeration
enum class FieldSize : uint8_t
{
    kByte0 = 0,
    kByte1 = 1,
    kByte2 = 2,
    kByte4 = 4,
    kByte8 = 8,
};
/// @brief Conversion element type
enum class Element : uint8_t
{
    kDefault,  ///< Default
    kEvent,    ///< Event
    kMethod,   ///< Method
    kField,    ///< Field
};
/// @brief Conversion properties type -- default
struct TransformationProps
{
    /// @brief Get alignment @ref [TPS_MANI_03073]
    /// @details shall be either 8, 16, 32, 64, 128, or 256 @ref [constr_3356]
    /// @return uint16_t -- default no alignment @ref [SWS_CM_11263]
    static constexpr auto alignment()  // NOLINT -- template interface name > naming convention
    {
        return static_cast< uint16_t >(AlignmentSize::kBit8);
    }
    /// @brief Get byte order
    /// @return ByteOrder -- default kMostSignificantByteFirst @ref [SWS_CM_10270] [SWS_CM_10274] [SWS_CM_10276]
    static constexpr auto byteOrder()  // NOLINT -- template interface name > naming convention
    {
        return ByteOrder::kMostSignificantByteFirst;
    }
    /// @brief Whether implementation serializes legacy strings -- to be deprecated
    /// @return bool -- default false
    static constexpr auto implementsLegacyStringSerialization()  // NOLINT -- template interface name > naming convention
    {
        return bool{false};
    }
    /// @brief Get whether length field size is dynamic
    /// @return bool -- default false
    /// @retval true wire type 5-7 shall be used @ref [TPS_MANI_01186]
    /// @retval false wire type 4 shall be used @ref [TPS_MANI_01186]
    static constexpr auto isDynamicLengthFieldSize()  // NOLINT -- template interface name > naming convention
    {
        return bool{false};
    }
    /// @brief Get session handling mode
    /// @return SessionHandling -- default kSessionHandlingActive
    /// @retval kSessionHandlingActive The SOME/IP Transformer shall use session handling
    /// @retval kSessionHandlingInactive The SOME/IP Transformer doesn’t use session handling
    static constexpr auto sessionHandling()  // NOLINT -- template interface name > naming convention
    {
        return SessionHandling::kSessionHandlingActive;
    }
    /// @brief Get sequence/associative map container length field size @ref [TPS_MANI_03070]
    /// @details shall be either 0, 1, 2 or 4 @ref [constr_3353]
    /// @details 0 is only allowed to be used if a fixed size array @ref [constr_3447]
    /// @details in case of TLV, sizeof*LengthField shall have an identical value and greater than 0 @ref [constr_1628]
    /// @return uint8_t -- default 4 @ref [SWS_CM_00258] [SWS_CM_10258]
    static constexpr auto sizeofArrayLengthField()  // NOLINT -- template interface name > naming convention
    {
        return static_cast< uint8_t >(FieldSize::kByte4);
    }
    /// @brief Get sequence/associative map container length field size (extended fixed-length array) @ref [TPS_MANI_03070]
    /// @details shall be either 0, 1, 2 or 4 @ref [constr_3353]
    /// @details 0 is only allowed to be used if a fixed size array @ref [constr_3447]
    /// @details in case of TLV, sizeof*LengthField shall have an identical value and greater than 0 @ref [constr_1628]
    /// @return uint8_t -- default 4 @ref [SWS_CM_00258] [SWS_CM_10258]
    static constexpr auto sizeofFixedArrayLengthField()  // NOLINT -- template interface name > naming convention
    {
        return static_cast< uint8_t >(FieldSize::kByte4);
    }
    /// @brief Get string length field size @ref [TPS_MANI_03116]
    /// @details shall be either 0, 1, 2 or 4 @ref [constr_3372]
    /// @details in case of TLV, sizeof*LengthField shall have an identical value and greater than 0 @ref [constr_1628]
    /// @return uint8_t -- default 4 @ref [SWS_CM_10273] [SWS_CM_10275]
    static constexpr auto sizeofStringLengthField()  // NOLINT -- template interface name > naming convention
    {
        return static_cast< uint8_t >(FieldSize::kByte4);
    }
    /// @brief Get string length field size @ref [TPS_MANI_03116]
    /// @details shall be either 0, 1, 2 or 4 @ref [constr_3372]
    /// @details in case of TLV, sizeof*LengthField shall have an identical value and greater than 0 @ref [constr_1628]
    /// @return uint8_t -- default 0 @ref [SWS_CM_10273] [SWS_CM_10275]
    static constexpr auto sizeofFixedStringLengthField()  // NOLINT -- template interface name > naming convention
    {
        return static_cast< uint8_t >(FieldSize::kByte0);
    }
    /// @brief Get struct length field size @ref [TPS_MANI_03071]
    /// @details shall be either 0, 1, 2 or 4 @ref [constr_3354]
    /// @details in case of TLV, sizeof*LengthField shall have an identical value and greater than 0 @ref [constr_1628]
    /// @return uint8_t -- default no length field @ref [SWS_CM_00253] [SWS_CM_00255]
    static constexpr auto sizeofStructLengthField()  // NOLINT -- template interface name > naming convention
    {
        return static_cast< uint8_t >(FieldSize::kByte0);
    }
    /// @brief Get union length field size @ref [TPS_MANI_03072]
    /// @details shall be either 0, 1, 2 or 4 @ref [constr_3355]
    /// @details in case of TLV, sizeof*LengthField shall have an identical value and greater than 0 @ref [constr_1628]
    /// @return uint8_t -- default 4
    static constexpr auto sizeofUnionLengthField()  // NOLINT -- template interface name > naming convention
    {
        return static_cast< uint8_t >(FieldSize::kByte4);
    }
    /// @brief Get union type selector field size @ref [TPS_MANI_03074]
    /// @details shall be either 1, 2 or 4 @ref [constr_3357]
    /// @return uint8_t -- default 4
    static constexpr auto sizeofUnionTypeSelectorField()  // NOLINT -- template interface name > naming convention
    {
        return static_cast< uint8_t >(FieldSize::kByte4);
    }
    /// @brief Get string encoding
    /// @return StringEncoding -- default kUTF8
    static constexpr auto stringEncoding()  // NOLINT -- template interface name > naming convention
    {
        return StringEncoding::kUTF8;
    }
    /// @brief Get element type
    /// @return Element -- default kDefault
    static constexpr auto element()  // NOLINT -- template interface name > naming convention
    {
        return Element::kDefault;
    }
};
/// @brief Template type -- length field type -- default
/// @tparam size Number of bytes
template < uint8_t size >
struct LengthField
{
    /// @brief Type alias -- implementation type
    using Type = size_t;
};
/// @brief Template type -- length field type -- 1 byte
template <>
struct LengthField< static_cast< uint8_t >(FieldSize::kByte1) >
{
    /// @brief Type alias -- implementation type
    using Type = uint8_t;
};
/// @brief Template type -- length field type -- 2 bytes
template <>
struct LengthField< static_cast< uint8_t >(FieldSize::kByte2) >
{
    /// @brief Type alias -- implementation type
    using Type = uint16_t;
};
/// @brief Template type -- length field type -- 4 bytes
template <>
struct LengthField< static_cast< uint8_t >(FieldSize::kByte4) >
{
    /// @brief Type alias -- implementation type
    using Type = uint32_t;
};
/// @brief Template type -- length field type -- 8 bytes
template <>
struct LengthField< static_cast< uint8_t >(FieldSize::kByte8) >
{
    /// @brief Type alias -- implementation type
    using Type = uint64_t;
};
/// @brief Type alias -- length field type
/// @tparam size Number of bytes
template < uint8_t size >
using LengthFieldType = typename LengthField< size >::Type;

/// @brief Template type -- check if basic (scalar) type
/// @tparam T Value type
template < typename T >
using IsScalar = std::is_scalar< T >;
/// @brief Template type -- check if string type (no)
/// @tparam T Value type
template < typename T >
struct IsString : std::false_type
{
};
/// @brief Template type -- check if string type (yes)
template <>
struct IsString< ara::core::String > : std::true_type
{
};
/// @brief Template type -- check if struct (parameter list + enumerable members) type (no)
/// @tparam T Value type
/// @tparam Tagged A tag to enable SFINAE.
template < typename T, typename Tagged = void >
struct IsArgs : std::false_type
{
};
/// @brief Template type -- check if struct (parameter list + enumerable members) type (yes)
template < typename T >
struct IsArgs< T, typename T::TagArgs > : std::true_type
{
};
/// @brief Template type -- check if struct (parameter list + enumerable members) type (yes)
template < typename T >
struct IsArgs< T, typename T::IsArgsTag > : std::true_type
{
};
/// @brief Template type -- check if struct (enumerable members) type (no)
/// @tparam T Value type
/// @tparam Tagged A tag to enable SFINAE.
template < typename T, typename Tagged = void >
struct IsEnumerable : std::false_type
{
};
/// @brief Template type -- check if struct (enumerable members) type (yes)
template < typename T >
struct IsEnumerable< T, typename T::TagEnumerable > : std::true_type
{
};
/// @brief Template type -- check if struct (enumerable members) type (yes)
template < typename T >
struct IsEnumerable< T, typename T::IsEnumerableTag > : std::true_type
{
};
/// @brief Template type -- check if struct (has length field + enumerable members) type (yes)
template < typename T >
struct IsEnumerable< T, typename T::TagLengthField > : std::true_type
{
};
/// @brief Template type -- check if struct (has length field + enumerable members) type (yes)
template < typename T >
struct IsEnumerable< T, typename T::IsLengthFieldTag > : std::true_type
{
};
/// @brief Template type -- check if struct (has tag field + has length field + enumerable members) type (no)
/// @tparam T Value type
/// @tparam Tagged A tag to enable SFINAE.
template < typename T, typename Tagged = void >
struct IsTLV : std::false_type
{
};
/// @brief Template type -- check if struct (has tag field + has length field + enumerable members) type (yes)
template < typename T >
struct IsTLV< T, typename T::TagTLV > : std::true_type
{
};
/// @brief Template type -- check if struct (has tag field + has length field + enumerable members) type (yes)
template < typename T >
struct IsTLV< T, typename T::IsTLVTag > : std::true_type
{
};
/// @brief Template type -- check if struct (S2S conversion) type (no)
/// @tparam T Value type
/// @tparam Tagged A tag to enable SFINAE.
template < typename T, typename Tagged = void >
struct IsS2S : std::false_type
{
};
/// @brief Template type -- check if struct (S2S conversion) type (yes)
template < typename T >
struct IsS2S< T, typename T::TagS2S > : std::true_type
{
};
/// @brief Template type -- check if struct (S2S conversion) type (yes)
template < typename T >
struct IsS2S< T, typename T::IsS2STag > : std::true_type
{
};
/// @brief Template type -- check if container type (no)
/// @tparam T Value type
/// @tparam C Container type
template < typename T, template < typename... > class C >
struct IsContainer : std::false_type
{
};
/// @brief Template type -- check if container type (yes)
template < typename... T, template < typename... > class C >
struct IsContainer< C< T... >, C > : std::true_type
{
};
/// @brief Template type -- check if sequence container type
/// @tparam T Value type
template < typename T >
using IsSequenceContainer
    = std::conditional_t< IsContainer< T, ara::core::Vector >::value || IsContainer< T, std::vector >::value,
                          std::true_type,
                          std::false_type >;
/// @brief Template type -- check if associative map container type
/// @tparam T Value type
template < typename T >
using IsAssociativeContainer
    = std::conditional_t< IsContainer< T, ara::core::Map >::value || IsContainer< T, std::map >::value
                              || IsContainer< T, std::set >::value || IsContainer< T, std::unordered_map >::value
                              || IsContainer< T, std::unordered_set >::value,
                          std::true_type,
                          std::false_type >;
/// @brief Template type -- check if extendable fixed-length array length field (no)
/// @tparam T
template < typename T, typename = void >
struct EnableFixedArrayLengthField : std::false_type
{
};
/// @brief Template type -- check if extendable fixed-length array length field (yes)
/// @tparam T
template < typename T >
struct EnableFixedArrayLengthField<
    T,
    std::enable_if_t< std::is_same< uint8_t, decltype(T::sizeofFixedArrayLengthField()) >::value > > : std::true_type
{
};
/// @brief Template type -- check if extendable fixed-length string length field (no)
/// @tparam T
template < typename T, typename = void >
struct EnableFixedStringLengthField : std::false_type
{
};
/// @brief Template type -- check if extendable fixed-length string length field (yes)
/// @tparam T
template < typename T >
struct EnableFixedStringLengthField<
    T,
    std::enable_if_t< std::is_same< uint8_t, decltype(T::sizeofFixedStringLengthField()) >::value > > : std::true_type
{
};
/// @brief Template type -- basic string (character + count)
/// @tparam TChar Data type
/// @tparam TSize Count type
template < typename TChar, typename TSize >
class BasicStringBorrow
{
public:
    /// @brief Type alias -- data type
    using ValueType = TChar;
    /// @brief Type alias -- count type
    using SizeType = TSize;
    /// @brief Constructor
    /// @param[in] data Data pointer
    constexpr explicit BasicStringBorrow(ValueType* data) noexcept : data_{data}, size_{data ? strlen(data) : 0} {}
    /// @brief Constructor
    /// @param[in] data Data pointer
    /// @param[in] size Data count
    constexpr BasicStringBorrow(ValueType* data, SizeType size) noexcept : data_{data}, size_{size} {}
    /// @brief Constructor
    /// @param[in] data Data pointer
    /// @param[in] size Data count
    constexpr BasicStringBorrow(ValueType const* data, SizeType size) noexcept
        : data_{const_cast< ValueType* >(data)}, size_{size}
    {
    }
    /// @brief Constructor
    constexpr BasicStringBorrow() noexcept = default;
    /// @brief Destructor
    ~BasicStringBorrow() noexcept = default;
    /// @brief Copy constructor
    /// @param other
    constexpr BasicStringBorrow(BasicStringBorrow const& other) noexcept = default;
    /// @brief Move constructor
    /// @param other
    constexpr BasicStringBorrow(BasicStringBorrow&& other) noexcept = default;
    /// @brief Copy assignment operator
    /// @param other
    /// @return BasicStringBorrow
    constexpr BasicStringBorrow& operator=(BasicStringBorrow const& other) noexcept = default;
    /// @brief Move assignment operator
    /// @param other
    /// @return BasicStringBorrow
    constexpr BasicStringBorrow& operator=(BasicStringBorrow&& other) noexcept = default;
    /// @brief Get data pointer
    /// @return Data pointer
    constexpr ValueType* Data() const noexcept { return data_; }
    /// @brief Get data count
    /// @return Data count
    constexpr SizeType Size() const noexcept { return size_; }
    /// @brief Get data start pointer
    /// @return Data start pointer
    constexpr ValueType* Begin() const noexcept { return data_; }
    /// @brief Get data end pointer
    /// @return Data end pointer
    constexpr ValueType* End() const noexcept { return data_ + size_; }
    /// @brief Check if data is empty
    /// @return bool
    constexpr bool Empty() const noexcept { return size_ == 0; }
    /// @brief Convert to string
    /// @return Payload string
    constexpr auto ToString() const noexcept { return ara::core::StringView{reinterpret_cast< char* >(data_), size_}; }

private:
    /// @brief Data pointer
    ValueType* data_{};
    /// @brief Data count
    SizeType size_{};
};
/// @brief Type alias -- raw string (pointer + length)
using StringBorrow = BasicStringBorrow< char, size_t >;
}  // namespace serialize
}  // namespace icc
}  // namespace internal
}  // namespace com
}  // namespace ara
#endif
