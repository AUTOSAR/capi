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
/// @brief      This file provides serialize functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/isoft/uds/common
/// @interface_level=software
/// @trace_id_sr=2c8430a7-0d8d-45e9-b958-2117d7d92830
/// @unit_name=serialize
/// @unit_description=This file provides serialize functionality
/// @endcode
///
/// ================================================================

#ifndef ISOFT_UDS_SERVER_SERIALIZE_H_
#define ISOFT_UDS_SERVER_SERIALIZE_H_

#include <ara/core/string.h>
#include <isoft/serialize/serialize.h>

#include <cstddef>
#include <memory>

#include "isoft/uds/type_int.h"
#include "log.h"

namespace isoft {
namespace uds {
namespace server {

/// @brief Serialization class
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = dd
/// @endcode
class Serialize
{
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00824
    /// @needwork = dda
    /// @endcode

    /// @brief Raw data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    std::vector< std::uint8_t >& data_;
    /// @brief Read position
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    std::size_t position_;

public:
    /// @brief Constructor
    /// @param[in] data Raw data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    explicit Serialize(std::vector< std::uint8_t >& data) noexcept : data_{data}, position_{0U} {}

    /// @brief Read uint8_t
    /// @return Value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    std::uint8_t ReadUint8() noexcept
    {
        std::uint8_t value{0U};
        _read(position_, value, 1U);
        return value;
    }

    /// @brief Read uint8_t
    /// @return Value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    bool Read(std::uint8_t& value) noexcept
    {
        std::size_t result = _read(position_, value, 1U);
        return result == 1U;
    }

    /// @brief Read uint16_t
    /// @return Value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    std::uint16_t ReadUint16() noexcept
    {
        std::uint16_t value{0U};
        _read(position_, value, 2U);
        return value;
    }

    /// @brief Read all data
    /// @return Value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    std::vector< std::uint8_t > ReadAll() noexcept
    {
        std::vector< std::uint8_t > result{};
        std::ignore = result.insert(result.begin(), data_.begin() + static_cast< int32_t >(position_), data_.end());
        return result;
    }

    /// @brief Read uint8_t of specified length
    /// @return Value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    std::vector< std::uint8_t > ReadVector(std::size_t length) noexcept
    {
        std::vector< std::uint8_t > result{};
        if (length + position_ > Size()) {
            std::ignore = result.insert(result.begin(), data_.begin() + static_cast< int32_t >(position_), data_.end());
        } else {
            std::size_t const nextPosition{position_ + length};
            std::ignore = result.insert(result.begin(), data_.begin() + static_cast< int32_t >(position_),
                                        data_.begin() + static_cast< int32_t >(nextPosition));
            position_   = nextPosition;
        }

        return result;
    }

    /// @brief Move position
    /// @param[in] pos Position
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    void Seek(std::uint32_t pos) noexcept { position_ = pos; }

    /// @brief Get current position
    /// @return Size
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    std::size_t Tell() const noexcept { return position_; }

    /// @brief Get size of raw data
    /// @return Size
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    std::size_t Size() noexcept { return data_.size(); }

    /// @brief Read uint16_t
    /// @return Value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    bool Read(std::uint16_t& value) noexcept
    {
        std::size_t result = _read(position_, value, 2U);
        return result == 2U;
    }

    /// @brief Read uint32_t
    /// @return Value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    std::uint32_t ReadUint32(std::size_t const length = 4) noexcept
    {
        std::uint32_t value{0U};
        _read(position_, value, length);
        return value;
    }

    /// @brief Read uint32_t
    /// @param[in] value Read data
    /// @return Value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    bool Read(std::uint32_t& value) noexcept
    {
        std::size_t result = _read(position_, value, 4U);
        return result == 4U;
    }

    /// @brief Read uint64_t
    /// @return Value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    std::uint64_t ReadUint64(std::size_t const length = 8) noexcept
    {
        std::uint64_t value{0U};
        _read(position_, value, length);
        return value;
    }

    /// @brief Read uint64_t
    /// @param[in] value Read data
    /// @return Value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    bool Read(std::uint64_t& value) noexcept
    {
        std::size_t result = _read(position_, value, kInt32_8U);
        return result == kInt32_8U;
    }


    /// @brief Read data of specified length
    /// @param[in] length Length of message to read
    /// @return Value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    std::uint64_t ReadUint(std::size_t const length) noexcept
    {
        if (length == sizeof(std::uint8_t)) {
            return static_cast< std::uint64_t >(ReadUint8());
        }
        if (length == sizeof(std::uint16_t)) {
            return static_cast< std::uint64_t >(ReadUint16());
        }
        if (length <= sizeof(std::uint32_t)) {
            return static_cast< std::uint64_t >(ReadUint32(length));
        }
        if (length <= sizeof(std::uint64_t)) {
            return static_cast< std::uint64_t >(ReadUint64(length));
        }
        return 0U;
    }

