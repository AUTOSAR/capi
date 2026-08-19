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
/// @brief
/// @details
/// @date       2022-08-17
/// @author     mazelin
/// @version    1.2.0
///
/// ================================================================

#pragma once

#include "serialize_buffer.h"

namespace isoft {
namespace serialize {
template < typename TBuffer, typename TValue, typename = void >
class Stream;
template < typename TBuffer, typename TValue, typename... TValues >
typename TBuffer::result_t Serialize(TBuffer& buffer, const TValue& value, TValues&&... values)
{
    typename TBuffer::result_t ret = 0, retn = 0;
    retn = Stream< TBuffer, TValue >::Write(buffer, value);
    if (retn < 0)
        return retn;
    ret += retn;
    retn = Serialize(buffer, std::forward< TValues >(values)...);
    if (retn < 0)
        return retn;
    ret += retn;
    return ret;
}
template < typename TBuffer >
typename TBuffer::result_t Serialize(TBuffer& /*buffer*/)
{
    return 0;
}
template < typename TBuffer, typename... TValues >
typename TBuffer::result_t Deserialize(const TBuffer& buffer, TValues&&... values)
{
    auto point = buffer.First();
    return DeserializePart(buffer, point, std::forward< TValues >(values)...);
}
template < typename TBuffer, typename TValue, typename... TValues >
typename TBuffer::result_t DeserializePart(const TBuffer& buffer,
                                           typename TBuffer::point_t& point,
                                           TValue& value,
                                           TValues&&... values)
{
    typename TBuffer::result_t ret = 0, retn = 0;
    retn = Stream< TBuffer, TValue >::Read(buffer, point, value);
    if (retn < 0)
        return retn;
    ret += retn;
    retn = DeserializePart(buffer, point, std::forward< TValues >(values)...);
    if (retn < 0)
        return retn;
    ret += retn;
    return ret;
}
template < typename TBuffer >
typename TBuffer::result_t DeserializePart(const TBuffer& /*buffer*/, typename TBuffer::point_t& /*point*/)
{
    return 0;
}

template < typename TBuffer, typename TValue >
struct StreamBase
{
    using buffer_t = TBuffer;
    using value_t  = TValue;
    using result_t = typename buffer_t::result_t;
    using point_t  = typename buffer_t::point_t;
};

template < typename TBuffer >
class Stream< TBuffer, StringBorrow > : public StreamBase< TBuffer, StringBorrow >
{
public:
    using base_t   = StreamBase< TBuffer, StringBorrow >;
    using buffer_t = typename base_t::buffer_t;
    using value_t  = typename base_t::value_t;
    using result_t = typename base_t::result_t;
    using point_t  = typename base_t::point_t;
    static result_t Write(buffer_t& buffer, const value_t& value) { return buffer.Write(value); }
    static result_t Read(const buffer_t& buffer, point_t& point, value_t& value) { return buffer.Read(point, value); }
};

template < typename TBuffer, typename TValue >
class Stream< TBuffer, TValue, std::enable_if_t< IsBase< TValue >::value > > : public StreamBase< TBuffer, TValue >
{
public:
    using base_t   = StreamBase< TBuffer, TValue >;
    using buffer_t = typename base_t::buffer_t;
    using value_t  = typename base_t::value_t;
    using result_t = typename base_t::result_t;
    using point_t  = typename base_t::point_t;
    static result_t Write(buffer_t& buffer, const value_t& value) { return buffer.Write(value); }
    static result_t Read(const buffer_t& buffer, point_t& point, value_t& value) { return buffer.Read(point, value); }
};

template < typename TBuffer, typename TValue >
class Stream< TBuffer, TValue, std::enable_if_t< IsString< TValue >::value > > : public StreamBase< TBuffer, TValue >
{
public:
    using base_t         = StreamBase< TBuffer, TValue >;
    using buffer_t       = typename base_t::buffer_t;
    using value_t        = typename base_t::value_t;
    using result_t       = typename base_t::result_t;
    using point_t        = typename base_t::point_t;
    using length_field_t = uint32_t;
    static result_t Write(buffer_t& buffer, const value_t& value)
    {
        result_t ret = 0, retn = 0;
        length_field_t length = value.size() + UTF8Size();
        retn                  = Serialize(buffer, length);
        if (retn < 0)
            return retn;
        ret += retn;
        retn = Serialize(buffer, UTF8Head());
        if (retn < 0)
            return retn;
        ret += retn;
        if (!value.empty()) {
            StringBorrow cstring(value.data(), value.size());
            retn = Serialize(buffer, cstring);
            if (retn < 0)
                return retn;
            ret += retn;
        }
        retn = Serialize(buffer, UTF8Tail());
        if (retn < 0)
            return retn;
        ret += retn;
        return ret;
    }
    static result_t Read(const buffer_t& buffer, point_t& point, value_t& value)
    {
        result_t ret = 0, retn = 0;
        length_field_t length = 0;
        retn                  = DeserializePart(buffer, point, length);
        if (retn < 0)
            return retn;
        if (length < UTF8Size())
            return -1;
        ret += retn;
        std::decay_t< decltype(UTF8Head()) > bom{};
        retn = DeserializePart(buffer, point, bom);
        if (retn < 0)
            return retn;
        if (UTF8Head() != bom)
            return -1;
        ret += retn;
        value.resize(length - UTF8Size());
        if (!value.empty()) {
            StringBorrow cstring(value.data(), value.size());
            retn = DeserializePart(buffer, point, cstring);
            if (retn < 0)
                return retn;
            ret += retn;
        }
        std::decay_t< decltype(UTF8Tail()) > end{};
        retn = DeserializePart(buffer, point, end);
        if (retn < 0)
            return retn;
        if (UTF8Tail() != end)
            return -1;
        ret += retn;
        return ret;
    }

private:
    static constexpr ara::core::Array< uint8_t, 3 > UTF8Head() { return {0xEF, 0xBB, 0xBF}; }
    static constexpr ara::core::Array< uint8_t, 1 > UTF8Tail() { return {0x00}; }
    static constexpr size_t UTF8Size() { return UTF8Head().size() + UTF8Tail().size(); }
};

template < typename TBuffer, typename TValue >
class Stream< TBuffer, TValue, std::enable_if_t< IsEnumerable< TValue >::value > >
    : public StreamBase< TBuffer, TValue >
{
public:
    using base_t   = StreamBase< TBuffer, TValue >;
    using buffer_t = typename base_t::buffer_t;
    using value_t  = typename base_t::value_t;
    using result_t = typename base_t::result_t;
    using point_t  = typename base_t::point_t;
    static result_t Write(buffer_t& buffer, const value_t& value)
    {
        Writer writer{buffer};
        const_cast< value_t& >(value).enumerate(writer);
        return writer.ret;
    }
    static result_t Read(const buffer_t& buffer, point_t& point, value_t& value)
    {
        Reader reader{buffer, point};
        value.enumerate(reader);
        return reader.ret;
    }

private:
    struct Writer
    {
        buffer_t& buffer;
        result_t ret = 0;
        template < typename T >
        void operator()(const T& value)
        {
            if (ret < 0)
                return;
            auto retn = Serialize(buffer, value);
            ret       = retn < 0 ? retn : ret + retn;
        }
    };
    struct Reader
    {
        const buffer_t& buffer;
        point_t& point;
        result_t ret = 0;
        template < typename T >
        void operator()(T& value)
        {
            if (ret < 0)
                return;
            auto retn = DeserializePart(buffer, point, value);
            ret       = retn < 0 ? retn : ret + retn;
        }
    };
};

/// @uptrace{SWS_CM_10218, E2E errors domain}
template < typename TBuffer, typename TValue >
class Stream< TBuffer, TValue, std::enable_if_t< HasLengthField< TValue >::value > >
    : public StreamBase< TBuffer, TValue >
{
public:
    using base_t         = StreamBase< TBuffer, TValue >;
    using buffer_t       = typename base_t::buffer_t;
    using value_t        = typename base_t::value_t;
    using result_t       = typename base_t::result_t;
    using point_t        = typename base_t::point_t;
    using length_field_t = typename value_t::LengthFieldType;
    static result_t Write(buffer_t& buffer, const value_t& value)
    {
        result_t ret = 0, retn = 0;
        auto point            = buffer.Last();
        length_field_t length = 0;
        retn                  = Serialize(buffer, length);
        if (retn < 0)
            return retn;
        ret += retn;
        Writer writer{buffer};
        const_cast< value_t& >(value).enumerate(writer);
        retn = writer.ret;
        if (retn < 0)
            return retn;
        ret += retn;
        length += retn;
        retn = buffer.Update(point, length);
        if (retn < 0)
            return retn;
        return ret;
    }
    static result_t Read(const buffer_t& buffer, point_t& point, value_t& value)
    {
        result_t ret = 0, retn = 0;
        length_field_t length = 0;
        retn                  = DeserializePart(buffer, point, length);
        if (retn < 0)
            return retn;
        ret += retn;
        Reader reader{buffer, point, length};
        value.enumerate(reader);
        if (reader.length != 0)
            return -1;
        retn = reader.ret;
        if (retn < 0)
            return retn;
        ret += retn;
        return ret;
    }

private:
    struct Writer
    {
        buffer_t& buffer;
        result_t ret = 0;
        template < typename T >
        void operator()(const T& value)
        {
            if (ret < 0)
                return;
            auto retn = Serialize(buffer, value);
            if (retn < 0) {
                ret = retn;
                return;
            }
            ret += retn;
        }
    };
    struct Reader
    {
        const buffer_t& buffer;
        point_t& point;
        length_field_t length = 0;
        result_t ret          = 0;
        template < typename T >
        void operator()(T& value)
        {
            if (length == 0)
                return;
            if (ret < 0)
                return;
            auto retn = DeserializePart(buffer, point, value);
            if (retn < 0) {
                ret = retn;
                return;
            }
            if (retn > static_cast< result_t >(length)) {
                ret = -1;
                return;
            }
            ret += retn;
            length -= retn;
        }
    };
};

template < typename TBuffer, typename TValue >
class Stream< TBuffer, TValue, std::enable_if_t< IsTLV< TValue >::value > > : public StreamBase< TBuffer, TValue >
{
public:
    using base_t             = StreamBase< TBuffer, TValue >;
    using buffer_t           = typename base_t::buffer_t;
    using value_t            = typename base_t::value_t;
    using result_t           = typename base_t::result_t;
    using point_t            = typename base_t::point_t;
    using length_field_t     = typename value_t::LengthFieldType;
    using tlv_length_field_t = typename value_t::TLVLengthFieldType;
    using tlv_tag_field_t    = uint16_t;
    static result_t Write(buffer_t& buffer, const value_t& value)
    {
        result_t ret = 0, retn = 0;
        auto point            = buffer.Last();
        length_field_t length = 0;
        retn                  = Serialize(buffer, length);
        if (retn < 0)
            return retn;
        ret += retn;
        Writer writer{buffer};
        const_cast< value_t& >(value).enumerate(writer);
        retn = writer.ret;
        if (retn < 0)
            return retn;
        ret += retn;
        length += retn;
        retn = buffer.Update(point, length);
        if (retn < 0)
            return retn;
        return ret;
    }
    static result_t Read(const buffer_t& buffer, point_t& point, value_t& value)
    {
        result_t ret = 0, retn = 0;
        length_field_t length = 0;
        retn                  = DeserializePart(buffer, point, length);
        if (retn < 0)
            return retn;
        ret += retn;
        auto fields = value.required_fields();
        while (length != 0) {
            tlv_tag_field_t tag = 0;
            retn                = DeserializePart(buffer, point, tag);
            if (retn < 0)
                return retn;
            if (retn > length)
                return -1;
            ret += retn;
            length -= retn;
            tlv_tag_field_t dataId       = 0;
            tlv_length_field_t tlvLength = 0;
            Tag2Info(tag, dataId, tlvLength);
            if (tlvLength == 0) {
                retn = DeserializePart(buffer, point, tlvLength);
                if (retn < 0)
                    return retn;
                if (retn > length)
                    return -1;
                ret += retn;
                length -= retn;
            }
            Reader reader{buffer, point, tlvLength};
            auto recognized = value.dispatch(dataId, reader);
            if (reader.length != 0)
                return -1;
            retn = reader.ret;
            if (retn < 0)
                return retn;
            ret += retn;
            length -= tlvLength;
            if (recognized)
                fields.erase(dataId);
        }
        if (!fields.empty())
            return -2;
        return ret;
    }

private:
    struct Writer
    {
        buffer_t& buffer;
        result_t ret = 0;
        template < typename T >
        void operator()(const T& value, tlv_tag_field_t dataId, std::enable_if_t< IsBase< T >::value >* = nullptr)
        {
            if (ret < 0)
                return;
            result_t retn       = 0;
            tlv_tag_field_t tag = 0;
            Info2Tag< T >(tag, dataId);
            retn = Serialize(buffer, tag);
            if (retn < 0) {
                ret = retn;
                return;
            }
            ret += retn;
            retn = Serialize(buffer, value);
            if (retn < 0) {
                ret = retn;
                return;
            }
            ret += retn;
        }
        template < typename T >
        void operator()(const T& value, tlv_tag_field_t dataId, std::enable_if_t< !IsBase< T >::value >* = nullptr)
        {
            if (ret < 0)
                return;
            result_t retn       = 0;
            tlv_tag_field_t tag = 0;
            Info2Tag< T >(tag, dataId);
            retn = Serialize(buffer, tag);
            if (retn < 0) {
                ret = retn;
                return;
            }
            ret += retn;
            auto point                = buffer.Last();
            tlv_length_field_t length = 0;
            retn                      = Serialize(buffer, length);
            if (retn < 0) {
                ret = retn;
                return;
            }
            ret += retn;
            retn = Serialize(buffer, value);
            if (retn < 0) {
                ret = retn;
                return;
            }
            ret += retn;
            length += retn;
            retn = buffer.Update(point, length);
            if (retn < 0) {
                ret = retn;
                return;
            }
        }
        template < typename T >
        void operator()(const ara::core::Optional< T >& value, tlv_tag_field_t dataId)
        {
            if (ret < 0)
                return;
            if (value)
                operator()(*value, dataId);
        }
    };
    struct Reader
    {
        const buffer_t& buffer;
        point_t& point;
        length_field_t length = 0;
        result_t ret          = 0;
        template < typename T >
        void operator()(T& value)
        {
            if (length == 0)
                return;
            if (ret < 0)
                return;
            auto retn = DeserializePart(buffer, point, value);
            if (retn < 0) {
                ret = retn;
                return;
            }
            if (retn > length) {
                ret = -1;
                return;
            }
            ret += retn;
            length -= retn;
        }
        template < typename T >
        void operator()(ara::core::Optional< T >& value)
        {
            if (length == 0)
                return;
            if (ret < 0)
                return;
            value.emplace();
            operator()(*value);
        }
    };
    enum class WireTypes : uint16_t
    {
        Base8Bit             = 0U,
        Base16Bit            = 1U,
        Base32Bit            = 2U,
        Base64Bit            = 3U,
        ComplexWithLength    = 4U,
        ComplexForceLength8  = 5U,
        ComplexForceLength16 = 6U,
        ComplexForceLength32 = 7U
    };
    template < typename T >
    static void Info2Tag(tlv_tag_field_t& tag, tlv_tag_field_t dataId)
    {
        tag = (dataId & 0x0FFF) | (static_cast< tlv_tag_field_t >(Length2WireType< T >()) << 12);
    }
    static void Tag2Info(tlv_tag_field_t tag, tlv_tag_field_t& dataId, tlv_length_field_t& length)
    {
        dataId = tag & 0x0FFF;
        length = WireType2Length(static_cast< WireTypes >((tag >> 12) & 0x0007));
    }
    template < typename T >
    static constexpr WireTypes Length2WireType(std::enable_if_t< IsBase< T >::value && sizeof(T) == 1 >* = nullptr)
    {
        return WireTypes::Base8Bit;
    }
    template < typename T >
    static constexpr WireTypes Length2WireType(std::enable_if_t< IsBase< T >::value && sizeof(T) == 2 >* = nullptr)
    {
        return WireTypes::Base16Bit;
    }
    template < typename T >
    static constexpr WireTypes Length2WireType(std::enable_if_t< IsBase< T >::value && sizeof(T) == 4 >* = nullptr)
    {
        return WireTypes::Base32Bit;
    }
    template < typename T >
    static constexpr WireTypes Length2WireType(std::enable_if_t< IsBase< T >::value && sizeof(T) == 8 >* = nullptr)
    {
        return WireTypes::Base64Bit;
    }
    template < typename T >
    static constexpr WireTypes Length2WireType(std::enable_if_t< !IsBase< T >::value >* = nullptr)
    {
        return WireTypes::ComplexWithLength;
    }
    template < typename T >
    static constexpr WireTypes Length2WireType(std::enable_if_t< ara::core::is_optional< T >::value >* = nullptr)
    {
        return Length2WireType< T::value_type >();
    }
    static tlv_length_field_t WireType2Length(WireTypes wire_type)
    {
        switch (wire_type) {
            case WireTypes::Base8Bit:
                return 1;
            case WireTypes::Base16Bit:
                return 2;
            case WireTypes::Base32Bit:
                return 4;
            case WireTypes::Base64Bit:
                return 8;
            default:
                break;
        }
        return 0;
    }
};

template < typename TBuffer, typename TValue >
class Stream< TBuffer,
              TValue,
              std::enable_if_t< IsSequenceContainer< TValue >::value || IsAssociativeContainer< TValue >::value > >
    : public StreamBase< TBuffer, TValue >
{
public:
    using base_t         = StreamBase< TBuffer, TValue >;
    using buffer_t       = typename base_t::buffer_t;
    using value_t        = typename base_t::value_t;
    using result_t       = typename base_t::result_t;
    using point_t        = typename base_t::point_t;
    using length_field_t = uint32_t;
    static result_t Write(buffer_t& buffer, const value_t& value)
    {
        result_t ret = 0, retn = 0;
        auto point            = buffer.Last();
        length_field_t length = 0;
        retn                  = Serialize(buffer, length);
        if (retn < 0)
            return retn;
        ret += retn;
        for (auto& it : value) {
            retn = Serialize(buffer, it);
            if (retn < 0)
                return retn;
            ret += retn;
            length += retn;
        }
        retn = buffer.Update(point, length);
        if (retn < 0)
            return retn;
        return ret;
    }
    template < typename T = value_t >
    static result_t Read(const buffer_t& buffer,
                         point_t& point,
                         value_t& value,
                         std::enable_if_t< IsSequenceContainer< T >::value >* = nullptr)
    {
        result_t ret = 0, retn = 0;
        length_field_t length = 0;
        retn                  = DeserializePart(buffer, point, length);
        if (retn < 0)
            return retn;
        ret += retn;
        while (length != 0) {
            value.emplace_back();
            retn = DeserializePart(buffer, point, value.back());
            if (retn < 0)
                return retn;
            if (retn > static_cast< result_t >(length))
                return -1;
            ret += retn;
            length -= retn;
        }
        return ret;
    }
    template < typename T = value_t >
    static result_t Read(const buffer_t& buffer,
                         point_t& point,
                         value_t& value,
                         std::enable_if_t< IsAssociativeContainer< T >::value >* = nullptr)
    {
        result_t ret = 0, retn = 0;
        length_field_t length = 0;
        retn                  = DeserializePart(buffer, point, length);
        if (retn < 0)
            return retn;
        ret += retn;
        while (length != 0) {
            typename value_t::value_type it;
            retn = DeserializePart(buffer, point, it);
            if (retn < 0)
                return retn;
            if (retn > static_cast< result_t >(length))
                return -1;
            ret += retn;
            length -= retn;
            value.insert(std::move(it));
        }
        return ret;
    }
};

template < typename TBuffer, typename T, size_t N >
class Stream< TBuffer, ara::core::Array< T, N > > : public StreamBase< TBuffer, ara::core::Array< T, N > >
{
public:
    using base_t   = StreamBase< TBuffer, ara::core::Array< T, N > >;
    using buffer_t = typename base_t::buffer_t;
    using value_t  = typename base_t::value_t;
    using result_t = typename base_t::result_t;
    using point_t  = typename base_t::point_t;
    static result_t Write(buffer_t& buffer, const value_t& value)
    {
        result_t ret = 0, retn = 0;
        for (decltype(N) n = 0; n < N; ++n) {
            retn = Serialize(buffer, value[n]);
            if (retn < 0)
                return retn;
            ret += retn;
        }
        return ret;
    }
    static result_t Read(const buffer_t& buffer, point_t& point, value_t& value)
    {
        result_t ret = 0, retn = 0;
        for (decltype(N) n = 0; n < N; ++n) {
            retn = DeserializePart(buffer, point, value[n]);
            if (retn < 0)
                return retn;
            ret += retn;
        }
        return ret;
    }
};

template < typename TBuffer, typename T1, typename T2 >
class Stream< TBuffer, std::pair< T1, T2 > > : public StreamBase< TBuffer, std::pair< T1, T2 > >
{
public:
    using base_t   = StreamBase< TBuffer, std::pair< T1, T2 > >;
    using buffer_t = typename base_t::buffer_t;
    using value_t  = typename base_t::value_t;
    using result_t = typename base_t::result_t;
    using point_t  = typename base_t::point_t;
    static result_t Write(buffer_t& buffer, const value_t& value)
    {
        result_t ret = 0, retn = 0;
        retn = Serialize(buffer, value.first);
        if (retn < 0)
            return retn;
        ret += retn;
        retn = Serialize(buffer, value.second);
        if (retn < 0)
            return retn;
        ret += retn;
        return ret;
    }
    static result_t Read(const buffer_t& buffer, point_t& point, value_t& value)
    {
        result_t ret = 0, retn = 0;
        auto& valueFirst = const_cast< std::decay_t< typename value_t::first_type >& >(value.first);
        retn             = DeserializePart(buffer, point, valueFirst);
        if (retn < 0)
            return retn;
        ret += retn;
        retn = DeserializePart(buffer, point, value.second);
        if (retn < 0)
            return retn;
        ret += retn;
        return ret;
    }
};
}  // namespace serialize
}  // namespace isoft
