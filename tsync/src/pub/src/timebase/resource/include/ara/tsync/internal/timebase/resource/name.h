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
/// @file       name.h
/// @brief      time base name
/// @details
/// @date       2023-01-31
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_TSYNC_INTERNAL_TIMEBASE_RESOURCE_NAME_H_
#define ARA_TSYNC_INTERNAL_TIMEBASE_RESOURCE_NAME_H_

#include <ara/core/string.h>
#include <ara/core/string_view.h>

#include <cstdint>
#include <cstring>

#include "ara/tsync/internal/common.h"

namespace ara {
namespace tsync {
namespace internal {
namespace timebase {
namespace resource {

/// Keep 1-byte alignment
#pragma pack(push, 1)

/// @brief time base name
class Name final
{
public:
    /// @brief maximum length
    static std::uint32_t const kMaxLen{255U};

    /// @brief default constructor
    Name() = default;

    /// @brief default destructor
    ~Name() = default;

    /// @brief copy constructor is prohibited
    /// @param other - other object
    Name(Name const &other) = delete;

    /// @brief copy is prohibited
    /// @param other - other object
    /// @return reference to class object
    Name &operator=(Name const &other) = delete;

    /// @brief move constructor
    /// @param other an existing rvalue reference.
    Name(Name &&) noexcept = default;

    /// @brief '=' operator overload
    /// @param other an existing rvalue reference.
    /// @return reference to class object
    Name &operator=(Name &&) &noexcept = default;

    /// @brief construct time base name from characters
    /// @param str - string
    void FromString(ara::core::String const &str) noexcept
    {
        if (str.size() <= kMaxLen) {
            len_ = static_cast< std::uint8_t >(str.size());
        } else {
            len_ = kMaxLen;
        }
        static_cast< void >(
            ::memmove(static_cast< std::uint8_t * >(&data_[0]), str.data(), static_cast< std::size_t >(len_)));
    }

    /// @brief export user data to string
    /// @return string
    ara::core::String ToString() const noexcept
    {
        return ara::core::String{reinterpret_cast< char8_t const * >(data_), static_cast< std::size_t >(len_)};
    }

    /// @brief operator overload, comparison function
    /// @param lName - object reference
    /// @param rName - object reference
    /// @return true - equal
    /// @return false - not equal
    friend bool operator==(Name const &lName, Name const &rName) noexcept
    {
        if (lName.len_ != rName.len_) {
            return false;
        }
        if (0
            != ::memcmp(static_cast< std::uint8_t const * >(&lName.data_[0]),
                        static_cast< std::uint8_t const * >(&rName.data_[0]), static_cast< std::size_t >(rName.len_))) {
            return false;
        }
        return true;
    }

private:
    /// @name len_ - time base name length
    std::uint8_t len_{0U};

    /// @name data_ - time base name
    std::uint8_t data_[kMaxLen]{0U};

};  /// class Name
#pragma pack(pop)

}  // namespace resource
}  // namespace timebase
}  // namespace internal
}  // namespace tsync
}  // namespace ara

#endif  // ARA_TSYNC_INTERNAL_TIMEBASE_RESOURCE_NAME_H_
