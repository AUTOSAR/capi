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
/// @file       npc_serialize_stream.h
/// @brief      Binding layer serialization payload stream header file
/// @details
/// @date       2021-12-31
/// @author     mazelin
/// @version    1.2.0
///
/// ================================================================

#ifndef __COM_NPC_SERIALIZE_STREAM_H
#define __COM_NPC_SERIALIZE_STREAM_H

#include <list>

#include "npc_serialize_base.h"
#include "npc_types.h"

/// @brief Namespace -- internal binding layer serialization
namespace ara {
namespace com {
namespace internal {
namespace npc {
namespace serialize {
/// @brief Check if system is little-endian
/// @return bool
inline bool IsLittleEndian() noexcept
{
    uint32_t _{1};
    return reinterpret_cast< uint8_t* >(&_)[0] == 1;
}

/// @brief Template type -- payload stream writer -- declaration
/// @tparam TProps Property information
/// @tparam TPayload Payload type
template < typename TProps, typename TPayload, typename = void >
class StreamWriter;
/// @brief Template type -- payload stream reader -- declaration
/// @tparam TProps Property information
/// @tparam TPayload Payload type
template < typename TProps, typename TPayload, typename = void >
class StreamReader;
/// @brief Template type -- payload stream writer -- specialization for null pointer type (for calculating number of bytes written)
/// @tparam TProps Property information
template < typename TProps >
class StreamWriter< TProps, std::nullptr_t >
{
public:
    /// @brief Type alias -- property information
    using PropsType = TProps;
    /// @brief Type alias -- result type
    using ResultType = int32_t;
    /// @brief Type alias -- position type
    using PointType = std::nullptr_t;
    /// @brief Write
    /// @tparam TValue Value type
    /// @param[in] value Value
    /// @param[in] reverseOrder Whether to convert byte order
    /// @return Result -- >= 0: number of bytes written; < 0: error code;
    template < typename TValue >
    inline ResultType Write(TValue const& value, bool reverseOrder) noexcept
    {
        static_assert(IsScalar< TValue >::value, "invalid value");
        std::ignore = reverseOrder;
        return sizeof(value);
    }
    /// @brief Write
    /// @param[in] value Value
    /// @return Result -- >= 0: number of bytes written; < 0: error code;
    inline ResultType Write(StringBorrow const& value) noexcept { return static_cast< ResultType >(value.Size()); }
    /// @brief Write
    /// @tparam TValue Value type
    /// @param[in] value Value
    /// @param[in] reverseOrder Whether to convert byte order
    /// @param[in] point Position
    /// @return Result -- >= 0: number of bytes written; < 0: error code;
    template < typename TValue >
    inline ResultType Write(TValue const& value, bool reverseOrder, PointType const& point) noexcept
    {
        static_assert(IsScalar< TValue >::value, "invalid value");
        std::ignore = reverseOrder;
        std::ignore = point;
        return sizeof(value);
    }
    /// @brief Write
    /// @param[in] value Value
    /// @param[in] point Position
    /// @return Result -- >= 0: number of bytes written; < 0: error code;
    inline ResultType Write(StringBorrow const& value, PointType const& point) noexcept
    {
        std::ignore = point;
        return static_cast< ResultType >(value.Size());
    }
    /// @brief Get current position
    /// @return Current position
    inline PointType const& Point() const noexcept { return point_; }

private:
    PointType point_{};
};
/// @brief Template type -- payload stream writer -- specialization for raw string (pointer + length) type
/// @tparam TProps Property information
template < typename TProps >
class StreamWriter< TProps, StringBorrow >
{
public:
    /// @brief Type alias -- property information
    using PropsType = TProps;
    /// @brief Type alias -- payload type
    using PayloadType = StringBorrow;
    /// @brief Type alias -- result type
    using ResultType = int32_t;
    /// @brief Type alias -- position type
    using PointType = typename PayloadType::SizeType;
    /// @brief Constructor
    /// @param[in] payload Payload
    /// @param[in] capacity Payload capacity -- pre-allocation
    explicit StreamWriter(PayloadType& payload, size_t capacity) noexcept : payload_{payload}
    {
        assert(capacity <= payload_.Size());
        std::ignore = capacity;
    }
    /// @brief Write
    /// @tparam TValue Value type
    /// @param[in] value Value
    /// @param[in] reverseOrder Whether to convert byte order
    /// @return Result -- >= 0: number of bytes written; < 0: error code;
    template < typename TValue >
    inline ResultType Write(TValue const& value, bool reverseOrder) noexcept
    {
        static_assert(IsScalar< TValue >::value, "invalid value");
        constexpr size_t kSize{sizeof(value)};
        if (point_ + kSize > payload_.Size()) {
            return ErrorCode::kInvalidWrite;  // Write exceeds payload capacity limit (value)
        }
        auto const* data1{reinterpret_cast< uint8_t const* >(&value)};
        auto* data2{payload_.Data() + point_};
        if (reverseOrder) {
            std::ignore = std::reverse_copy(data1, data1 + kSize, data2);
        } else {
            std::ignore = std::copy(data1, data1 + kSize, data2);
        }
        point_ += kSize;
        return static_cast< ResultType >(kSize);
    }
    /// @brief Write
    /// @param[in] value Value
    /// @return Result -- >= 0: number of bytes written; < 0: error code;
    inline ResultType Write(StringBorrow const& value) noexcept
    {
        size_t const kSize{value.Size()};
        if (point_ + kSize > payload_.Size()) {
            return ErrorCode::kInvalidWrite;  // Write exceeds payload capacity limit (string)
        }
        auto* data1{value.Data()};
        auto* data2{payload_.Data() + point_};
        std::ignore = std::copy(data1, data1 + kSize, data2);
        point_ += kSize;
        return static_cast< ResultType >(kSize);
    }
    /// @brief Write
    /// @tparam TValue Value type
    /// @param[in] value Value
    /// @param[in] reverseOrder Whether to convert byte order
    /// @param[in] point Position
    /// @return Result -- >= 0: number of bytes written; < 0: error code;
    template < typename TValue >
    inline ResultType Write(TValue const& value, bool reverseOrder, PointType const& point) noexcept
    {
        static_assert(IsScalar< TValue >::value, "invalid value");
        constexpr size_t kSize{sizeof(value)};
        if (point + kSize > payload_.Size()) {
            return ErrorCode::kInvalidWrite;  // Write exceeds payload capacity limit (value)
        }
        auto const* data1{reinterpret_cast< uint8_t const* >(&value)};
        auto* data2{payload_.Data() + point};
        if (reverseOrder) {
            std::ignore = std::reverse_copy(data1, data1 + kSize, data2);
        } else {
            std::ignore = std::copy(data1, data1 + kSize, data2);
        }
        return static_cast< ResultType >(kSize);
    }
    /// @brief Write
    /// @param[in] value Value
    /// @param[in] point Position
    /// @return Result -- >= 0: number of bytes written; < 0: error code;
    inline ResultType Write(StringBorrow const& value, PointType const& point) noexcept
    {
        size_t const kSize{value.Size()};
        if (point + kSize > payload_.Size()) {
            return ErrorCode::kInvalidWrite;  // Write exceeds payload capacity limit (string)
        }
        auto* data1{value.Data()};
        auto* data2{payload_.Data() + point};
        std::ignore = std::copy(data1, data1 + kSize, data2);
        return static_cast< ResultType >(kSize);
    }
    /// @brief Get current position
    /// @return Current position
    inline PointType const& Point() const noexcept { return point_; }

private:
    /// @brief Payload reference
    PayloadType& payload_;
    /// @brief Current position
    PointType point_{};
};
/// @brief Template type -- payload stream reader -- specialization for raw string (pointer + length) type
/// @tparam TProps Property information
template < typename TProps >
class StreamReader< TProps, StringBorrow >
{
public:
    /// @brief Type alias -- property information
    using PropsType = TProps;
    /// @brief Type alias -- payload type
    using PayloadType = StringBorrow const;
    /// @brief Type alias -- result type
    using ResultType = int32_t;
    /// @brief Type alias -- position type
    using PointType = typename PayloadType::SizeType;
    /// @brief Constructor
    /// @param[in] payload Payload
    explicit StreamReader(PayloadType& payload) noexcept : payload_{payload} {}
    /// @brief Read
    /// @tparam TValue Value type
    /// @param[in] value Value
    /// @param[in] reverseOrder Whether to convert byte order
    /// @return Result -- >= 0: number of bytes read; < 0: error code;
    template < typename TValue >
    inline ResultType Read(TValue& value, bool reverseOrder) noexcept
    {
        static_assert(IsScalar< TValue >::value, "invalid value");
        constexpr size_t kSize{sizeof(value)};
        if (point_ + kSize > payload_.Size()) {
            return ErrorCode::kInvalidRead;  // Read exceeds payload capacity limit (value)
        }
        auto* data1{payload_.Data() + point_};
        auto* data2{reinterpret_cast< uint8_t* >(&value)};
        if (reverseOrder) {
            std::ignore = std::reverse_copy(data1, data1 + kSize, data2);
        } else {
            std::ignore = std::copy(data1, data1 + kSize, data2);
        }
        point_ += kSize;
        return static_cast< ResultType >(kSize);
    }
    /// @brief Read
    /// @param[in] value Value
    /// @return Result -- >= 0: number of bytes read; < 0: error code;
    inline ResultType Read(StringBorrow const& value) noexcept
    {
        size_t const kSize{value.Size()};
        if (point_ + kSize > payload_.Size()) {
            return ErrorCode::kInvalidRead;  // Read exceeds payload capacity limit (string)
        }
        auto* data1{payload_.Data() + point_};
        auto* data2{value.Data()};
        std::ignore = std::copy(data1, data1 + kSize, data2);
        point_ += kSize;
        return static_cast< ResultType >(kSize);
    }
    /// @brief Read
    /// @tparam TValue Value type
    /// @param[in] value Value
    /// @param[in] reverseOrder Whether to convert byte order
    /// @param[in] point Position
    /// @return Result -- >= 0: number of bytes read; < 0: error code;
    template < typename TValue >
    inline ResultType Read(TValue& value, bool reverseOrder, PointType const& point) const noexcept
    {
        static_assert(IsScalar< TValue >::value, "invalid value");
        constexpr size_t kSize{sizeof(value)};
        if (point + kSize > payload_.Size()) {
            return ErrorCode::kInvalidRead;  // Read exceeds payload capacity limit (value)
        }
        auto* data1{payload_.Data() + point};
        auto* data2{reinterpret_cast< uint8_t* >(&value)};
        if (reverseOrder) {
            std::ignore = std::reverse_copy(data1, data1 + kSize, data2);
        } else {
            std::ignore = std::copy(data1, data1 + kSize, data2);
        }
        return static_cast< ResultType >(kSize);
    }
    /// @brief Read
    /// @param[in] value Value
    /// @param[in] point Position
    /// @return Result -- >= 0: number of bytes read; < 0: error code;
    inline ResultType Read(StringBorrow const& value, PointType const& point) const noexcept
    {
        size_t const kSize{value.Size()};
        if (point + kSize > payload_.Size()) {
            return ErrorCode::kInvalidRead;  // Read exceeds payload capacity limit (string)
        }
        auto* data1{payload_.Data() + point};
        auto* data2{value.Data()};
        std::ignore = std::copy(data1, data1 + kSize, data2);
        return static_cast< ResultType >(kSize);
    }
    /// @brief Get current position
    /// @return Current position
    inline PointType const& Point() const noexcept { return point_; }

private:
    /// @brief Payload reference
    PayloadType& payload_;
    /// @brief Current position
    PointType point_{};
};
/// @brief Template type -- payload stream writer -- specialization for variable-length array type
/// @tparam TProps Property information
template < typename TProps >
class StreamWriter< TProps, ara::core::Vector< uint8_t > >
{
public:
    /// @brief Type alias -- property information
    using PropsType = TProps;
    /// @brief Type alias -- payload type
    using PayloadType = ara::core::Vector< uint8_t >;
    /// @brief Type alias -- result type
    using ResultType = int32_t;
    /// @brief Type alias -- position type
    using PointType = typename PayloadType::size_type;
    /// @brief Constructor
    /// @param[in] payload Payload
    /// @param[in] capacity Payload capacity -- pre-allocation
    explicit StreamWriter(PayloadType& payload, size_t capacity) noexcept : payload_{payload}
    {
        payload_.reserve(capacity);
    }
    /// @brief Write
    /// @tparam TValue Value type
    /// @param[in] value Value
    /// @param[in] reverseOrder Whether to convert byte order
    /// @return Result -- >= 0: number of bytes written; < 0: error code;
    template < typename TValue >
    inline ResultType Write(TValue const& value, bool reverseOrder) noexcept
    {
        static_assert(IsScalar< TValue >::value, "invalid value");
        constexpr size_t kSize{sizeof(value)};
        if (point_ + kSize > payload_.size()) {
            payload_.resize(point_ + kSize);
        }
        auto const* data1{reinterpret_cast< uint8_t const* >(&value)};
        auto* data2{payload_.data() + point_};
        if (reverseOrder) {
            std::ignore = std::reverse_copy(data1, data1 + kSize, data2);
        } else {
            std::ignore = std::copy(data1, data1 + kSize, data2);
        }
        point_ += kSize;
        return static_cast< ResultType >(kSize);
    }
    /// @brief Write
    /// @param[in] value Value
    /// @return Result -- >= 0: number of bytes written; < 0: error code;
    inline ResultType Write(StringBorrow const& value) noexcept
    {
        size_t const kSize{value.Size()};
        if (point_ + kSize > payload_.size()) {
            payload_.resize(point_ + kSize);
        }
        auto* data1{value.Data()};
        auto* data2{payload_.data() + point_};
        std::ignore = std::copy(data1, data1 + kSize, data2);
        point_ += kSize;
        return static_cast< ResultType >(kSize);
    }
    /// @brief Write
    /// @tparam TValue Value type
    /// @param[in] value Value
    /// @param[in] reverseOrder Whether to convert byte order
    /// @param[in] point Position
    /// @return Result -- >= 0: number of bytes written; < 0: error code;
    template < typename TValue >
    inline ResultType Write(TValue const& value, bool reverseOrder, PointType const& point) noexcept
    {
        static_assert(IsScalar< TValue >::value, "invalid value");
        constexpr size_t kSize{sizeof(value)};
        if (point + kSize > payload_.size()) {
            return ErrorCode::kInvalidWrite;  // Write exceeds payload capacity limit (value)
        }
        auto const* data1{reinterpret_cast< uint8_t const* >(&value)};
        auto* data2{payload_.data() + point};
        if (reverseOrder) {
            std::ignore = std::reverse_copy(data1, data1 + kSize, data2);
        } else {
            std::ignore = std::copy(data1, data1 + kSize, data2);
        }
        return static_cast< ResultType >(kSize);
    }
    /// @brief Write
    /// @param[in] value Value
    /// @param[in] point Position
    /// @return Result -- >= 0: number of bytes written; < 0: error code;
    inline ResultType Write(StringBorrow const& value, PointType const& point) noexcept
    {
        size_t const kSize{value.Size()};
        if (point + kSize > payload_.size()) {
            return ErrorCode::kInvalidWrite;  // Write exceeds payload capacity limit (string)
        }
        auto* data1{value.Data()};
        auto* data2{payload_.data() + point};
        std::ignore = std::copy(data1, data1 + kSize, data2);
        return static_cast< ResultType >(kSize);
    }
    /// @brief Get current position
    /// @return Current position
    inline PointType const& Point() const noexcept { return point_; }

private:
    /// @brief Payload reference
    PayloadType& payload_;
    /// @brief Current position
    PointType point_{};
};
/// @brief Template type -- payload stream reader -- specialization for variable-length array type
/// @tparam TProps Property information
template < typename TProps >
class StreamReader< TProps, ara::core::Vector< uint8_t > >
{
public:
    /// @brief Type alias -- property information
    using PropsType = TProps;
    /// @brief Type alias -- payload type
    using PayloadType = ara::core::Vector< uint8_t > const;
    /// @brief Type alias -- result type
    using ResultType = int32_t;
    /// @brief Type alias -- position type
    using PointType = typename PayloadType::size_type;
    /// @brief Constructor
    /// @param[in] payload Payload
    explicit StreamReader(PayloadType& payload) noexcept : payload_{payload} {}
    /// @brief Read
    /// @tparam TValue Value type
    /// @param[in] value Value
    /// @param[in] reverseOrder Whether to convert byte order
    /// @return Result -- >= 0: number of bytes read; < 0: error code;
    template < typename TValue >
    inline ResultType Read(TValue& value, bool reverseOrder) noexcept
    {
        static_assert(IsScalar< TValue >::value, "invalid value");
        constexpr size_t kSize{sizeof(value)};
        if (point_ + kSize > payload_.size()) {
            return ErrorCode::kInvalidRead;  // Read exceeds payload capacity limit (value)
        }
        auto const* data1{payload_.data() + point_};
        auto* data2{reinterpret_cast< uint8_t* >(&value)};
        if (reverseOrder) {
            std::ignore = std::reverse_copy(data1, data1 + kSize, data2);
        } else {
            std::ignore = std::copy(data1, data1 + kSize, data2);
        }
        point_ += kSize;
        return static_cast< ResultType >(kSize);
    }
    /// @brief Read
    /// @param[in] value Value
    /// @return Result -- >= 0: number of bytes read; < 0: error code;
    inline ResultType Read(StringBorrow const& value) noexcept
    {
        size_t const kSize{value.Size()};
        if (point_ + kSize > payload_.size()) {
            return ErrorCode::kInvalidRead;  // Read exceeds payload capacity limit (string)
        }
        auto const* data1{payload_.data() + point_};
        auto* data2{value.Data()};
        std::ignore = std::copy(data1, data1 + kSize, data2);
        point_ += kSize;
        return static_cast< ResultType >(kSize);
    }
    /// @brief Read
    /// @tparam TValue Value type
    /// @param[in] value Value
    /// @param[in] reverseOrder Whether to convert byte order
    /// @param[in] point Position
    /// @return Result -- >= 0: number of bytes read; < 0: error code;
    template < typename TValue >
    inline ResultType Read(TValue& value, bool reverseOrder, PointType const& point) const noexcept
    {
        static_assert(IsScalar< TValue >::value, "invalid value");
        constexpr size_t kSize{sizeof(value)};
        if (point + kSize > payload_.size()) {
            return ErrorCode::kInvalidRead;  // Read exceeds payload capacity limit (value)
        }
        auto const* data1{payload_.data() + point};
        auto* data2{reinterpret_cast< uint8_t* >(&value)};
        if (reverseOrder) {
            std::ignore = std::reverse_copy(data1, data1 + kSize, data2);
        } else {
            std::ignore = std::copy(data1, data1 + kSize, data2);
        }
        return static_cast< ResultType >(kSize);
    }
    /// @brief Read
    /// @param[in] value Value
    /// @param[in] point Position
    /// @return Result -- >= 0: number of bytes read; < 0: error code;
    inline ResultType Read(StringBorrow const& value, PointType const& point) const noexcept
    {
        size_t const kSize{value.Size()};
        if (point + kSize > payload_.size()) {
            return ErrorCode::kInvalidRead;  // Read exceeds payload capacity limit (string)
        }
        auto const* data1{payload_.data() + point};
        auto* data2{value.Data()};
        std::ignore = std::copy(data1, data1 + kSize, data2);
        return static_cast< ResultType >(kSize);
    }
    /// @brief Get current position
    /// @return Current position
    inline PointType const& Point() const noexcept { return point_; }

private:
    /// @brief Payload reference
    PayloadType& payload_;
    /// @brief Current position
    PointType point_{};
};
/// @brief Template type -- payload stream writer -- specialization for variable-length array list type
/// @tparam TProps Property information
template < typename TProps >
class StreamWriter< TProps, std::list< ara::core::Vector< uint8_t > > >
{
public:
    /// @brief Type alias -- property information
    using PropsType = TProps;
    /// @brief Type alias -- payload type
    using PayloadType = std::list< ara::core::Vector< uint8_t > >;
    /// @brief Type alias -- result type
    using ResultType = int32_t;
    /// @brief Type alias -- position type
    using PointType = std::pair< typename PayloadType::iterator, typename PayloadType::value_type::size_type >;
    /// @brief Constructor
    /// @param[in] payload Payload
    /// @param[in] capacity Payload capacity -- pre-allocation
    explicit StreamWriter(PayloadType& payload, size_t capacity) noexcept : payload_{payload}
    {
        std::ignore = capacity;
    }
    /// @brief Write
    /// @tparam TValue Value type
    /// @param[in] value Value
    /// @param[in] reverseOrder Whether to convert byte order
    /// @return Result -- >= 0: number of bytes written; < 0: error code;
    template < typename TValue >
    inline ResultType Write(TValue const& value, bool reverseOrder) noexcept
    {
        static_assert(IsScalar< TValue >::value, "invalid value");
        constexpr size_t kSize{sizeof(value)};
        auto const* data{reinterpret_cast< char const* >(&value)};
        if (reverseOrder) {
            char data1[kSize];
            std::ignore = std::reverse_copy(data, data + kSize, data1);
            return Write(StringBorrow{data1, kSize});
        }
        return Write(StringBorrow{data, kSize});
    }
    /// @brief Write
    /// @param[in] value Value
    /// @return Result -- >= 0: number of bytes written; < 0: error code;
    inline ResultType Write(StringBorrow const& value) noexcept
    {
        size_t kSize{value.Size()};
        if (kSize == 0) {
            return static_cast< ResultType >(kSize);
        }
        auto& it{point_.first};
        auto& pos{point_.second};
        auto* data1{value.Data()};
        for (auto size{kSize}; size != 0;) {
            if (it == payload_.end()) {
                payload_.emplace_back(size);
                it  = std::prev(payload_.end());
                pos = 0;
            }
            auto* data2{(*it).data() + pos};
            auto remain{(*it).size() - pos};
            if (remain >= size) {
                std::ignore = std::copy(data1, data1 + size, data2);
                pos += size;
                break;
            }
            std::ignore = std::copy(data1, data1 + remain, data2);
            data1 += remain, size -= remain, ++it, pos = 0;
        }
        return static_cast< ResultType >(kSize);
    }
    /// @brief Write
    /// @tparam TValue Value type
    /// @param[in] value Value
    /// @param[in] reverseOrder Whether to convert byte order
    /// @param[in] point Position
    /// @return Result -- >= 0: number of bytes written; < 0: error code;
    template < typename TValue >
    inline ResultType Write(TValue const& value, bool reverseOrder, PointType const& point) noexcept
    {
        static_assert(IsScalar< TValue >::value, "invalid value");
        constexpr size_t kSize{sizeof(value)};
        auto const* data{reinterpret_cast< char const* >(&value)};
        if (reverseOrder) {
            char data1[kSize];
            std::ignore = std::reverse_copy(data, data + kSize, data1);
            return Write(StringBorrow{data1, kSize}, point);
        }
        return Write(StringBorrow{data, kSize}, point);
    }
    /// @brief Write
    /// @param[in] value Value
    /// @param[in] point Position
    /// @return Result -- >= 0: number of bytes written; < 0: error code;
    inline ResultType Write(StringBorrow const& value, PointType const& point) noexcept
    {
        size_t kSize{value.Size()};
        if (kSize == 0) {
            return static_cast< ResultType >(kSize);
        }
        auto it{point.first};  // Only initial position index is zero
        auto pos{point.second};
        if (pos == 0) {
            it = payload_.begin();
        }
        auto* data1{value.Data()};
        for (auto size{kSize}; size != 0;) {
            if (it == payload_.end()) {
                return ErrorCode::kInvalidWrite;  // Write exceeds payload capacity limit
            }
            auto* data2{(*it).data() + pos};
            auto remain{(*it).size() - pos};
            if (remain >= size) {
                std::ignore = std::copy(data1, data1 + size, data2);
                pos += size;
                break;
            }
            std::ignore = std::copy(data1, data1 + remain, data2);
            data1 += remain, size -= remain, ++it, pos = 0;
        }
        return static_cast< ResultType >(kSize);
    }
    /// @brief Get current position
    /// @return Current position
    inline PointType const& Point() const noexcept { return point_; }

private:
    /// @brief Payload reference
    PayloadType& payload_;
    /// @brief Current position
    PointType point_{payload_.begin(), {}};
};
/// @brief Template type -- payload stream reader -- specialization for variable-length array list type
/// @tparam TProps Property information
template < typename TProps >
class StreamReader< TProps, std::list< ara::core::Vector< uint8_t > > >
{
public:
    /// @brief Type alias -- property information
    using PropsType = TProps;
    /// @brief Type alias -- payload type
    using PayloadType = std::list< ara::core::Vector< uint8_t > > const;
    /// @brief Type alias -- result type
    using ResultType = int32_t;
    /// @brief Type alias -- position type
    using PointType = std::pair< typename PayloadType::const_iterator, typename PayloadType::value_type::size_type >;
    /// @brief Constructor
    /// @param[in] payload Payload
    explicit StreamReader(PayloadType& payload) noexcept : payload_{payload} {}
    /// @brief Read
    /// @tparam TValue Value type
    /// @param[in] value Value
    /// @param[in] reverseOrder Whether to convert byte order
    /// @return Result -- >= 0: number of bytes read; < 0: error code;
    template < typename TValue >
    inline ResultType Read(TValue& value, bool reverseOrder) noexcept
    {
        static_assert(IsScalar< TValue >::value, "invalid value");
        constexpr size_t kSize{sizeof(value)};
        auto* data1{reinterpret_cast< char* >(&value)};
        auto ret{Read(StringBorrow{data1, kSize})};
        if (ret < 0) {
            return ret;
        }
        if (reverseOrder) {
            std::reverse(data1, data1 + kSize);
        }
        return static_cast< ResultType >(kSize);
    }
    /// @brief Read
    /// @param[in] value Value
    /// @return Result -- >= 0: number of bytes read; < 0: error code;
    inline ResultType Read(StringBorrow const& value) noexcept
    {
        size_t kSize{value.Size()};
        if (kSize == 0) {
            return static_cast< ResultType >(kSize);
        }
        auto& it{point_.first};
        auto& pos{point_.second};
        auto* data2{value.Data()};
        for (auto size{kSize}; size != 0;) {
            if (it == payload_.end()) {
                return ErrorCode::kInvalidRead;  // Read exceeds payload capacity limit
            }
            auto const* data1{(*it).data() + pos};
            auto remain{(*it).size() - pos};
            if (remain >= size) {
                std::ignore = std::copy(data1, data1 + size, data2);
                pos += size;
                break;
            }
            std::ignore = std::copy(data1, data1 + remain, data2);
            data1 += remain, data2 += remain, size -= remain, ++it, pos = 0;
        }
        return static_cast< ResultType >(kSize);
    }
    /// @brief Read
    /// @tparam TValue Value type
    /// @param[in] value Value
    /// @param[in] reverseOrder Whether to convert byte order
    /// @param[in] point Position
    /// @return Result -- >= 0: number of bytes read; < 0: error code;
    template < typename TValue >
    inline ResultType Read(TValue& value, bool reverseOrder, PointType const& point) const noexcept
    {
        static_assert(IsScalar< TValue >::value, "invalid value");
        constexpr size_t kSize{sizeof(value)};
        auto* data1{reinterpret_cast< char* >(&value)};
        auto ret{Read(StringBorrow{data1, kSize}, point)};
        if (ret < 0) {
            return ret;
        }
        if (reverseOrder) {
            std::reverse(data1, data1 + kSize);
        }
        return static_cast< ResultType >(kSize);
    }
    /// @brief Read
    /// @param[in] value Value
    /// @param[in] point Position
    /// @return Result -- >= 0: number of bytes read; < 0: error code;
    inline ResultType Read(StringBorrow const& value, PointType const& point) const noexcept
    {
        size_t kSize{value.Size()};
        if (kSize == 0) {
            return static_cast< ResultType >(kSize);
        }
        auto it{point.first};
        auto pos{point.second};
        auto* data2{value.Data()};
        for (auto size{kSize}; size != 0;) {
            if (it == payload_.end()) {
                return ErrorCode::kInvalidRead;  // Read exceeds payload capacity limit
            }
            auto const* data1{(*it).data() + pos};
            auto remain{(*it).size() - pos};
            if (remain >= size) {
                std::ignore = std::copy(data1, data1 + size, data2);
                pos += size;
                break;
            }
            std::ignore = std::copy(data1, data1 + remain, data2);
            data1 += remain, data2 += remain, size -= remain, ++it, pos = 0;
        }
        return static_cast< ResultType >(kSize);
    }
    /// @brief Get current position
    /// @return Current position
    inline PointType const& Point() const noexcept { return point_; }

private:
    /// @brief Payload reference
    PayloadType& payload_;
    /// @brief Current position
    PointType point_{payload_.begin(), 0};
};
/// @brief Custom payload type
class Payload
{
public:
    /// @brief Constructor
    explicit Payload() noexcept
    {
        constexpr size_t kDefaultCapacity{256};
        reserve(kDefaultCapacity);
    }
    /// @brief Constructor
    /// @param[in] capacity Payload capacity -- pre-allocation
    explicit Payload(size_t capacity) noexcept { reserve(capacity); }
    /// @brief Copy constructor
    /// @param[in] other
    Payload(Payload const& other) noexcept { copy(other); }
    /// @brief Move constructor
    /// @param[in] other
    Payload(Payload&& other) noexcept { move(std::move(other)); }
    /// @brief Destructor
    ~Payload() noexcept { free(); }
    /// @brief Copy assignment operator
    /// @param other
    /// @return Payload
    Payload& operator=(Payload const& other) noexcept = default;
    /// @brief Move assignment operator
    /// @param other
    /// @return Payload
    Payload& operator=(Payload&& other) noexcept = default;
    /// @brief Reserve payload capacity
    /// @param[in] capacity Payload capacity
    void reserve(size_t capacity) noexcept  // NOLINT -- standard library interface name > naming convention
    {
        if (capacity <= capacity_) {
            return;
        }
        auto* data{data_};
        data_       = new uint8_t[capacity];
        std::ignore = std::copy(data, data + size_, data_);
        delete[] data;
        capacity_ = capacity;
    }
    /// @brief Resize payload
    /// @param[in] kSize Bytes
    void resize(size_t kSize) noexcept  // NOLINT -- standard library interface name > naming convention
    {
        reserve(kSize);
        size_ = kSize;
    }
    /// @brief Copy payload
    /// @param[in] other
    void copy(Payload const& other) noexcept  // NOLINT -- standard library interface name > naming convention
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
    void move(Payload&& other) noexcept  // NOLINT -- standard library interface name > naming convention
    {
        if (&other == this) {
            return;
        }
        std::swap(other.data_, data_);
        std::swap(other.size_, size_);
        std::swap(other.capacity_, capacity_);
    }
    /// @brief Clear payload
    void clear() noexcept  // NOLINT -- standard library interface name > naming convention
    {
        size_ = 0;
    }
    /// @brief Free payload
    void free() noexcept  // NOLINT -- standard library interface name > naming convention
    {
        delete[] data_;
        data_     = nullptr;
        size_     = 0;
        capacity_ = 0;
    }
    /// @brief Get payload pointer
    /// @return Payload pointer
    uint8_t* data() const noexcept  // NOLINT -- standard library interface name > naming convention
    {
        return data_;
    }
    /// @brief Get payload size
    /// @return Payload size
    size_t size() const noexcept  // NOLINT -- standard library interface name > naming convention
    {
        return size_;
    }
    /// @brief Get payload capacity
    /// @return Payload capacity
    size_t capacity() const noexcept  // NOLINT -- standard library interface name > naming convention
    {
        return capacity_;
    }
    /// @brief Convert to string
    /// @return Payload string
    auto ToString() const noexcept { return ara::core::StringView{reinterpret_cast< char* >(data_), size_}; }

private:
    /// @brief Payload pointer
    uint8_t* data_{};
    /// @brief Payload size
    size_t size_{};
    /// @brief Payload capacity
    size_t capacity_{};
};
/// @brief Template type -- payload stream writer -- specialization for custom payload type
/// @tparam TProps Property information
template < typename TProps >
class StreamWriter< TProps, Payload >
{
public:
    /// @brief Type alias -- property information
    using PropsType = TProps;
    /// @brief Type alias -- payload type
    using PayloadType = Payload;
    /// @brief Type alias -- result type
    using ResultType = int32_t;
    /// @brief Type alias -- position type
    using PointType = size_t;
    /// @brief Constructor
    /// @param[in] payload Payload
    /// @param[in] capacity Payload capacity -- pre-allocation
    explicit StreamWriter(PayloadType& payload, size_t capacity) noexcept : payload_{payload}
    {
        payload_.reserve(capacity);
    }
    /// @brief Write
    /// @tparam TValue Value type
    /// @param[in] value Value
    /// @param[in] reverseOrder Whether to convert byte order
    /// @return Result -- >= 0: number of bytes written; < 0: error code;
    template < typename TValue >
    inline ResultType Write(TValue const& value, bool reverseOrder) noexcept
    {
        static_assert(IsScalar< TValue >::value, "invalid value");
        constexpr size_t kSize{sizeof(value)};
        if (point_ + kSize > payload_.size()) {
            payload_.resize(point_ + kSize);
        }
        auto const* data1{reinterpret_cast< uint8_t const* >(&value)};
        auto* data2{payload_.data() + point_};
        if (reverseOrder) {
            std::ignore = std::reverse_copy(data1, data1 + kSize, data2);
        } else {
            std::ignore = std::copy(data1, data1 + kSize, data2);
        }
        point_ += kSize;
        return static_cast< ResultType >(kSize);
    }
    /// @brief Write
    /// @param[in] value Value
    /// @return Result -- >= 0: number of bytes written; < 0: error code;
    inline ResultType Write(StringBorrow const& value) noexcept
    {
        size_t const kSize{value.Size()};
        if (point_ + kSize > payload_.size()) {
            payload_.resize(point_ + kSize);
        }
        auto* data1{value.Data()};
        auto* data2{payload_.data() + point_};
        std::ignore = std::copy(data1, data1 + kSize, data2);
        point_ += kSize;
        return static_cast< ResultType >(kSize);
    }
    /// @brief Write
    /// @tparam TValue Value type
    /// @param[in] value Value
    /// @param[in] reverseOrder Whether to convert byte order
    /// @param[in] point Position
    /// @return Result -- >= 0: number of bytes written; < 0: error code;
    template < typename TValue >
    inline ResultType Write(TValue const& value, bool reverseOrder, PointType const& point) noexcept
    {
        static_assert(IsScalar< TValue >::value, "invalid value");
        constexpr size_t kSize{sizeof(value)};
        if (point + kSize > payload_.size()) {
            return ErrorCode::kInvalidWrite;  // Write exceeds payload capacity limit (value)
        }
        auto const* data1{reinterpret_cast< uint8_t const* >(&value)};
        auto* data2{payload_.data() + point};
        if (reverseOrder) {
            std::ignore = std::reverse_copy(data1, data1 + kSize, data2);
        } else {
            std::ignore = std::copy(data1, data1 + kSize, data2);
        }
        return static_cast< ResultType >(kSize);
    }
    /// @brief Write
    /// @param[in] value Value
    /// @param[in] point Position
    /// @return Result -- >= 0: number of bytes written; < 0: error code;
    inline ResultType Write(StringBorrow const& value, PointType const& point) noexcept
    {
        size_t const kSize{value.Size()};
        if (point + kSize > payload_.size()) {
            return ErrorCode::kInvalidWrite;  // Write exceeds payload capacity limit (string)
        }
        auto* data1{value.Data()};
        auto* data2{payload_.data() + point};
        std::ignore = std::copy(data1, data1 + kSize, data2);
        return static_cast< ResultType >(kSize);
    }
    /// @brief Get current position
    /// @return Current position
    inline PointType const& Point() const noexcept { return point_; }

private:
    /// @brief Payload reference
    PayloadType& payload_;
    /// @brief Current position
    PointType point_{};
};
/// @brief Template type -- payload stream reader -- specialization for custom payload type
/// @tparam TProps Property information
template < typename TProps >
class StreamReader< TProps, Payload >
{
public:
    /// @brief Type alias -- property information
    using PropsType = TProps;
    /// @brief Type alias -- payload type
    using PayloadType = Payload const;
    /// @brief Type alias -- result type
    using ResultType = int32_t;
    /// @brief Type alias -- position type
    using PointType = size_t;
    /// @brief Constructor
    /// @param[in] payload Payload
    explicit StreamReader(PayloadType& payload) noexcept : payload_{payload} {}
    /// @brief Read
    /// @tparam TValue Value type
    /// @param[in] value Value
    /// @param[in] reverseOrder Whether to convert byte order
    /// @return Result -- >= 0: number of bytes read; < 0: error code;
    template < typename TValue >
    inline ResultType Read(TValue& value, bool reverseOrder) noexcept
    {
        static_assert(IsScalar< TValue >::value, "invalid value");
        constexpr size_t kSize{sizeof(value)};
        if (point_ + kSize > payload_.size()) {
            return ErrorCode::kInvalidRead;  // Read exceeds payload capacity limit (value)
        }
        auto* data1{payload_.data() + point_};
        auto* data2{reinterpret_cast< uint8_t* >(&value)};
        if (reverseOrder) {
            std::ignore = std::reverse_copy(data1, data1 + kSize, data2);
        } else {
            std::ignore = std::copy(data1, data1 + kSize, data2);
        }
        point_ += kSize;
        return static_cast< ResultType >(kSize);
    }
    /// @brief Read
    /// @param[in] value Value
    /// @return Result -- >= 0: number of bytes read; < 0: error code;
    inline ResultType Read(StringBorrow const& value) noexcept
    {
        size_t const kSize{value.Size()};
        if (point_ + kSize > payload_.size()) {
            return ErrorCode::kInvalidRead;  // Read exceeds payload capacity limit (string)
        }
        auto* data1{payload_.data() + point_};
        auto* data2{value.Data()};
        std::ignore = std::copy(data1, data1 + kSize, data2);
        point_ += kSize;
        return static_cast< ResultType >(kSize);
    }
    /// @brief Read
    /// @tparam TValue Value type
    /// @param[in] value Value
    /// @param[in] reverseOrder Whether to convert byte order
    /// @param[in] point Position
    /// @return Result -- >= 0: number of bytes read; < 0: error code;
    template < typename TValue >
    inline ResultType Read(TValue& value, bool reverseOrder, PointType const& point) const noexcept
    {
        static_assert(IsScalar< TValue >::value, "invalid value");
        constexpr size_t kSize{sizeof(value)};
        if (point + kSize > payload_.size()) {
            return ErrorCode::kInvalidRead;  // Read exceeds payload capacity limit (value)
        }
        auto* data1{payload_.data() + point};
        auto* data2{reinterpret_cast< uint8_t* >(&value)};
        if (reverseOrder) {
            std::ignore = std::reverse_copy(data1, data1 + kSize, data2);
        } else {
            std::ignore = std::copy(data1, data1 + kSize, data2);
        }
        return static_cast< ResultType >(kSize);
    }
    /// @brief Read
    /// @param[in] value Value
    /// @param[in] point Position
    /// @return Result -- >= 0: number of bytes read; < 0: error code;
    inline ResultType Read(StringBorrow const& value, PointType const& point) const noexcept
    {
        size_t const kSize{value.Size()};
        if (point + kSize > payload_.size()) {
            return ErrorCode::kInvalidRead;  // Read exceeds payload capacity limit (string)
        }
        auto* data1{payload_.data() + point};
        auto* data2{value.Data()};
        std::ignore = std::copy(data1, data1 + kSize, data2);
        return static_cast< ResultType >(kSize);
    }
    /// @brief Get current position
    /// @return Current position
    inline PointType const& Point() const noexcept { return point_; }

private:
    /// @brief Payload reference
    PayloadType& payload_;
    /// @brief Current position
    PointType point_{};
};
/// @brief Template type -- payload stream writer -- specialization for custom message payload type
/// @tparam TProps Property information
template < typename TProps >
class StreamWriter< TProps, npc_message_t >
{
public:
    /// @brief Type alias -- property information
    using PropsType = TProps;
    /// @brief Type alias -- payload type
    using PayloadType = npc_message_t;
    /// @brief Type alias -- result type
    using ResultType = int32_t;
    /// @brief Type alias -- position type
    using PointType = npc_message_io_t;
    /// @brief Constructor
    /// @param[in] payload Payload
    /// @param[in] capacity Payload capacity -- pre-allocation
    explicit StreamWriter(PayloadType& payload, size_t capacity) noexcept : payload_{payload}
    {
        std::ignore = npc_message_write_start(&point_, &payload_);
        std::ignore = npc_message_reserve(&point_, capacity);
    }
    /// @brief Destructor
    ~StreamWriter() noexcept { std::ignore = npc_message_write_end(&point_); }
    /// @brief Copy constructor
    /// @param other
    StreamWriter(StreamWriter const& other) noexcept = default;
    /// @brief Move constructor
    /// @param other
    StreamWriter(StreamWriter&& other) noexcept = default;
    /// @brief Copy assignment operator
    /// @param other
    /// @return StreamWriter
    StreamWriter& operator=(StreamWriter const& other) noexcept = default;
    /// @brief Move assignment operator
    /// @param other
    /// @return StreamWriter
    StreamWriter& operator=(StreamWriter&& other) noexcept = default;
    /// @brief Write
    /// @tparam TValue Value type
    /// @param[in] value Value
    /// @param[in] reverseOrder Whether to convert byte order
    /// @return Result -- >= 0: number of bytes written; < 0: error code;
    template < typename TValue >
    inline ResultType Write(TValue const& value, bool reverseOrder) noexcept
    {
        static_assert(IsScalar< TValue >::value, "invalid value");
        constexpr size_t kSize{sizeof(value)};
        auto const* data{reinterpret_cast< uint8_t const* >(&value)};
        if (reverseOrder && kSize != 1) {
            uint8_t data1[kSize];
            std::ignore = std::reverse_copy(data, data + kSize, data1);
            if (static_cast< size_t >(npc_message_write(&point_, data1, kSize)) != kSize) {
                return ErrorCode::kInvalidWrite;  // Write exceeds payload capacity limit (value)
            }
        } else {
            if (static_cast< size_t >(npc_message_write(&point_, data, kSize)) != kSize) {
                return ErrorCode::kInvalidWrite;  // Write exceeds payload capacity limit (value)
            }
        }
        return static_cast< ResultType >(kSize);
    }
    /// @brief Write
    /// @param[in] value Value
    /// @return Result -- >= 0: number of bytes written; < 0: error code;
    inline ResultType Write(StringBorrow const& value) noexcept
    {
        size_t const kSize{value.Size()};
        if (kSize == 0) {
            return static_cast< ResultType >(kSize);
        }
        auto* data{value.Data()};
        if (static_cast< size_t >(npc_message_write(&point_, data, kSize)) != kSize) {
            return ErrorCode::kInvalidWrite;  // Write exceeds payload capacity limit (string)
        }
        return static_cast< ResultType >(kSize);
    }
    /// @brief Write
    /// @tparam TValue Value type
    /// @param[in] value Value
    /// @param[in] reverseOrder Whether to convert byte order
    /// @param[in] point Position
    /// @return Result -- >= 0: number of bytes written; < 0: error code;
    template < typename TValue >
    inline ResultType Write(TValue const& value, bool reverseOrder, PointType const& point) noexcept
    {
        static_assert(IsScalar< TValue >::value, "invalid value");
        if (point.msg != point_.msg) {
            return ErrorCode::kInvalidPoint;  // Write position invalid
        }
        constexpr size_t kSize{sizeof(value)};
        auto const* data{reinterpret_cast< uint8_t const* >(&value)};
        if (reverseOrder && kSize != 1) {
            uint8_t data1[kSize];
            std::ignore = std::reverse_copy(data, data + kSize, data1);
            if (npc_message_overwrite(const_cast< PointType* >(&point), data1, kSize) != kSize) {
                return ErrorCode::kInvalidWrite;  // Write exceeds payload capacity limit (value)
            }
        } else {
            if (npc_message_overwrite(const_cast< PointType* >(&point), data, kSize) != kSize) {
                return ErrorCode::kInvalidWrite;  // Write exceeds payload capacity limit (value)
            }
        }
        return static_cast< ResultType >(kSize);
    }
    /// @brief Write
    /// @param[in] value Value
    /// @param[in] point Position
    /// @return Result -- >= 0: number of bytes written; < 0: error code;
    inline ResultType Write(StringBorrow const& value, PointType const& point) noexcept
    {
        if (point.msg != point_.msg) {
            return ErrorCode::kInvalidPoint;  // Write position invalid
        }
        size_t const kSize{value.Size()};
        if (kSize == 0) {
            return static_cast< ResultType >(kSize);
        }
        auto* data{value.Data()};
        if (npc_message_overwrite(const_cast< PointType* >(&point), data, kSize) != kSize) {
            return ErrorCode::kInvalidWrite;  // Write exceeds payload capacity limit (string)
        }
        return static_cast< ResultType >(kSize);
    }
    /// @brief Get current position
    /// @return Current position
    inline PointType const& Point() const noexcept { return point_; }

private:
    /// @brief Payload reference
    PayloadType& payload_;
    /// @brief Current position
    PointType point_{};
};
/// @brief Template type -- payload stream reader -- specialization for custom message payload type
/// @tparam TProps Property information
template < typename TProps >
class StreamReader< TProps, npc_message_t >
{
public:
    /// @brief Type alias -- property information
    using PropsType = TProps;
    /// @brief Type alias -- payload type
    using PayloadType = npc_message_t const;
    /// @brief Type alias -- result type
    using ResultType = int32_t;
    /// @brief Type alias -- position type
    using PointType = npc_message_io_t;
    /// @brief Constructor
    /// @param[in] payload Payload
    explicit StreamReader(PayloadType& payload) noexcept : payload_{payload}
    {
        std::ignore = npc_message_read_start(&point_, const_cast< npc_message_t* >(&payload_), 0);
    }
    /// @brief Destructor
    ~StreamReader() noexcept { std::ignore = npc_message_read_end(&point_); }
    /// @brief Copy constructor
    /// @param other
    StreamReader(StreamReader const& other) noexcept = default;
    /// @brief Move constructor
    /// @param other
    StreamReader(StreamReader&& other) noexcept = default;
    /// @brief Copy assignment operator
    /// @param other
    /// @return StreamReader
    StreamReader& operator=(StreamReader const& other) noexcept = default;
    /// @brief Move assignment operator
    /// @param other
    /// @return StreamReader
    StreamReader& operator=(StreamReader&& other) noexcept = default;
    /// @brief Read
    /// @tparam TValue Value type
    /// @param[in] value Value
    /// @param[in] reverseOrder Whether to convert byte order
    /// @return Result -- >= 0: number of bytes read; < 0: error code;
    template < typename TValue >
    inline ResultType Read(TValue& value, bool reverseOrder) noexcept
    {
        static_assert(IsScalar< TValue >::value, "invalid value");
        constexpr size_t kSize{sizeof(value)};
        auto* data{reinterpret_cast< uint8_t* >(&value)};
        if (static_cast< size_t >(npc_message_read(&point_, static_cast< void* >(data), kSize)) != kSize) {
            return ErrorCode::kInvalidRead;
        }
        if (reverseOrder && kSize != 1) {
            std::reverse(data, data + kSize);
        }
        return static_cast< ResultType >(kSize);
    }
    /// @brief Read
    /// @param[in] value Value
    /// @return Result -- >= 0: number of bytes read; < 0: error code;
    inline ResultType Read(StringBorrow const& value) noexcept
    {
        size_t const kSize{value.Size()};
        if (kSize == 0) {
            return static_cast< ResultType >(kSize);
        }
        auto* data{value.Data()};
        if (static_cast< size_t >(npc_message_read(&point_, static_cast< void* >(data), kSize)) != kSize) {
            return ErrorCode::kInvalidRead;
        }
        return static_cast< ResultType >(kSize);
    }
    /// @brief Read
    /// @tparam TValue Value type
    /// @param[in] value Value
    /// @param[in] reverseOrder Whether to convert byte order
    /// @param[in] point Position
    /// @return Result -- >= 0: number of bytes read; < 0: error code;
    template < typename TValue >
    inline ResultType Read(TValue& value, bool reverseOrder, PointType const& point) const noexcept
    {
        static_assert(IsScalar< TValue >::value, "invalid value");
        if (point.msg != point_.msg) {
            return ErrorCode::kInvalidPoint;  // Read position invalid
        }
        constexpr size_t kSize{sizeof(value)};
        auto* data{reinterpret_cast< uint8_t* >(&value)};
        if (static_cast< size_t >(
                npc_message_read(const_cast< npc_message_io_t* >(&point), static_cast< void* >(data), kSize))
            != kSize) {
            return ErrorCode::kInvalidRead;
        }
        if (reverseOrder && kSize != 1) {
            std::reverse(data, data + kSize);
        }
        return static_cast< ResultType >(kSize);
    }
    /// @brief Read
    /// @param[in] value Value
    /// @param[in] point Position
    /// @return Result -- >= 0: number of bytes read; < 0: error code;
    inline ResultType Read(StringBorrow const& value, PointType const& point) const noexcept
    {
        if (point.msg != point_.msg) {
            return ErrorCode::kInvalidPoint;  // Read position invalid
        }
        size_t const kSize{value.Size()};
        if (kSize == 0) {
            return static_cast< ResultType >(kSize);
        }
        auto* data{value.Data()};
        if (static_cast< size_t >(
                npc_message_read(const_cast< npc_message_io_t* >(&point), static_cast< void* >(data), kSize))
            != kSize) {
            return ErrorCode::kInvalidRead;
        }
        return static_cast< ResultType >(kSize);
    }
    /// @brief Get current position
    /// @return Current position
    inline PointType const& Point() const noexcept { return point_; }

private:
    /// @brief Payload reference
    PayloadType& payload_;
    /// @brief Current position
    PointType point_{};
};
/// @brief E2E message payload type
class E2EPayload
{
public:
    /// @brief Constructor
    /// @param[in] message Message
    /// @param[in] dataId Data identifier
    E2EPayload(npc_message_t const& message, ara::com::e2exf::IdlistType dataId) noexcept
        : m{const_cast< npc_message_t& >(message)}, id{std::move(dataId)}
    {
    }
    /// @brief Message
    npc_message_t& m;
    /// @brief Variable-length array
    ara::core::Vector< uint8_t > v{};
    /// @brief Data identifier
    ara::com::e2exf::IdlistType id{};
    /// @brief E2E check result
    ara::com::e2exf::E2EResult r{{}};
};
/// @brief Template type -- payload stream writer -- specialization for E2E message payload type
/// @tparam TProps Property information
/// @tparam E2EPayload E2E message payload type
template < typename TProps >
class StreamWriter< TProps, E2EPayload >
{
public:
    /// @brief Type alias -- property information
    using PropsType = TProps;
    /// @brief Type alias -- payload type
    using PayloadType = E2EPayload;
    /// @brief Type alias -- result type
    using ResultType = int32_t;
    /// @brief Position type
    struct Point
    {
        /// @brief Message position
        npc_message_io_t m{};
        /// @brief Array position
        size_t v{};
        /// @brief Array offset (relative to E2E local info)
        uint16_t offset{};
    };
    /// @brief Type alias -- position type
    using PointType = Point;
    /// @brief Type alias -- E2E converter type
    using Transformer = ara::com::e2exf::Transformer;
    /// @brief Type alias -- E2E check handle
    using StatusHandler = ara::com::e2exf::StatusHandler;
    /// @brief Type alias -- E2E configuration source identifier type
    using SourceidType = ara::com::profile::SourceidType;
    /// @brief Constructor
    /// @param[in] payload Payload
    /// @param[in] capacity Payload capacity -- pre-allocation
    explicit StreamWriter(PayloadType& payload, size_t capacity) noexcept : payload_{payload}
    {
        // Add E2E local info
        auto reverseOrder{(PropsType::byteOrder() == ByteOrder::kMostSignificantByteFirst && IsLittleEndian())
                          || (PropsType::byteOrder() == ByteOrder::kMostSignificantByteLast && !IsLittleEndian())};
        if (Message::IsEvent(payload_.m.hdr.type)) {
            point_.offset += Write(decltype(payload_.m.hdr.client){}, reverseOrder);
        } else {
            point_.offset += Write(payload_.m.hdr.client, reverseOrder);
        }
        point_.offset += Write(payload_.m.hdr.session, reverseOrder);
        point_.offset += Write(payload_.m.hdr.protocol, reverseOrder);
        point_.offset += Write(payload_.m.hdr.interface, reverseOrder);
        point_.offset += Write(payload_.m.hdr.type, reverseOrder);
        point_.offset += Write(payload_.m.hdr.code, reverseOrder);
        // Reserve E2E header
        auto e2eHeaderSize{Transformer::E2E_GetHeaderLength(payload_.id)};
        payload_.v.reserve(point_.v + e2eHeaderSize + capacity);
        payload_.v.resize(point_.v + e2eHeaderSize);
        point_.v += e2eHeaderSize;
    }
    /// @brief Destructor
    ~StreamWriter() noexcept
    {
        // Update E2E header
        ara::core::Result< void > result;
        if (Message::IsRequest(payload_.m.hdr.type)
            || Message::IsRequestNoReturn(payload_.m.hdr.type)) {  // proxy method request use
            auto sourceId{StatusHandler::GetSourceId(payload_.m.hdr.serv, payload_.m.inst, payload_.m.hdr.method)};
            result = Transformer::E2E_protect(payload_.id, profile::MessageType::kMessageTypeRequest,
                                              profile::MessageResult::kMessageResultOk, sourceId, payload_.v);
        } else if (Message::IsResponse(payload_.m.hdr.type)) {  // skeleton method response use
            auto resultSourceId{Transformer::E2E_ClientIdToSourceId(payload_.m.hdr.client, payload_.m.hdr.method)};
            if (resultSourceId) {
                auto sourceId{resultSourceId.Value()};
                result = Transformer::E2E_protect(payload_.id, profile::MessageType::kMessageTypeResponse,
                                                  profile::MessageResult::kMessageResultOk, sourceId, payload_.v);
            } else {
                ComLogError(resultSourceId);
            }
        } else if (Message::IsError(payload_.m.hdr.type)) {  // skeleton method response use
            auto resultSourceId{Transformer::E2E_ClientIdToSourceId(payload_.m.hdr.client, payload_.m.hdr.method)};
            if (resultSourceId) {
                auto sourceId{resultSourceId.Value()};
                result = Transformer::E2E_protect(payload_.id, profile::MessageType::kMessageTypeResponse,
                                                  profile::MessageResult::kMessageResultError, sourceId, payload_.v);
            } else {
                ComLogError("");
            }
        } else if (Message::IsEvent(payload_.m.hdr.type)) {  // skeleton event use
            result = Transformer::E2E_protect(payload_.id, payload_.v);
        }
        if (!result) {
            ComLogError("serialize payload error: protect E2E failed", GenK2V("DataID", payload_.id),
                        GenK2V("Error", result));
            payload_.r.EmplaceError(result.Error());
        }
        std::ignore = npc_message_write_start(&point_.m, &payload_.m);
        auto data{payload_.v.data() + point_.offset};
        auto kSize{payload_.v.size() - point_.offset};
        if (static_cast< size_t >(npc_message_write(&point_.m, data, kSize)) != kSize) {
            ComLogError("");  // Write exceeds payload capacity limit
        }
        std::ignore = npc_message_write_end(&point_.m);
    }
    /// @brief Copy constructor
    /// @param other
    StreamWriter(StreamWriter const& other) noexcept = default;
    /// @brief Move constructor
    /// @param other
    StreamWriter(StreamWriter&& other) noexcept = default;
    /// @brief Copy assignment operator
    /// @param other
    /// @return StreamWriter
    StreamWriter& operator=(StreamWriter const& other) noexcept = default;
    /// @brief Move assignment operator
    /// @param other
    /// @return StreamWriter
    StreamWriter& operator=(StreamWriter&& other) noexcept = default;
    /// @brief Write
    /// @tparam TValue Value type
    /// @param[in] value Value
    /// @param[in] reverseOrder Whether to convert byte order
    /// @return Result -- >= 0: number of bytes written; < 0: error code;
    template < typename TValue >
    inline ResultType Write(TValue const& value, bool reverseOrder) noexcept
    {
        static_assert(IsScalar< TValue >::value, "invalid value");
        constexpr size_t kSize{sizeof(value)};
        if (point_.v + kSize > payload_.v.size()) {
            payload_.v.resize(point_.v + kSize);
        }
        auto const* data1{reinterpret_cast< uint8_t const* >(&value)};
        auto* data2{payload_.v.data() + point_.v};
        if (reverseOrder) {
            std::ignore = std::reverse_copy(data1, data1 + kSize, data2);
        } else {
            std::ignore = std::copy(data1, data1 + kSize, data2);
        }
        point_.v += kSize;
        return static_cast< ResultType >(kSize);
    }
    /// @brief Write
    /// @param[in] value Value
    /// @return Result -- >= 0: number of bytes written; < 0: error code;
    inline ResultType Write(StringBorrow const& value) noexcept
    {
        size_t const kSize{value.Size()};
        if (point_.v + kSize > payload_.v.size()) {
            payload_.v.resize(point_.v + kSize);
        }
        auto* data1{value.Data()};
        auto* data2{payload_.v.data() + point_.v};
        std::ignore = std::copy(data1, data1 + kSize, data2);
        point_.v += kSize;
        return static_cast< ResultType >(kSize);
    }
    /// @brief Write
    /// @tparam TValue Value type
    /// @param[in] value Value
    /// @param[in] reverseOrder Whether to convert byte order
    /// @param[in] point Position
    /// @return Result -- >= 0: number of bytes written; < 0: error code;
    template < typename TValue >
    inline ResultType Write(TValue const& value, bool reverseOrder, PointType const& point) noexcept
    {
        static_assert(IsScalar< TValue >::value, "invalid value");
        if (point.m.msg != point_.m.msg) {
            return ErrorCode::kInvalidPoint;  // Write position invalid
        }
        constexpr size_t kSize{sizeof(value)};
        if (point.v + kSize > payload_.v.size()) {
            return ErrorCode::kInvalidWrite;  // Write exceeds payload capacity limit (value)
        }
        auto const* data1{reinterpret_cast< uint8_t const* >(&value)};
        auto* data2{payload_.v.data() + point.v};
        if (reverseOrder) {
            std::ignore = std::reverse_copy(data1, data1 + kSize, data2);
        } else {
            std::ignore = std::copy(data1, data1 + kSize, data2);
        }
        return static_cast< ResultType >(kSize);
    }
    /// @brief Write
    /// @param[in] value Value
    /// @param[in] point Position
    /// @return Result -- >= 0: number of bytes written; < 0: error code;
    inline ResultType Write(StringBorrow const& value, PointType const& point) noexcept
    {
        if (point.m.msg != point_.m.msg) {
            return ErrorCode::kInvalidPoint;  // Write position invalid
        }
        size_t const kSize{value.Size()};
        if (point.v + kSize > payload_.v.size()) {
            return ErrorCode::kInvalidWrite;  // Write exceeds payload capacity limit (string)
        }
        auto* data1{value.Data()};
        auto* data2{payload_.v.data() + point.v};
        std::ignore = std::copy(data1, data1 + kSize, data2);
        return static_cast< ResultType >(kSize);
    }
    /// @brief Get current position
    /// @return Current position
    inline PointType const& Point() const noexcept { return point_; }

private:
    /// @brief Payload reference
    PayloadType& payload_;
    /// @brief Current position
    PointType point_{};
};
/// @brief Template type -- payload stream reader -- specialization for E2E message payload type
/// @tparam TProps Property information
/// @tparam E2EPayload E2E message payload type
template < typename TProps >
class StreamReader< TProps, E2EPayload >
{
public:
    /// @brief Type alias -- property information
    using PropsType = TProps;
    /// @brief Type alias -- payload type
    using PayloadType = E2EPayload;
    /// @brief Type alias -- result type
    using ResultType = int32_t;
    /// @brief Position type
    struct Point
    {
        /// @brief Message position
        npc_message_io_t m{};
        /// @brief Array position
        size_t v{};
    };
    /// @brief Type alias -- position type
    using PointType = Point;
    /// @brief Type alias -- E2E converter type
    using Transformer = ara::com::e2exf::Transformer;
    /// @brief Type alias -- E2E check handle
    using StatusHandler = ara::com::e2exf::StatusHandler;
    /// @brief Type alias -- E2E configuration source identifier type
    using SourceidType = ara::com::profile::SourceidType;
    /// @brief Constructor
    /// @param[in] payload Payload
    explicit StreamReader(PayloadType const& payload) noexcept : payload_{const_cast< PayloadType& >(payload)}
    {
        // Add E2E local info
        auto reverseOrder{(PropsType::byteOrder() == ByteOrder::kMostSignificantByteFirst && IsLittleEndian())
                          || (PropsType::byteOrder() == ByteOrder::kMostSignificantByteLast && !IsLittleEndian())};
        if (Message::IsEvent(payload_.m.hdr.type)) {
            std::ignore = Write(decltype(payload_.m.hdr.client){}, reverseOrder);
        } else {
            std::ignore = Write(payload_.m.hdr.client, reverseOrder);
        }
        std::ignore = Write(payload_.m.hdr.session, reverseOrder);
        std::ignore = Write(payload_.m.hdr.protocol, reverseOrder);
        std::ignore = Write(payload_.m.hdr.interface, reverseOrder);
        std::ignore = Write(payload_.m.hdr.type, reverseOrder);
        std::ignore = Write(payload_.m.hdr.code, reverseOrder);
        // Reserve E2E header
        auto e2eHeaderSize{Transformer::E2E_GetHeaderLength(payload_.id)};
        payload_.v.resize(point_.v + e2eHeaderSize);
        // Read E2E header
        std::ignore = npc_message_read_start(&point_.m, &payload_.m, 0);
        auto data{payload_.v.data() + point_.v};
        auto kSize{payload_.v.size() - point_.v};
        if (static_cast< size_t >(npc_message_read(&point_.m, data, kSize)) != kSize) {
            ComLogError("");  // Read exceeds payload capacity limit
        }
        point_.v += e2eHeaderSize;
    }
    /// @brief Destructor
    ~StreamReader() noexcept
    {
        std::ignore = npc_message_read_end(&point_.m);
        if (Message::IsRequest(payload_.m.hdr.type)
            || Message::IsRequestNoReturn(payload_.m.hdr.type)) {  // skeleton method request use
            SourceidType sourceId{};
            payload_.r = Transformer::E2E_check(payload_.id, payload_.m.hdr.client, payload_.m.hdr.method,
                                                profile::MessageType::kMessageTypeRequest,
                                                profile::MessageResult::kMessageResultOk, &sourceId, payload_.v);
            if (payload_.r) {
                Transformer::E2E_InsertClientSourceIdMap(payload_.m.hdr.client, payload_.m.hdr.method, sourceId);
            }
        } else if (Message::IsResponse(payload_.m.hdr.type)) {  // proxy method response use
            auto sourceId{StatusHandler::GetSourceId(payload_.m.hdr.serv, payload_.m.inst, payload_.m.hdr.method)};
            payload_.r = Transformer::E2E_check(payload_.id, profile::MessageType::kMessageTypeResponse,
                                                profile::MessageResult::kMessageResultOk, sourceId, payload_.v);
        } else if (Message::IsError(payload_.m.hdr.type)) {  // proxy method response use
            auto sourceId{StatusHandler::GetSourceId(payload_.m.hdr.serv, payload_.m.inst, payload_.m.hdr.method)};
            payload_.r = Transformer::E2E_check(payload_.id, profile::MessageType::kMessageTypeResponse,
                                                profile::MessageResult::kMessageResultError, sourceId, payload_.v);
        } else if (Message::IsEvent(payload_.m.hdr.type)) {  // proxy event use
            payload_.r = Transformer::E2E_check(payload_.id, payload_.v);
        }
    }
    /// @brief Copy constructor
    /// @param other
    StreamReader(StreamReader const& other) noexcept = default;
    /// @brief Move constructor
    /// @param other
    StreamReader(StreamReader&& other) noexcept = default;
    /// @brief Copy assignment operator
    /// @param other
    /// @return StreamReader
    StreamReader& operator=(StreamReader const& other) noexcept = default;
    /// @brief Move assignment operator
    /// @param other
    /// @return StreamReader
    StreamReader& operator=(StreamReader&& other) noexcept = default;
    /// @brief Write
    /// @tparam TValue
    /// @param[in] value Value
    /// @param[in] reverseOrder Whether to convert byte order
    /// @return Result -- >= 0: number of bytes written; < 0: error code;
    template < typename TValue >
    inline ResultType Write(TValue const& value, bool reverseOrder) noexcept
    {
        static_assert(IsScalar< TValue >::value, "invalid value");
        constexpr size_t kSize{sizeof(value)};
        if (point_.v + kSize > payload_.v.size()) {
            payload_.v.resize(point_.v + kSize);
        }
        auto const* data1{reinterpret_cast< uint8_t const* >(&value)};
        auto* data2{payload_.v.data() + point_.v};
        if (reverseOrder) {
            std::ignore = std::reverse_copy(data1, data1 + kSize, data2);
        } else {
            std::ignore = std::copy(data1, data1 + kSize, data2);
        }
        point_.v += kSize;
        return static_cast< ResultType >(kSize);
    }
    /// @brief Read
    /// @tparam TValue
    /// @param[in] value Value
    /// @param[in] reverseOrder Whether to convert byte order
    /// @return Result -- >= 0: number of bytes read; < 0: error code;
    template < typename TValue >
    inline ResultType Read(TValue& value, bool reverseOrder) noexcept
    {
        static_assert(IsScalar< TValue >::value, "invalid value");
        constexpr size_t kSize{sizeof(value)};
        auto* data{reinterpret_cast< uint8_t* >(&value)};
        if (static_cast< size_t >(npc_message_read(&point_.m, data, kSize)) != kSize) {
            return ErrorCode::kInvalidRead;
        }
        if (point_.v + kSize > payload_.v.size()) {
            payload_.v.resize(point_.v + kSize);
        }
        std::ignore = std::copy(data, data + kSize, &payload_.v[point_.v]);
        if (reverseOrder && kSize != 1) {
            std::reverse(data, data + kSize);
        }
        point_.v += kSize;
        return static_cast< ResultType >(kSize);
    }
    /// @brief Read
    /// @param[in] value Value
    /// @return Result -- >= 0: number of bytes read; < 0: error code;
    inline ResultType Read(StringBorrow const& value) noexcept
    {
        size_t const kSize{value.Size()};
        if (kSize == 0) {
            return static_cast< ResultType >(kSize);
        }
        auto* data{value.Data()};
        if (static_cast< size_t >(npc_message_read(&point_.m, data, kSize)) != kSize) {
            return ErrorCode::kInvalidRead;
        }
        if (point_.v + kSize > payload_.v.size()) {
            payload_.v.resize(point_.v + kSize);
        }
        std::ignore = std::copy(data, data + kSize, &payload_.v[point_.v]);
        point_.v += kSize;
        return static_cast< ResultType >(kSize);
    }
    /// @brief Read
    /// @tparam TValue
    /// @param[in] value Value
    /// @param[in] reverseOrder Whether to convert byte order
    /// @param[in] point Position
    /// @return Result -- >= 0: number of bytes read; < 0: error code;
    template < typename TValue >
    inline ResultType Read(TValue& value, bool reverseOrder, PointType const& point) const noexcept
    {
        static_assert(IsScalar< TValue >::value, "invalid value");
        if (&point.m != &point_.m) {
            return ErrorCode::kInvalidPoint;  // Read position invalid
        }
        constexpr size_t kSize{sizeof(value)};
        auto* data{reinterpret_cast< uint8_t* >(&value)};
        if (static_cast< size_t >(
                npc_message_read(const_cast< npc_message_io_t* >(&point.m), static_cast< void* >(data), kSize))
            != kSize) {
            return ErrorCode::kInvalidRead;
        }
        if (point.v + kSize > payload_.v.size()) {
            return ErrorCode::kInvalidWrite;  // Write exceeds payload capacity limit (value)
        }
        std::ignore = std::copy(data, data + kSize, &payload_.v[point.v]);
        if (reverseOrder && kSize != 1) {
            std::reverse(data, data + kSize);
        }
        return static_cast< ResultType >(kSize);
    }
    /// @brief Read
    /// @param[in] value Value
    /// @param[in] point Position
    /// @return Result -- >= 0: number of bytes read; < 0: error code;
    inline ResultType Read(StringBorrow const& value, PointType const& point) const noexcept
    {
        if (&point.m != &point_.m) {
            return ErrorCode::kInvalidPoint;  // Read position invalid
        }
        size_t const kSize{value.Size()};
        if (kSize == 0) {
            return static_cast< ResultType >(kSize);
        }
        auto* data{value.Data()};
        if (static_cast< size_t >(
                npc_message_read(const_cast< npc_message_io_t* >(&point.m), static_cast< void* >(data), kSize))
            != kSize) {
            return ErrorCode::kInvalidRead;
        }
        if (point.v + kSize > payload_.v.size()) {
            return ErrorCode::kInvalidWrite;  // Write exceeds payload capacity limit (value)
        }
        std::ignore = std::copy(data, data + kSize, &payload_.v[point.v]);
        return static_cast< ResultType >(kSize);
    }
    /// @brief Get current position
    /// @return Current position
    inline PointType const& Point() const noexcept { return point_; }

private:
    /// @brief Payload reference
    PayloadType& payload_;
    /// @brief Current position
    PointType point_{};
};
}  // namespace serialize
}  // namespace npc
}  // namespace internal
}  // namespace com
}  // namespace ara
#endif
