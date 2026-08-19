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
/// @file       serialize_stream.h
/// @brief      Binding layer serialization payload stream header file
/// @details
/// @date       2024-11-13
/// @author     xuhua
/// @version    1.2.0
///
/// ================================================================

#ifndef __DIAG_SERIALIZE_STREAM_H
#define __DIAG_SERIALIZE_STREAM_H

#include <cassert>
#include <list>

#include "common_data_type.h"
#include "serialize_base.h"
/// @brief Namespace -- internal binding layer serialization
namespace isoft {
namespace serialize {
/// @brief Determine if system is little-endian
/// @return bool
inline bool IsLittleEndian()
{
    uint32_t _{1};
    return reinterpret_cast< uint8_t * >(&_)[0] == 1;
}

/// @brief Template type -- Payload stream writer -- Declaration
/// @tparam TProps Property information
/// @tparam TPayload Payload type
template < typename TProps, typename TPayload, typename = void >
class StreamWriter;
/// @brief Template type -- Payload stream reader -- Declaration
/// @tparam TProps Property information
/// @tparam TPayload Payload type
template < typename TProps, typename TPayload, typename = void >
class StreamReader;
/// @brief Template type -- Payload stream writer -- Specialization for null pointer type (used for calculating number of bytes written)
/// @tparam TProps Property information
template < typename TProps >
class StreamWriter< TProps, std::nullptr_t >
{
public:
    /// @brief Type alias -- Property information
    using props_t = TProps;  // NOLINT
    /// @brief Type alias -- Result type
    using result_t = int32_t;  // NOLINT
    /// @brief Type alias -- Bit position type
    using point_t = std::nullptr_t;  // NOLINT
    /// @brief Write
    /// @tparam TValue Value type
    /// @param[in] value Value
    /// @param[in] reverseOrder Whether to convert byte order
    /// @return Result -- >= 0: number of bytes written; < 0: error code
    template < typename TValue >
    inline result_t Write(TValue const &value, bool reverseOrder)
    {
        static_assert(IsScalar< TValue >::value, "invalid value");
        std::ignore = reverseOrder;
        return sizeof(value);
    }
    /// @brief Write
    /// @param[in] value Value
    /// @return Result -- >= 0: number of bytes written; < 0: error code
    inline result_t Write(StringBorrow const &value) { return static_cast< result_t >(value.size()); }
    /// @brief Write
    /// @tparam TValue Value type
    /// @param[in] value Value
    /// @param[in] reverseOrder Whether to convert byte order
    /// @param[in] point Bit position
    /// @return Result -- >= 0: number of bytes written; < 0: error code
    template < typename TValue >
    inline result_t Write(TValue const &value, bool reverseOrder, point_t const &point)
    {
        static_assert(IsScalar< TValue >::value, "invalid value");
        std::ignore = reverseOrder;
        std::ignore = point;
        return sizeof(value);
    }
    /// @brief Write
    /// @param[in] value Value
    /// @param[in] point Bit position
    /// @return Result -- >= 0: number of bytes written; < 0: error code
    inline result_t Write(StringBorrow const &value, point_t const &point)
    {
        std::ignore = point;
        return static_cast< result_t >(value.size());
    }
    /// @brief Get current bit position
    /// @return Current bit position
    inline point_t const &Point() const { return point_; }

private:
    point_t point_{nullptr};
};
/// @brief Template type -- Payload stream writer -- Specialization for raw string (pointer + length) type
/// @tparam TProps Property information
template < typename TProps >
class StreamWriter< TProps, StringBorrow >
{
public:
    /// @brief Type alias -- Property information
    using props_t = TProps;  // NOLINT
    /// @brief Type alias -- Payload type
    using payload_t = StringBorrow;  // NOLINT
    /// @brief Type alias -- Result type
    using result_t = int32_t;  // NOLINT
    /// @brief Type alias -- Bit position type
    using point_t = typename payload_t::size_t;  // NOLINT
    /// @brief Constructor
    /// @param[in] payload Payload
    /// @param[in] capacity Payload capacity -- pre-allocation
    explicit StreamWriter(payload_t &payload, size_t capacity) : payload_{payload}
    {
        assert(capacity <= payload_.size());
        std::ignore = capacity;
    }
    /// @brief Write
    /// @tparam TValue Value type
    /// @param[in] value Value
    /// @param[in] reverseOrder Whether to convert byte order
    /// @return Result -- >= 0: number of bytes written; < 0: error code
    template < typename TValue >
    inline result_t Write(TValue const &value, bool reverseOrder)
    {
        static_assert(IsScalar< TValue >::value, "invalid value");
        constexpr size_t kSize{sizeof(value)};
        if (point_ + kSize > payload_.size()) {
            return ErrorCode::kInvalidWrite;  // Write exceeds payload capacity limit (value)
        }
        auto const *data1{reinterpret_cast< uint8_t const * >(&value)};
        auto *data2{payload_.data() + point_};
        if (reverseOrder) {
            std::ignore = std::reverse_copy(data1, data1 + kSize, data2);
        } else {
            std::ignore = std::copy(data1, data1 + kSize, data2);
        }
        point_ += kSize;
        return static_cast< result_t >(kSize);
    }
    /// @brief Write
    /// @param[in] value Value
    /// @return Result -- >= 0: number of bytes written; < 0: error code
    inline result_t Write(StringBorrow const &value)
    {
        const size_t kSize{value.size()};
        if (point_ + kSize > payload_.size()) {
            return ErrorCode::kInvalidWrite;  // Write exceeds payload capacity limit (string)
        }
        auto const *data1{value.data()};
        auto *data2{payload_.data() + point_};
        std::ignore = std::copy(data1, data1 + kSize, data2);
        point_ += kSize;
        return static_cast< result_t >(kSize);
    }
    /// @brief Write
    /// @tparam TValue Value type
    /// @param[in] value Value
    /// @param[in] reverseOrder Whether to convert byte order
    /// @param[in] point Bit position
    /// @return Result -- >= 0: number of bytes written; < 0: error code
    template < typename TValue >
    inline result_t Write(TValue const &value, bool reverseOrder, point_t const &point)
    {
        static_assert(IsScalar< TValue >::value, "invalid value");
        constexpr size_t kSize{sizeof(value)};
        if (point + kSize > payload_.size()) {
            return ErrorCode::kInvalidWrite;  // Write exceeds payload capacity limit (value)
        }
        auto const *data1{reinterpret_cast< uint8_t const * >(&value)};
        auto *data2{payload_.data() + point};
        if (reverseOrder) {
            std::ignore = std::reverse_copy(data1, data1 + kSize, data2);
        } else {
            std::ignore = std::copy(data1, data1 + kSize, data2);
        }
        return static_cast< result_t >(kSize);
    }
    /// @brief Write
    /// @param[in] value Value
    /// @param[in] point Bit position
    /// @return Result -- >= 0: number of bytes written; < 0: error code
    inline result_t Write(StringBorrow const &value, point_t const &point)
    {
        const size_t kSize{value.size()};
        if (point + kSize > payload_.size()) {
            return ErrorCode::kInvalidWrite;  // Write exceeds payload capacity limit (string)
        }
        auto const *data1{value.data()};
        auto *data2{payload_.data() + point};
        std::ignore = std::copy(data1, data1 + kSize, data2);
        return static_cast< result_t >(kSize);
    }
    /// @brief Get current bit position
    /// @return Current bit position
    inline point_t const &Point() const { return point_; }

private:
    /// @brief Payload reference
    payload_t &payload_;
    /// @brief Current bit position
    point_t point_{};
};
/// @brief Template type -- Payload stream reader -- Specialization for raw string (pointer + length) type
/// @tparam TProps Property information
template < typename TProps >
class StreamReader< TProps, StringBorrow >
{
public:
    /// @brief Type alias -- Property information
    using props_t = TProps;  // NOLINT
    /// @brief Type alias -- Payload type
    using payload_t = StringBorrow const;  // NOLINT
    /// @brief Type alias -- Result type
    using result_t = int32_t;  // NOLINT
    /// @brief Type alias -- Bit position type
    using point_t = typename payload_t::size_t;  // NOLINT
    /// @brief Constructor
    /// @param[in] payload Payload
    explicit StreamReader(payload_t &payload) : payload_{payload} {}
    /// @brief Read
    /// @tparam TValue Value type
    /// @param[in] value Value
    /// @param[in] reverseOrder Whether to convert byte order
    /// @return Result -- >= 0: number of bytes read; < 0: error code
    template < typename TValue >
    inline result_t Read(TValue &value, bool reverseOrder)
    {
        static_assert(IsScalar< TValue >::value, "invalid value");
        constexpr size_t kSize{sizeof(value)};
        if (point_ + kSize > payload_.size()) {
            return ErrorCode::kInvalidRead;  // Read exceeds payload capacity limit (value)
        }
        auto const *data1{payload_.data() + point_};
        auto *data2{reinterpret_cast< uint8_t * >(&value)};
        if (reverseOrder) {
            std::ignore = std::reverse_copy(data1, data1 + kSize, data2);
        } else {
            std::ignore = std::copy(data1, data1 + kSize, data2);
        }
        point_ += kSize;
        return static_cast< result_t >(kSize);
    }
    /// @brief Read
    /// @param[in] value Value
    /// @return Result -- >= 0: number of bytes read; < 0: error code
    inline result_t Read(StringBorrow const &value)
    {
        const size_t kSize{value.size()};
        if (point_ + kSize > payload_.size()) {
            return ErrorCode::kInvalidRead;  // Read exceeds payload capacity limit (string)
        }
        auto const *data1{payload_.data() + point_};
        auto *data2{value.data()};
        std::ignore = std::copy(data1, data1 + kSize, data2);
        point_ += kSize;
        return static_cast< result_t >(kSize);
    }
    /// @brief Read
    /// @tparam TValue Value type
    /// @param[in] value Value
    /// @param[in] reverseOrder Whether to convert byte order
    /// @param[in] point Bit position
    /// @return Result -- >= 0: number of bytes read; < 0: error code
    template < typename TValue >
    inline result_t Read(TValue &value, bool reverseOrder, point_t const &point) const
    {
        static_assert(IsScalar< TValue >::value, "invalid value");
        constexpr size_t kSize{sizeof(value)};
        if (point + kSize > payload_.size()) {
            return ErrorCode::kInvalidRead;  // Read exceeds payload capacity limit (value)
        }
        auto const *data1{payload_.data() + point};
        auto *data2{reinterpret_cast< uint8_t * >(&value)};
        if (reverseOrder) {
            std::ignore = std::reverse_copy(data1, data1 + kSize, data2);
        } else {
            std::ignore = std::copy(data1, data1 + kSize, data2);
        }
        return static_cast< result_t >(kSize);
    }
    /// @brief Read
    /// @param[in] value Value
    /// @param[in] point Bit position
    /// @return Result -- >= 0: number of bytes read; < 0: error code
    inline result_t Read(StringBorrow const &value, point_t const &point) const
    {
        const size_t kSize{value.size()};
        if (point + kSize > payload_.size()) {
            return ErrorCode::kInvalidRead;  // Read exceeds payload capacity limit (string)
        }
        auto const *data1{payload_.data() + point};
        auto *data2{value.data()};
        std::ignore = std::copy(data1, data1 + kSize, data2);
        return static_cast< result_t >(kSize);
    }
    /// @brief Get current bit position
    /// @return Current bit position
    inline point_t const &Point() const { return point_; }

private:
    /// @brief Payload reference
    payload_t &payload_;
    /// @brief Current bit position
    point_t point_{};
};
/// @brief Template type -- Payload stream writer -- Specialization for variable-length array type
/// @tparam TProps Property information
template < typename TProps >
class StreamWriter< TProps, ara::core::Vector< uint8_t > >
{
public:
    /// @brief Type alias -- Property information
    using props_t = TProps;  // NOLINT
    /// @brief Type alias -- Payload type
    using payload_t = ara::core::Vector< uint8_t >;  // NOLINT
    /// @brief Type alias -- Result type
    using result_t = int32_t;  // NOLINT
    /// @brief Type alias -- Bit position type
    using point_t = typename payload_t::size_type;  // NOLINT
    /// @brief Constructor
    /// @param[in] payload Payload
    /// @param[in] capacity Payload capacity -- pre-allocation
    explicit StreamWriter(payload_t &payload, size_t capacity) : payload_{payload} { payload_.reserve(capacity); }
    /// @brief Write
    /// @tparam TValue Value type
    /// @param[in] value Value
    /// @param[in] reverseOrder Whether to convert byte order
    /// @return Result -- >= 0: number of bytes written; < 0: error code
    template < typename TValue >
    inline result_t Write(TValue const &value, bool reverseOrder)
    {
        static_assert(IsScalar< TValue >::value, "invalid value");
        constexpr size_t kSize{sizeof(value)};
        if (point_ + kSize > payload_.size()) {
            payload_.resize(point_ + kSize);
        }
        auto const *data1{reinterpret_cast< uint8_t const * >(&value)};
        auto *data2{payload_.data() + point_};
        if (reverseOrder) {
            std::ignore = std::reverse_copy(data1, data1 + kSize, data2);
        } else {
            std::ignore = std::copy(data1, data1 + kSize, data2);
        }
        point_ += kSize;
        return static_cast< result_t >(kSize);
    }
    /// @brief Write
    /// @param[in] value Value
    /// @return Result -- >= 0: number of bytes written; < 0: error code
    inline result_t Write(StringBorrow const &value)
    {
        const size_t kSize{value.size()};
        if (point_ + kSize > payload_.size()) {
            payload_.resize(point_ + kSize);
        }
        auto const *data1{value.data()};
        auto *data2{payload_.data() + point_};
        std::ignore = std::copy(data1, data1 + kSize, data2);
        point_ += kSize;
        return static_cast< result_t >(kSize);
    }
    /// @brief Write
    /// @tparam TValue Value type
    /// @param[in] value Value
    /// @param[in] reverseOrder Whether to convert byte order
    /// @param[in] point Bit position
    /// @return Result -- >= 0: number of bytes written; < 0: error code
    template < typename TValue >
    inline result_t Write(TValue const &value, bool reverseOrder, point_t const &point)
    {
        static_assert(IsScalar< TValue >::value, "invalid value");
        constexpr size_t kSize{sizeof(value)};
        if (point + kSize > payload_.size()) {
            return ErrorCode::kInvalidWrite;  // Write exceeds payload capacity limit (value)
        }
        auto const *data1{reinterpret_cast< uint8_t const * >(&value)};
        auto *data2{payload_.data() + point};
        if (reverseOrder) {
            std::ignore = std::reverse_copy(data1, data1 + kSize, data2);
        } else {
            std::ignore = std::copy(data1, data1 + kSize, data2);
        }
        return static_cast< result_t >(kSize);
    }
    /// @brief Write
    /// @param[in] value Value
    /// @param[in] point Bit position
    /// @return Result -- >= 0: number of bytes written; < 0: error code
    inline result_t Write(StringBorrow const &value, point_t const &point)
    {
        const size_t kSize{value.size()};
        if (point + kSize > payload_.size()) {
            return ErrorCode::kInvalidWrite;  // Write exceeds payload capacity limit (string)
        }
        auto const *data1{value.data()};
        auto *data2{payload_.data() + point};
        std::ignore = std::copy(data1, data1 + kSize, data2);
        return static_cast< result_t >(kSize);
    }
    /// @brief Get current bit position
    /// @return Current bit position
    inline point_t const &Point() const { return point_; }

private:
    /// @brief Payload reference
    payload_t &payload_;
    /// @brief Current bit position
    point_t point_{};
};
/// @brief Template type -- Payload stream reader -- Specialization for variable-length array type
/// @tparam TProps Property information
template < typename TProps >
class StreamReader< TProps, ara::core::Vector< uint8_t > >
{
public:
    /// @brief Type alias -- Property information
    using props_t = TProps;  // NOLINT
    /// @brief Type alias -- Payload type
    using payload_t = ara::core::Vector< uint8_t > const;  // NOLINT
    /// @brief Type alias -- Result type
    using result_t = int32_t;  // NOLINT
    /// @brief Type alias -- Bit position type
    using point_t = typename payload_t::size_type;  // NOLINT
    /// @brief Constructor
    /// @param[in] payload Payload
    explicit StreamReader(payload_t &payload) : payload_{payload} {}
    /// @brief Read
    /// @tparam TValue Value type
    /// @param[in] value Value
    /// @param[in] reverseOrder Whether to convert byte order
    /// @return Result -- >= 0: number of bytes read; < 0: error code
    template < typename TValue >
    inline result_t Read(TValue &value, bool reverseOrder)
    {
        static_assert(IsScalar< TValue >::value, "invalid value");
        constexpr size_t kSize{sizeof(value)};
        if (point_ + kSize > payload_.size()) {
            return ErrorCode::kInvalidRead;  // Read exceeds payload capacity limit (value)
        }
        auto const *data1{payload_.data() + point_};
        auto *data2{reinterpret_cast< uint8_t * >(&value)};
        if (reverseOrder) {
            std::ignore = std::reverse_copy(data1, data1 + kSize, data2);
        } else {
            std::ignore = std::copy(data1, data1 + kSize, data2);
        }
        point_ += kSize;
        return static_cast< result_t >(kSize);
    }
    /// @brief Read
    /// @param[in] value Value
    /// @return Result -- >= 0: number of bytes read; < 0: error code
    inline result_t Read(StringBorrow const &value)
    {
        const size_t kSize{value.size()};
        if (point_ + kSize > payload_.size()) {
            return ErrorCode::kInvalidRead;  // Read exceeds payload capacity limit (string)
        }
        auto const *data1{payload_.data() + point_};
        auto *data2{value.data()};
        std::ignore = std::copy(data1, data1 + kSize, data2);
        point_ += kSize;
        return static_cast< result_t >(kSize);
    }
    /// @brief Read
    /// @tparam TValue Value type
    /// @param[in] value Value
    /// @param[in] reverseOrder Whether to convert byte order
    /// @param[in] point Bit position
    /// @return Result -- >= 0: number of bytes read; < 0: error code
    template < typename TValue >
    inline result_t Read(TValue &value, bool reverseOrder, point_t const &point) const
    {
        static_assert(IsScalar< TValue >::value, "invalid value");
        constexpr size_t kSize{sizeof(value)};
        if (point + kSize > payload_.size()) {
            return ErrorCode::kInvalidRead;  // Read exceeds payload capacity limit (value)
        }
        auto const *data1{payload_.data() + point};
        auto *data2{reinterpret_cast< uint8_t * >(&value)};
        if (reverseOrder) {
            std::ignore = std::reverse_copy(data1, data1 + kSize, data2);
        } else {
            std::ignore = std::copy(data1, data1 + kSize, data2);
        }
        return static_cast< result_t >(kSize);
    }
    /// @brief Read
    /// @param[in] value Value
    /// @param[in] point Bit position
    /// @return Result -- >= 0: number of bytes read; < 0: error code
    inline result_t Read(StringBorrow const &value, point_t const &point) const
    {
        const size_t kSize{value.size()};
        if (point + kSize > payload_.size()) {
            return ErrorCode::kInvalidRead;  // Read exceeds payload capacity limit (string)
        }
        auto const *data1{payload_.data() + point};
        auto *data2{value.data()};
        std::ignore = std::copy(data1, data1 + kSize, data2);
        return static_cast< result_t >(kSize);
    }
    /// @brief Get current bit position
    /// @return Current bit position
    inline point_t const &Point() const { return point_; }

private:
    /// @brief Payload reference
    payload_t &payload_;
    /// @brief Current bit position
    point_t point_{};
};
/// @brief Template type -- Payload stream writer -- Specialization for variable-length array list type
/// @tparam TProps Property information
template < typename TProps >
class StreamWriter< TProps, std::list< ara::core::Vector< uint8_t > > >
{
public:
    /// @brief Type alias -- Property information
    using props_t = TProps;  // NOLINT
    /// @brief Type alias -- Payload type
    using payload_t = std::list< ara::core::Vector< uint8_t > >;  // NOLINT
    /// @brief Type alias -- Result type
    using result_t = int32_t;  // NOLINT
    /// @brief Type alias -- Bit position type
    using point_t = std::pair< typename payload_t::iterator,  // NOLINT
                               typename payload_t::value_type::size_type >;
    /// @brief Constructor
    /// @param[in] payload Payload
    /// @param[in] capacity Payload capacity -- pre-allocation
    explicit StreamWriter(payload_t &payload, size_t capacity) : payload_{payload} { std::ignore = capacity; }
    /// @brief Write
    /// @tparam TValue Value type
    /// @param[in] value Value
    /// @param[in] reverseOrder Whether to convert byte order
    /// @return Result -- >= 0: number of bytes written; < 0: error code
    template < typename TValue >
    inline result_t Write(TValue const &value, bool reverseOrder)
    {
        static_assert(IsScalar< TValue >::value, "invalid value");
        constexpr size_t kSize{sizeof(value)};
        payload_.emplace_back(kSize);
        auto const *data1{reinterpret_cast< uint8_t const * >(&value)};
        auto *data2{payload_.back().data()};
        if (reverseOrder) {
            std::ignore = std::reverse_copy(data1, data1 + kSize, data2);
        } else {
            std::ignore = std::copy(data1, data1 + kSize, data2);
        }
        point_ = {std::prev(payload_.end()), kSize};
        return static_cast< result_t >(kSize);
    }
    /// @brief Write
    /// @param[in] value Value
    /// @return Result -- >= 0: number of bytes written; < 0: error code
    inline result_t Write(StringBorrow const &value)
    {
        const size_t kSize{value.size()};
        if (kSize == 0) {
            return static_cast< result_t >(kSize);
        }
        payload_.emplace_back(kSize);
        auto const *data1{value.data()};
        auto *data2{payload_.back().data()};
        std::ignore = std::copy(data1, data1 + kSize, data2);
        point_      = {std::prev(payload_.end()), kSize};
        return static_cast< result_t >(kSize);
    }
    /// @brief Write
    /// @tparam TValue Value type
    /// @param[in] value Value
    /// @param[in] reverseOrder Whether to convert byte order
    /// @param[in] point Bit position
    /// @return Result -- >= 0: number of bytes written; < 0: error code
    template < typename TValue >
    inline result_t Write(TValue const &value, bool reverseOrder, point_t const &point)
    {
        static_assert(IsScalar< TValue >::value, "invalid value");
        constexpr size_t kSize{sizeof(value)};
        auto const *data{reinterpret_cast< char const * >(&value)};
        if (reverseOrder) {
            char data1[kSize];
            std::ignore = std::reverse_copy(data, data + kSize, data1);
            return Write(StringBorrow{data1, kSize}, point);
        }
        return Write(StringBorrow{data, kSize}, point);
    }
    /// @brief Write
    /// @param[in] value Value
    /// @param[in] point Bit position
    /// @return Result -- >= 0: number of bytes written; < 0: error code
    inline result_t Write(StringBorrow const &value, point_t const &point)
    {
        size_t kSize{value.size()};
        if (kSize == 0) {
            return static_cast< result_t >(kSize);
        }
        auto it{point.first};  // Only the initial bit position index is zero
        auto pos{point.second};
        if (pos == 0) {
            it = payload_.begin();
        }
        auto const *data1{value.data()};
        while (kSize != 0) {
            if (it == payload_.end()) {
                return ErrorCode::kInvalidWrite;  // Write exceeds payload capacity limit
            }
            auto *data2{(*it).data() + pos};
            auto remain{(*it).size() - pos};
            if (remain >= kSize) {
                std::ignore = std::copy(data1, data1 + kSize, data2);
                pos         = kSize;
                break;
            }
            std::ignore = std::copy(data1, data1 + remain, data2);
            data1 += remain, kSize -= remain, ++it, pos = 0;
        }
        return static_cast< result_t >(kSize);
    }
    /// @brief Get current bit position
    /// @return Current bit position
    inline point_t const &Point() const { return point_; }

private:
    /// @brief Payload reference
    payload_t &payload_;
    /// @brief Current bit position
    point_t point_{payload_.begin(), {}};
};
/// @brief Template type -- Payload stream reader -- Specialization for variable-length array list type
/// @tparam TProps Property information
template < typename TProps >
class StreamReader< TProps, std::list< ara::core::Vector< uint8_t > > >
{
public:
    /// @brief Type alias -- Property information
    using props_t = TProps;  // NOLINT
    /// @brief Type alias -- Payload type
    using payload_t = std::list< ara::core::Vector< uint8_t > > const;  // NOLINT
    /// @brief Type alias -- Result type
    using result_t = int32_t;  // NOLINT
    /// @brief Type alias -- Bit position type
    using point_t  // NOLINT
        = std::pair< typename payload_t::const_iterator, typename payload_t::value_type::size_type >;
    /// @brief Constructor
    /// @param[in] payload Payload
    explicit StreamReader(payload_t &payload) : payload_{payload} {}
    /// @brief Read
    /// @tparam TValue Value type
    /// @param[in] value Value
    /// @param[in] reverseOrder Whether to convert byte order
    /// @return Result -- >= 0: number of bytes read; < 0: error code
    template < typename TValue >
    inline result_t Read(TValue &value, bool reverseOrder)
    {
        static_assert(IsScalar< TValue >::value, "invalid value");
        constexpr size_t kSize{sizeof(value)};
        auto *data1{reinterpret_cast< char * >(&value)};
        auto ret{Read(StringBorrow{data1, kSize})};
        if (ret < 0) {
            return ret;
        }
        if (reverseOrder) {
            std::reverse(data1, data1 + kSize);
        }
        return static_cast< result_t >(kSize);
    }
    /// @brief Read
    /// @param[in] value Value
    /// @return Result -- >= 0: number of bytes read; < 0: error code
    inline result_t Read(StringBorrow const &value)
    {
        size_t kSize{value.size()};
        if (kSize == 0) {
            return static_cast< result_t >(kSize);
        }
        auto &it{point_.first};
        auto &pos{point_.second};
        auto *data2{value.data()};
        while (kSize != 0) {
            if (it == payload_.end()) {
                return ErrorCode::kInvalidRead;  // Read exceeds payload capacity limit
            }
            auto const *data1{(*it).data() + pos};
            auto remain{(*it).size() - pos};
            if (remain >= kSize) {
                std::ignore = std::copy(data1, data1 + kSize, data2);
                pos += kSize;
                break;
            }
            std::ignore = std::copy(data1, data1 + remain, data2);
            data1 += remain, data2 += remain, kSize -= remain, ++it, pos = 0;
        }
        return static_cast< result_t >(value.size());
    }
    /// @brief Read
    /// @tparam TValue Value type
    /// @param[in] value Value
    /// @param[in] reverseOrder Whether to convert byte order
    /// @param[in] point Bit position
    /// @return Result -- >= 0: number of bytes read; < 0: error code
    template < typename TValue >
    inline result_t Read(TValue &value, bool reverseOrder, point_t const &point) const
    {
        static_assert(IsScalar< TValue >::value, "invalid value");
        constexpr size_t kSize{sizeof(value)};
        auto *data1{reinterpret_cast< char * >(&value)};
        auto ret{Read(StringBorrow{data1, kSize}, point)};
        if (ret < 0) {
            return ret;
        }
        if (reverseOrder) {
            std::reverse(data1, data1 + kSize);
        }
        return static_cast< result_t >(kSize);
    }
    /// @brief Read
    /// @param[in] value Value
    /// @param[in] point Bit position
    /// @return Result -- >= 0: number of bytes read; < 0: error code
    inline result_t Read(StringBorrow const &value, point_t const &point) const
    {
        size_t kSize{value.size()};
        if (kSize == 0) {
            return static_cast< result_t >(kSize);
        }
        auto it{point.first};
        auto pos{point.second};
        auto *data2{value.data()};
        while (kSize != 0) {
            if (it == payload_.end()) {
                return ErrorCode::kInvalidRead;  // Read exceeds payload capacity limit
            }
            auto const *data1{(*it).data() + pos};
            auto remain{(*it).size() - pos};
            if (remain >= kSize) {
                std::ignore = std::copy(data1, data1 + kSize, data2);
                pos += kSize;
                break;
            }
            std::ignore = std::copy(data1, data1 + remain, data2);
            data1 += remain, data2 += remain, kSize -= remain, ++it, pos = 0;
        }
        return static_cast< result_t >(value.size());
    }
    /// @brief Get current bit position
    /// @return Current bit position
    inline point_t const &Point() const { return point_; }

private:
    /// @brief Payload reference
    payload_t &payload_;
    /// @brief Current bit position
    point_t point_{payload_.begin(), 0};
};
/// @brief Custom payload type
class Payload
{
public:
    /// @brief Constructor
    /// @param[in] capacity Payload capacity -- pre-allocation
    explicit Payload(size_t capacity = kInt32_256U) { reserve(capacity); }
    /// @brief Copy constructor
    /// @param[in] other
    Payload(Payload const &other) { copy(other); }
    /// @brief Move constructor
    /// @param[in] other
    Payload(Payload &&other) noexcept { move(std::move(other)); }
    /// @brief Destructor
    ~Payload() noexcept { free(); }
    /// @brief Copy assignment operator
    /// @param other
    /// @return Payload
    Payload &operator=(Payload const &other) = default;
    /// @brief Move assignment operator
    /// @param other
    /// @return Payload
    Payload &operator=(Payload &&other) noexcept = default;
    /// @brief Reserve payload capacity
    /// @param[in] capacity Payload capacity
    void reserve(size_t capacity)  // NOLINT
    {
        if (capacity <= capacity_) {
            return;
        }
        auto const *data{data_};
        data_       = new uint8_t[capacity];
        std::ignore = std::copy(data, data + size_, data_);
        delete[] data;
        capacity_ = capacity;
    }
    /// @brief Resize payload
    /// @param[in] kSize Bytes
    void resize(size_t kSize)  // NOLINT
    {
        reserve(kSize);
        size_ = kSize;
    }
    /// @brief Copy payload
    /// @param[in] other
    void copy(Payload const &other)  // NOLINT
    {
        if (&other == this) {
            return;
        }
        resize(other.capacity_);
        std::ignore = std::copy(other.data_, other.data_ + other.size_, data_);
        size_       = other.size_;
    }
    /// @brief Move payload
    /// @param[in] other
    void move(Payload &&other)  // NOLINT
    {
        if (&other == this) {
            return;
        }
        std::swap(other.data_, data_);
        std::swap(other.size_, size_);
        std::swap(other.capacity_, capacity_);
    }
    /// @brief Clear payload
    void clear() { size_ = 0; }  // NOLINT
    /// @brief Release payload
    void free()  // NOLINT
    {
        delete[] data_;
        data_     = nullptr;
        size_     = 0;
        capacity_ = 0;
    }
    /// @brief Get payload pointer
    /// @return Payload pointer
    uint8_t *data() const { return data_; }  // NOLINT
    /// @brief Get payload size
    /// @return Payload size
    size_t size() const { return size_; }  // NOLINT
    /// @brief Get payload capacity
    /// @return Payload capacity
    size_t capacity() const { return capacity_; }  // NOLINT
    /// @brief Convert to string
    /// @return Payload string
    auto ToString() const { return ara::core::StringView{reinterpret_cast< char * >(data_), size_}; }

private:
    /// @brief Payload pointer
    uint8_t *data_{nullptr};
    /// @brief Payload size
    size_t size_{0};
    /// @brief Payload capacity
    size_t capacity_{0};
};
/// @brief Template type -- Payload stream writer -- Specialization for custom payload type
/// @tparam TProps Property information
template < typename TProps >
class StreamWriter< TProps, Payload >
{
public:
    /// @brief Type alias -- Property information
    using props_t = TProps;  // NOLINT
    /// @brief Type alias -- Payload type
    using payload_t = Payload;  // NOLINT
    /// @brief Type alias -- Result type
    using result_t = int32_t;  // NOLINT
    /// @brief Type alias -- Bit position type
    using point_t = size_t;  // NOLINT
    /// @brief Constructor
    /// @param[in] payload Payload
    /// @param[in] capacity Payload capacity -- pre-allocation
    explicit StreamWriter(payload_t &payload, size_t capacity) : payload_{payload} { payload_.reserve(capacity); }
    /// @brief Write
    /// @tparam TValue Value type
    /// @param[in] value Value
    /// @param[in] reverseOrder Whether to convert byte order
    /// @return Result -- >= 0: number of bytes written; < 0: error code
    template < typename TValue >
    inline result_t Write(TValue const &value, bool reverseOrder)
    {
        static_assert(IsScalar< TValue >::value, "invalid value");
        constexpr size_t kSize{sizeof(value)};
        if (point_ + kSize > payload_.size()) {
            payload_.resize(point_ + kSize);
        }
        auto const *data1{reinterpret_cast< uint8_t const * >(&value)};
        auto *data2{payload_.data() + point_};
        if (reverseOrder) {
            std::ignore = std::reverse_copy(data1, data1 + kSize, data2);
        } else {
            std::ignore = std::copy(data1, data1 + kSize, data2);
        }
        point_ += kSize;
        return static_cast< result_t >(kSize);
    }
    /// @brief Write
    /// @param[in] value Value
    /// @return Result -- >= 0: number of bytes written; < 0: error code
    inline result_t Write(StringBorrow const &value)
    {
        const size_t kSize{value.size()};
        if (point_ + kSize > payload_.size()) {
            payload_.resize(point_ + kSize);
        }
        auto const *data1{value.data()};
        auto *data2{payload_.data() + point_};
        std::ignore = std::copy(data1, data1 + kSize, data2);
        point_ += kSize;
        return static_cast< result_t >(kSize);
    }
    /// @brief Write
    /// @tparam TValue Value type
    /// @param[in] value Value
    /// @param[in] reverseOrder Whether to convert byte order
    /// @param[in] point Bit position
    /// @return Result -- >= 0: number of bytes written; < 0: error code
    template < typename TValue >
    inline result_t Write(TValue const &value, bool reverseOrder, point_t const &point)
    {
        static_assert(IsScalar< TValue >::value, "invalid value");
        constexpr size_t kSize{sizeof(value)};
        if (point + kSize > payload_.size()) {
            return ErrorCode::kInvalidWrite;  // Write exceeds payload capacity limit (value)
        }
        auto const *data1{reinterpret_cast< uint8_t const * >(&value)};
        auto *data2{payload_.data() + point};
        if (reverseOrder) {
            std::ignore = std::reverse_copy(data1, data1 + kSize, data2);
        } else {
            std::ignore = std::copy(data1, data1 + kSize, data2);
        }
        return static_cast< result_t >(kSize);
    }
    /// @brief Write
    /// @param[in] value Value
    /// @param[in] point Bit position
    /// @return Result -- >= 0: number of bytes written; < 0: error code
    inline result_t Write(StringBorrow const &value, point_t const &point)
    {
        const size_t kSize{value.size()};
        if (point + kSize > payload_.size()) {
            return ErrorCode::kInvalidWrite;  // Write exceeds payload capacity limit (string)
        }
        auto const *data1{value.data()};
        auto *data2{payload_.data() + point};
        std::ignore = std::copy(data1, data1 + kSize, data2);
        return static_cast< result_t >(kSize);
    }
    /// @brief Get current bit position
    /// @return Current bit position
    inline point_t const &Point() const { return point_; }

private:
    /// @brief Payload reference
    payload_t &payload_;
    /// @brief Current bit position
    point_t point_{};
};
/// @brief Template type -- Payload stream reader -- Specialization for custom payload type
/// @tparam TProps Property information
template < typename TProps >
class StreamReader< TProps, Payload >
{
public:
    /// @brief Type alias -- Property information
    using props_t = TProps;  // NOLINT
    /// @brief Type alias -- Payload type
    using payload_t = Payload const;  // NOLINT
    /// @brief Type alias -- Result type
    using result_t = int32_t;  // NOLINT
    /// @brief Type alias -- Bit position type
    using point_t = size_t;  // NOLINT
    /// @brief Constructor
    /// @param[in] payload Payload
    explicit StreamReader(payload_t &payload) : payload_{payload} {}
    /// @brief Read
    /// @tparam TValue Value type
    /// @param[in] value Value
    /// @param[in] reverseOrder Whether to convert byte order
    /// @return Result -- >= 0: number of bytes read; < 0: error code
    template < typename TValue >
    inline result_t Read(TValue &value, bool reverseOrder)
    {
        static_assert(IsScalar< TValue >::value, "invalid value");
        constexpr size_t kSize{sizeof(value)};
        if (point_ + kSize > payload_.size()) {
            return ErrorCode::kInvalidRead;  // Read exceeds payload capacity limit (value)
        }
        auto const *data1{payload_.data() + point_};
        auto *data2{reinterpret_cast< uint8_t * >(&value)};
        if (reverseOrder) {
            std::ignore = std::reverse_copy(data1, data1 + kSize, data2);
        } else {
            std::ignore = std::copy(data1, data1 + kSize, data2);
        }
        point_ += kSize;
        return static_cast< result_t >(kSize);
    }
    /// @brief Read
    /// @param[in] value Value
    /// @return Result -- >= 0: number of bytes read; < 0: error code
    inline result_t Read(StringBorrow const &value)
    {
        const size_t kSize{value.size()};
        if (point_ + kSize > payload_.size()) {
            return ErrorCode::kInvalidRead;  // Read exceeds payload capacity limit (string)
        }
        auto const *data1{payload_.data() + point_};
        auto *data2{value.data()};
        std::ignore = std::copy(data1, data1 + kSize, data2);
        point_ += kSize;
        return static_cast< result_t >(kSize);
    }
    /// @brief Read
    /// @tparam TValue Value type
    /// @param[in] value Value
    /// @param[in] reverseOrder Whether to convert byte order
    /// @param[in] point Bit position
    /// @return Result -- >= 0: number of bytes read; < 0: error code
    template < typename TValue >
    inline result_t Read(TValue &value, bool reverseOrder, point_t const &point) const
    {
        static_assert(IsScalar< TValue >::value, "invalid value");
        constexpr size_t kSize{sizeof(value)};
        if (point + kSize > payload_.size()) {
            return ErrorCode::kInvalidRead;  // Read exceeds payload capacity limit (value)
        }
        auto const *data1{payload_.data() + point};
        auto *data2{reinterpret_cast< uint8_t * >(&value)};
        if (reverseOrder) {
            std::ignore = std::reverse_copy(data1, data1 + kSize, data2);
        } else {
            std::ignore = std::copy(data1, data1 + kSize, data2);
        }
        return static_cast< result_t >(kSize);
    }
    /// @brief Read
    /// @param[in] value Value
    /// @param[in] point Bit position
    /// @return Result -- >= 0: number of bytes read; < 0: error code
    inline result_t Read(StringBorrow const &value, point_t const &point) const
    {
        const size_t kSize{value.size()};
        if (point + kSize > payload_.size()) {
            return ErrorCode::kInvalidRead;  // Read exceeds payload capacity limit (string)
        }
        auto const *data1{payload_.data() + point};
        auto *data2{value.data()};
        std::ignore = std::copy(data1, data1 + kSize, data2);
        return static_cast< result_t >(kSize);
    }
    /// @brief Get current bit position
    /// @return Current bit position
    inline point_t const &Point() const { return point_; }

private:
    /// @brief Payload reference
    payload_t &payload_;
    /// @brief Current bit position
    point_t point_{};
};

}  // namespace serialize
}  // namespace isoft

#endif