    /// @brief Read string
    /// @param[in] length String length
    /// @return Value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    std::string ReadString(std::uint16_t const length) noexcept
    {
        std::string value;
        std::uint32_t maxLength{length + 1U};
        std::ignore = value.append(static_cast< std::size_t >(maxLength), '\n');
        isoft::serialize::StringBorrow borrow{value.data(), static_cast< std::size_t >(length)};
        isoft::serialize::Buffer< ara::core::Vector< uint8_t > > buffer{data_};
        std::ignore = isoft::serialize::DeserializePart(buffer, position_, borrow);
        value.pop_back();
        return value;
    }


    /// @brief Write uint8_t
    /// @param value Parameter
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    void Write(std::uint8_t value) noexcept
    {
        std::size_t result{0U};
        result = _write< std::uint8_t >(value);
        if (result == sizeof(value)) {
            LogVerbose() << "value:" << value << " write success!";
        } else {
            LogVerbose() << "value:" << value << " write fails!";
        }
    }

    /// @brief Write function
    /// @param value Parameter
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    void Write(std::uint16_t value) noexcept
    {
        std::size_t result{0U};
        result = _write< std::uint16_t >(value);
        if (result == sizeof(value)) {
            LogVerbose() << "value:" << value << " write success!";
        } else {
            LogVerbose() << "value:" << value << " write fails!";
        }
    }

    /// @brief Write function
    /// @param value Parameter
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    void Write(std::uint32_t value) noexcept
    {
        std::size_t result{0U};
        result = _write< std::uint32_t >(value);
        if (result == sizeof(value)) {
            LogVerbose() << "value:" << value << " write success!";
        } else {
            LogVerbose() << "value:" << value << " write fails!";
        }
    }

    /// @brief Write function
    /// @param value Parameter
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    void Write(std::uint64_t value) noexcept
    {
        std::size_t result{0U};
        result = _write< std::uint64_t >(value);
        if (result == sizeof(value)) {
            LogVerbose() << "value:" << value << " write success!";
        } else {
            LogVerbose() << "value:" << value << " write fails!";
        }
    }

    /// @brief Write array
    /// @param value Parameter
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    void Write(std::vector< std::uint8_t > const& value) noexcept
    {
        std::size_t result{0U};
        result = _write< std::uint8_t >(value);
        if (result == sizeof(value)) {
            LogVerbose() << "value:" << value << " write success!";
        } else {
            LogVerbose() << "value:" << value << " write fails!";
        }
    }

private:
    /// @brief Read data and transfer to TValue type
    /// @param[in] point Position in raw data
    /// @param[in] value Value
    /// @return Length that has been read
    /// @code{.isoft}
    /// @tparam TValue Type of value
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    template < typename TValue >
    std::size_t _read(std::size_t& point, TValue& value, std::size_t const length = 0) noexcept
    {
        if (data_.size() - point < length) {
            return 0U;
        }
        auto* data  = reinterpret_cast< uint8_t* >(&value);
        std::ignore = std::reverse_copy(data_.data() + point, data_.data() + point + length, data);
        if (length > 0 && length < sizeof(value)) {
            TValue mask{0U};
            mask  = ~0;
            mask  = mask >> (sizeof(value) - length) * sizeof(std::uint8_t);
            value = value & mask;
            point += length;
        } else {
            point += sizeof(value);
        }

        return sizeof(value);
    }
    /// @brief Write array
    /// @param value Parameter
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    template < typename TValue >
    std::size_t _write(const TValue& value)
    {
        auto point = data_.size();
        data_.resize(point + sizeof(value));
        auto const* data = reinterpret_cast< const uint8_t* >(&value);
        std::ignore      = std::reverse_copy(data, data + sizeof(value), &data_[point]);


        return sizeof(value);
    }

    /// @brief Write array
    /// @param value Parameter
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    template < typename TValue >
    std::size_t _write(const std::vector< TValue >& value)
    {
        auto point = data_.size();
        data_.resize(point + value.size() * sizeof(TValue));
        auto const* data = reinterpret_cast< const uint8_t* >(value.data());
        std::ignore      = std::copy(data, data + value.size() * sizeof(TValue), &data_[point]);


        return sizeof(value);
    }
};

}  // namespace server
}  // namespace uds
}  // namespace isoft
#endif  // ISOFT_UDS_SERVER_SERIALIZE_H_