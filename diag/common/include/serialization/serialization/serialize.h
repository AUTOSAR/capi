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
/// @file       serialize.h
/// @brief      Binding layer serialization header file
/// @details
/// @date       2024-11-13
/// @author     xuhua
/// @version    1.2.0
///
/// ================================================================

#ifndef __DIAG_SERIALIZE_H
#define __DIAG_SERIALIZE_H

#include <cassert>
#include <codecvt>
#include <locale>

#include "common_data_type.h"
#include "serialize_stream.h"

/// @brief Namespace -- internal binding layer serialization
namespace isoft {
namespace serialize {
/// @brief Serialize value list to payload (empty) and return number of bytes written or error code
/// @tparam TProps Property information
/// @tparam TValues Value list type
/// @param[in] payload Payload (empty)
/// @param[in] values Value list
/// @return Result -- >= 0: number of bytes written; < 0: error code
template < typename TProps = TransformationProps, typename... TValues >
inline auto Serialize(std::nullptr_t payload, TValues const &... values)
{
    std::ignore = payload;
    StreamWriter< TProps, std::nullptr_t > viewer;
    return SerializeImpl(viewer, values...);
}
/// @brief Serialize value list to payload and return number of bytes written or error code
/// @tparam TProps Property information
/// @tparam TPayload Payload type
/// @tparam TValues Value list type
/// @param[in] payload Payload
/// @param[in] values Value list
/// @return Result -- >= 0: number of bytes written; < 0: error code
template < typename TProps = TransformationProps, typename TPayload, typename... TValues >
inline auto Serialize(TPayload &payload, TValues const &... values)
{
    auto size{Serialize< TProps >(nullptr, values...)};
    if (size < 0) {
        return size;
    }
    StreamWriter< TProps, TPayload > writer(payload, size);
    return SerializeImpl(writer, values...);
}
/// @brief Deserialize payload to value list and return number of bytes read or error code
/// @tparam TProps Property information
/// @tparam TPayload Payload type
/// @tparam TValues Value list type
/// @param[in] payload Payload
/// @param[in] values Value list
/// @return Result -- >= 0: number of bytes read; < 0: error code
template < typename TProps = TransformationProps, typename TPayload, typename... TValues >
inline auto Deserialize(TPayload const &payload, TValues &... values)
{
    StreamReader< TProps, TPayload > reader(payload);
    return DeserializeImpl(reader, values...);
}
/// @brief Template type -- Converter -- Declaration
/// @tparam TStream Payload stream type
/// @tparam TValue Value type
template < typename TStream, typename TValue, typename = void >
class Translator;
/// @brief Serialization implementation -- multiple parameters
/// @tparam TStream Payload stream type
/// @tparam TValue Value type
/// @tparam TValues Value list type
/// @param[in] stream Payload stream
/// @param[in] value Value
/// @param[in] values Value list
/// @return Result -- >= 0: number of bytes written; < 0: error code
template < typename TStream, typename TValue, typename... TValues >
inline typename TStream::result_t SerializeImpl(TStream &stream, TValue const &value, TValues const &... values)
{
    typename TStream::result_t ret{0};
    typename TStream::result_t retn{0};
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
/// @return Result -- >= 0: number of bytes written; < 0: error code
template < typename TStream, typename TValue >
inline typename TStream::result_t SerializeImpl(TStream &stream, TValue const &value)
{
    typename TStream::result_t ret{0};
    typename TStream::result_t retn{0};
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
/// @return Result -- >= 0: number of bytes written; < 0: error code
template < typename TStream >
inline typename TStream::result_t SerializeImpl(TStream &stream)
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
/// @return Result -- >= 0: number of bytes read; < 0: error code
template < typename TStream, typename TValue, typename... TValues >
inline typename TStream::result_t DeserializeImpl(TStream &stream, TValue &value, TValues &... values)
{
    typename TStream::result_t ret{0};
    typename TStream::result_t retn{0};
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
/// @return Result -- >= 0: number of bytes read; < 0: error code
template < typename TStream, typename TValue >
inline typename TStream::result_t DeserializeImpl(TStream &stream, TValue &value)
{
    typename TStream::result_t ret{0};
    typename TStream::result_t retn{0};
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
/// @return Result -- >= 0: number of bytes read; < 0: error code
template < typename TStream >
inline typename TStream::result_t DeserializeImpl(TStream &stream)
{
    std::ignore = stream;
    return 0;
}

/// @brief Template type -- Converter -- Specialization for scalar (basic) types
/// @tparam TStream Payload stream type
/// @tparam TValue Value type
template < typename TStream, typename TValue >
struct Translator< TStream, TValue, std::enable_if_t< IsScalar< TValue >::value > >  //NOLINT
{
    /// @brief Type alias -- Payload stream type
    using stream_t = TStream;  // NOLINT
    /// @brief Type alias -- Value type
    using value_t = TValue;  // NOLINT
    /// @brief Type alias -- Property information
    using props_t = typename stream_t::props_t;  // NOLINT
    /// @brief Type alias -- Result type
    using result_t = typename stream_t::result_t;  // NOLINT
    /// @brief Type alias -- Bit position type
    using point_t = typename stream_t::point_t;  // NOLINT
    /// @brief Write
    /// @param[in] stream Payload stream
    /// @param[in] value Value
    /// @return Result -- >= 0: number of bytes written; < 0: error code
    static result_t Write(stream_t &stream, value_t const &value)
    {
        auto reverseOrder{(props_t::byteOrder() == ByteOrder::kMostSignificantByteFirst && IsLittleEndian())
                          || (props_t::byteOrder() == ByteOrder::kMostSignificantByteLast && !IsLittleEndian())};
        return stream.Write(value, reverseOrder);
    }
    /// @brief Read
    /// @param[in] stream Payload stream
    /// @param[in] value Value
    /// @return Result -- >= 0: number of bytes read; < 0: error code
    static result_t Read(stream_t &stream, value_t &value)
    {
        auto reverseOrder{(props_t::byteOrder() == ByteOrder::kMostSignificantByteFirst && IsLittleEndian())
                          || (props_t::byteOrder() == ByteOrder::kMostSignificantByteLast && !IsLittleEndian())};
        return stream.Read(value, reverseOrder);
    }
};

/// @brief Template type -- Converter -- Specialization for raw string (pointer + length) type
/// @tparam TStream Payload stream type
template < typename TStream >
struct Translator< TStream, StringBorrow >  //NOLINT
{
    /// @brief Type alias -- Payload stream type
    using stream_t = TStream;  // NOLINT
    /// @brief Type alias -- Value type
    using value_t = StringBorrow;  // NOLINT
    /// @brief Type alias -- Property information
    using props_t = typename stream_t::props_t;  // NOLINT
    /// @brief Type alias -- Result type
    using result_t = typename stream_t::result_t;  // NOLINT
    /// @brief Type alias -- Bit position type
    using point_t = typename stream_t::point_t;  // NOLINT
    /// @brief Write
    /// @param[in] stream Payload stream
    /// @param[in] value Value
    /// @return Result -- >= 0: number of bytes written; < 0: error code
    static result_t Write(stream_t &stream, value_t const &value) { return stream.Write(value); }
    /// @brief Read
    /// @param[in] stream Payload stream
    /// @param[in] value Value
    /// @return Result -- >= 0: number of bytes read; < 0: error code
    static result_t Read(stream_t &stream, value_t &value) { return stream.Read(value); }
};

/// @brief Template type -- Converter -- Specialization for string type
/// @tparam TStream Payload stream type
/// @tparam TValue Value type
template < typename TStream, typename TValue >
struct Translator< TStream, TValue, std::enable_if_t< IsString< TValue >::value > >  //NOLINT
{
    /// @brief Type alias -- Payload stream type
    using stream_t = TStream;  // NOLINT
    /// @brief Type alias -- Value type
    using value_t = TValue;  // NOLINT
    /// @brief Type alias -- Property information
    using props_t = typename stream_t::props_t;  // NOLINT
    /// @brief Type alias -- Result type
    using result_t = typename stream_t::result_t;  // NOLINT
    /// @brief Type alias -- Bit position type
    using point_t = typename stream_t::point_t;  // NOLINT
    /// @brief Type alias -- Length field type
    using length_t = length_field_t< props_t::SizeofStringLengthField() >;  // NOLINT
    static_assert(props_t::SizeofStringLengthField() == 0 || props_t::SizeofStringLengthField() == 1
                      || props_t::SizeofStringLengthField() == 2 || props_t::SizeofStringLengthField() == 4,
                  "SizeofStringLengthField[0,1,2,4]");
    /// @brief Write -- UTF-8
    /// @param[in] stream Payload stream
    /// @param[in] value Value
    /// @return Result -- >= 0: number of bytes written; < 0: error code
    template < typename TProps = props_t >
    static result_t Write(stream_t &stream,
                          value_t const &value,
                          std::enable_if_t< TProps::stringEncoding() == StringEncoding::kUTF8 > *pBuff
                          = nullptr)  //NOLINT
    {
        std::ignore = pBuff;
        auto reverseOrder{(props_t::byteOrder() == ByteOrder::kMostSignificantByteFirst && IsLittleEndian())
                          || (props_t::byteOrder() == ByteOrder::kMostSignificantByteLast && !IsLittleEndian())};
        result_t ret{0};
        result_t retn{0};
        constexpr auto kU8bom{UTF8BOM()};
        constexpr auto kU8end{UTF8End()};
        auto data{value.data()};
        auto size{value.size()};
        if (size > kU8bom.size() && data[0] == kU8bom[0] && data[1] == kU8bom[1] && data[2] == kU8bom[2]) {
            data += kU8bom.size();
            size -= kU8bom.size();
        }
        auto point{stream.Point()};
        length_t length{0};
        if (props_t::SizeofStringLengthField() != 0) {
            retn = stream.Write(length, reverseOrder);
            if (retn < 0) {
                return retn;
            }
            ret += retn;
        }
        if (props_t::implementsLegacyStringSerialization()) {
            // do nothing
        } else {
            for (size_t n{0}; n < kU8bom.size(); ++n) {
                retn = stream.Write(kU8bom[n], reverseOrder);
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
        for (size_t n{0}; n < kU8end.size(); ++n) {
            retn = stream.Write(kU8end[n], reverseOrder);
            if (retn < 0) {
                return retn;
            }
            ret += retn;
            length += retn;
        }
        if (props_t::SizeofStringLengthField() != 0) {
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
    /// @return Result -- >= 0: number of bytes written; < 0: error code
    template < typename TProps = props_t >
    static result_t Write(stream_t &stream,
                          value_t const &value,
                          std::enable_if_t< TProps::stringEncoding() == StringEncoding::kUTF16 > *pBuff = nullptr)
    {
        std::ignore = pBuff;
        auto reverseOrder{(props_t::byteOrder() == ByteOrder::kMostSignificantByteFirst && IsLittleEndian())
                          || (props_t::byteOrder() == ByteOrder::kMostSignificantByteLast && !IsLittleEndian())};
        result_t ret{0};
        result_t retn{0};
        constexpr auto kU16bom{UTF16BOM()};
        constexpr auto kU16end{UTF16End()};
        std::string u8{value.begin(), value.end()};
        std::u16string u16 = std::wstring_convert< std::codecvt_utf8_utf16< char16_t >, char16_t >{}.from_bytes(u8);
        auto const *data{u16.data()};
        auto size{u16.size()};
        if (size > kU16bom.size() && data[0] == kU16bom[0]) {
            data += kU16bom.size();
            size -= kU16bom.size();
        }
        auto point{stream.Point()};
        length_t length{0};
        if (props_t::SizeofStringLengthField() != 0) {
            retn = stream.Write(length, reverseOrder);
            if (retn < 0) {
                return retn;
            }
            ret += retn;
        }
        if (props_t::implementsLegacyStringSerialization()) {
            // do nothing
        } else {
            for (size_t n{0}; n < kU16bom.size(); ++n) {
                retn = stream.Write(kU16bom[n], reverseOrder);
                if (retn < 0) {
                    return retn;
                }
                ret += retn;
                length += retn;
            }
        }
        for (size_t n{0}; n < size; ++n) {
            retn = stream.Write(data[n], reverseOrder);
            if (retn < 0) {
                return retn;
            }
            ret += retn;
            length += retn;
        }
        for (size_t n{0}; n < kU16end.size(); ++n) {
            retn = stream.Write(kU16end[n], reverseOrder);
            if (retn < 0) {
                return retn;
            }
            ret += retn;
            length += retn;
        }
        if (props_t::SizeofStringLengthField() != 0) {
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
    /// @return Result -- >= 0: number of bytes read; < 0: error code
    template < typename TProps = props_t >
    static result_t Read(stream_t &stream,  //NOLINT
                         value_t &value,
                         std::enable_if_t< TProps::stringEncoding() == StringEncoding::kUTF8 > *pBuff = nullptr)
    {
        std::ignore = pBuff;
        auto reverseOrder{(props_t::byteOrder() == ByteOrder::kMostSignificantByteFirst && IsLittleEndian())
                          || (props_t::byteOrder() == ByteOrder::kMostSignificantByteLast && !IsLittleEndian())};
        result_t ret{0};
        result_t retn{0};
        constexpr auto kU8bom{UTF8BOM()};
        constexpr auto kU8end{UTF8End()};
        length_t length{0};
        length_t len{0};
        if (props_t::SizeofStringLengthField() != 0) {
            retn = stream.Read(length, reverseOrder);
            if (retn < 0) {
                return retn;
            }
            ret += retn;
        } else {
            length = std::numeric_limits< length_t >::max();
        }
        if (props_t::implementsLegacyStringSerialization()) {
            if (length < kU8end.size()) {
                return ErrorCode::kInvalidValue;  // Read value (UTF-8 string)
                                                  // length field value less than lower limit
            }
            length -= kU8end.size();
        } else {
            if (length < (kU8bom.size() + kU8end.size())) {
                return ErrorCode::kInvalidValue;  // Read value (UTF-8 string)
                                                  // length field value less than lower limit
            }
            length -= (kU8bom.size() + kU8end.size());
            std::remove_cv_t< decltype(kU8bom) > bom{};
            for (size_t n{0}; n < bom.size(); ++n) {
                retn = stream.Read(bom[n], reverseOrder);
                if (retn < 0) {
                    return retn;
                }
                ret += retn;
            }
            if (bom != kU8bom) {
                return ErrorCode::kInvalidValue;  // Read value (UTF-8 string)
                                                  // BOM format does not conform to specification
            }
        }
        if (props_t::SizeofStringLengthField() != 0) {
            try {
                value.resize(length);
            } catch (std::exception const &e) {
                return ErrorCode::kInvalidValue;  // Read value (UTF-8 string)
                                                  // length field value setting failed
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
            std::remove_cv_t< decltype(kU8end) > end{};
            for (size_t n{0}; n < end.size(); ++n) {
                retn = stream.Read(end[n], reverseOrder);
                if (retn < 0) {
                    return retn;
                }
                ret += retn;
            }
            if (end != kU8end) {
                return ErrorCode::kInvalidValue;  // Read value (UTF-8 string)
                                                  // termination format does not conform to specification
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
                if (c == kU8end[0]) {  //NOLINT
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
    /// @return Result -- >= 0: number of bytes read; < 0: error code
    template < typename TProps = props_t >
    static result_t Read(stream_t &stream,  //NOLINT
                         value_t &value,
                         std::enable_if_t< TProps::stringEncoding() == StringEncoding::kUTF16 > *pBuff = nullptr)
    {
        std::ignore = pBuff;
        auto reverseOrder{(props_t::byteOrder() == ByteOrder::kMostSignificantByteFirst && IsLittleEndian())
                          || (props_t::byteOrder() == ByteOrder::kMostSignificantByteLast && !IsLittleEndian())};
        result_t ret{0};
        result_t retn{0};
        constexpr auto kU16bom{UTF16BOM()};
        constexpr auto kU16end{UTF16End()};
        length_t length{0};
        length_t len{0};
        if (props_t::SizeofStringLengthField() != 0) {
            retn = stream.Read(length, reverseOrder);
            if (retn < 0) {
                return retn;
            }
            ret += retn;
        } else {
            length = std::numeric_limits< length_t >::max();
        }
        if (props_t::implementsLegacyStringSerialization()) {
            if (length < kU16end.size() * 2) {
                return ErrorCode::kInvalidValue;  // Read value (UTF-16 string)
                                                  // length field value less than lower limit
            }
            length -= kU16end.size() * 2;
        } else {
            if (length < (kU16bom.size() + kU16end.size()) * 2) {
                return ErrorCode::kInvalidValue;  // Read value (UTF-16 string)
                                                  // length field value less than lower limit
            }
            length -= (kU16bom.size() + kU16end.size()) * 2;
            std::remove_cv_t< decltype(kU16bom) > bom{};
            for (size_t n{0}; n < bom.size(); ++n) {
                retn = stream.Read(bom[n], reverseOrder);
                if (retn < 0) {
                    return retn;
                }
                ret += retn;
            }
            if (bom != kU16bom) {
                return ErrorCode::kInvalidValue;  // Read value (UTF-16 string)
                                                  // BOM format does not conform to specification
            }
        }
        std::u16string u16;
        if (props_t::SizeofStringLengthField() != 0) {
            while (len < (length - 1)) {  // Not the end (inclusive odd number)
                uint16_t c{};
                retn = stream.Read(c, reverseOrder);
                if (retn < 0) {
                    return retn;
                }
                ret += retn;
                len += retn;
                u16 += c;  //NOLINT
            }
            std::remove_cv_t< decltype(kU16end) > end{};
            for (size_t n{0}; n < end.size(); ++n) {
                retn = stream.Read(end[n], reverseOrder);
                if (retn < 0) {
                    return retn;
                }
                ret += retn;
            }
            if (end != kU16end) {
                return ErrorCode::kInvalidValue;  // Read value (UTF-16 string)
                                                  // termination format does not conform to specification
            }
            if (len == (length - 1)) {  // Ignore value (UTF-16 string), compatible with odd number endings
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
                if (c == kU16end[0]) {
                    length = len;
                    break;
                }
                len += retn;
                u16 += c;  //NOLINT
            }
        }
        if (len != length) {
            return ErrorCode::kInvalidValue;  // Read length (UTF-16 string)
                                              // not equal to length field value
        }
        value = std::wstring_convert< std::codecvt_utf8_utf16< char16_t >, char16_t >{}.to_bytes(u16);
        return ret;
    }

private:
    /// @brief Get UTF-8 BOM mark
    /// @return UTF-8 BOM mark
    static constexpr ara::core::Array< uint8_t, 3 > UTF8BOM() { return {kInt8_0xEFU, kInt8_0xBBU, kInt8_0xBFU}; }
    /// @return UTF-8 terminator
    static constexpr ara::core::Array< uint8_t, 1 > UTF8End() { return {0x00U}; }
    /// @brief Get UTF-16 BOM mark
    /// @return UTF-16 BOM mark
    static constexpr ara::core::Array< uint16_t, 1 > UTF16BOM() { return {kInt16_0xFEFFU}; }
    /// @brief Get UTF-16 terminator
    /// @return UTF-16 terminator
    static constexpr ara::core::Array< uint16_t, 1 > UTF16End() { return {0x0000U}; }
};

/// @brief Template type -- Converter -- Specialization for structure (enumerable members) type
/// @tparam TStream Payload stream type
/// @tparam TValue Value type
template < typename TStream, typename TValue >
struct Translator< TStream,
                   TValue,
                   std::enable_if_t< IsEnumerable< TValue >::value || IsArgs< TValue >::value > >  //NOLINT
{
    /// @brief Type alias -- Payload stream type
    using stream_t = TStream;  // NOLINT
    /// @brief Type alias -- Value type
    using value_t = TValue;  // NOLINT
    /// @brief Type alias -- Property information
    using props_t = typename stream_t::props_t;  // NOLINT
    /// @brief Type alias -- Result type
    using result_t = typename stream_t::result_t;  // NOLINT
    /// @brief Type alias -- Bit position type
    using point_t = typename stream_t::point_t;  // NOLINT
    /// @brief Type alias -- Length field type
    using length_t = length_field_t< props_t::SizeofStructLengthField() >;  // NOLINT
    static_assert(props_t::SizeofStructLengthField() == 0 || props_t::SizeofStructLengthField() == 1
                      || props_t::SizeofStructLengthField() == 2 || props_t::SizeofStructLengthField() == 4,
                  "SizeofStructLengthField[0,1,2,4]");
    /// @brief Write
    /// @param[in] stream Payload stream
    /// @param[in] value Value
    /// @return Result -- >= 0: number of bytes written; < 0: error code
    static result_t Write(stream_t &stream, value_t const &value)
    {
        auto reverseOrder{(props_t::byteOrder() == ByteOrder::kMostSignificantByteFirst && IsLittleEndian())
                          || (props_t::byteOrder() == ByteOrder::kMostSignificantByteLast && !IsLittleEndian())};
        result_t ret{0};
        result_t retn{0};
        auto point{stream.Point()};
        length_t length{0};
        if (props_t::SizeofStructLengthField() != 0 && !IsArgs< value_t >::value) {
            retn = stream.Write(length, reverseOrder);
            if (retn < 0) {
                return retn;
            }
            ret += retn;
        }
        Writer writer{stream};
        const_cast< value_t & >(value).enumerate(writer);
        retn = writer.ret;
        if (retn < 0) {
            return retn;
        }
        ret += retn;
        length += retn;
        if (props_t::SizeofStructLengthField() != 0 && !IsArgs< value_t >::value) {
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
    /// @return Result -- >= 0: number of bytes read; < 0: error code
    template < typename TProps = props_t >
    static result_t Read(stream_t &stream, value_t &value)
    {
        auto reverseOrder{(props_t::byteOrder() == ByteOrder::kMostSignificantByteFirst && IsLittleEndian())
                          || (props_t::byteOrder() == ByteOrder::kMostSignificantByteLast && !IsLittleEndian())};
        result_t ret{0};
        result_t retn{0};
        length_t length{0};
        length_t len{0};
        if (props_t::SizeofStructLengthField() != 0 && !IsArgs< value_t >::value) {
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
        if (props_t::SizeofStructLengthField() != 0 && !IsArgs< value_t >::value) {
            if (len < length) {
                ara::core::Vector< char > _(length - len);
                StringBorrow cstring(_.data(), _.size());
                retn = stream.Read(cstring);  // Read length (structure) less than length field value (skip)
                if (retn < 0) {
                    return retn;
                }
                ret += retn;
                len += retn;
            }
            if (len != length) {
                return ErrorCode::kInvalidValue;  // Read length (structure) not equal to length field value
            }
        }
        return ret;
    }

private:
    /// @brief Writer
    struct Writer
    {
        /// @brief Payload stream
        stream_t &stream;
        /// @brief Return result
        result_t ret{0};
        /// @brief Operator -- call -- write
        /// @tparam T Value type
        /// @param[in] value Value
        template < typename T >
        void operator()(T const &value)
        {
            if (ret < 0) {
                return;
            }
            auto retn{Translator< stream_t, T >::Write(stream, value)};
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
        stream_t &stream;
        /// @brief Return result
        result_t ret{0};
        /// @brief Operator -- call -- read
        /// @tparam T Value type
        /// @param[in] value Value
        template < typename T >
        void operator()(T &value)
        {
            if (ret < 0) {
                return;
            }
            auto retn{Translator< stream_t, T >::Read(stream, value)};
            if (retn < 0) {
                ret = retn;
                return;
            }
            ret += retn;
        }
    };
};

/// @brief Template type -- Converter -- Specialization for structure (with tag field + length field + enumerable members) type
/// @tparam TStream Payload stream type
/// @tparam TValue Value type
template < typename TStream, typename TValue >
struct Translator< TStream, TValue, std::enable_if_t< IsTLV< TValue >::value > >  //NOLINT
{
    /// @brief Type alias -- Payload stream type
    using stream_t = TStream;  // NOLINT
    /// @brief Type alias -- Value type
    using value_t = TValue;  // NOLINT
    /// @brief Type alias -- Property information
    using props_t = typename stream_t::props_t;  // NOLINT
    /// @brief Type alias -- Result type
    using result_t = typename stream_t::result_t;  // NOLINT
    /// @brief Type alias -- Bit position type
    using point_t = typename stream_t::point_t;  // NOLINT
    /// @brief Type alias -- Length field type
    using length_t = length_field_t< props_t::SizeofStructLengthField() >;  // NOLINT
    /// @brief Type alias -- Type field length type
    using type_length_t = length_t;  // NOLINT
    /// @brief Type alias -- Tag field type
    using tag_field_t = uint16_t;  // NOLINT
    static_assert(props_t::SizeofStructLengthField() == 1 || props_t::SizeofStructLengthField() == 2
                      || props_t::SizeofStructLengthField() == 4,
                  "SizeofStructLengthField(TLV)[1,2,4]");
    /// @ref [PRS_SOMEIP_00242] -- The configuration size of the length field for arrays,
    /// structures, unions, and strings must be the same
    static_assert(props_t::SizeofStructLengthField() == props_t::SizeofArrayLengthField()
                      && props_t::SizeofStructLengthField() == props_t::SizeofStringLengthField()
                      && props_t::SizeofStructLengthField() == props_t::SizeofStructLengthField()  //NOLINT
                      && props_t::SizeofStructLengthField() == props_t::SzeofUnionLengthField(),
                  "sizeof*LengthField must be the same");
    /// @brief Write
    /// @param[in] stream Payload stream
    /// @param[in] value Value
    /// @return Result -- >= 0: number of bytes written; < 0: error code
    static result_t Write(stream_t &stream, value_t const &value)
    {
        auto reverseOrder{(props_t::byteOrder() == ByteOrder::kMostSignificantByteFirst && IsLittleEndian())
                          || (props_t::byteOrder() == ByteOrder::kMostSignificantByteLast && !IsLittleEndian())};
        result_t ret{0};
        result_t retn{0};
        auto point{stream.Point()};
        length_t length{0};
        retn = stream.Write(length, reverseOrder);
        if (retn < 0) {
            return retn;
        }
        ret += retn;
        Writer writer{stream};
        const_cast< value_t & >(value).enumerate(writer);
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
    /// @return Result -- >= 0: number of bytes read; < 0: error code
    static result_t Read(stream_t &stream, value_t &value)  //NOLINT
    {
        auto reverseOrder{(props_t::byteOrder() == ByteOrder::kMostSignificantByteFirst && IsLittleEndian())
                          || (props_t::byteOrder() == ByteOrder::kMostSignificantByteLast && !IsLittleEndian())};
        result_t ret{0};
        result_t retn{0};
        length_t length{0};
        length_t len{0};
        retn = stream.Read(length, reverseOrder);
        if (retn < 0) {
            return retn;
        }
        ret += retn;
        auto fields{value.required_fields()};
        while (len < length) {
            tag_field_t tag{0};
            /// @ref [PRS_SOMEIP_00203] -- tag is not affected by byteOrder (should be network byte order)
            retn = stream.Read(tag, IsLittleEndian());
            if (retn < 0) {
                return retn;
            }
            ret += retn;
            len += retn;
            if (len >= length) {
                return ErrorCode::kInvalidValue;  // Read length (TLV structure) greater than length field value
            }
            tag_field_t field{0};
            type_length_t typeLength{0};
            Tag2Info(tag, field, typeLength);
            if (typeLength == 0) {  // Non-basic (scalar) type
                auto point{stream.Point()};
                retn = stream.Read(typeLength, reverseOrder, point);
                if (retn < 0) {
                    return retn;
                }
                if (retn > static_cast< result_t >(length)) {
                    return ErrorCode::kInvalidValue;  // Read length (TLV structure) greater than length field value
                }
                typeLength += sizeof(typeLength);  // Non-basic (scalar) type read length retains length field length
            }
            auto itField{fields.find(field)};
            if (itField != fields.end()) {
                Reader reader{stream, typeLength};
                auto recognized{value.dispatch(field, reader)};
                if (reader.typeLength != 0) {
                    return ErrorCode::kInvalidValue;  // Read length (TLV structure) less than length field value
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
            retn = stream.Read(cstring);  // Read length (TLV structure) less than length field value (skip)
            if (retn < 0) {
                return retn;
            }
            ret += retn;
            len += retn;
        }
        if (len != length) {
            return ErrorCode::kInvalidValue;  // Read length (TLV structure) not equal to length field value
        }
        if (!fields.empty()) {
            /// @ref [PRS_SOMEIP_00218] -- Failure to find required field should terminate
            return ErrorCode::kInvalidValue;  // Required field not found in read value (TLV structure)
        }
        return ret;
    }

private:
    /// @brief Writer
    struct Writer
    {
        /// @brief Payload stream
        stream_t &stream;
        /// @brief Return result
        result_t ret{0};
        /// @brief Operator -- call -- write (basic/non-basic type)
        /// @tparam T Value type
        /// @param[in] value Value
        /// @param[in] field Field identifier
        /// @ref [PRS_SOMEIP_00212] -- Basic data types (wire type
        /// 0-3) configured with Data ID do not insert length field
        /// @ref [PRS_SOMEIP_00214] -- Non-basic data types (wire type
        /// 4-7) configured with Data ID insert length field (once)
        template < typename T >
        void operator()(T const &value, tag_field_t field)
        {
            if (ret < 0) {
                return;
            }
            result_t retn{0};
            tag_field_t tag{0};
            Info2Tag< T >(tag, field);
            /// @ref [PRS_SOMEIP_00203] -- tag is not affected by byteOrder (should be network byte order)
            retn = stream.Write(tag, IsLittleEndian());
            if (retn < 0) {
                ret = retn;
                return;
            }
            ret += retn;
            retn = Translator< stream_t, T >::Write(stream, value);
            if (retn < 0) {
                ret = retn;
                return;
            }
            ret += retn;
        }
        /// @brief Operator -- call -- write (fixed-size array type)
        /// @tparam T Value element type
        /// @tparam nSize Number of value elements
        /// @param[in] value Value
        /// @param[in] field Field identifier
        /// @ref [PRS_SOMEIP_00214] -- Non-basic data types (wire type
        /// 4-7) configured with Data ID insert length field (once)
        template < typename T, size_t nSize >
        void operator()(ara::core::Array< T, nSize > const &value, tag_field_t field)
        {
            auto reverseOrder{(props_t::byteOrder() == ByteOrder::kMostSignificantByteFirst && IsLittleEndian())
                              || (props_t::byteOrder() == ByteOrder::kMostSignificantByteLast && !IsLittleEndian())};
            if (ret < 0) {
                return;
            }
            result_t retn{0};
            tag_field_t tag{0};
            Info2Tag< decltype(value) >(tag, field);
            /// @ref [PRS_SOMEIP_00203] -- tag is not affected by byteOrder (should be network byte order)
            retn = stream.Write(tag, IsLittleEndian());
            if (retn < 0) {
                ret = retn;
                return;
            }
            ret += retn;
            auto point{stream.Point()};
            type_length_t typeLength{0};
            retn = stream.Write(typeLength, reverseOrder);
            if (retn < 0) {
                ret = retn;
                return;
            }
            ret += retn;
            retn = Translator< stream_t, ara::core::Array< T, nSize > >::Write(stream, value);
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
        void operator()(ara::core::Optional< T > const &value, tag_field_t field)
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
        stream_t &stream;
        /// @brief Type field length value
        type_length_t typeLength{0};
        /// @brief Return result
        result_t ret{0};
        /// @brief Operator -- call -- read
        /// @tparam T Value type
        /// @param[in] value Value
        template < typename T >
        void operator()(T &value)
        {
            if (ret < 0) {
                return;
            }
            if (typeLength == 0) {
                // Read value (TLV structure) has no remaining members
                return;
            }
            result_t retn{0};
            retn = Translator< stream_t, T >::Read(stream, value);
            if (retn < 0) {
                ret = retn;
                return;
            }
            if (retn > static_cast< result_t >(typeLength)) {
                ret = ErrorCode::kInvalidValue;  // Read length greater than length field value
                return;
            }
            ret += retn;
            typeLength -= retn;
        }
        /// @brief Operator -- call -- read
        /// @tparam T Value element type
        /// @tparam nSize Number of value elements
        /// @param[in] value Value
        template < typename T, size_t nSize >
        void operator()(ara::core::Array< T, nSize > &value)
        {
            if (ret < 0) {
                return;
            }
            if (typeLength == 0) {
                // Read value (TLV structure) has no remaining members
                return;
            }
            auto reverseOrder{(props_t::byteOrder() == ByteOrder::kMostSignificantByteFirst && IsLittleEndian())
                              || (props_t::byteOrder() == ByteOrder::kMostSignificantByteLast && !IsLittleEndian())};
            result_t retn{0};
            length_t length{0};
            retn = stream.Read(length, reverseOrder);
            if (retn < 0) {
                ret = retn;
                return;
            }
            ret += retn;
            typeLength -= retn;
            retn = Translator< stream_t, ara::core::Array< T, nSize > >(stream, value);
            if (retn < 0) {
                ret = retn;
                return;
            }
            if (retn > static_cast< result_t >(typeLength)) {
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
        void operator()(ara::core::Optional< T > &value)
        {
            if (ret < 0) {
                return;
            }
            if (typeLength == 0) {
                // Read value (TLV structure) has no remaining members
                return;
            }
            value.emplace();
            operator()(*value);
        }
    };
    /// @brief Wire type enumeration
    enum class WireTypes : uint16_t
    {
        kBase8Bit             = 0U,
        kBase16Bit            = 1U,
        kBase32Bit            = 2U,
        kBase64Bit            = 3U,
        kComplexWithLength    = 4U,
        kComplexForceLength8  = 5U,
        kComplexForceLength16 = 6U,
        kComplexForceLength32 = 7U
    };
    /// @brief Convert data info to tag
    /// @tparam T Value type
    /// @param[in] tag Tag
    /// @param[in] field Field identifier
    template < typename T >
    static void Info2Tag(tag_field_t &tag, tag_field_t field)
    {
        assert((field & kInt16_0x0FFFU) == field);
        tag = (field & kInt16_0x0FFFU) | (static_cast< tag_field_t >(Length2WireType< T >()) << kInt32_12U);  //NOLINT
    }
    /// @brief Convert tag to data info
    /// @param[in] tag Tag
    /// @param[in] field Field identifier
    /// @param[in] typeLength Type field length
    static void Tag2Info(tag_field_t tag, tag_field_t &field, type_length_t &typeLength)
    {
        field      = tag & kInt16_0x0FFFU;
        typeLength = WireType2Length(static_cast< WireTypes >((tag >> kInt32_12U) & kInt32_0x0007U));  //NOLINT
    }
    /// @brief Convert data info to wire type -- occupies 1-byte basic type
    /// @tparam T Value type
    /// @return Wire type
    template < typename T >
    static constexpr WireTypes Length2WireType(std::enable_if_t< IsScalar< T >::value && sizeof(T) == 1 > *pBuff
                                               = nullptr)
    {
        std::ignore = pBuff;
        return WireTypes::kBase8Bit;
    }
    /// @brief Convert data info to wire type -- occupies 2-byte basic type
    /// @tparam T Value type
    /// @return Wire type
    template < typename T >
    static constexpr WireTypes Length2WireType(std::enable_if_t< IsScalar< T >::value && sizeof(T) == 2 > *pBuff
                                               = nullptr)
    {
        std::ignore = pBuff;
        return WireTypes::kBase16Bit;
    }
    /// @brief Convert data info to wire type -- occupies 4-byte basic type
    /// @tparam T Value type
    /// @return Wire type
    template < typename T >
    static constexpr WireTypes Length2WireType(std::enable_if_t< IsScalar< T >::value && sizeof(T) == 4 > *pBuff
                                               = nullptr)
    {
        std::ignore = pBuff;
        return WireTypes::kBase32Bit;
    }
    /// @brief Convert data info to wire type -- occupies 8-byte basic type
    /// @tparam T Value type
    /// @return Wire type
    template < typename T >
    static constexpr WireTypes Length2WireType(std::enable_if_t< IsScalar< T >::value && sizeof(T) == kInt32_8U > *pBuff
                                               = nullptr)
    {
        std::ignore = pBuff;
        return WireTypes::kBase64Bit;
    }
    /// @brief Convert data info to wire type -- non-basic type (fixed length)
    /// @tparam T Value type
    /// @return Wire type
    template < typename T >
    static constexpr WireTypes Length2WireType(
        std::enable_if_t< !IsScalar< T >::value && !props_t::isDynamicLengthFieldSize() > *pBuff = nullptr)
    {
        std::ignore = pBuff;
        return WireTypes::kComplexWithLength;
    }
    /// @brief Convert data info to wire type -- non-basic type (variable length occupies 1 byte)
    /// @tparam T Value type
    /// @return Wire type
    template < typename T >
    static constexpr WireTypes Length2WireType(
        std::enable_if_t< !IsScalar< T >::value && props_t::isDynamicLengthFieldSize()
                          && props_t::SizeofArrayLengthField() == 1 && props_t::SizeofStringLengthField() == 1
                          && props_t::SizeofStructLengthField() == 1 && props_t::SzeofUnionLengthField() == 1 > *pBuff
        = nullptr)
    {
        std::ignore = pBuff;
        return WireTypes::kComplexForceLength8;
    }
    /// @brief Convert data info to wire type -- non-basic type (variable length occupies 2 bytes)
    /// @tparam T Value type
    /// @return Wire type
    template < typename T >
    static constexpr WireTypes Length2WireType(
        std::enable_if_t< !IsScalar< T >::value && props_t::isDynamicLengthFieldSize()
                          && props_t::SizeofArrayLengthField() == 2 && props_t::SizeofStringLengthField() == 2
                          && props_t::SizeofStructLengthField() == 2 && props_t::SzeofUnionLengthField() == 2 > *pBuff
        = nullptr)
    {
        std::ignore = pBuff;
        return WireTypes::kComplexForceLength16;
    }
    /// @brief Convert data info to wire type -- non-basic type (variable length occupies 4 bytes)
    /// @tparam T Value type
    /// @return Wire type
    template < typename T >
    static constexpr WireTypes Length2WireType(
        std::enable_if_t< !IsScalar< T >::value && props_t::isDynamicLengthFieldSize()
                          && props_t::SizeofArrayLengthField() == 4 && props_t::SizeofStringLengthField() == 4
                          && props_t::SizeofStructLengthField() == 4 && props_t::SzeofUnionLengthField() == 4 > *pBuff
        = nullptr)
    {
        std::ignore = pBuff;
        return WireTypes::kComplexForceLength32;
    }
    /// @brief Convert data info to wire type -- optional value type
    /// @tparam T Value type
    /// @return Wire type
    template < typename T >
    static constexpr WireTypes Length2WireType(std::enable_if_t< ara::core::is_optional< T >::value > *pBuff = nullptr)
    {
        std::ignore = pBuff;
        return Length2WireType< T::value_type >();
    }
    /// @brief Convert wire type to type field length
    /// @param[in] wireType Wire type
    /// @return Type field length
    static type_length_t WireType2Length(WireTypes wireType)
    {
        switch (wireType) {
            case WireTypes::kBase8Bit:
                return 1;
            case WireTypes::kBase16Bit:
                return 2;
            case WireTypes::kBase32Bit:
                return 4;
            case WireTypes::kBase64Bit:
                return kInt32_8U;
            default:
                break;
        }
        return 0;
    }
};

/// @brief Template type -- Converter -- Specialization for structure (S2S conversion) type
/// @tparam TStream Payload stream type
/// @tparam TValue Value type
template < typename TStream, typename TValue >
struct Translator< TStream, TValue, std::enable_if_t< IsS2S< TValue >::value > >  //NOLINT
{
    /// @brief Type alias -- Payload stream type
    using stream_t = TStream;  // NOLINT
    /// @brief Type alias -- Value type
    using value_t = TValue;  // NOLINT
    /// @brief Type alias -- Property information
    using props_t = typename stream_t::props_t;  // NOLINT
    /// @brief Type alias -- Result type
    using result_t = typename stream_t::result_t;  // NOLINT
    /// @brief Type alias -- Bit position type
    using point_t = typename stream_t::point_t;  // NOLINT
    /// @brief Write
    /// @param[in] stream Payload stream
    /// @param[in] value Value
    /// @return Result -- >= 0: number of bytes written; < 0: error code
    static result_t Write(stream_t &stream, value_t const &value)
    {
        Writer writer{stream};
        const_cast< value_t & >(value).enumerate(writer);
        return writer.ret;
    }
    /// @brief Read
    /// @param[in] stream Payload stream
    /// @param[in] value Value
    /// @return Result -- >= 0: number of bytes read; < 0: error code
    static result_t Read(stream_t &stream, value_t &value)
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
        stream_t &stream;
        /// @brief Return result
        result_t ret{0};
        /// @brief Operator -- call -- write (basic type)
        /// @tparam T Value type
        /// @param[in] value Value
        /// @param[in] bit Starting bit
        /// @param[in] msb Byte order
        template < typename T >
        void operator()(T value,
                        uint16_t bit,
                        bool msb                                        = true,
                        std::enable_if_t< IsScalar< T >::value > *pBuff = nullptr)
        {
            auto reverseOrder{(msb && IsLittleEndian()) || (!msb && !IsLittleEndian())};
            std::ignore = bit;
            std::ignore = pBuff;
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
        /// @brief Operator -- call -- write (fixed-size array type)
        /// @tparam T Value type
        /// @tparam nSize Number of value elements
        /// @param[in] value Value
        /// @param[in] bit Starting bit
        /// @param[in] msb Byte order
        template < typename T, size_t nSize >
        void operator()(T const (&value)[nSize],
                        uint16_t bit,
                        bool msb                                                          = true,
                        std::enable_if_t< IsScalar< T >::value && sizeof(T) == 1 > *pBuff = nullptr)
        {
            std::ignore = bit;
            std::ignore = msb;
            std::ignore = pBuff;
            if (ret < 0) {
                return;
            }
            StringBorrow cstring(reinterpret_cast< char const * >(value), nSize);
            auto retn{stream.Write(cstring)};
            if (retn < 0) {
                ret = retn;
                return;
            }
            ret += retn;
        }
        /// @brief Operator -- call -- write (fixed-size array type)
        /// @tparam T Value element type
        /// @tparam nSize Number of value elements
        /// @param[in] value Value
        /// @param[in] bit Starting bit
        /// @param[in] msb Byte order
        template < typename T, size_t nSize >
        void operator()(ara::core::Array< T, nSize > const &value,
                        uint16_t bit,
                        bool msb                                                          = true,
                        std::enable_if_t< IsScalar< T >::value && sizeof(T) == 1 > *pBuff = nullptr)
        {
            std::ignore = bit;
            std::ignore = msb;
            std::ignore = pBuff;
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
        /// @param[in] bit Starting bit
        /// @param[in] msb Byte order
        void operator()(ara::core::String const &value, uint16_t bit, bool msb = true)
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
        stream_t &stream;
        /// @brief Return result
        result_t ret{0};
        /// @brief Operator -- call -- read (basic type)
        /// @tparam T Value type
        /// @param[in] value Value
        /// @param[in] bit Starting bit
        /// @param[in] msb Byte order
        template < typename T >
        void operator()(T &value,
                        uint16_t bit,
                        bool msb                                        = true,
                        std::enable_if_t< IsScalar< T >::value > *pBuff = nullptr)
        {
            auto reverseOrder{(msb && IsLittleEndian()) || (!msb && !IsLittleEndian())};
            std::ignore = bit;
            std::ignore = pBuff;
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
        /// @brief Operator -- call -- read (fixed-size array type)
        /// @tparam T Value type
        /// @tparam nSize Number of value elements
        /// @param[in] value Value
        /// @param[in] bit Starting bit
        /// @param[in] msb Byte order
        template < typename T, size_t nSize >
        void operator()(T (&value)[nSize],
                        uint16_t bit,
                        bool msb                                                          = true,
                        std::enable_if_t< IsScalar< T >::value && sizeof(T) == 1 > *pBuff = nullptr)
        {
            std::ignore = bit;
            std::ignore = msb;
            std::ignore = pBuff;
            if (ret < 0) {
                return;
            }
            StringBorrow cstring(reinterpret_cast< char * >(value), nSize);
            auto retn{stream.Read(cstring)};
            if (retn < 0) {
                ret = retn;
                return;
            }
            ret += retn;
        }
        /// @brief Operator -- call -- read (fixed-size array type)
        /// @tparam T Value element type
        /// @tparam nSize Number of value elements
        /// @param[in] value Value
        /// @param[in] bit Starting bit
        /// @param[in] msb Byte order
        template < typename T, size_t nSize >
        void operator()(ara::core::Array< T, nSize > &value,
                        uint16_t bit,
                        bool msb                                                          = true,
                        std::enable_if_t< IsScalar< T >::value && sizeof(T) == 1 > *pBuff = nullptr)
        {
            std::ignore = bit;
            std::ignore = msb;
            std::ignore = pBuff;
            if (ret < 0) {
                return;
            }
            StringBorrow cstring(reinterpret_cast< char * >(value.data()), value.size());
            auto retn{stream.Read(cstring)};
            if (retn < 0) {
                ret = retn;
                return;
            }
            ret += retn;
        }
        /// @brief Operator -- call -- read (string type)
        /// @param[in] value Value
        /// @param[in] bit Starting bit
        /// @param[in] msb Byte order
        void operator()(ara::core::String &value, uint16_t bit, bool msb = true)
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

/// @brief Template type -- Converter -- Specialization for associative map container type
/// @tparam TStream Payload stream type
/// @tparam TValue Value type
template < typename TStream, typename TValue >
struct Translator< TStream, TValue, std::enable_if_t< IsAssociativeContainer< TValue >::value > >  //NOLINT
{
    /// @brief Type alias -- Payload stream type
    using stream_t = TStream;  // NOLINT
    /// @brief Type alias -- Value type
    using value_t = TValue;  // NOLINT
    /// @brief Type alias -- Property information
    using props_t = typename stream_t::props_t;  // NOLINT
    /// @brief Type alias -- Result type
    using result_t = typename stream_t::result_t;  // NOLINT
    /// @brief Type alias -- Bit position type
    using point_t = typename stream_t::point_t;  // NOLINT
    /// @brief Type alias -- Length field type
    using length_t = length_field_t< props_t::SizeofArrayLengthField() >;  // NOLINT
    static_assert(props_t::SizeofArrayLengthField() == 1 || props_t::SizeofArrayLengthField() == 2
                      || props_t::SizeofArrayLengthField() == 4,
                  "SizeofArrayLengthField(Associative)[1,2,4]");
    /// @brief Write
    /// @param[in] stream Payload stream
    /// @param[in] value Value
    /// @return Result -- >= 0: number of bytes written; < 0: error code
    static result_t Write(stream_t &stream, value_t const &value)
    {
        auto reverseOrder{(props_t::byteOrder() == ByteOrder::kMostSignificantByteFirst && IsLittleEndian())
                          || (props_t::byteOrder() == ByteOrder::kMostSignificantByteLast && !IsLittleEndian())};
        result_t ret{0};
        result_t retn{0};
        auto point{stream.Point()};
        length_t length{0};
        retn = stream.Write(length, reverseOrder);
        if (retn < 0) {
            return retn;
        }
        ret += retn;
        /// @brief Value element type
        using TElement = typename value_t::value_type;
        for (auto const &it : value) {
            retn = Translator< stream_t, TElement >::Write(stream, it);
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
    /// @return Result -- >= 0: number of bytes read; < 0: error code
    static result_t Read(stream_t &stream, value_t &value)
    {
        auto reverseOrder{(props_t::byteOrder() == ByteOrder::kMostSignificantByteFirst && IsLittleEndian())
                          || (props_t::byteOrder() == ByteOrder::kMostSignificantByteLast && !IsLittleEndian())};
        result_t ret{0};
        result_t retn{0};
        length_t length{0};
        length_t len{0};
        retn = stream.Read(length, reverseOrder);
        if (retn < 0) {
            return retn;
        }
        ret += retn;
        /// @brief Value element type
        using TElement = typename value_t::value_type;
        value.clear();
        while (len < length) {
            TElement it;
            retn = Translator< stream_t, TElement >::Read(stream, it);
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

/// @brief Template type -- Converter -- Specialization for sequence container type
/// @tparam TStream Payload stream type
/// @tparam TValue Value type
template < typename TStream, typename TValue >
struct Translator< TStream, TValue, std::enable_if_t< IsSequenceContainer< TValue >::value > >  //NOLINT
{
    /// @brief Type alias -- Payload stream type
    using stream_t = TStream;  // NOLINT
    /// @brief Type alias -- Value type
    using value_t = TValue;  // NOLINT
    /// @brief Type alias -- Property information
    using props_t = typename stream_t::props_t;  // NOLINT
    /// @brief Type alias -- Result type
    using result_t = typename stream_t::result_t;  // NOLINT
    /// @brief Type alias -- Bit position type
    using point_t = typename stream_t::point_t;  // NOLINT
    /// @brief Type alias -- Length field type
    using length_t = length_field_t< props_t::SizeofArrayLengthField() >;  // NOLINT
    static_assert(props_t::SizeofArrayLengthField() == 1 || props_t::SizeofArrayLengthField() == 2
                      || props_t::SizeofArrayLengthField() == 4,
                  "SizeofArrayLengthField(Sequence)[1,2,4]");
    /// @brief Write -- !std::is_same<TElement, bool>::value
    /// @param[in] stream Payload stream
    /// @param[in] value Value
    /// @return Result -- >= 0: number of bytes written; < 0: error code
    template < typename TElement = typename value_t::value_type >
    static result_t Write(stream_t &stream,
                          value_t const &value,
                          std::enable_if_t< !std::is_same< TElement, bool >::value > *pBuff = nullptr)
    {
        std::ignore = pBuff;
        auto reverseOrder{(props_t::byteOrder() == ByteOrder::kMostSignificantByteFirst && IsLittleEndian())
                          || (props_t::byteOrder() == ByteOrder::kMostSignificantByteLast && !IsLittleEndian())};
        result_t ret{0};
        result_t retn{0};
        auto point{stream.Point()};
        length_t length{0};
        retn = stream.Write(length, reverseOrder);
        if (retn < 0) {
            return retn;
        }
        ret += retn;
        if (!IsScalar< TElement >::value || sizeof(TElement) != 1) {
            for (auto const &it : value) {
                retn = Translator< stream_t, TElement >::Write(stream, it);
                if (retn < 0) {
                    return retn;
                }
                ret += retn;
                length += retn;
            }
        } else {
            StringBorrow cstring(reinterpret_cast< char const * >(value.data()), value.size());
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
    /// @brief Write -- std::is_same<TElement, bool>::value
    /// @param[in] stream Payload stream
    /// @param[in] value Value
    /// @return Result -- >= 0: number of bytes written; < 0: error code
    template < typename TElement = typename value_t::value_type >
    static result_t Write(stream_t &stream,
                          value_t const &value,
                          std::enable_if_t< std::is_same< TElement, bool >::value > *pBuff = nullptr)
    {
        std::ignore = pBuff;
        auto reverseOrder{(props_t::byteOrder() == ByteOrder::kMostSignificantByteFirst && IsLittleEndian())
                          || (props_t::byteOrder() == ByteOrder::kMostSignificantByteLast && !IsLittleEndian())};
        result_t ret{0};
        result_t retn{0};
        auto point{stream.Point()};
        length_t length{0};
        retn = stream.Write(length, reverseOrder);
        if (retn < 0) {
            return retn;
        }
        ret += retn;
        for (auto const &it : value) {
            retn = Translator< stream_t, TElement >::Write(stream, it);
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
    /// @brief Read -- !std::is_same<TElement, bool>::value
    /// @param[in] stream Payload stream
    /// @param[in] value Value
    /// @return Result -- >= 0: number of bytes read; < 0: error code
    template < typename TElement = typename value_t::value_type >
    static result_t Read(stream_t &stream,
                         value_t &value,
                         std::enable_if_t< !std::is_same< TElement, bool >::value > *pBuff = nullptr)
    {
        std::ignore = pBuff;
        auto reverseOrder{(props_t::byteOrder() == ByteOrder::kMostSignificantByteFirst && IsLittleEndian())
                          || (props_t::byteOrder() == ByteOrder::kMostSignificantByteLast && !IsLittleEndian())};
        result_t ret{0};
        result_t retn{0};
        length_t length{0};
        length_t len{0};
        retn = stream.Read(length, reverseOrder);
        if (retn < 0) {
            return retn;
        }
        ret += retn;
        if (!IsScalar< TElement >::value || sizeof(TElement) != 1) {
            value.clear();
            while (len < length) {
                value.emplace_back();
                retn = Translator< stream_t, TElement >::Read(stream, value.back());
                if (retn < 0) {
                    return retn;
                }
                ret += retn;
                len += retn;
            }
        } else {
            try {
                value.resize(length);
            } catch (std::exception const &e) {
                return ErrorCode::kInvalidValue;  // Read value (dynamic array) length field value setting failed
            }
            StringBorrow cstring(reinterpret_cast< char * >(value.data()), value.size());
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
    /// @brief Read -- std::is_same<TElement, bool>::value
    /// @param[in] stream Payload stream
    /// @param[in] value Value
    /// @return Result -- >= 0: number of bytes read; < 0: error code
    template < typename TElement = typename value_t::value_type >
    static result_t Read(stream_t &stream,
                         value_t &value,
                         std::enable_if_t< std::is_same< TElement, bool >::value > *pBuff = nullptr)
    {
        std::ignore = pBuff;
        auto reverseOrder{(props_t::byteOrder() == ByteOrder::kMostSignificantByteFirst && IsLittleEndian())
                          || (props_t::byteOrder() == ByteOrder::kMostSignificantByteLast && !IsLittleEndian())};
        result_t ret{0};
        result_t retn{0};
        length_t length{0};
        length_t len{0};
        retn = stream.Read(length, reverseOrder);
        if (retn < 0) {
            return retn;
        }
        ret += retn;
        value.clear();
        while (len < length) {
#if 0  // Disable `bool& v = value.back()`, vector<bool> implementation does not support returning bit reference
            value.emplace_back();
            retn = Translator<stream_t, TElement>::Read(stream, value.back());
#else  // Allow `value.back() = true`
            TElement v;
            retn = Translator< stream_t, TElement >::Read(stream, v);
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

/// @brief Template type -- Converter -- Specialization for sequence container (fixed-size array + standard length field) type
/// @tparam TStream Payload stream type
/// @tparam T Value element type
/// @tparam N Number of value elements
template < typename TStream, typename T, size_t nSize >
struct Translator< TStream,  //NOLINT
                   ara::core::Array< T, nSize >,
                   std::enable_if_t< !EnableFixedArrayLengthField< typename TStream::props_t >::value > >  //NOLINT
{
    /// @brief Type alias -- Payload stream type
    using stream_t = TStream;  // NOLINT
    /// @brief Type alias -- Value type
    using value_t = ara::core::Array< T, nSize >;  // NOLINT
    /// @brief Type alias -- Property information
    using props_t = typename stream_t::props_t;  // NOLINT
    /// @brief Type alias -- Result type
    using result_t = typename stream_t::result_t;  // NOLINT
    /// @brief Type alias -- Bit position type
    using point_t = typename stream_t::point_t;  // NOLINT
    /// @brief Type alias -- Length field type
    using length_t = length_field_t< props_t::SizeofArrayLengthField() >;  // NOLINT
    static_assert(props_t::SizeofArrayLengthField() == 0 || props_t::SizeofArrayLengthField() == 1
                      || props_t::SizeofArrayLengthField() == 2 || props_t::SizeofArrayLengthField() == 4,
                  "SizeofArrayLengthField(Array)[0,1,2,4]");
    /// @brief Write
    /// @param[in] stream Payload stream
    /// @param[in] value Value
    /// @return Result -- >= 0: number of bytes written; < 0: error code
    static result_t Write(stream_t &stream, value_t const &value)
    {
        auto reverseOrder{(props_t::byteOrder() == ByteOrder::kMostSignificantByteFirst && IsLittleEndian())
                          || (props_t::byteOrder() == ByteOrder::kMostSignificantByteLast && !IsLittleEndian())};
        result_t ret{0};
        result_t retn{0};
        auto point{stream.Point()};
        length_t length{0};
        if (props_t::SizeofArrayLengthField() != 0) {
            retn = stream.Write(length, reverseOrder);
            if (retn < 0) {
                return retn;
            }
            ret += retn;
        }
        /// @brief Value element type
        using TElement = T;
        if (!IsScalar< TElement >::value || sizeof(TElement) != 1 || std::is_same< TElement, bool >::value) {
            for (size_t n{0}; n < nSize; ++n) {
                retn = Translator< stream_t, TElement >::Write(stream, value[n]);
                if (retn < 0) {
                    return retn;
                }
                ret += retn;
                length += retn;
            }
        } else {
            StringBorrow cstring(reinterpret_cast< char const * >(value.data()), value.size());
            retn = stream.Write(cstring);
            if (retn < 0) {
                return retn;
            }
            ret += retn;
            length += retn;
        }
        if (props_t::SizeofArrayLengthField() != 0) {
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
    /// @return Result -- >= 0: number of bytes read; < 0: error code
    static result_t Read(stream_t &stream, value_t &value)
    {
        auto reverseOrder{(props_t::byteOrder() == ByteOrder::kMostSignificantByteFirst && IsLittleEndian())
                          || (props_t::byteOrder() == ByteOrder::kMostSignificantByteLast && !IsLittleEndian())};
        result_t ret{0};
        result_t retn{0};
        length_t length{0};
        length_t len{0};
        if (props_t::SizeofArrayLengthField() != 0) {
            retn = stream.Read(length, reverseOrder);
            if (retn < 0) {
                return retn;
            }
            ret += retn;
        }
        /// @brief Value element type
        using TElement = T;
        if (!IsScalar< TElement >::value || sizeof(TElement) != 1 || std::is_same< TElement, bool >::value) {
            for (size_t n{0}; n < nSize; ++n) {
                retn = Translator< stream_t, TElement >::Read(stream, value[n]);
                if (retn < 0) {
                    return retn;
                }
                ret += retn;
                len += retn;
            }
        } else {
            StringBorrow cstring(reinterpret_cast< char * >(value.data()), value.size());
            retn = stream.Read(cstring);
            if (retn < 0) {
                return retn;
            }
            ret += retn;
            len += retn;
        }
        if (props_t::SizeofArrayLengthField() != 0) {
            if (len != length) {
                return ErrorCode::kInvalidValue;  // Read length (fixed-size array) not equal to length field value
            }
        }
        return ret;
    }
};

/// @brief Template type -- Converter -- Specialization for sequence container (fixed-size array + extended length field) type
/// @tparam TStream Payload stream type
/// @tparam T Value element type
/// @tparam N Number of value elements
template < typename TStream, typename T, size_t N >  //NOLINT
struct Translator< TStream,                          //NOLINT
                   ara::core::Array< T, N >,         //NOLINT
                   std::enable_if_t< EnableFixedArrayLengthField< typename TStream::props_t >::value > >
{
    /// @brief Type alias -- Payload stream type
    using stream_t = TStream;  //NOLINT
    /// @brief Type alias -- Value type
    using value_t = ara::core::Array< T, N >;  //NOLINT
    /// @brief Type alias -- Property information
    using props_t = typename stream_t::props_t;  //NOLINT
    /// @brief Type alias -- Result type
    using result_t = typename stream_t::result_t;  //NOLINT
    /// @brief Type alias -- Bit position type
    using point_t = typename stream_t::point_t;  //NOLINT
    /// @brief Type alias -- Length field type
    using length_t = length_field_t< props_t::SizeofFixedArrayLengthField() >;  //NOLINT
    static_assert(props_t::SizeofFixedArrayLengthField() == 0 || props_t::SizeofFixedArrayLengthField() == 1
                      || props_t::SizeofFixedArrayLengthField() == 2 || props_t::SizeofFixedArrayLengthField() == 4,
                  "SizeofFixedArrayLengthField(Array)[0,1,2,4]");
    /// @brief Write
    /// @param[in] stream Payload stream
    /// @param[in] value Value
    /// @return Result -- >= 0: number of bytes written; < 0: error code
    static result_t Write(stream_t &stream, value_t const &value)
    {
        auto reverseOrder{(props_t::byteOrder() == ByteOrder::kMostSignificantByteFirst && IsLittleEndian())
                          || (props_t::byteOrder() == ByteOrder::kMostSignificantByteLast && !IsLittleEndian())};
        result_t ret{0};
        result_t retn{0};
        auto point{stream.Point()};
        length_t length{0};
        if (props_t::SizeofFixedArrayLengthField() != 0) {
            retn = stream.Write(length, reverseOrder);
            if (retn < 0) {
                return retn;
            }
            ret += retn;
        }
        /// @brief Value element type
        using element_t = T;  //NOLINT
        if (!IsScalar< element_t >::value || sizeof(element_t) != 1 || std::is_same< element_t, bool >::value) {
            for (size_t n{0}; n < N; ++n) {  //NOLINT
                retn = Translator< stream_t, element_t >::Write(stream, value[n]);
                if (retn < 0) {
                    return retn;
                }
                ret += retn;
                length += retn;
            }
        } else {
            StringBorrow cstring(reinterpret_cast< char const * >(value.data()), value.size());
            retn = stream.Write(cstring);
            if (retn < 0) {
                return retn;
            }
            ret += retn;
            length += retn;
        }
        if (props_t::SizeofFixedArrayLengthField() != 0) {
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
    /// @return Result -- >= 0: number of bytes read; < 0: error code
    static result_t Read(stream_t &stream, value_t &value)
    {
        auto reverseOrder{(props_t::byteOrder() == ByteOrder::kMostSignificantByteFirst && IsLittleEndian())
                          || (props_t::byteOrder() == ByteOrder::kMostSignificantByteLast && !IsLittleEndian())};
        result_t ret{0};
        result_t retn{0};
        length_t length{0};
        length_t len{0};
        if (props_t::SizeofFixedArrayLengthField() != 0) {
            retn = stream.Read(length, reverseOrder);
            if (retn < 0) {
                return retn;
            }
            ret += retn;
        }
        /// @brief Value element type
        using element_t = T;  //NOLINT
        if (!IsScalar< element_t >::value || sizeof(element_t) != 1 || std::is_same< element_t, bool >::value) {
            for (size_t n{0}; n < N; ++n) {
                retn = Translator< stream_t, element_t >::Read(stream, value[n]);
                if (retn < 0) {
                    return retn;
                }
                ret += retn;
                len += retn;
            }
        } else {
            StringBorrow cstring(reinterpret_cast< char * >(value.data()), value.size());
            retn = stream.Read(cstring);
            if (retn < 0) {
                return retn;
            }
            ret += retn;
            len += retn;
        }
        if (props_t::SizeofFixedArrayLengthField() != 0) {
            if (len != length) {
                return ErrorCode::kInvalidValue;  // Read length (fixed-size array) not equal to length field value
            }
        }
        return ret;
    }
};

/// @brief Template type -- Converter -- Specialization for associative container (key-value pair) type
/// @tparam TStream Payload stream type
/// @tparam T1 Key type
/// @tparam T2 Value type
template < typename TStream, typename T1, typename T2 >
struct Translator< TStream, std::pair< T1, T2 > >  //NOLINT
{
    /// @brief Type alias -- Payload stream type
    using stream_t = TStream;  //NOLINT
    /// @brief Type alias -- Value type
    using value_t = std::pair< T1, T2 >;  //NOLINT
    /// @brief Type alias -- Property information
    using props_t = typename stream_t::props_t;  //NOLINT
    /// @brief Type alias -- Result type
    using result_t = typename stream_t::result_t;  //NOLINT
    /// @brief Type alias -- Bit position type
    using point_t = typename stream_t::point_t;  //NOLINT
    /// @brief Write
    /// @param stream Payload stream
    /// @param value Value
    /// @return result_t
    /// @throws none
    static result_t Write(stream_t &stream, value_t const &value)
    {
        result_t ret{0};
        result_t retn{SerializeImpl(stream, value.first)};
        if (retn < 0) {
            return retn;
        }
        ret += retn;
        retn = SerializeImpl(stream, value.second);
        if (retn < 0) {
            return retn;
        }
        ret += retn;
        return ret;
    }
    /// @brief Read
    /// @param stream Payload stream
    /// @param value Value
    /// @return result_t
    /// @throws none
    static result_t Read(stream_t &stream, value_t &value)
    {
        result_t ret{0};
        std::decay_t< typename value_t::first_type > &valueFirst{
            const_cast< std::decay_t< typename value_t::first_type > & >(value.first)};
        result_t retn{DeserializeImpl(stream, valueFirst)};
        if (retn < 0) {
            return retn;
        }
        ret += retn;
        retn = DeserializeImpl(stream, value.second);
        if (retn < 0) {
            return retn;
        }
        ret += retn;
        return ret;
    }
};

/// @brief Deserialization error code
/// @param[in] value Value
/// @param[in] errorCode Error code
/// @param[in] domainId Error domain identifier
/// @param[in] supportData Error support data
/// @return Result -- >= 0: number of bytes read; < 0: error code
bool DeserializeErrorCode(ara::core::ErrorCode &value,
                          ara::core::ErrorDomain::CodeType const &errorCode,
                          ara::core::ErrorDomain::IdType const &domainId,
                          ara::core::ErrorDomain::SupportDataType const &supportData);
/// @brief Template type -- Converter -- Specialization for error type
/// @tparam TStream Payload stream type
template < typename TStream >
struct Translator< TStream, ara::core::ErrorCode >  //NOLINT
{
    /// @brief Type alias -- Payload stream type
    using stream_t = TStream;  // NOLINT
    /// @brief Type alias -- Value type
    using value_t = ara::core::ErrorCode;  // NOLINT
    /// @brief Type alias -- Property information
    using props_t = typename stream_t::props_t;  // NOLINT
    /// @brief Type alias -- Result type
    using result_t = typename stream_t::result_t;  // NOLINT
    /// @brief Type alias -- Bit position type
    using point_t = typename stream_t::point_t;  // NOLINT
    /// @brief Write
    /// @param[in] stream Payload stream
    /// @param[in] value Value
    /// @return Result -- >= 0: number of bytes written; < 0: error code
    static result_t Write(stream_t &stream, value_t const &value)
    {
        return SerializeImpl(stream, UnionLength(), UnionType(), StructLength(), value.Domain().Id(), value.Value(),
                             value.SupportData());
    }
    /// @brief Read
    /// @param[in] stream Payload stream
    /// @param[in] value Value
    /// @return Result -- >= 0: number of bytes read; < 0: error code
    static result_t Read(stream_t &stream, value_t &value)
    {
        std::ignore = value;
        result_t ret{0};
        result_t retn{0};
        union_length_t unionLength{0U};
        union_type_t unionType{0U};
        struct_length_t structLength{0U};
        domain_id_t domainId{0U};
        error_code_t errorCode{0U};
        support_data_t supportData{0U};
        retn = DeserializeImpl(stream, unionLength, unionType, structLength, domainId, errorCode, supportData);
        if (retn < 0) {
            return retn;
        }

        ret += retn;
        return ret;
    }

private:
    /// @brief Type alias -- Union length type
    using union_length_t = uint32_t;  // NOLINT
    /// @brief Type alias -- Union type type
    using union_type_t = uint8_t;  // NOLINT
    /// @brief Type alias -- Structure length type
    using struct_length_t = uint16_t;  // NOLINT
    /// @brief Type alias -- Error domain identifier type
    using domain_id_t = typename ara::core::ErrorDomain::IdType;  // NOLINT
    /// @brief Type alias -- Error code type
    using error_code_t = typename ara::core::ErrorDomain::CodeType;  // NOLINT
    /// @brief Type alias -- Error support data type
    using support_data_t = typename ara::core::ErrorDomain::SupportDataType;  // NOLINT
    /// @brief Get structure length
    /// @return Structure length
    static constexpr struct_length_t StructLength()
    {
        return sizeof(domain_id_t) + sizeof(error_code_t) + sizeof(support_data_t);
    }
    /// @brief Get union type
    /// @return Union type
    static constexpr union_type_t UnionType() { return 0x01; }
    /// @brief Get union length
    /// @return Union length
    static constexpr union_length_t UnionLength()
    {
        return sizeof(union_type_t) + sizeof(struct_length_t) + StructLength();
    }
};
}  // namespace serialize
}  // namespace isoft
#endif
