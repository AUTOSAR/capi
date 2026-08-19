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
/// @file       serialize_buffer.h
/// @brief
/// @details
/// @date       2022-08-17
/// @author     mazelin
/// @version    1.2.0
///
/// ================================================================

#pragma once

#include <list>

#include "serialize_base.h"

namespace isoft {
namespace serialize {
template < typename TPayload, typename = void >
class Buffer;

template < typename TPayload >
struct BufferBase
{
    using payload_t = TPayload;
    using result_t  = int32_t;
    explicit BufferBase(payload_t& _payload) : payload{_payload} {}

protected:
    payload_t& payload;
};

inline bool IsLittleEndian()
{
    uint32_t _ = 0x01;
    return reinterpret_cast< uint8_t* >(&_)[0] == 1;
}

template <>
class Buffer< StringBorrow > : public BufferBase< StringBorrow >
{
public:
    using base_t    = BufferBase< StringBorrow >;
    using payload_t = typename base_t::payload_t;
    using result_t  = typename base_t::result_t;
    using point_t   = typename payload_t::size_t;
    using base_t::BufferBase;
    template < typename TValue >
    result_t Read(point_t& point, TValue& value, std::enable_if_t< IsBase< TValue >::value >* = nullptr) const
    {
        if (payload.size() - point < sizeof(value))
            return -1;
        auto data = reinterpret_cast< uint8_t* >(&value);
        if (IsLittleEndian()) {
            std::reverse_copy(payload.data() + point, payload.data() + point + sizeof(value), data);
        } else {
            std::copy(payload.data() + point, payload.data() + point + sizeof(value), data);
        }
        point += sizeof(value);
        return sizeof(value);
    }
    result_t Read(point_t& point, StringBorrow& value) const
    {
        if (payload.size() - point < value.size())
            return -1;
        std::copy(payload.data() + point, payload.data() + point + value.size(), value.data());
        point += value.size();
        return value.size();
    }
    point_t First() const { return 0; }
};

template <>
class Buffer< ara::core::Vector< uint8_t > > : public BufferBase< ara::core::Vector< uint8_t > >
{
public:
    using base_t    = BufferBase< ara::core::Vector< uint8_t > >;
    using payload_t = typename base_t::payload_t;
    using result_t  = typename base_t::result_t;
    using point_t   = typename payload_t::size_type;
    using base_t::BufferBase;
    template < typename TValue >
    result_t Write(const TValue& value, std::enable_if_t< IsBase< TValue >::value >* = nullptr)
    {
        auto point = payload.size();
        payload.resize(point + sizeof(value));
        auto data = reinterpret_cast< const uint8_t* >(&value);
        if (IsLittleEndian()) {
            std::reverse_copy(data, data + sizeof(value), &payload[point]);
        } else {
            std::copy(data, data + sizeof(value), &payload[point]);
        }
        return sizeof(value);
    }
    result_t Write(const StringBorrow& value)
    {
        auto point = payload.size();
        payload.resize(point + value.size());
        std::copy(value.data(), value.data() + value.size(), &payload[point]);
        return value.size();
    }
    template < typename TValue >
    result_t Read(point_t& point, TValue& value, std::enable_if_t< IsBase< TValue >::value >* = nullptr) const
    {
        if (payload.size() - point < sizeof(value))
            return -1;
        auto data = reinterpret_cast< uint8_t* >(&value);
        if (IsLittleEndian()) {
            std::reverse_copy(&payload[point], &payload[point + sizeof(value)], data);
        } else {
            std::copy(&payload[point], &payload[point + sizeof(value)], data);
        }
        point += sizeof(value);
        return sizeof(value);
    }
    result_t Read(point_t& point, StringBorrow& value) const
    {
        if (payload.size() - point < value.size())
            return -1;
        std::copy(&payload[point], &payload[point + value.size()], value.data());
        point += value.size();
        return value.size();
    }
    template < typename TValue >
    result_t Update(point_t& point, const TValue& value, std::enable_if_t< IsBase< TValue >::value >* = nullptr)
    {
        if (payload.size() - point < sizeof(value))
            return -1;
        auto data = reinterpret_cast< const uint8_t* >(&value);
        if (IsLittleEndian()) {
            std::reverse_copy(data, data + sizeof(value), &payload[point]);
        } else {
            std::copy(data, data + sizeof(value), &payload[point]);
        }
        point += sizeof(value);
        return sizeof(value);
    }
    result_t Update(point_t& point, const StringBorrow& value)
    {
        if (payload.size() - point < value.size())
            return -1;
        std::copy(value.data(), value.data() + value.size(), &payload[point]);
        point += value.size();
        return value.size();
    }
    point_t First() const { return 0; }
    point_t Last() const { return payload.size(); }
};

template <>
class Buffer< std::list< ara::core::Vector< uint8_t > > >
    : public BufferBase< std::list< ara::core::Vector< uint8_t > > >
{
public:
    using base_t    = BufferBase< std::list< ara::core::Vector< uint8_t > > >;
    using payload_t = typename base_t::payload_t;
    using result_t  = typename base_t::result_t;
    using point_t   = std::pair< typename payload_t::iterator, typename payload_t::value_type::size_type >;
    using base_t::BufferBase;
    template < typename TValue >
    result_t Write(const TValue& value, std::enable_if_t< IsBase< TValue >::value >* = nullptr)
    {
        payload.emplace_back(sizeof(value));
        auto data = reinterpret_cast< const uint8_t* >(&value);
        if (IsLittleEndian()) {
            std::reverse_copy(data, data + sizeof(value), &(payload.back()[0]));
        } else {
            std::copy(data, data + sizeof(value), &(payload.back()[0]));
        }
        return sizeof(value);
    }
    result_t Write(const StringBorrow& value)
    {
        if (value.size() == 0)
            return 0;
        payload.emplace_back(value.size());
        std::copy(value.data(), value.data() + value.size(), &(payload.back()[0]));
        return value.size();
    }
    template < typename TValue >
    result_t Read(point_t& point, TValue& value, std::enable_if_t< IsBase< TValue >::value >* = nullptr) const
    {
        auto& it  = point.first;
        auto& pos = point.second;
        if (it == payload.end()) {
            if (payload.empty())
                return -1;
            point = First();
        }
        auto data = reinterpret_cast< uint8_t* >(&value);
        auto size = sizeof(value);
        while (size != 0) {
            auto source = &(*it)[0] + pos;
            auto remain = (*it).size() - pos;
            if (remain >= size) {
                std::copy(source, source + size, data);
                pos += size;
                break;
            }
            std::copy(source, source + remain, data);
            pos = (*it).size(), size -= remain, data += remain;
            if (std::next(it) == payload.end())
                return -1;
            ++it, pos = 0;
        }
        if (IsLittleEndian()) {
            data = reinterpret_cast< uint8_t* >(&value);
            std::reverse(data, data + sizeof(value));
        }
        return sizeof(value);
    }
    result_t Read(point_t& point, StringBorrow& value) const
    {
        if (value.size() == 0)
            return 0;
        auto& it  = point.first;
        auto& pos = point.second;
        if (it == payload.end()) {
            if (payload.empty())
                return -1;
            point = First();
        }
        auto data = value.data();
        auto size = value.size();
        while (size != 0) {
            auto source = &(*it)[0] + pos;
            auto remain = (*it).size() - pos;
            if (remain >= size) {
                std::copy(source, source + size, data);
                pos += size;
                break;
            }
            std::copy(source, source + remain, data);
            pos = (*it).size(), size -= remain, data += remain;
            if (std::next(it) == payload.end())
                return -1;
            ++it, pos = 0;
        }
        return value.size();
    }
    template < typename TValue >
    result_t Update(point_t& point, const TValue& value, std::enable_if_t< IsBase< TValue >::value >* = nullptr)
    {
        auto& it  = point.first;
        auto& pos = point.second;
        if (it == payload.end()) {
            if (payload.empty())
                return -1;
            point = First();
        }
        auto value_ = value;
        auto data   = reinterpret_cast< uint8_t* >(&value_);
        auto size   = sizeof(value);
        if (IsLittleEndian()) {
            std::reverse(data, data + sizeof(value));
        }
        while (size != 0) {
            auto source = &(*it)[0] + pos;
            auto remain = (*it).size() - pos;
            if (remain >= size) {
                std::copy(data, data + size, source);
                pos += size;
                break;
            }
            std::copy(data, data + remain, source);
            pos = (*it).size(), size -= remain, data += remain;
            if (std::next(it) == payload.end())
                return -1;
            ++it, pos = 0;
        }
        return sizeof(value);
    }
    result_t Update(point_t& point, const StringBorrow& value)
    {
        if (value.size() == 0)
            return 0;
        auto& it  = point.first;
        auto& pos = point.second;
        if (it == payload.end()) {
            if (payload.empty())
                return -1;
            point = First();
        }
        auto data = value.data();
        auto size = value.size();
        while (size != 0) {
            auto source = &(*it)[0] + pos;
            auto remain = (*it).size() - pos;
            if (remain >= size) {
                std::copy(data, data + size, source);
                pos += size;
                break;
            }
            std::copy(data, data + remain, source);
            pos = (*it).size(), size -= remain, data += remain;
            if (std::next(it) == payload.end())
                return -1;
            ++it, pos = 0;
        }
        return value.size();
    }
    point_t First() const { return point_t{payload.begin(), 0}; }
    point_t Last() const
    {
        if (payload.empty())
            return First();
        auto it = std::prev(payload.end());
        return point_t{it, (*it).size()};
    }
};

class Payload
{
public:
    explicit Payload(size_t capacity = 256) { reserve(capacity); }
    Payload(const Payload& other) { copy(other); }
    Payload(Payload&& other) { move(std::move(other)); }
    ~Payload() { free(); }
    void reserve(size_t capacity)
    {
        if (capacity <= capacity_)
            return;
        auto data = data_;
        data_     = new uint8_t[capacity];
        std::copy(data, data + size_, data_);
        delete[] data;
        capacity_ = capacity;
    }
    void resize(size_t size)
    {
        reserve(size);
        size_ = size;
    }
    void copy(const Payload& other)
    {
        if (&other == this)
            return;
        resize(other.capacity_);
        std::copy(other.data_, other.data_ + other.size_, data_);
        size_ = other.size_;
    }
    void move(Payload&& other)
    {
        if (&other == this)
            return;
        std::swap(other.data_, data_);
        std::swap(other.size_, size_);
        std::swap(other.capacity_, capacity_);
    }
    void clear() { size_ = 0; }
    void free()
    {
        delete[] data_;
        data_     = nullptr;
        size_     = 0;
        capacity_ = 0;
    }
    uint8_t* data() const { return data_; }
    size_t size() const { return size_; }
    size_t capacity() const { return capacity_; }

private:
    uint8_t* data_   = nullptr;
    size_t size_     = 0;
    size_t capacity_ = 0;
};

template <>
class Buffer< Payload > : public BufferBase< Payload >
{
public:
    using base_t    = BufferBase< Payload >;
    using payload_t = typename base_t::payload_t;
    using result_t  = typename base_t::result_t;
    using point_t   = size_t;
    using base_t::BufferBase;
    template < typename TValue >
    result_t Write(const TValue& value, std::enable_if_t< IsBase< TValue >::value >* = nullptr)
    {
        payload.resize(payload.size() + sizeof(value));
        auto data = reinterpret_cast< const uint8_t* >(&value);
        if (IsLittleEndian()) {
            std::reverse_copy(data, data + sizeof(value), payload.data() + payload.size() - sizeof(value));
        } else {
            std::copy(data, data + sizeof(value), payload.data() + payload.size() - sizeof(value));
        }
        return sizeof(value);
    }
    result_t Write(const StringBorrow& value)
    {
        payload.resize(payload.size() + value.size());
        std::copy(value.data(), value.data() + value.size(), payload.data() + payload.size() - value.size());
        return value.size();
    }
    template < typename TValue >
    result_t Read(point_t& point, TValue& value, std::enable_if_t< IsBase< TValue >::value >* = nullptr) const
    {
        if (payload.size() - point < sizeof(value))
            return -1;
        auto data = reinterpret_cast< uint8_t* >(&value);
        if (IsLittleEndian()) {
            std::reverse_copy(payload.data() + point, payload.data() + point + sizeof(value), data);
        } else {
            std::copy(payload.data() + point, payload.data() + point + sizeof(value), data);
        }
        point += sizeof(value);
        return sizeof(value);
    }
    result_t Read(point_t& point, StringBorrow& value) const
    {
        if (payload.size() - point < value.size())
            return -1;
        std::copy(payload.data() + point, payload.data() + point + value.size(), value.data());
        point += value.size();
        return value.size();
    }
    template < typename TValue >
    result_t Update(point_t& point, const TValue& value, std::enable_if_t< IsBase< TValue >::value >* = nullptr)
    {
        if (payload.size() - point < sizeof(value))
            return -1;
        auto data = reinterpret_cast< const uint8_t* >(&value);
        if (IsLittleEndian()) {
            std::reverse_copy(data, data + sizeof(value), payload.data() + point);
        } else {
            std::copy(data, data + sizeof(value), payload.data() + point);
        }
        point += sizeof(value);
        return sizeof(value);
    }
    result_t Update(point_t& point, const StringBorrow& value)
    {
        if (payload.size() - point < value.size())
            return -1;
        std::copy(value.data(), value.data() + value.size(), payload.data() + point);
        point += value.size();
        return value.size();
    }
    point_t First() const { return 0; }
    point_t Last() const { return payload.size(); }
};
}  // namespace serialize
}  // namespace isoft
