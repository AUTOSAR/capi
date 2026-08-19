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
/// @file       userdata.h
/// @brief      user data definition
/// @details
/// @date       2023-01-31
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_TSYNC_INTERNAL_USERDATA_H_
#define ARA_TSYNC_INTERNAL_USERDATA_H_

#include <ara/core/span.h>
#include <ara/core/string.h>
#include <ara/core/string_view.h>
#include <ara/core/utility.h>
#include <ara/core/vector.h>

#include <cstdint>
#include <cstring>

#include "ara/tsync/internal/common.h"
#include "isoft/util/type_cast.h"

namespace ara {
namespace tsync {
namespace internal {

/// @brief user data
class UserData final
{
public:
    /// @brief default constructor
    UserData() noexcept = default;

    /// @brief default destructor
    ~UserData() = default;

    /// @brief default copy constructor
    /// @param other
    UserData(UserData const &other) = default;

    /// @brief default move constructor
    /// @param other
    UserData(UserData &&other) = default;

    /// @brief default move assignment
    /// @param other - other object
    /// @return reference to self
    UserData &operator=(UserData &&other) = default;

    /// @brief default copy assignment
    /// @param other - other object
    /// @return reference to self
    UserData &operator=(UserData const &other) = default;

    /// @brief get maximum length
    /// @return length
    static std::uint8_t GetMaxLen() noexcept { return kMaxLen; }

    /// @brief construct user data from characters
    /// @param str - string
    void FromString(ara::core::StringView const &str) noexcept
    {
        if (str.size() <= kMaxLen) {
            len_ = static_cast< std::uint8_t >(str.size());
        } else {
            len_ = kMaxLen;
        }

        /// FIXME: There is a hidden danger here; the entire line may be optimized away by the compiler, causing assignment failure.
        std::ignore = std::memmove(data_.data(), str.data(), static_cast< std::size_t >(len_));
    }

    /// @brief export user data to string
    /// @return string
    ara::core::String ToString() const noexcept
    {
        ara::core::String const ret{
            isoft::util::PointerCast< ara::tsync::internal::char8_t const, std::uint8_t >(data_.data()),
            static_cast< std::size_t >(len_)};
        return ret;
    }

    /// @brief construct user data from vector
    /// @param vd - vector
    void FromVector(ara::core::Vector< std::uint8_t > const &vd) noexcept
    {
        if (vd.size() <= kMaxLen) {
            /// FIXME: There is a hidden danger here; the entire line may be optimized away by the compiler, causing assignment failure.
            std::ignore = std::memmove(data_.data(), vd.data(), vd.size());
            len_        = static_cast< std::uint8_t >(vd.size());
        }
    }

    /// @brief export to vector
    /// @return vector
    ara::core::Vector< std::uint8_t > ToVector() const noexcept
    {
        ara::core::Vector< std::uint8_t > ret;
        std::size_t const len{len_};
        ret.resize(len);
        if (len <= kMaxLen) {
            /// FIXME: There is a hidden danger here; the entire line may be optimized away by the compiler, causing assignment failure.
            std::ignore = std::memmove(ret.data(), data_.data(), len);
        }
        return ret;
    }

    /// @brief import from ByteSpan
    /// @param sd - user data imported from span
    void FromSpan(ara::core::Span< std::uint8_t const > const &sd) noexcept
    {
        if (sd.size() <= kMaxLen) {
            /// FIXME: There is a hidden danger here; the entire line may be optimized away by the compiler, causing assignment failure.
            std::ignore = std::memmove(data_.data(), sd.data(), sd.size());
            len_        = static_cast< std::uint8_t >(sd.size());
        }
    }

    /// @brief export as Span<std::uint8_t>
    /// @return exported user data
    ara::core::Span< std::uint8_t const > ToSpan() const noexcept
    {
        return ara::core::MakeSpan(isoft::util::PointerCast< std::uint8_t const, std::uint8_t >(data_.data()),
                                   static_cast< std::size_t >(len_));
    }

    /// @brief import from ByteSpan
    /// @param bsd - imported user data
    void FromByteSpan(ara::core::Span< ara::core::Byte const > const &bsd) noexcept
    {
        if (bsd.size() <= kMaxLen) {
            /// FIXME: There is a hidden danger here; the entire line may be optimized away by the compiler, causing assignment failure.
            std::ignore = std::memmove(data_.data(), bsd.data(), bsd.size());
            len_        = static_cast< std::uint8_t >(bsd.size());
        }
    }

    /// @brief export as ByteSpan
    /// @return exported user data
    ara::core::Span< ara::core::Byte const > ToByteSpan() const noexcept
    {
        return ara::core::MakeSpan(isoft::util::PointerCast< ara::core::Byte const, std::uint8_t >(data_.data()),
                                   static_cast< std::size_t >(len_));
    }

    /// @brief operator overload, comparison function
    /// @param lhs - object reference
    /// @param rhs - object reference
    /// @return true - equal
    /// @return false - not equal
    friend bool operator==(UserData const &lhs, UserData const &rhs) noexcept
    {
        if (lhs.len_ != rhs.len_) {
            return false;
        }
        if (0 != std::memcmp(lhs.data_.data(), lhs.data_.data(), static_cast< std::size_t >(lhs.len_))) {
            return false;
        }
        return true;
    }

    /// @brief operator overload, comparison function
    /// @param lhs - object reference
    /// @param rhs - object reference
    /// @return false - equal
    /// @return true - not equal
    friend bool operator!=(UserData const &lhs, UserData const &rhs) noexcept { return !(lhs == rhs); }

private:
    /// @brief maximum length
    static std::uint8_t const kMaxLen = static_cast< std::uint8_t >(3);

    /// @brief user data length
    std::uint8_t len_{0};

    ara::core::Array< std::uint8_t, kMaxLen >
        /// @name data_ - user data
        data_{static_cast< std::uint8_t >(0)};
};  /// class UserData

}  // namespace internal
}  // namespace tsync
}  // namespace ara

#endif  /// ARA_TSYNC_INTERNAL_USERDATA_H_
