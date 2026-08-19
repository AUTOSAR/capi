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
/// @file       npc_serialize.h
/// @brief      Binding layer serialization header file
/// @details
/// @date       2021-12-31
/// @author     mazelin
/// @version    1.2.0
///
/// ================================================================

#ifndef __COM_NPC_SERIALIZE_H
#define __COM_NPC_SERIALIZE_H

#include <codecvt>
#include <locale>

#include "npc_serialize_stream.h"

/// @brief Namespace -- internal binding layer serialization
namespace ara {
namespace com {
namespace internal {
namespace npc {
namespace serialize {
/// @brief Serialize value list to payload (empty) and return number of bytes written or error code
/// @tparam TProps Property information
/// @tparam TValues Value list type
/// @param[in] payload Payload (empty)
/// @param[in] values Value list
/// @return Result -- >= 0: number of bytes read; < 0: error code;
template < typename TProps = TransformationProps, typename... TValues >
inline auto Serialize(std::nullptr_t payload, TValues const&... values) noexcept
{
    std::ignore = payload;
    StreamWriter< TProps, std::nullptr_t > viewer{};
    return SerializeImpl(viewer, values...);
}
/// @brief Serialize value list to payload and return number of bytes written or error code
/// @tparam TProps Property information
/// @tparam TPayload Payload type
/// @tparam TValues Value list type
/// @param[in] payload Payload
/// @param[in] values Value list
/// @return Result -- >= 0: number of bytes read; < 0: error code;
template < typename TProps = TransformationProps, typename TPayload, typename... TValues >
inline auto Serialize(TPayload& payload, TValues const&... values) noexcept
{
    TimeRecorder(TimeNM::Timer, "NPCSerialize");
    auto size{Serialize< TProps >(nullptr, values...)};
    if (size < 0) {
        return size;
    }
    StreamWriter< TProps, TPayload > writer{payload, static_cast< size_t >(size)};
    return SerializeImpl(writer, values...);
}
/// @brief Deserialize payload to value list and return number of bytes read or error code
/// @tparam TProps Property information
/// @tparam TPayload Payload type
/// @tparam TValues Value list type
/// @param[in] payload Payload
/// @param[in] values Value list
/// @return Result -- >= 0: number of bytes read; < 0: error code;
template < typename TProps = TransformationProps, typename TPayload, typename... TValues >
inline auto Deserialize(TPayload const& payload, TValues&... values) noexcept
{
    TimeRecorder(TimeNM::Timer, "NPCDeserialize");
    StreamReader< TProps, TPayload > reader{payload};
    return DeserializeImpl(reader, values...);
}
/// @brief Template type -- converter -- declaration
/// @tparam TStream Payload stream type
/// @tparam TValue Value type
template < typename TStream, typename TValue, typename = void >
struct Translator;
/// @brief Serialization implementation -- multiple parameters
/// @tparam TStream Payload stream type
/// @tparam TValue Value type
/// @tparam TValues Value list type
/// @param[in] stream Payload stream
/// @param[in] value Value
/// @param[in] values Value list
/// @return Result -- >= 0: number of bytes written; < 0: error code;
template < typename TStream, typename TValue, typename... TValues >
inline typename TStream::ResultType SerializeImpl(TStream& stream,
                                                  TValue const& value,
                                                  TValues const&... values) noexcept
{
    typename TStream::ResultType ret{};
    typename TStream::ResultType retn{};
    retn = Translator< TStream, TValue >::Write(stream, value);
    if (retn < 0) {
        return retn;
    }
    ret += retn;
    retn = SerializeImpl(stream, values...);
    if (retn < 0) {
        return retn;
    }
    ret += retn;
    return ret;
}
/// @brief Serialization implementation -- single parameter
/// @tparam TStream Payload stream type
/// @tparam TValue Value type
/// @param[in] stream Payload stream
/// @param[in] value Value
/// @return Result -- >= 0: number of bytes written; < 0: error code;
template < typename TStream, typename TValue >
inline typename TStream::ResultType SerializeImpl(TStream& stream, TValue const& value) noexcept
{
    typename TStream::ResultType ret{};
    typename TStream::ResultType retn{};
    retn = Translator< TStream, TValue >::Write(stream, value);
    if (retn < 0) {
        return retn;
    }
    ret += retn;
    return ret;
}
/// @brief Serialization implementation -- no parameters
/// @tparam TStream Payload stream type
/// @param[in] stream Payload stream
/// @return Result -- >= 0: number of bytes written; < 0: error code;
template < typename TStream >
inline typename TStream::ResultType SerializeImpl(TStream& stream) noexcept
{
    std::ignore = stream;
    return 0;
}
/// @brief Deserialization implementation -- multiple parameters
/// @tparam TStream Payload stream type
/// @tparam TValue Value type
/// @tparam TValues Value list type
/// @param[in] stream Payload stream
/// @param[in] value Value
/// @param[in] values Value list
/// @return Result -- >= 0: number of bytes read; < 0: error code;
template < typename TStream, typename TValue, typename... TValues >
inline typename TStream::ResultType DeserializeImpl(TStream& stream, TValue& value, TValues&... values) noexcept
{
    typename TStream::ResultType ret{};
    typename TStream::ResultType retn{};
    retn = Translator< TStream, TValue >::Read(stream, value);
    if (retn < 0) {
        return retn;
    }
    ret += retn;
    retn = DeserializeImpl(stream, values...);
    if (retn < 0) {
        return retn;
    }
    ret += retn;
    return ret;
}
/// @brief Deserialization implementation -- single parameter
/// @tparam TStream Payload stream type
/// @tparam TValue Value type
/// @param[in] stream Payload stream
/// @param[in] value Value
/// @return Result -- >= 0: number of bytes read; < 0: error code;
template < typename TStream, typename TValue >
inline typename TStream::ResultType DeserializeImpl(TStream& stream, TValue& value) noexcept
{
    typename TStream::ResultType ret{};
    typename TStream::ResultType retn{};
    retn = Translator< TStream, TValue >::Read(stream, value);
    if (retn < 0) {
        return retn;
    }
    ret += retn;
    return ret;
}
/// @brief Deserialization implementation -- no parameters
/// @tparam TStream Payload stream type
/// @param[in] stream Payload stream
/// @return Result -- >= 0: number of bytes read; < 0: error code;
template < typename TStream >
inline typename TStream::ResultType DeserializeImpl(TStream& stream) noexcept
{
    std::ignore = stream;
    return 0;
}

/// @brief Template type -- converter -- specialization for scalar (basic) types
/// @tparam TStream Payload stream type
/// @tparam TValue Value type
template < typename TStream, typename TValue >
struct Translator< TStream, TValue, std::enable_if_t< IsScalar< TValue >::value > >
{
    /// @brief Type alias -- payload stream type
    using StreamType = TStream;
    /// @brief Type alias -- value type
    using ValueType = TValue;
    /// @brief Type alias -- property information
    using PropsType = typename StreamType::PropsType;
    /// @brief Type alias -- result type
    using ResultType = typename StreamType::ResultType;
    /// @brief Type alias -- position type
    using PointType = typename StreamType::PointType;
    /// @brief Write
    /// @param[in] stream Payload stream
    /// @param[in] value Value
    /// @return Result -- >= 0: number of bytes written; < 0: error code;
    static ResultType Write(StreamType& stream, ValueType const& value) noexcept
    {
        auto reverseOrder{(PropsType::byteOrder() == ByteOrder::kMostSignificantByteFirst && IsLittleEndian())
                          || (PropsType::byteOrder() == ByteOrder::kMostSignificantByteLast && !IsLittleEndian())};
        return stream.Write(value, reverseOrder);
    }
    /// @brief Read
    /// @param[in] stream Payload stream
    /// @param[in] value Value
    /// @return Result -- >= 0: number of bytes read; < 0: error code;
    static ResultType Read(StreamType& stream, ValueType& value) noexcept
    {
        auto reverseOrder{(PropsType::byteOrder() == ByteOrder::kMostSignificantByteFirst && IsLittleEndian())
                          || (PropsType::byteOrder() == ByteOrder::kMostSignificantByteLast && !IsLittleEndian())};
        return stream.Read(value, reverseOrder);
    }
};

/// @brief Template type -- converter -- specialization for raw string (pointer + length) type
/// @tparam TStream Payload stream type
template < typename TStream >
struct Translator< TStream, StringBorrow >
{
    /// @brief Type alias -- payload stream type
    using StreamType = TStream;
    /// @brief Type alias -- value type
    using ValueType = StringBorrow;
    /// @brief Type alias -- property information
    using PropsType = typename StreamType::PropsType;
    /// @brief Type alias -- result type
    using ResultType = typename StreamType::ResultType;
    /// @brief Type alias -- position type
    using PointType = typename StreamType::PointType;
    /// @brief Write
    /// @param[in] stream Payload stream
    /// @param[in] value Value
    /// @return Result -- >= 0: number of bytes written; < 0: error code;
    static ResultType Write(StreamType& stream, ValueType const& value) noexcept { return stream.Write(value); }
    /// @brief Read
    /// @param[in] stream Payload stream
    /// @param[in] value Value
    /// @return Result -- >= 0: number of bytes read; < 0: error code;
    static ResultType Read(StreamType& stream, ValueType& value) noexcept { return stream.Read(value); }
};

/// @brief Template type -- converter -- specialization for raw string (array) type
/// @tparam TStream Payload stream type
/// @tparam TValue Value type
template < typename TStream, size_t kN >
struct Translator< TStream, char[kN] >
{
    /// @brief Type alias -- payload stream type
    using StreamType = TStream;
    /// @brief Type alias -- value type
    using ValueType = char[kN];
    /// @brief Type alias -- property information
    using PropsType = typename StreamType::PropsType;
    /// @brief Type alias -- result type
    using ResultType = typename StreamType::ResultType;
    /// @brief Type alias -- position type
    using PointType = typename StreamType::PointType;
    template < typename TProps = PropsType >
    static constexpr auto SizeofStringLengthField(
        std::enable_if_t< EnableFixedStringLengthField< TProps >::value >* = nullptr) noexcept
    {
        return PropsType::sizeofFixedStringLengthField();
    }
    template < typename TProps = PropsType >
    static constexpr auto SizeofStringLengthField(
        std::enable_if_t< !EnableFixedStringLengthField< TProps >::value >* = nullptr) noexcept
    {
        return PropsType::sizeofStringLengthField();
    }
    /// @brief Type alias -- length field type
    using LengthType = LengthFieldType< SizeofStringLengthField() >;
    static_assert(SizeofStringLengthField() == 0 || SizeofStringLengthField() == 1 || SizeofStringLengthField() == 2
                      || SizeofStringLengthField() == 4,
                  "sizeofFixedStringLengthField[0,1,2,4]");
    /// @brief Write -- UTF-8
    /// @param[in] stream Payload stream
    /// @param[in] value Value
    /// @return Result -- >= 0: number of bytes written; < 0: error code;
    template < typename TProps = PropsType >
    static ResultType Write(StreamType& stream,
                            ValueType const& value,
                            std::enable_if_t< TProps::stringEncoding() == StringEncoding::kUTF8 >* = nullptr) noexcept
    {
        auto reverseOrder{(PropsType::byteOrder() == ByteOrder::kMostSignificantByteFirst && IsLittleEndian())
                          || (PropsType::byteOrder() == ByteOrder::kMostSignificantByteLast && !IsLittleEndian())};
        ResultType ret{};
        ResultType retn{};
        auto data{value};
        auto size{kN};
        if (size > kUTF8BOM.size() && data[0] == kUTF8BOM[0] && data[1] == kUTF8BOM[1] && data[2] == kUTF8BOM[2]) {
            data += kUTF8BOM.size();
            size -= kUTF8BOM.size();
        }
        auto point{stream.Point()};
        LengthType length{};
        if (SizeofStringLengthField() != 0) {
            retn = stream.Write(length, reverseOrder);
            if (retn < 0) {
                return retn;
            }
            ret += retn;
        }
        if (PropsType::implementsLegacyStringSerialization()) {
            ;  // do nothing
        } else {
            for (size_t n{}; n < kUTF8BOM.size(); ++n) {
                retn = stream.Write(kUTF8BOM[n], reverseOrder);
                if (retn < 0) {
                    return retn;
                }
                ret += retn;
                length += retn;
            }
        }
        if (size != 0) {
            StringBorrow cstring(data, size);
            retn = stream.Write(cstring);
            if (retn < 0) {
                return retn;
            }
            ret += retn;
            length += retn;
        }
        for (size_t n{}; n < kUTF8End.size(); ++n) {
            retn = stream.Write(kUTF8End[n], reverseOrder);
            if (retn < 0) {
                return retn;
            }
            ret += retn;
            length += retn;
        }
        if (SizeofStringLengthField() != 0) {
            retn = stream.Write(length, reverseOrder, point);
            if (retn < 0) {
                return retn;
            }
        }
        return ret;
    }
    /// @brief Write -- UTF-16
    /// @param[in] stream Payload stream
    /// @param[in] value Value
    /// @return Result -- >= 0: number of bytes written; < 0: error code;
    template < typename TProps = PropsType >
    static ResultType Write(StreamType& stream,
                            ValueType const& value,
                            std::enable_if_t< TProps::stringEncoding() == StringEncoding::kUTF16 >* = nullptr) noexcept
    {
        auto reverseOrder{(PropsType::byteOrder() == ByteOrder::kMostSignificantByteFirst && IsLittleEndian())
                          || (PropsType::byteOrder() == ByteOrder::kMostSignificantByteLast && !IsLittleEndian())};
        ResultType ret{};
        ResultType retn{};
        std::string u8{value, value + kN};
        std::u16string u16 = std::wstring_convert< std::codecvt_utf8_utf16< char16_t >, char16_t >{}.from_bytes(u8);
        auto const* data{u16.data()};
        auto size{u16.size()};
        if (size > kUTF16BOM.size() && data[0] == kUTF16BOM[0]) {
            data += kUTF16BOM.size();
            size -= kUTF16BOM.size();
        }
        auto point{stream.Point()};
        LengthType length{};
        if (SizeofStringLengthField() != 0) {
            retn = stream.Write(length, reverseOrder);
            if (retn < 0) {
                return retn;
            }
            ret += retn;
        }
        if (PropsType::implementsLegacyStringSerialization()) {
            ;  // do nothing
        } else {
            for (size_t n{}; n < kUTF16BOM.size(); ++n) {
                retn = stream.Write(kUTF16BOM[n], reverseOrder);
                if (retn < 0) {
                    return retn;
                }
                ret += retn;
                length += retn;
            }
        }
        for (size_t n{}; n < size; ++n) {
            retn = stream.Write(data[n], reverseOrder);
            if (retn < 0) {
                return retn;
            }
            ret += retn;
            length += retn;
        }
        for (size_t n{}; n < kUTF16End.size(); ++n) {
            retn = stream.Write(kUTF16End[n], reverseOrder);
            if (retn < 0) {
                return retn;
            }
            ret += retn;
            length += retn;
        }
        if (SizeofStringLengthField() != 0) {
            retn = stream.Write(length, reverseOrder, point);
            if (retn < 0) {
                return retn;
            }
        }
        return ret;
    }
    /// @brief Read -- UTF-8
    /// @param[in] stream Payload stream
    /// @param[in] value Value
    /// @return Result -- >= 0: number of bytes read; < 0: error code;
    template < typename TProps = PropsType >
    static ResultType Read(StreamType& stream,
                           ValueType& value,
                           std::enable_if_t< TProps::stringEncoding() == StringEncoding::kUTF8 >* = nullptr) noexcept
    {
        auto reverseOrder{(PropsType::byteOrder() == ByteOrder::kMostSignificantByteFirst && IsLittleEndian())
                          || (PropsType::byteOrder() == ByteOrder::kMostSignificantByteLast && !IsLittleEndian())};
        ResultType ret{};
        ResultType retn{};
        LengthType length{};
        LengthType len{};
        if (SizeofStringLengthField() != 0) {
            retn = stream.Read(length, reverseOrder);
            if (retn < 0) {
                return retn;
            }
            ret += retn;
        }
        if (PropsType::implementsLegacyStringSerialization()) {
            ;  // do nothing
        } else {
            std::remove_cv_t< decltype(kUTF8BOM) > bom{};
            for (size_t n{}; n < bom.size(); ++n) {
                retn = stream.Read(bom[n], reverseOrder);
                if (retn < 0) {
                    return retn;
                }
                ret += retn;
            }
            if (bom != kUTF8BOM) {
                return ErrorCode::kInvalidValue;  // Read value (UTF-8 string) BOM format does not conform to specification
            }
        }
        StringBorrow cstring(value, kN);
        retn = stream.Read(cstring);
        if (retn < 0) {
            return retn;
        }
        ret += retn;
        len += retn;
        if (len != kN) {
            return ErrorCode::kInvalidValue;  // Read length (UTF-8 string) not equal to length field value
        }
        std::remove_cv_t< decltype(kUTF8End) > end{};
        for (size_t n{}; n < end.size(); ++n) {
            retn = stream.Read(end[n], reverseOrder);
            if (retn < 0) {
                return retn;
            }
            ret += retn;
        }
        if (end != kUTF8End) {
            return ErrorCode::kInvalidValue;  // Read value (UTF-8 string) termination format does not conform to specification
        }
        return ret;
    }
    /// @brief Read -- UTF-16
    /// @param[in] stream Payload stream
    /// @param[in] value Value
    /// @return Result -- >= 0: number of bytes read; < 0: error code;
    template < typename TProps = PropsType >
    static ResultType Read(StreamType& stream,
                           ValueType& value,
                           std::enable_if_t< TProps::stringEncoding() == StringEncoding::kUTF16 >* = nullptr) noexcept
    {
        auto reverseOrder{(PropsType::byteOrder() == ByteOrder::kMostSignificantByteFirst && IsLittleEndian())
                          || (PropsType::byteOrder() == ByteOrder::kMostSignificantByteLast && !IsLittleEndian())};
        ResultType ret{};
        ResultType retn{};
        LengthType length{};
        LengthType len{};
        if (SizeofStringLengthField() != 0) {
            retn = stream.Read(length, reverseOrder);
            if (retn < 0) {
                return retn;
            }
            ret += retn;
        }
        if (PropsType::implementsLegacyStringSerialization()) {
            ;  // do nothing
        } else {
            std::remove_cv_t< decltype(kUTF16BOM) > bom{};
            for (size_t n{}; n < bom.size(); ++n) {
                retn = stream.Read(bom[n], reverseOrder);
                if (retn < 0) {
                    return retn;
                }
                ret += retn;
            }
            if (bom != kUTF16BOM) {
                return ErrorCode::kInvalidValue;  // Read value (UTF-16 string) BOM format does not conform to specification
            }
        }
        uint16_t u16[kN]{};
        for (size_t n{}; n < kN; ++n) {
            retn = stream.Read(u16[n], reverseOrder);
            if (retn < 0) {
                return retn;
            }
            ret += retn;
            len += retn;
        }
        auto u8{std::wstring_convert< std::codecvt_utf8_utf16< char16_t >, char16_t >{}.to_bytes(
            reinterpret_cast< char16_t* >(u16), reinterpret_cast< char16_t* >(&u16[kN]))};
        if (u8.size() != kN) {
            return ErrorCode::kInvalidValue;  // Read length (UTF-16 string) not equal to length field value
        }
        memcpy(value, u8.data(), u8.size());
        std::remove_cv_t< decltype(kUTF16End) > end{};
        for (size_t n{}; n < end.size(); ++n) {
            retn = stream.Read(end[n], reverseOrder);
            if (retn < 0) {
                return retn;
            }
            ret += retn;
        }
        if (end != kUTF16End) {
            return ErrorCode::kInvalidValue;  // Read value (UTF-16 string) termination format does not conform to specification
        }
        return ret;
    }

private:
    /// @brief UTF-8 BOM marker
    static constexpr ara::core::Array< uint8_t, 3 > kUTF8BOM{0xEF, 0xBB, 0xBF};
    /// @brief UTF-8 terminator
    static constexpr ara::core::Array< uint8_t, 1 > kUTF8End{0x00};
    /// @brief UTF-16 BOM marker
    static constexpr ara::core::Array< uint16_t, 1 > kUTF16BOM{0xFEFF};
    /// @brief UTF-16 terminator
    static constexpr ara::core::Array< uint16_t, 1 > kUTF16End{0x0000};
};
template < typename TStream, size_t kN >
constexpr ara::core::Array< uint8_t, 3 > Translator< TStream, char[kN] >::kUTF8BOM;
template < typename TStream, size_t kN >
constexpr ara::core::Array< uint8_t, 1 > Translator< TStream, char[kN] >::kUTF8End;
template < typename TStream, size_t kN >
constexpr ara::core::Array< uint16_t, 1 > Translator< TStream, char[kN] >::kUTF16BOM;
template < typename TStream, size_t kN >
constexpr ara::core::Array< uint16_t, 1 > Translator< TStream, char[kN] >::kUTF16End;

/// @brief Template type -- converter -- specialization for string type
/// @tparam TStream Payload stream type
/// @tparam TValue Value type
template < typename TStream, typename TValue >
struct Translator< TStream, TValue, std::enable_if_t< IsString< TValue >::value > >
{
    /// @brief Type alias -- payload stream type
    using StreamType = TStream;
    /// @brief Type alias -- value type
    using ValueType = TValue;
    /// @brief Type alias -- property information
    using PropsType = typename StreamType::PropsType;
    /// @brief Type alias -- result type
    using ResultType = typename StreamType::ResultType;
    /// @brief Type alias -- position type
    using PointType = typename StreamType::PointType;
    /// @brief Type alias -- length field type
    using LengthType = LengthFieldType< PropsType::sizeofStringLengthField() >;
    static_assert(PropsType::sizeofStringLengthField() == 0 || PropsType::sizeofStringLengthField() == 1
                      || PropsType::sizeofStringLengthField() == 2 || PropsType::sizeofStringLengthField() == 4,
                  "sizeofStringLengthField[0,1,2,4]");
    /// @brief Write -- UTF-8
    /// @param[in] stream Payload stream
    /// @param[in] value Value
    /// @return Result -- >= 0: number of bytes written; < 0: error code;
    template < typename TProps = PropsType >
    static ResultType Write(StreamType& stream,
                            ValueType const& value,
                            std::enable_if_t< TProps::stringEncoding() == StringEncoding::kUTF8 >* = nullptr) noexcept
    {
        auto reverseOrder{(PropsType::byteOrder() == ByteOrder::kMostSignificantByteFirst && IsLittleEndian())
                          || (PropsType::byteOrder() == ByteOrder::kMostSignificantByteLast && !IsLittleEndian())};
        ResultType ret{};
        ResultType retn{};
        auto data{value.data()};
        auto size{value.size()};
        if (size > kUTF8BOM.size() && data[0] == kUTF8BOM[0] && data[1] == kUTF8BOM[1] && data[2] == kUTF8BOM[2]) {
            data += kUTF8BOM.size();
            size -= kUTF8BOM.size();
        }
        auto point{stream.Point()};
        LengthType length{};
        if (PropsType::sizeofStringLengthField() != 0) {
            retn = stream.Write(length, reverseOrder);
            if (retn < 0) {
                return retn;
            }
            ret += retn;
        }
        if (PropsType::implementsLegacyStringSerialization()) {
            ;  // do nothing
        } else {
            for (size_t n{}; n < kUTF8BOM.size(); ++n) {
                retn = stream.Write(kUTF8BOM[n], reverseOrder);
                if (retn < 0) {
                    return retn;
                }
                ret += retn;
                length += retn;
            }
        }
        if (size != 0) {
            StringBorrow cstring(data, size);
            retn = stream.Write(cstring);
            if (retn < 0) {
                return retn;
            }
            ret += retn;
            length += retn;
        }
        for (size_t n{}; n < kUTF8End.size(); ++n) {
            retn = stream.Write(kUTF8End[n], reverseOrder);
            if (retn < 0) {
                return retn;
            }
            ret += retn;
            length += retn;
        }
        if (PropsType::sizeofStringLengthField() != 0) {
            retn = stream.Write(length, reverseOrder, point);
            if (retn < 0) {
                return retn;
            }
        }
        return ret;
    }
    /// @brief Write -- UTF-16
    /// @param[in] stream Payload stream
    /// @param[in] value Value
    /// @return Result -- >= 0: number of bytes written; < 0: error code;
    template < typename TProps = PropsType >
    static ResultType Write(StreamType& stream,
                            ValueType const& value,
                            std::enable_if_t< TProps::stringEncoding() == StringEncoding::kUTF16 >* = nullptr) noexcept
    {
        auto reverseOrder{(PropsType::byteOrder() == ByteOrder::kMostSignificantByteFirst && IsLittleEndian())
                          || (PropsType::byteOrder() == ByteOrder::kMostSignificantByteLast && !IsLittleEndian())};
        ResultType ret{};
        ResultType retn{};
        std::string u8{value.begin(), value.end()};
        std::u16string u16 = std::wstring_convert< std::codecvt_utf8_utf16< char16_t >, char16_t >{}.from_bytes(u8);
        auto const* data{u16.data()};
        auto size{u16.size()};
        if (size > kUTF16BOM.size() && data[0] == kUTF16BOM[0]) {
            data += kUTF16BOM.size();
            size -= kUTF16BOM.size();
        }
        auto point{stream.Point()};
        LengthType length{};
        if (PropsType::sizeofStringLengthField() != 0) {
            retn = stream.Write(length, reverseOrder);
            if (retn < 0) {
                return retn;
            }
            ret += retn;
        }
        if (PropsType::implementsLegacyStringSerialization()) {
            ;  // do nothing
        } else {
            for (size_t n{}; n < kUTF16BOM.size(); ++n) {
                retn = stream.Write(kUTF16BOM[n], reverseOrder);
                if (retn < 0) {
                    return retn;
                }
                ret += retn;
                length += retn;
            }
        }
        for (size_t n{}; n < size; ++n) {
            retn = stream.Write(data[n], reverseOrder);
            if (retn < 0) {
                return retn;
            }
            ret += retn;
            length += retn;
        }
        for (size_t n{}; n < kUTF16End.size(); ++n) {
            retn = stream.Write(kUTF16End[n], reverseOrder);
            if (retn < 0) {
                return retn;
            }
            ret += retn;
            length += retn;
        }
        if (PropsType::sizeofStringLengthField() != 0) {
            retn = stream.Write(length, reverseOrder, point);
            if (retn < 0) {
                return retn;
            }
        }
        return ret;
    }
    /// @brief Read -- UTF-8
    /// @param[in] stream Payload stream
    /// @param[in] value Value
    /// @return Result -- >= 0: number of bytes read; < 0: error code;
    template < typename TProps = PropsType >
    static ResultType Read(StreamType& stream,
                           ValueType& value,
                           std::enable_if_t< TProps::stringEncoding() == StringEncoding::kUTF8 >* = nullptr) noexcept
    {
        auto reverseOrder{(PropsType::byteOrder() == ByteOrder::kMostSignificantByteFirst && IsLittleEndian())
                          || (PropsType::byteOrder() == ByteOrder::kMostSignificantByteLast && !IsLittleEndian())};
        ResultType ret{};
        ResultType retn{};
        LengthType length{};
        LengthType len{};
        if (PropsType::sizeofStringLengthField() != 0) {
            retn = stream.Read(length, reverseOrder);
            if (retn < 0) {
                return retn;
            }
            ret += retn;
        } else {
            length = std::numeric_limits< LengthType >::max();
        }
        if (PropsType::implementsLegacyStringSerialization()) {
            if (length < kUTF8End.size()) {
                return ErrorCode::kInvalidValue;  // Read value (UTF-8 string) length field value less than lower bound
            }
            length -= kUTF8End.size();
        } else {
            if (length < (kUTF8BOM.size() + kUTF8End.size())) {
                return ErrorCode::kInvalidValue;  // Read value (UTF-8 string) length field value less than lower bound
            }
            length -= (kUTF8BOM.size() + kUTF8End.size());
            std::remove_cv_t< decltype(kUTF8BOM) > bom{};
            for (size_t n{}; n < bom.size(); ++n) {
                retn = stream.Read(bom[n], reverseOrder);
                if (retn < 0) {
                    return retn;
                }
                ret += retn;
            }
            if (bom != kUTF8BOM) {
                return ErrorCode::kInvalidValue;  // Read value (UTF-8 string) BOM format does not conform to specification
            }
        }
        if (PropsType::sizeofStringLengthField() != 0) {
            try {
                value.resize(length);
            } catch (std::exception const& e) {
                return ErrorCode::kInvalidValue;  // Read value (UTF-8 string) failed to set length field value
            }
            if (!value.empty()) {
                StringBorrow cstring(value.data(), value.size());
                retn = stream.Read(cstring);
                if (retn < 0) {
                    return retn;
                }
                ret += retn;
                len += retn;
            }
            std::remove_cv_t< decltype(kUTF8End) > end{};
            for (size_t n{}; n < end.size(); ++n) {
                retn = stream.Read(end[n], reverseOrder);
                if (retn < 0) {
                    return retn;
                }
                ret += retn;
            }
            if (end != kUTF8End) {
                return ErrorCode::kInvalidValue;  // Read value (UTF-8 string) termination format does not conform to specification
            }
        } else {
            value.clear();
            while (len < length) {
                char c{};
                retn = stream.Read(c, reverseOrder);
                if (retn < 0) {
                    return retn;
                }
                ret += retn;
                if (c == kUTF8End[0]) {
                    length = len;
                    break;
                }
                len += retn;
                value += c;
            }
        }
        if (len != length) {
            return ErrorCode::kInvalidValue;  // Read length (UTF-8 string) not equal to length field value
        }
        return ret;
    }
    /// @brief Read -- UTF-16
    /// @param[in] stream Payload stream
    /// @param[in] value Value
    /// @return Result -- >= 0: number of bytes read; < 0: error code;
    template < typename TProps = PropsType >
    static ResultType Read(StreamType& stream,
                           ValueType& value,
                           std::enable_if_t< TProps::stringEncoding() == StringEncoding::kUTF16 >* = nullptr) noexcept
    {
        auto reverseOrder{(PropsType::byteOrder() == ByteOrder::kMostSignificantByteFirst && IsLittleEndian())
                          || (PropsType::byteOrder() == ByteOrder::kMostSignificantByteLast && !IsLittleEndian())};
        ResultType ret{};
        ResultType retn{};
        LengthType length{};
        LengthType len{};
        if (PropsType::sizeofStringLengthField() != 0) {
            retn = stream.Read(length, reverseOrder);
            if (retn < 0) {
                return retn;
            }
            ret += retn;
        } else {
            length = std::numeric_limits< LengthType >::max();
        }
        if (PropsType::implementsLegacyStringSerialization()) {
            if (length < kUTF16End.size() * 2) {
                return ErrorCode::kInvalidValue;  // Read value (UTF-16 string) length field value less than lower bound
            }
            length -= kUTF16End.size() * 2;
        } else {
            if (length < (kUTF16BOM.size() + kUTF16End.size()) * 2) {
                return ErrorCode::kInvalidValue;  // Read value (UTF-16 string) length field value less than lower bound
            }
            length -= (kUTF16BOM.size() + kUTF16End.size()) * 2;
            std::remove_cv_t< decltype(kUTF16BOM) > bom{};
            for (size_t n{}; n < bom.size(); ++n) {
                retn = stream.Read(bom[n], reverseOrder);
                if (retn < 0) {
                    return retn;
                }
                ret += retn;
            }
            if (bom != kUTF16BOM) {
                return ErrorCode::kInvalidValue;  // Read value (UTF-16 string) BOM format does not conform to specification
            }
        }
        std::u16string u16;
        if (PropsType::sizeofStringLengthField() != 0) {
            while (len < (length - 1)) {  // Not terminator (including odd)
                uint16_t c{};
                retn = stream.Read(c, reverseOrder);
                if (retn < 0) {
                    return retn;
                }
                ret += retn;
                len += retn;
                u16 += c;
            }
            std::remove_cv_t< decltype(kUTF16End) > end{};
            for (size_t n{}; n < end.size(); ++n) {
                retn = stream.Read(end[n], reverseOrder);
                if (retn < 0) {
                    return retn;
                }
                ret += retn;
            }
            if (end != kUTF16End) {
                return ErrorCode::kInvalidValue;  // Read value (UTF-16 string) termination format does not conform to specification
            }
            if (len == (length - 1)) {  // Ignore value (UTF-16 string), compatible with odd terminator
                uint8_t c{};
                retn = stream.Read(c, reverseOrder);
                if (retn < 0) {
                    return retn;
                }
                ret += retn;
                len += 1;
            }
        } else {
            while (true) {
                uint16_t c{};
                retn = stream.Read(c, reverseOrder);
                if (retn < 0) {
                    return retn;
                }
                ret += retn;
                if (c == kUTF16End[0]) {
                    length = len;
                    break;
                }
                len += retn;
                u16 += c;
            }
        }
        if (len != length) {
            return ErrorCode::kInvalidValue;  // Read length (UTF-16 string) not equal to length field value
        }
        value = std::wstring_convert< std::codecvt_utf8_utf16< char16_t >, char16_t >{}.to_bytes(u16);
        return ret;
    }

private:
    /// @brief UTF-8 BOM marker
    static constexpr ara::core::Array< uint8_t, 3 > kUTF8BOM{0xEF, 0xBB, 0xBF};
    /// @brief UTF-8 terminator
    static constexpr ara::core::Array< uint8_t, 1 > kUTF8End{0x00};
    /// @brief UTF-16 BOM marker
    static constexpr ara::core::Array< uint16_t, 1 > kUTF16BOM{0xFEFF};
    /// @brief UTF-16 terminator
    static constexpr ara::core::Array< uint16_t, 1 > kUTF16End{0x0000};
};
template < typename TStream, typename TValue >
constexpr ara::core::Array< uint8_t, 3 >
    Translator< TStream, TValue, std::enable_if_t< IsString< TValue >::value > >::kUTF8BOM;
template < typename TStream, typename TValue >
constexpr ara::core::Array< uint8_t, 1 >
    Translator< TStream, TValue, std::enable_if_t< IsString< TValue >::value > >::kUTF8End;
template < typename TStream, typename TValue >
constexpr ara::core::Array< uint16_t, 1 >
    Translator< TStream, TValue, std::enable_if_t< IsString< TValue >::value > >::kUTF16BOM;
template < typename TStream, typename TValue >
constexpr ara::core::Array< uint16_t, 1 >
    Translator< TStream, TValue, std::enable_if_t< IsString< TValue >::value > >::kUTF16End;

/// @brief Template type -- converter -- specialization for struct (enumerable members) type
/// @tparam TStream Payload stream type
/// @tparam TValue Value type
template < typename TStream, typename TValue >
struct Translator< TStream, TValue, std::enable_if_t< IsEnumerable< TValue >::value || IsArgs< TValue >::value > >
{
    /// @brief Type alias -- payload stream type
    using StreamType = TStream;
    /// @brief Type alias -- value type
    using ValueType = TValue;
    /// @brief Type alias -- property information
    using PropsType = typename StreamType::PropsType;
    /// @brief Type alias -- result type
    using ResultType = typename StreamType::ResultType;
    /// @brief Type alias -- position type
    using PointType = typename StreamType::PointType;
    /// @brief Type alias -- length field type
    using LengthType = LengthFieldType< PropsType::sizeofStructLengthField() >;
    static_assert(PropsType::sizeofStructLengthField() == 0 || PropsType::sizeofStructLengthField() == 1
                      || PropsType::sizeofStructLengthField() == 2 || PropsType::sizeofStructLengthField() == 4,
                  "sizeofStructLengthField[0,1,2,4]");
    /// @brief Write
    /// @param[in] stream Payload stream
    /// @param[in] value Value
    /// @return Result -- >= 0: number of bytes written; < 0: error code;
    static ResultType Write(StreamType& stream, ValueType const& value) noexcept
    {
        auto reverseOrder{(PropsType::byteOrder() == ByteOrder::kMostSignificantByteFirst && IsLittleEndian())
                          || (PropsType::byteOrder() == ByteOrder::kMostSignificantByteLast && !IsLittleEndian())};
        ResultType ret{};
        ResultType retn{};
        auto point{stream.Point()};
        LengthType length{};
        if (PropsType::sizeofStructLengthField() != 0 && !IsArgs< ValueType >::value) {
            retn = stream.Write(length, reverseOrder);
            if (retn < 0) {
                return retn;
            }
            ret += retn;
        }
        Writer writer{stream};
        const_cast< ValueType& >(value).enumerate(writer);
        retn = writer.ret;
        if (retn < 0) {
            return retn;
        }
        ret += retn;
        length += retn;
        if (PropsType::sizeofStructLengthField() != 0 && !IsArgs< ValueType >::value) {
            retn = stream.Write(length, reverseOrder, point);
            if (retn < 0) {
                return retn;
            }
        }
        return ret;
    }
    /// @brief Read
    /// @param[in] stream Payload stream
    /// @param[in] value Value
    /// @return Result -- >= 0: number of bytes read; < 0: error code;
    template < typename TProps = PropsType >
    static ResultType Read(StreamType& stream, ValueType& value) noexcept
    {
        auto reverseOrder{(PropsType::byteOrder() == ByteOrder::kMostSignificantByteFirst && IsLittleEndian())
                          || (PropsType::byteOrder() == ByteOrder::kMostSignificantByteLast && !IsLittleEndian())};
        ResultType ret{};
        ResultType retn{};
        LengthType length{};
        LengthType len{};
        if (PropsType::sizeofStructLengthField() != 0 && !IsArgs< ValueType >::value) {
            retn = stream.Read(length, reverseOrder);
            if (retn < 0) {
                return retn;
            }
            ret += retn;
        }
        Reader reader{stream};
        value.enumerate(reader);
        retn = reader.ret;
        if (retn < 0) {
            return retn;
        }
        ret += retn;
        len += retn;
        if (PropsType::sizeofStructLengthField() != 0 && !IsArgs< ValueType >::value) {
            if (len < length) {
                ara::core::Vector< char > _(length - len);
                StringBorrow cstring(_.data(), _.size());
                retn = stream.Read(cstring);  // Read length (struct) less than length field value (skip)
                if (retn < 0) {
                    return retn;
                }
                ret += retn;
                len += retn;
            }
            if (len != length) {
                return ErrorCode::kInvalidValue;  // Read length (struct) not equal to length field value
            }
        }
        return ret;
    }

private:
    /// @brief Writer
    struct Writer
    {
        /// @brief Payload stream
        StreamType& stream;
        /// @brief Return result
        ResultType ret{};
        /// @brief Operator -- call -- write
        /// @tparam T Value type
        /// @param[in] value Value
        template < typename T >
        void operator()(T const& value) noexcept
        {
            if (ret < 0) {
                return;
            }
            auto retn{Translator< StreamType, T >::Write(stream, value)};
            if (retn < 0) {
                ret = retn;
                return;
            }
            ret += retn;
        }
    };
    /// @brief Reader
    struct Reader
    {
        /// @brief Payload stream
        StreamType& stream;
        /// @brief Return result
        ResultType ret{};
        /// @brief Operator -- call -- read
        /// @tparam T Value type
        /// @param[in] value Value
        template < typename T >
        void operator()(T& value) noexcept
        {
            if (ret < 0) {
                return;
            }
            auto retn{Translator< StreamType, T >::Read(stream, value)};
            if (retn < 0) {
                ret = retn;
                return;
            }
            ret += retn;
        }
    };
};

/// @brief Template type -- converter -- specialization for struct (with tag field + length field + enumerable members) type
/// @tparam TStream Payload stream type
/// @tparam TValue Value type
template < typename TStream, typename TValue >
struct Translator< TStream, TValue, std::enable_if_t< IsTLV< TValue >::value > >
{
    /// @brief Type alias -- payload stream type
    using StreamType = TStream;
    /// @brief Type alias -- value type
    using ValueType = TValue;
    /// @brief Type alias -- property information
    using PropsType = typename StreamType::PropsType;
    /// @brief Type alias -- result type
    using ResultType = typename StreamType::ResultType;
    /// @brief Type alias -- position type
    using PointType = typename StreamType::PointType;
    /// @brief Type alias -- length field type
    using LengthType = LengthFieldType< PropsType::sizeofStructLengthField() >;
    /// @brief Type alias -- type field length type
    using TypeLengthType = LengthType;
    /// @brief Type alias -- tag field type
    using TagFieldType = uint16_t;
    static_assert(PropsType::sizeofStructLengthField() == 1 || PropsType::sizeofStructLengthField() == 2
                      || PropsType::sizeofStructLengthField() == 4,
                  "sizeofStructLengthField(TLV)[1,2,4]");
    /// @ref [PRS_SOMEIP_00242] -- The configuration size of length fields for arrays, structures, unions, and strings must be the same
    static_assert(PropsType::sizeofStructLengthField() == PropsType::sizeofArrayLengthField()
                      && PropsType::sizeofStructLengthField() == PropsType::sizeofStringLengthField()
                      && PropsType::sizeofStructLengthField() == PropsType::sizeofStructLengthField()
                      && PropsType::sizeofStructLengthField() == PropsType::sizeofUnionLengthField(),
                  "sizeof*LengthField must be the same");
    /// @brief Write
    /// @param[in] stream Payload stream
    /// @param[in] value Value
    /// @return Result -- >= 0: number of bytes written; < 0: error code;
    static ResultType Write(StreamType& stream, ValueType const& value) noexcept
    {
        auto reverseOrder{(PropsType::byteOrder() == ByteOrder::kMostSignificantByteFirst && IsLittleEndian())
                          || (PropsType::byteOrder() == ByteOrder::kMostSignificantByteLast && !IsLittleEndian())};
        ResultType ret{};
        ResultType retn{};
        auto point{stream.Point()};
        LengthType length{};
        retn = stream.Write(length, reverseOrder);
        if (retn < 0) {
            return retn;
        }
        ret += retn;
        Writer writer{stream};
        const_cast< ValueType& >(value).enumerate(writer);
        retn = writer.ret;
        if (retn < 0) {
            return retn;
        }
        ret += retn;
        length += retn;
        retn = stream.Write(length, reverseOrder, point);
        if (retn < 0) {
            return retn;
        }
        return ret;
    }
    /// @brief Read
    /// @param[in] stream Payload stream
    /// @param[in] value Value
    /// @return Result -- >= 0: number of bytes read; < 0: error code;
    static ResultType Read(StreamType& stream, ValueType& value) noexcept
    {
        auto reverseOrder{(PropsType::byteOrder() == ByteOrder::kMostSignificantByteFirst && IsLittleEndian())
                          || (PropsType::byteOrder() == ByteOrder::kMostSignificantByteLast && !IsLittleEndian())};
        ResultType ret{};
        ResultType retn{};
        LengthType length{};
        LengthType len{};
        retn = stream.Read(length, reverseOrder);
        if (retn < 0) {
            return retn;
        }
        ret += retn;
        auto fields{value.required_fields()};
        while (len < length) {
            TagFieldType tag{};
            /// @ref [PRS_SOMEIP_00203] -- tag is not affected by byteOrder (should be network byte order)
            retn = stream.Read(tag, IsLittleEndian());
            if (retn < 0) {
                return retn;
            }
            ret += retn;
            len += retn;
            if (len >= length) {
                return ErrorCode::kInvalidValue;  // Read length (TLV struct) greater than length field value
            }
            TagFieldType field{};
            TypeLengthType typeLength{};
            Tag2Info(tag, field, typeLength);
            if (typeLength == 0) {  // Non-basic (scalar) type
                auto point{stream.Point()};
                retn = stream.Read(typeLength, reverseOrder, point);
                if (retn < 0) {
                    return retn;
                }
                if (retn > static_cast< ResultType >(length)) {
                    return ErrorCode::kInvalidValue;  // Read length (TLV struct) greater than length field value
                }
                typeLength += sizeof(typeLength);  // Non-basic (scalar) type read length retains length field length
            }
            auto itField{fields.find(field)};
            if (itField != fields.end()) {
                Reader reader{stream, typeLength};
                auto recognized{value.dispatch(field, reader)};
                if (reader.typeLength != 0) {
                    return ErrorCode::kInvalidValue;  // Read length (TLV struct) less than length field value
                }
                retn = reader.ret;
                if (retn < 0) {
                    return retn;
                }
                ret += typeLength;
                len += typeLength;
                if (recognized) {
                    std::ignore = fields.erase(itField);
                }
            } else if (typeLength != 0) {
                ara::core::Vector< char > _(typeLength);
                StringBorrow cstring(_.data(), _.size());
                retn = stream.Read(cstring);
                if (retn < 0) {
                    return retn;
                }
                ret += retn;
                len += typeLength;
            }
        }
        if (len < length) {
            ara::core::Vector< char > _(length - len);
            StringBorrow cstring(_.data(), _.size());
            retn = stream.Read(cstring);  // Read length (TLV struct) less than length field value (skip)
            if (retn < 0) {
                return retn;
            }
            ret += retn;
            len += retn;
        }
        if (len != length) {
            return ErrorCode::kInvalidValue;  // Read length (TLV struct) not equal to length field value
        }
        if (!fields.empty()) {
            /// @ref [PRS_SOMEIP_00218] -- Termination if required field not found
            return ErrorCode::kInvalidValue;  // Read value (TLV struct) missing required field
        }
        return ret;
    }

private:
    /// @brief Writer
    struct Writer
    {
        /// @brief Payload stream
        StreamType& stream;
        /// @brief Return result
        ResultType ret{};
        /// @brief Operator -- call -- write (basic/non-basic type)
        /// @tparam T Value type
        /// @param[in] value Value
        /// @param[in] field Field identifier
        /// @ref [PRS_SOMEIP_00212] -- Basic data types (wire types 0-3) configured with Data ID do not insert a length field
        /// @ref [PRS_SOMEIP_00214] -- Non-basic data types (wire types 4-7) configured with Data ID insert a length field (once)
        template < typename T >
        void operator()(T const& value, TagFieldType field) noexcept
        {
            if (ret < 0) {
                return;
            }
            ResultType retn{};
            TagFieldType tag{};
            Info2Tag< T >(tag, field);
            /// @ref [PRS_SOMEIP_00203] -- tag is not affected by byteOrder (should be network byte order)
            retn = stream.Write(tag, IsLittleEndian());
            if (retn < 0) {
                ret = retn;
                return;
            }
            ret += retn;
            retn = Translator< StreamType, T >::Write(stream, value);
            if (retn < 0) {
                ret = retn;
                return;
            }
            ret += retn;
        }
        /// @brief Operator -- call -- write (fixed-length array type)
        /// @tparam T Value element type
        /// @tparam kN Number of value elements
        /// @param[in] value Value
        /// @param[in] field Field identifier
        /// @ref [PRS_SOMEIP_00214] -- Non-basic data types (wire types 4-7) configured with Data ID insert a length field (once)
        template < typename T, size_t kN >
        void operator()(ara::core::Array< T, kN > const& value, TagFieldType field) noexcept
        {
            auto reverseOrder{(PropsType::byteOrder() == ByteOrder::kMostSignificantByteFirst && IsLittleEndian())
                              || (PropsType::byteOrder() == ByteOrder::kMostSignificantByteLast && !IsLittleEndian())};
            if (ret < 0) {
                return;
            }
            ResultType retn{};
            TagFieldType tag{};
            Info2Tag< decltype(value) >(tag, field);
            /// @ref [PRS_SOMEIP_00203] -- tag is not affected by byteOrder (should be network byte order)
            retn = stream.Write(tag, IsLittleEndian());
            if (retn < 0) {
                ret = retn;
                return;
            }
            ret += retn;
            auto point{stream.Point()};
            TypeLengthType typeLength{};
            retn = stream.Write(typeLength, reverseOrder);
            if (retn < 0) {
                ret = retn;
                return;
            }
            ret += retn;
            retn = Translator< StreamType, ara::core::Array< T, kN > >::Write(stream, value);
            if (retn < 0) {
                ret = retn;
                return;
            }
            ret += retn;
            typeLength += retn;
            retn = stream.Write(typeLength, reverseOrder, point);
            if (retn < 0) {
                ret = retn;
                return;
            }
        }
        /// @brief Operator -- call -- write (optional value type)
        /// @tparam T Value type
        /// @param[in] value Value
        /// @param[in] field Field identifier
        template < typename T >
        void operator()(ara::core::Optional< T > const& value, TagFieldType field) noexcept
        {
            if (ret < 0) {
                return;
            }
            if (value) {
                operator()(*value, field);
            }
        }
    };
    /// @brief Reader
    struct Reader
    {
        /// @brief Payload stream
        StreamType& stream;
        /// @brief Type field length value
        TypeLengthType typeLength{};
        /// @brief Return result
        ResultType ret{};
        /// @brief Operator -- call -- read
        /// @tparam T Value type
        /// @param[in] value Value
        template < typename T >
        void operator()(T& value) noexcept
        {
            if (ret < 0) {
                return;
            }
            if (typeLength == 0) {
                // No remaining members in read value (TLV struct)
                return;
            }
            ResultType retn{};
            retn = Translator< StreamType, T >::Read(stream, value);
            if (retn < 0) {
                ret = retn;
                return;
            }
            if (retn > static_cast< ResultType >(typeLength)) {
                ret = ErrorCode::kInvalidValue;  // Read length greater than length field value
                return;
            }
            ret += retn;
            typeLength -= retn;
        }
        /// @brief Operator -- call -- read
        /// @tparam T Value element type
        /// @tparam kN Number of value elements
        /// @param[in] value Value
        template < typename T, size_t kN >
        void operator()(ara::core::Array< T, kN >& value) noexcept
        {
            if (ret < 0) {
                return;
            }
            if (typeLength == 0) {
                // No remaining members in read value (TLV struct)
                return;
            }
            auto reverseOrder{(PropsType::byteOrder() == ByteOrder::kMostSignificantByteFirst && IsLittleEndian())
                              || (PropsType::byteOrder() == ByteOrder::kMostSignificantByteLast && !IsLittleEndian())};
            ResultType retn{};
            LengthType length{};
            retn = stream.Read(length, reverseOrder);
            if (retn < 0) {
                ret = retn;
                return;
            }
            ret += retn;
            typeLength -= retn;
            retn = Translator< StreamType, ara::core::Array< T, kN > >(stream, value);
            if (retn < 0) {
                ret = retn;
                return;
            }
            if (retn > static_cast< ResultType >(typeLength)) {
                ret = ErrorCode::kInvalidValue;  // Read length greater than length field value
                return;
            }
            ret += retn;
            typeLength -= retn;
        }
        /// @brief Operator -- call -- read (optional value type)
        /// @tparam T Value type
        /// @param[in] value Value
        template < typename T >
        void operator()(ara::core::Optional< T >& value) noexcept
        {
            if (ret < 0) {
                return;
            }
            if (typeLength == 0) {
                // No remaining members in read value (TLV struct)
                return;
            }
            value.emplace();
            operator()(*value);
        }
    };
    /// @brief Wire type enumeration
    enum class WireTypes : uint16_t
    {
        kBase8Bit             = 0,
        kBase16Bit            = 1,
        kBase32Bit            = 2,
        kBase64Bit            = 3,
        kComplexWithLength    = 4,
        kComplexForceLength8  = 5,
        kComplexForceLength16 = 6,
        kComplexForceLength32 = 7
    };
    static constexpr TagFieldType kTagFieldMask{0x0FFF};
    static constexpr TagFieldType kWireTypeShift{12U};
    static constexpr TagFieldType kWireTypeMask{7U};
    static constexpr TypeLengthType kWireLength0{0};
    static constexpr TypeLengthType kWireLength1{1};
    static constexpr TypeLengthType kWireLength2{2};
    static constexpr TypeLengthType kWireLength4{4};
    static constexpr TypeLengthType kWireLength8{8};
    /// @brief Convert data info to tag
    /// @tparam T Value type
    /// @param[in] tag Tag
    /// @param[in] field Field identifier
    template < typename T >
    static void Info2Tag(TagFieldType& tag, TagFieldType field) noexcept
    {
        // tag[1,15] used
        assert((field & kTagFieldMask) == field);
        tag = (field & kTagFieldMask) | (static_cast< TagFieldType >(Length2WireType< T >()) << kWireTypeShift);
    }
    /// @brief Convert tag to data info
    /// @param[in] tag Tag
    /// @param[in] field Field identifier
    /// @param[in] typeLength Type field length
    static void Tag2Info(TagFieldType tag, TagFieldType& field, TypeLengthType& typeLength) noexcept
    {
        // tag[1,3]: typeLength; tag[4,15]: field
        field      = tag & kTagFieldMask;
        typeLength = WireType2Length(static_cast< WireTypes >((tag >> kWireTypeShift) & kWireTypeMask));
    }
    /// @brief Convert data info to wire type -- 1-byte basic type
    /// @tparam T Value type
    /// @return Wire type
    template < typename T >
    static constexpr WireTypes Length2WireType(
        std::enable_if_t< IsScalar< T >::value
                          && sizeof(T) == static_cast< uint8_t >(FieldSize::kByte1) >* = nullptr) noexcept
    {
        return WireTypes::kBase8Bit;
    }
    /// @brief Convert data info to wire type -- 2-byte basic type
    /// @tparam T Value type
    /// @return Wire type
    template < typename T >
    static constexpr WireTypes Length2WireType(
        std::enable_if_t< IsScalar< T >::value
                          && sizeof(T) == static_cast< uint8_t >(FieldSize::kByte2) >* = nullptr) noexcept
    {
        return WireTypes::kBase16Bit;
    }
    /// @brief Convert data info to wire type -- 4-byte basic type
    /// @tparam T Value type
    /// @return Wire type
    template < typename T >
    static constexpr WireTypes Length2WireType(
        std::enable_if_t< IsScalar< T >::value
                          && sizeof(T) == static_cast< uint8_t >(FieldSize::kByte4) >* = nullptr) noexcept
    {
        return WireTypes::kBase32Bit;
    }
    /// @brief Convert data info to wire type -- 8-byte basic type
    /// @tparam T Value type
    /// @return Wire type
    template < typename T >
    static constexpr WireTypes Length2WireType(
        std::enable_if_t< IsScalar< T >::value
                          && sizeof(T) == static_cast< uint8_t >(FieldSize::kByte8) >* = nullptr) noexcept
    {
        return WireTypes::kBase64Bit;
    }
    /// @brief Convert data info to wire type -- non-basic type (fixed-length)
    /// @tparam T Value type
    /// @return Wire type
    template < typename T >
    static constexpr WireTypes Length2WireType(
        std::enable_if_t< !IsScalar< T >::value && !PropsType::isDynamicLengthFieldSize() >* = nullptr) noexcept
    {
        return WireTypes::kComplexWithLength;
    }
    /// @brief Convert data info to wire type -- non-basic type (variable-length, 1 byte)
    /// @tparam T Value type
    /// @return Wire type
    template < typename T >
    static constexpr WireTypes Length2WireType(
        std::enable_if_t< !IsScalar< T >::value && PropsType::isDynamicLengthFieldSize()
                          && PropsType::sizeofArrayLengthField() == 1 && PropsType::sizeofStringLengthField() == 1
                          && PropsType::sizeofStructLengthField() == 1
                          && PropsType::sizeofUnionLengthField() == 1 >* = nullptr) noexcept
    {
        return WireTypes::kComplexForceLength8;
    }
    /// @brief Convert data info to wire type -- non-basic type (variable-length, 2 bytes)
    /// @tparam T Value type
    /// @return Wire type
    template < typename T >
    static constexpr WireTypes Length2WireType(
        std::enable_if_t< !IsScalar< T >::value && PropsType::isDynamicLengthFieldSize()
                          && PropsType::sizeofArrayLengthField() == 2 && PropsType::sizeofStringLengthField() == 2
                          && PropsType::sizeofStructLengthField() == 2
                          && PropsType::sizeofUnionLengthField() == 2 >* = nullptr) noexcept
    {
        return WireTypes::kComplexForceLength16;
    }
    /// @brief Convert data info to wire type -- non-basic type (variable-length, 4 bytes)
    /// @tparam T Value type
    /// @return Wire type
    template < typename T >
    static constexpr WireTypes Length2WireType(
        std::enable_if_t< !IsScalar< T >::value && PropsType::isDynamicLengthFieldSize()
                          && PropsType::sizeofArrayLengthField() == 4 && PropsType::sizeofStringLengthField() == 4
                          && PropsType::sizeofStructLengthField() == 4
                          && PropsType::sizeofUnionLengthField() == 4 >* = nullptr) noexcept
    {
        return WireTypes::kComplexForceLength32;
    }
    /// @brief Convert data info to wire type -- optional value type
    /// @tparam T Value type
    /// @return Wire type
    template < typename T >
    static constexpr WireTypes Length2WireType(
        std::enable_if_t< ara::core::is_optional< T >::value >* = nullptr) noexcept
    {
        return Length2WireType< T::value_type >();
    }
    /// @brief Convert wire type to type field length
    /// @param[in] wireType Wire type
    /// @return Type field length
    static TypeLengthType WireType2Length(WireTypes wireType) noexcept
    {
        switch (wireType) {
            case WireTypes::kBase8Bit:
                return kWireLength1;
            case WireTypes::kBase16Bit:
                return kWireLength2;
            case WireTypes::kBase32Bit:
                return kWireLength4;
            case WireTypes::kBase64Bit:
                return kWireLength8;
            default:
                break;
        }
        return kWireLength0;
    }
};
template < typename TStream, typename TValue >
constexpr typename Translator< TStream, TValue, std::enable_if_t< IsTLV< TValue >::value > >::TagFieldType
    Translator< TStream, TValue, std::enable_if_t< IsTLV< TValue >::value > >::kTagFieldMask;
template < typename TStream, typename TValue >
constexpr typename Translator< TStream, TValue, std::enable_if_t< IsTLV< TValue >::value > >::TagFieldType
    Translator< TStream, TValue, std::enable_if_t< IsTLV< TValue >::value > >::kWireTypeShift;
template < typename TStream, typename TValue >
constexpr typename Translator< TStream, TValue, std::enable_if_t< IsTLV< TValue >::value > >::TagFieldType
    Translator< TStream, TValue, std::enable_if_t< IsTLV< TValue >::value > >::kWireTypeMask;
template < typename TStream, typename TValue >
constexpr typename Translator< TStream, TValue, std::enable_if_t< IsTLV< TValue >::value > >::TypeLengthType
    Translator< TStream, TValue, std::enable_if_t< IsTLV< TValue >::value > >::kWireLength0;
template < typename TStream, typename TValue >
constexpr typename Translator< TStream, TValue, std::enable_if_t< IsTLV< TValue >::value > >::TypeLengthType
    Translator< TStream, TValue, std::enable_if_t< IsTLV< TValue >::value > >::kWireLength1;
template < typename TStream, typename TValue >
constexpr typename Translator< TStream, TValue, std::enable_if_t< IsTLV< TValue >::value > >::TypeLengthType
    Translator< TStream, TValue, std::enable_if_t< IsTLV< TValue >::value > >::kWireLength2;
template < typename TStream, typename TValue >
constexpr typename Translator< TStream, TValue, std::enable_if_t< IsTLV< TValue >::value > >::TypeLengthType
    Translator< TStream, TValue, std::enable_if_t< IsTLV< TValue >::value > >::kWireLength4;
template < typename TStream, typename TValue >
constexpr typename Translator< TStream, TValue, std::enable_if_t< IsTLV< TValue >::value > >::TypeLengthType
    Translator< TStream, TValue, std::enable_if_t< IsTLV< TValue >::value > >::kWireLength8;

/// @brief Template type -- converter -- specialization for struct (S2S conversion) type
/// @tparam TStream Payload stream type
/// @tparam TValue Value type
template < typename TStream, typename TValue >
struct Translator< TStream, TValue, std::enable_if_t< IsS2S< TValue >::value > >
{
    /// @brief Type alias -- payload stream type
    using StreamType = TStream;
    /// @brief Type alias -- value type
    using ValueType = TValue;
    /// @brief Type alias -- property information
    using PropsType = typename StreamType::PropsType;
    /// @brief Type alias -- result type
    using ResultType = typename StreamType::ResultType;
    /// @brief Type alias -- position type
    using PointType = typename StreamType::PointType;
    /// @brief Write
    /// @param[in] stream Payload stream
    /// @param[in] value Value
    /// @return Result -- >= 0: number of bytes written; < 0: error code;
    static ResultType Write(StreamType& stream, ValueType const& value) noexcept
    {
        Writer writer{stream};
        const_cast< ValueType& >(value).enumerate(writer);
        return writer.ret;
    }
    /// @brief Read
    /// @param[in] stream Payload stream
    /// @param[in] value Value
    /// @return Result -- >= 0: number of bytes read; < 0: error code;
    static ResultType Read(StreamType& stream, ValueType& value) noexcept
    {
        Reader reader{stream};
        value.enumerate(reader);
        return reader.ret;
    }

private:
    /// @brief Writer
    struct Writer
    {
        /// @brief Payload stream
        StreamType& stream;
        /// @brief Return result
        ResultType ret{};
        /// @brief Operator -- call -- write (basic type)
        /// @tparam T Value type
        /// @param[in] value Value
        /// @param[in] bit Start bit
        /// @param[in] msb Byte order
        template < typename T >
        void operator()(T value,
                        uint16_t bit,
                        bool msb                                  = true,
                        std::enable_if_t< IsScalar< T >::value >* = nullptr) noexcept
        {
            auto reverseOrder{(msb && IsLittleEndian()) || (!msb && !IsLittleEndian())};
            std::ignore = bit;
            if (ret < 0) {
                return;
            }
            auto retn{stream.Write(value, reverseOrder)};
            if (retn < 0) {
                ret = retn;
                return;
            }
            ret += retn;
        }
        /// @brief Operator -- call -- write (fixed-length array type)
        /// @tparam T Value type
        /// @tparam kN Number of value elements
        /// @param[in] value Value
        /// @param[in] bit Start bit
        /// @param[in] msb Byte order
        template < typename T, size_t kN >
        void operator()(
            T const (&value)[kN],
            uint16_t bit,
            bool msb                                                                       = true,
            std::enable_if_t< IsScalar< T >::value
                              && sizeof(T) == static_cast< uint8_t >(FieldSize::kByte1) >* = nullptr) noexcept
        {
            std::ignore = bit;
            std::ignore = msb;
            if (ret < 0) {
                return;
            }
            StringBorrow cstring(reinterpret_cast< char const* >(value), kN);
            auto retn{stream.Write(cstring)};
            if (retn < 0) {
                ret = retn;
                return;
            }
            ret += retn;
        }
        /// @brief Operator -- call -- write (fixed-length array type)
        /// @tparam T Value element type
        /// @tparam kN Number of value elements
        /// @param[in] value Value
        /// @param[in] bit Start bit
        /// @param[in] msb Byte order
        template < typename T, size_t kN >
        void operator()(
            ara::core::Array< T, kN > const& value,
            uint16_t bit,
            bool msb                                                                       = true,
            std::enable_if_t< IsScalar< T >::value
                              && sizeof(T) == static_cast< uint8_t >(FieldSize::kByte1) >* = nullptr) noexcept
        {
            std::ignore = bit;
            std::ignore = msb;
            if (ret < 0) {
                return;
            }
            StringBorrow cstring(value.data(), value.size());
            auto retn{stream.Write(cstring)};
            if (retn < 0) {
                ret = retn;
                return;
            }
            ret += retn;
        }
        /// @brief Operator -- call -- write (string type)
        /// @param[in] value Value
        /// @param[in] bit Start bit
        /// @param[in] msb Byte order
        void operator()(ara::core::String const& value, uint16_t bit, bool msb = true) noexcept
        {
            std::ignore = bit;
            std::ignore = msb;
            if (ret < 0) {
                return;
            }
            StringBorrow cstring(value.data(), value.size());
            auto retn{stream.Write(cstring)};
            if (retn < 0) {
                ret = retn;
                return;
            }
            ret += retn;
        }
    };
    /// @brief Reader
    struct Reader
    {
        /// @brief Payload stream
        StreamType& stream;
        /// @brief Return result
        ResultType ret{};
        /// @brief Operator -- call -- read (basic type)
        /// @tparam T Value type
        /// @param[in] value Value
        /// @param[in] bit Start bit
        /// @param[in] msb Byte order
        template < typename T >
        void operator()(T& value,
                        uint16_t bit,
                        bool msb                                  = true,
                        std::enable_if_t< IsScalar< T >::value >* = nullptr) noexcept
        {
            auto reverseOrder{(msb && IsLittleEndian()) || (!msb && !IsLittleEndian())};
            std::ignore = bit;
            if (ret < 0) {
                return;
            }
            auto retn{stream.Read(value, reverseOrder)};
            if (retn < 0) {
                ret = retn;
                return;
            }
            ret += retn;
        }
        /// @brief Operator -- call -- read (fixed-length array type)
        /// @tparam T Value type
        /// @tparam kN Number of value elements
        /// @param[in] value Value
        /// @param[in] bit Start bit
        /// @param[in] msb Byte order
        template < typename T, size_t kN >
        void operator()(
            T (&value)[kN],
            uint16_t bit,
            bool msb                                                                       = true,
            std::enable_if_t< IsScalar< T >::value
                              && sizeof(T) == static_cast< uint8_t >(FieldSize::kByte1) >* = nullptr) noexcept
        {
            std::ignore = bit;
            std::ignore = msb;
            if (ret < 0) {
                return;
            }
            StringBorrow cstring(reinterpret_cast< char* >(value), kN);
            auto retn{stream.Read(cstring)};
            if (retn < 0) {
                ret = retn;
                return;
            }
            ret += retn;
        }
        /// @brief Operator -- call -- read (fixed-length array type)
        /// @tparam T Value element type
        /// @tparam kN Number of value elements
        /// @param[in] value Value
        /// @param[in] bit Start bit
        /// @param[in] msb Byte order
        template < typename T, size_t kN >
        void operator()(
            ara::core::Array< T, kN >& value,
            uint16_t bit,
            bool msb                                                                       = true,
            std::enable_if_t< IsScalar< T >::value
                              && sizeof(T) == static_cast< uint8_t >(FieldSize::kByte1) >* = nullptr) noexcept
        {
            std::ignore = bit;
            std::ignore = msb;
            if (ret < 0) {
                return;
            }
            StringBorrow cstring(reinterpret_cast< char* >(value.data()), value.size());
            auto retn{stream.Read(cstring)};
            if (retn < 0) {
                ret = retn;
                return;
            }
            ret += retn;
        }
        /// @brief Operator -- call -- read (string type)
        /// @param[in] value Value
        /// @param[in] bit Start bit
        /// @param[in] msb Byte order
        void operator()(ara::core::String& value, uint16_t bit, bool msb = true) noexcept
        {
            std::ignore = bit;
            auto reverseOrder{(msb && IsLittleEndian()) || (!msb && !IsLittleEndian())};
            if (ret < 0) {
                return;
            }
            while (true) {
                char c{};
                auto retn{stream.Read(c, reverseOrder)};
                if (retn < 0 || c == 0) {
                    return;
                }
                value += c;
                ret += retn;
            }
        }
    };
};

/// @brief Template type -- converter -- specialization for associative map container type
/// @tparam TStream Payload stream type
/// @tparam TValue Value type
template < typename TStream, typename TValue >
struct Translator< TStream, TValue, std::enable_if_t< IsAssociativeContainer< TValue >::value > >
{
    /// @brief Type alias -- payload stream type
    using StreamType = TStream;
    /// @brief Type alias -- value type
    using ValueType = TValue;
    /// @brief Type alias -- property information
    using PropsType = typename StreamType::PropsType;
    /// @brief Type alias -- result type
    using ResultType = typename StreamType::ResultType;
    /// @brief Type alias -- position type
    using PointType = typename StreamType::PointType;
    /// @brief Type alias -- length field type
    using LengthType = LengthFieldType< PropsType::sizeofArrayLengthField() >;
    static_assert(PropsType::sizeofArrayLengthField() == 1 || PropsType::sizeofArrayLengthField() == 2
                      || PropsType::sizeofArrayLengthField() == 4,
                  "sizeofArrayLengthField(Associative)[1,2,4]");
    /// @brief Write
    /// @param[in] stream Payload stream
    /// @param[in] value Value
    /// @return Result -- >= 0: number of bytes written; < 0: error code;
    static ResultType Write(StreamType& stream, ValueType const& value) noexcept
    {
        auto reverseOrder{(PropsType::byteOrder() == ByteOrder::kMostSignificantByteFirst && IsLittleEndian())
                          || (PropsType::byteOrder() == ByteOrder::kMostSignificantByteLast && !IsLittleEndian())};
        ResultType ret{};
        ResultType retn{};
        auto point{stream.Point()};
        LengthType length{};
        retn = stream.Write(length, reverseOrder);
        if (retn < 0) {
            return retn;
        }
        ret += retn;
        /// @brief Value element type
        using ElementType = typename ValueType::value_type;
        for (auto const& it : value) {
            retn = Translator< StreamType, ElementType >::Write(stream, it);
            if (retn < 0) {
                return retn;
            }
            ret += retn;
            length += retn;
        }
        retn = stream.Write(length, reverseOrder, point);
        if (retn < 0) {
            return retn;
        }
        return ret;
    }
    /// @brief Read
    /// @param[in] stream Payload stream
    /// @param[in] value Value
    /// @return Result -- >= 0: number of bytes read; < 0: error code;
    static ResultType Read(StreamType& stream, ValueType& value) noexcept
    {
        auto reverseOrder{(PropsType::byteOrder() == ByteOrder::kMostSignificantByteFirst && IsLittleEndian())
                          || (PropsType::byteOrder() == ByteOrder::kMostSignificantByteLast && !IsLittleEndian())};
        ResultType ret{};
        ResultType retn{};
        LengthType length{};
        LengthType len{};
        retn = stream.Read(length, reverseOrder);
        if (retn < 0) {
            return retn;
        }
        ret += retn;
        /// @brief Value element type
        using ElementType = typename ValueType::value_type;
        value.clear();
        while (len < length) {
            ElementType it;
            retn = Translator< StreamType, ElementType >::Read(stream, it);
            if (retn < 0) {
                return retn;
            }
            ret += retn;
            len += retn;
            std::ignore = value.insert(std::move(it));
        }
        if (len != length) {
            return ErrorCode::kInvalidValue;  // Read length (map) not equal to length field value
        }
        return ret;
    }
};

/// @brief Template type -- converter -- specialization for sequence container type
/// @tparam TStream Payload stream type
/// @tparam TValue Value type
template < typename TStream, typename TValue >
struct Translator< TStream, TValue, std::enable_if_t< IsSequenceContainer< TValue >::value > >
{
    /// @brief Type alias -- payload stream type
    using StreamType = TStream;
    /// @brief Type alias -- value type
    using ValueType = TValue;
    /// @brief Type alias -- property information
    using PropsType = typename StreamType::PropsType;
    /// @brief Type alias -- result type
    using ResultType = typename StreamType::ResultType;
    /// @brief Type alias -- position type
    using PointType = typename StreamType::PointType;
    /// @brief Type alias -- length field type
    using LengthType = LengthFieldType< PropsType::sizeofArrayLengthField() >;
    static_assert(PropsType::sizeofArrayLengthField() == 1 || PropsType::sizeofArrayLengthField() == 2
                      || PropsType::sizeofArrayLengthField() == 4,
                  "sizeofArrayLengthField(Sequence)[1,2,4]");
    /// @brief Write -- !std::is_same<ElementType, bool>::value
    /// @param[in] stream Payload stream
    /// @param[in] value Value
    /// @return Result -- >= 0: number of bytes written; < 0: error code;
    template < typename ElementType = typename ValueType::value_type >
    static ResultType Write(StreamType& stream,
                            ValueType const& value,
                            std::enable_if_t< !std::is_same< ElementType, bool >::value >* = nullptr) noexcept
    {
        auto reverseOrder{(PropsType::byteOrder() == ByteOrder::kMostSignificantByteFirst && IsLittleEndian())
                          || (PropsType::byteOrder() == ByteOrder::kMostSignificantByteLast && !IsLittleEndian())};
        ResultType ret{};
        ResultType retn{};
        auto point{stream.Point()};
        LengthType length{};
        retn = stream.Write(length, reverseOrder);
        if (retn < 0) {
            return retn;
        }
        ret += retn;
        if (!IsScalar< ElementType >::value || sizeof(ElementType) != 1) {
            for (auto const& it : value) {
                retn = Translator< StreamType, ElementType >::Write(stream, it);
                if (retn < 0) {
                    return retn;
                }
                ret += retn;
                length += retn;
            }
        } else {
            StringBorrow cstring(reinterpret_cast< char const* >(value.data()), value.size());
            retn = stream.Write(cstring);
            if (retn < 0) {
                return retn;
            }
            ret += retn;
            length += retn;
        }
        retn = stream.Write(length, reverseOrder, point);
        if (retn < 0) {
            return retn;
        }
        return ret;
    }
    /// @brief Write -- std::is_same<ElementType, bool>::value
    /// @param[in] stream Payload stream
    /// @param[in] value Value
    /// @return Result -- >= 0: number of bytes written; < 0: error code;
    template < typename ElementType = typename ValueType::value_type >
    static ResultType Write(StreamType& stream,
                            ValueType const& value,
                            std::enable_if_t< std::is_same< ElementType, bool >::value >* = nullptr) noexcept
    {
        auto reverseOrder{(PropsType::byteOrder() == ByteOrder::kMostSignificantByteFirst && IsLittleEndian())
                          || (PropsType::byteOrder() == ByteOrder::kMostSignificantByteLast && !IsLittleEndian())};
        ResultType ret{};
        ResultType retn{};
        auto point{stream.Point()};
        LengthType length{};
        retn = stream.Write(length, reverseOrder);
        if (retn < 0) {
            return retn;
        }
        ret += retn;
        for (auto const& it : value) {
            retn = Translator< StreamType, ElementType >::Write(stream, it);
            if (retn < 0) {
                return retn;
            }
            ret += retn;
            length += retn;
        }
        retn = stream.Write(length, reverseOrder, point);
        if (retn < 0) {
            return retn;
        }
        return ret;
    }
    /// @brief Read -- !std::is_same<ElementType, bool>::value
    /// @param[in] stream Payload stream
    /// @param[in] value Value
    /// @return Result -- >= 0: number of bytes read; < 0: error code;
    template < typename ElementType = typename ValueType::value_type >
    static ResultType Read(StreamType& stream,
                           ValueType& value,
                           std::enable_if_t< !std::is_same< ElementType, bool >::value >* = nullptr) noexcept
    {
        auto reverseOrder{(PropsType::byteOrder() == ByteOrder::kMostSignificantByteFirst && IsLittleEndian())
                          || (PropsType::byteOrder() == ByteOrder::kMostSignificantByteLast && !IsLittleEndian())};
        ResultType ret{};
        ResultType retn{};
        LengthType length{};
        LengthType len{};
        retn = stream.Read(length, reverseOrder);
        if (retn < 0) {
            return retn;
        }
        ret += retn;
        if (!IsScalar< ElementType >::value || sizeof(ElementType) != 1) {
            value.clear();
            while (len < length) {
                value.emplace_back();
                retn = Translator< StreamType, ElementType >::Read(stream, value.back());
                if (retn < 0) {
                    return retn;
                }
                ret += retn;
                len += retn;
            }
        } else {
            try {
                value.resize(length);
            } catch (std::exception const& e) {
                return ErrorCode::kInvalidValue;  // Read value (dynamic array) failed to set length field value
            }
            StringBorrow cstring(reinterpret_cast< char* >(value.data()), value.size());
            retn = stream.Read(cstring);
            if (retn < 0) {
                return retn;
            }
            ret += retn;
            len += retn;
        }
        if (len != length) {
            return ErrorCode::kInvalidValue;  // Read length (dynamic array) not equal to length field value
        }
        return ret;
    }
    /// @brief Read -- std::is_same<ElementType, bool>::value
    /// @param[in] stream Payload stream
    /// @param[in] value Value
    /// @return Result -- >= 0: number of bytes read; < 0: error code;
    template < typename ElementType = typename ValueType::value_type >
    static ResultType Read(StreamType& stream,
                           ValueType& value,
                           std::enable_if_t< std::is_same< ElementType, bool >::value >* = nullptr) noexcept
    {
        auto reverseOrder{(PropsType::byteOrder() == ByteOrder::kMostSignificantByteFirst && IsLittleEndian())
                          || (PropsType::byteOrder() == ByteOrder::kMostSignificantByteLast && !IsLittleEndian())};
        ResultType ret{};
        ResultType retn{};
        LengthType length{};
        LengthType len{};
        retn = stream.Read(length, reverseOrder);
        if (retn < 0) {
            return retn;
        }
        ret += retn;
        value.clear();
        while (len < length) {
#if 0  // Disable `bool& v = value.back()`, vector<bool> implementation does not support returning bit reference
            value.emplace_back();
            retn = Translator< StreamType, ElementType >::Read(stream, value.back());
#else  // Allow `value.back() = true`
            ElementType v;
            retn = Translator< StreamType, ElementType >::Read(stream, v);
            value.emplace_back(v);
#endif
            if (retn < 0) {
                return retn;
            }
            ret += retn;
            len += retn;
        }
        if (len != length) {
            return ErrorCode::kInvalidValue;  // Read length (dynamic array) not equal to length field value
        }
        return ret;
    }
};

/// @brief Template type -- converter -- specialization for sequence container (fixed-length array) type
/// @tparam TStream Payload stream type
/// @tparam T Value element type
/// @tparam kN Number of value elements
template < typename TStream, typename T, size_t kN >
struct Translator< TStream, ara::core::Array< T, kN > >
{
    /// @brief Type alias -- payload stream type
    using StreamType = TStream;
    /// @brief Type alias -- value type
    using ValueType = ara::core::Array< T, kN >;
    /// @brief Type alias -- property information
    using PropsType = typename StreamType::PropsType;
    /// @brief Type alias -- result type
    using ResultType = typename StreamType::ResultType;
    /// @brief Type alias -- position type
    using PointType = typename StreamType::PointType;
    template < typename TProps = PropsType >
    static constexpr auto SizeofArrayLengthField(
        std::enable_if_t< EnableFixedArrayLengthField< TProps >::value >* = nullptr) noexcept
    {
        return PropsType::sizeofFixedArrayLengthField();
    }
    template < typename TProps = PropsType >
    static constexpr auto SizeofArrayLengthField(
        std::enable_if_t< !EnableFixedArrayLengthField< TProps >::value >* = nullptr) noexcept
    {
        return PropsType::sizeofArrayLengthField();
    }
    /// @brief Type alias -- length field type
    using LengthType = LengthFieldType< SizeofArrayLengthField() >;
    static_assert(SizeofArrayLengthField() == 0 || SizeofArrayLengthField() == 1 || SizeofArrayLengthField() == 2
                      || SizeofArrayLengthField() == 4,
                  "sizeofArrayLengthField(Array)[0,1,2,4]");
    /// @brief Write
    /// @param[in] stream Payload stream
    /// @param[in] value Value
    /// @return Result -- >= 0: number of bytes written; < 0: error code;
    static ResultType Write(StreamType& stream, ValueType const& value) noexcept
    {
        auto reverseOrder{(PropsType::byteOrder() == ByteOrder::kMostSignificantByteFirst && IsLittleEndian())
                          || (PropsType::byteOrder() == ByteOrder::kMostSignificantByteLast && !IsLittleEndian())};
        ResultType ret{};
        ResultType retn{};
        auto point{stream.Point()};
        LengthType length{};
        if (SizeofArrayLengthField() != 0) {
            retn = stream.Write(length, reverseOrder);
            if (retn < 0) {
                return retn;
            }
            ret += retn;
        }
        /// @brief Value element type
        using ElementType = T;
        if (!IsScalar< ElementType >::value || sizeof(ElementType) != 1 || std::is_same< ElementType, bool >::value) {
            for (size_t n{}; n < kN; ++n) {
                retn = Translator< StreamType, ElementType >::Write(stream, value[n]);
                if (retn < 0) {
                    return retn;
                }
                ret += retn;
                length += retn;
            }
        } else {
            StringBorrow cstring(reinterpret_cast< char const* >(value.data()), value.size());
            retn = stream.Write(cstring);
            if (retn < 0) {
                return retn;
            }
            ret += retn;
            length += retn;
        }
        if (SizeofArrayLengthField() != 0) {
            retn = stream.Write(length, reverseOrder, point);
            if (retn < 0) {
                return retn;
            }
        }
        return ret;
    }
    /// @brief Read
    /// @param[in] stream Payload stream
    /// @param[in] value Value
    /// @return Result -- >= 0: number of bytes read; < 0: error code;
    static ResultType Read(StreamType& stream, ValueType& value) noexcept
    {
        auto reverseOrder{(PropsType::byteOrder() == ByteOrder::kMostSignificantByteFirst && IsLittleEndian())
                          || (PropsType::byteOrder() == ByteOrder::kMostSignificantByteLast && !IsLittleEndian())};
        ResultType ret{};
        ResultType retn{};
        LengthType length{};
        LengthType len{};
        if (SizeofArrayLengthField() != 0) {
            retn = stream.Read(length, reverseOrder);
            if (retn < 0) {
                return retn;
            }
            ret += retn;
        }
        /// @brief Value element type
        using ElementType = T;
        if (!IsScalar< ElementType >::value || sizeof(ElementType) != 1 || std::is_same< ElementType, bool >::value) {
            for (size_t n{}; n < kN; ++n) {
                retn = Translator< StreamType, ElementType >::Read(stream, value[n]);
                if (retn < 0) {
                    return retn;
                }
                ret += retn;
                len += retn;
            }
        } else {
            StringBorrow cstring(reinterpret_cast< char* >(value.data()), value.size());
            retn = stream.Read(cstring);
            if (retn < 0) {
                return retn;
            }
            ret += retn;
            len += retn;
        }
        if (SizeofArrayLengthField() != 0) {
            if (len != length) {
                return ErrorCode::kInvalidValue;  // Read length (fixed-length array) not equal to length field value
            }
        }
        return ret;
    }
};

/// @brief Template type -- converter -- specialization for associative container (key-value pair) type
/// @tparam TStream Payload stream type
/// @tparam T1 Key type
/// @tparam T2 Value type
template < typename TStream, typename T1, typename T2 >
struct Translator< TStream, std::pair< T1, T2 > >
{
    /// @brief Type alias -- payload stream type
    using StreamType = TStream;
    /// @brief Type alias -- value type
    using ValueType = std::pair< T1, T2 >;
    /// @brief Type alias -- property information
    using PropsType = typename StreamType::PropsType;
    /// @brief Type alias -- result type
    using ResultType = typename StreamType::ResultType;
    /// @brief Type alias -- position type
    using PointType = typename StreamType::PointType;
    /// @brief Write
    /// @param[in] stream Payload stream
    /// @param[in] value Value
    /// @return Result -- >= 0: number of bytes written; < 0: error code;
    static ResultType Write(StreamType& stream, ValueType const& value) noexcept
    {
        ResultType ret{};
        ResultType retn{};
        retn = Translator< StreamType, std::remove_cv_t< T1 > >::Write(stream, value.first);
        if (retn < 0) {
            return retn;
        }
        ret += retn;
        retn = Translator< StreamType, std::remove_cv_t< T2 > >::Write(stream, value.second);
        if (retn < 0) {
            return retn;
        }
        ret += retn;
        return ret;
    }
    /// @brief Read
    /// @param[in] stream Payload stream
    /// @param[in] value Value
    /// @return Result -- >= 0: number of bytes read; < 0: error code;
    static ResultType Read(StreamType& stream, ValueType& value) noexcept
    {
        ResultType ret{};
        ResultType retn{};
        auto& valueFirst{const_cast< std::remove_cv_t< T1 >& >(value.first)};
        retn = Translator< StreamType, std::remove_cv_t< T1 > >::Read(stream, valueFirst);
        if (retn < 0) {
            return retn;
        }
        ret += retn;
        retn = Translator< StreamType, std::remove_cv_t< T2 > >::Read(stream, value.second);
        if (retn < 0) {
            return retn;
        }
        ret += retn;
        return ret;
    }
};

/// @brief Template type -- converter -- specialization for variant container (union) type
/// @tparam TStream Payload stream type
/// @tparam T1 Key type
/// @tparam T2 Value type
template < typename TStream, typename... Types >
struct Translator< TStream, ara::core::Variant< Types... > >
{
    /// @brief Type alias -- payload stream type
    using StreamType = TStream;
    /// @brief Type alias -- value type
    using ValueType = ara::core::Variant< Types... >;
    /// @brief Type alias -- property information
    using PropsType = typename StreamType::PropsType;
    /// @brief Type alias -- result type
    using ResultType = typename StreamType::ResultType;
    /// @brief Type alias -- position type
    using PointType = typename StreamType::PointType;
    /// @brief Type alias -- variant length field type
    using UnionLengthType = LengthFieldType< PropsType::sizeofUnionLengthField() >;
    /// @brief Type alias -- variant type selector field type
    using UnionTypeSelectorType = LengthFieldType< PropsType::sizeofUnionTypeSelectorField() >;
    static_assert(PropsType::sizeofUnionLengthField() == 0 || PropsType::sizeofUnionLengthField() == 1
                      || PropsType::sizeofUnionLengthField() == 2 || PropsType::sizeofUnionLengthField() == 4,
                  "sizeofUnionLengthField(Variant)[0,1,2,4]");
    static_assert(PropsType::sizeofUnionTypeSelectorField() == 1 || PropsType::sizeofUnionTypeSelectorField() == 2
                      || PropsType::sizeofUnionTypeSelectorField() == 4,
                  "sizeofUnionTypeSelectorField(Variant)[1,2,4]");
    /// @brief Write
    /// @param[in] stream Payload stream
    /// @param[in] value Value
    /// @return Result -- >= 0: number of bytes written; < 0: error code;
    static ResultType Write(StreamType& stream, ValueType const& value) noexcept
    {
        auto reverseOrder{(PropsType::byteOrder() == ByteOrder::kMostSignificantByteFirst && IsLittleEndian())
                          || (PropsType::byteOrder() == ByteOrder::kMostSignificantByteLast && !IsLittleEndian())};
        ResultType ret{};
        ResultType retn{};
        auto point{stream.Point()};
        UnionLengthType length{};
        if (PropsType::sizeofUnionLengthField() != 0) {
            retn = stream.Write(length, reverseOrder);
            if (retn < 0) {
                return retn;
            }
            ret += retn;
        }
        UnionTypeSelectorType typeSelector{static_cast< UnionTypeSelectorType >(value.index() + 1)};
        retn = stream.Write(typeSelector, reverseOrder);
        if (retn < 0) {
            return retn;
        }
        ret += retn;
        Visitor visitor{stream};
        ara::core::visit(visitor, value);
        retn = visitor.ret;
        if (retn < 0) {
            return retn;
        }
        ret += retn;
        length += retn;
        if (PropsType::sizeofUnionLengthField() != 0) {
            retn = stream.Write(length, reverseOrder, point);
            if (retn < 0) {
                return retn;
            }
        }
        return ret;
    }
    /// @brief Read
    /// @param[in] stream Payload stream
    /// @param[in] value Value
    /// @return Result -- >= 0: number of bytes read; < 0: error code;
    static ResultType Read(StreamType& stream, ValueType& value) noexcept
    {
        auto reverseOrder{(PropsType::byteOrder() == ByteOrder::kMostSignificantByteFirst && IsLittleEndian())
                          || (PropsType::byteOrder() == ByteOrder::kMostSignificantByteLast && !IsLittleEndian())};
        ResultType ret{};
        ResultType retn{};
        UnionLengthType length{};
        UnionLengthType len{};
        if (PropsType::sizeofUnionLengthField() != 0) {
            retn = stream.Read(length, reverseOrder);
            if (retn < 0) {
                return retn;
            }
            ret += retn;
        }
        UnionTypeSelectorType typeSelector{};
        retn = stream.Read(typeSelector, reverseOrder);
        if (retn < 0) {
            return retn;
        }
        ret += retn;
        size_t index{static_cast< size_t >(typeSelector - 1)};
        if (index >= sizeof...(Types)) {
            return ErrorCode::kInvalidValue;  // Read value (variant) type index invalid
        }
        retn = Read< sizeof...(Types) >(stream, value, index);
        if (retn < 0) {
            return retn;
        }
        ret += retn;
        len += retn;
        if (PropsType::sizeofUnionLengthField() != 0) {
            if (len != length) {
                return ErrorCode::kInvalidValue;  // Read length (variant) not equal to length field value
            }
        }
        return ret;
    }

private:
    struct Visitor
    {
        StreamType& stream;
        ResultType ret{};
        template < typename T >
        void operator()(T const& value) noexcept
        {
            ret = Translator< StreamType, T >::Write(stream, value);
        }
    };
    template < size_t kN >
    static ResultType Read(StreamType& stream,
                           ValueType& value,
                           size_t index,
                           std::enable_if_t< kN == 1 >* = nullptr) noexcept
    {
        std::ignore = index;
        auto& v{value.template emplace< sizeof...(Types) - kN >()};
        return Translator< StreamType, std::remove_reference_t< decltype(v) > >::Read(stream, v);
    }
    template < size_t kN >
    static ResultType Read(StreamType& stream,
                           ValueType& value,
                           size_t index,
                           std::enable_if_t< kN != 1 >* = nullptr) noexcept
    {
        if (index == 0) {
            auto& v{value.template emplace< sizeof...(Types) - kN >()};
            return Translator< StreamType, std::remove_reference_t< decltype(v) > >::Read(stream, v);
        }
        return Read< kN - 1 >(stream, value, index - 1);
    }
};

/// @brief Deserialize error code
/// @param[in] value Value
/// @param[in] errorCode Error code
/// @param[in] domainId Error domain identifier
/// @param[in] supportData Error support data
/// @return Result -- >= 0: number of bytes read; < 0: error code;
bool DeserializeErrorCode(ara::core::ErrorCode& value,
                          ara::core::ErrorDomain::CodeType const& errorCode,
                          ara::core::ErrorDomain::IdType const& domainId,
                          ara::core::ErrorDomain::SupportDataType const& supportData) noexcept;
/// @brief Template type -- converter -- specialization for error type
/// @tparam TStream Payload stream type
template < typename TStream >
struct Translator< TStream, ara::core::ErrorCode >
{
    /// @brief Type alias -- payload stream type
    using StreamType = TStream;
    /// @brief Type alias -- value type
    using ValueType = ara::core::ErrorCode;
    /// @brief Type alias -- property information
    using PropsType = typename StreamType::PropsType;
    /// @brief Type alias -- result type
    using ResultType = typename StreamType::ResultType;
    /// @brief Type alias -- position type
    using PointType = typename StreamType::PointType;
    /// @brief Write
    /// @param[in] stream Payload stream
    /// @param[in] value Value
    /// @return Result -- >= 0: number of bytes written; < 0: error code;
    static ResultType Write(StreamType& stream, ValueType const& value) noexcept
    {
        auto reverseOrder{IsLittleEndian()};
        ResultType ret{};
        ResultType retn{};
        retn = stream.Write(UnionLength(), reverseOrder);
        if (retn < 0) {
            return retn;
        }
        ret += retn;
        retn = stream.Write(UnionTypeSelector(), reverseOrder);
        if (retn < 0) {
            return retn;
        }
        ret += retn;
        retn = stream.Write(StructLength(), reverseOrder);
        if (retn < 0) {
            return retn;
        }
        ret += retn;
        retn = stream.Write(value.Domain().Id(), reverseOrder);
        if (retn < 0) {
            return retn;
        }
        ret += retn;
        retn = stream.Write(value.Value(), reverseOrder);
        if (retn < 0) {
            return retn;
        }
        ret += retn;
        retn = stream.Write(value.SupportData(), reverseOrder);
        if (retn < 0) {
            return retn;
        }
        ret += retn;
        return ret;
    }
    /// @brief Read
    /// @param[in] stream Payload stream
    /// @param[in] value Value
    /// @return Result -- >= 0: number of bytes read; < 0: error code;
    static ResultType Read(StreamType& stream, ValueType& value) noexcept
    {
        auto reverseOrder{IsLittleEndian()};
        ResultType ret{};
        ResultType retn{};
        UnionLengthType unionLength{};
        UnionTypeSelectorType unionTypeSelector{};
        StructLengthType structLength{};
        DomainIdType domainId{};
        ErrorCodeType errorCode{};
        SupportDataType supportData{};
        retn = stream.Read(unionLength, reverseOrder);
        if (retn < 0) {
            return retn;
        }
        ret += retn;
        retn = stream.Read(unionTypeSelector, reverseOrder);
        if (retn < 0) {
            return retn;
        }
        ret += retn;
        retn = stream.Read(structLength, reverseOrder);
        if (retn < 0) {
            return retn;
        }
        ret += retn;
        retn = stream.Read(domainId, reverseOrder);
        if (retn < 0) {
            return retn;
        }
        ret += retn;
        retn = stream.Read(errorCode, reverseOrder);
        if (retn < 0) {
            return retn;
        }
        ret += retn;
        retn = stream.Read(supportData, reverseOrder);
        if (retn < 0) {
            return retn;
        }
        ret += retn;
        if (unionLength != UnionLength() || unionTypeSelector != UnionTypeSelector()
            || structLength != StructLength()) {
            ComLogWarning("deserialize error code error: data inconsistent", GenArg(unionLength),
                          GenArg(unionTypeSelector), GenArg(structLength));
            return ErrorCode::kInvalidValue;  // Read value (error) format does not conform to specification
        }
        if (!DeserializeErrorCode(value, errorCode, domainId, supportData)) {
            ComLogWarning("deserialize error code error: localization failed", GenArg(errorCode), GenArg(domainId),
                          GenArg(supportData));
            return ErrorCode::kInvalidValue;  // Read value (error) cannot be localized
        }
        return ret;
    }

private:
    /// @brief Type alias -- union length type
    using UnionLengthType = uint32_t;
    /// @brief Type alias -- union type selector type
    using UnionTypeSelectorType = uint8_t;
    /// @brief Type alias -- struct length type
    using StructLengthType = uint16_t;
    /// @brief Type alias -- error domain identifier type
    using DomainIdType = typename ara::core::ErrorDomain::IdType;
    /// @brief Type alias -- error code type
    using ErrorCodeType = typename ara::core::ErrorDomain::CodeType;
    /// @brief Type alias -- error support data type
    using SupportDataType = typename ara::core::ErrorDomain::SupportDataType;
    /// @brief Get struct length
    /// @return Struct length
    static constexpr StructLengthType StructLength() noexcept
    {
        return sizeof(DomainIdType) + sizeof(ErrorCodeType) + sizeof(SupportDataType);
    }
    /// @brief Get union type
    /// @return Union type selector
    static constexpr UnionTypeSelectorType UnionTypeSelector() noexcept { return 0x01; }
    /// @brief Get union length
    /// @return Union length
    static constexpr UnionLengthType UnionLength() noexcept
    {
        return sizeof(UnionTypeSelectorType) + sizeof(StructLengthType) + StructLength();
    }
};
}  // namespace serialize
}  // namespace npc
}  // namespace internal
}  // namespace com
}  // namespace ara
#endif
