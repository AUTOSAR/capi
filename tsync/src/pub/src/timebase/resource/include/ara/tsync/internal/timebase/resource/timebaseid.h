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
/// @file       timebaseid.h
/// @brief      time base ID definition
/// @details
/// @date       2023-02-09
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_TSYNC_INTERNAL_TIMEBASEID_H_
#define ARA_TSYNC_INTERNAL_TIMEBASEID_H_

#include <cstdint>
#include <utility>

#include "ara/tsync/internal/TimeDomainId.h"
#include "ara/tsync/internal/common.h"

namespace ara {
namespace tsync {
namespace internal {
namespace timebase {
namespace resource {

/// @brief time base ID
class TimeBaseId final
{
public:
    /// @brief invalid value
    constexpr static std::uint16_t kInvalidId{0xffffU};

    /// @brief kByte0Flag constant declaration
    constexpr static std::uint8_t kByte0Flag{0xffU};

    /// @brief default constructor
    TimeBaseId() noexcept : id_{kInvalidId} {}

    /// @brief constructor with parameters
    /// @param id - time base ID
    explicit TimeBaseId(std::uint16_t const id) noexcept : id_{id} {}

    /// @brief constructor with parameters
    /// @param domainId - time domain ID
    /// @param tbIindex - index number
    TimeBaseId(internal::TimeDomainId const &domainId, std::uint8_t const tbIindex) noexcept
    {
        id_ = Make(domainId, tbIindex);
    }

    /// @brief default destructor
    ~TimeBaseId() = default;

    /// @brief copy constructor
    /// @param other - other object
    TimeBaseId(TimeBaseId const &other) = default;

    /// @brief copy assignment
    /// @param other - other object
    /// @return reference to class object
    TimeBaseId &operator=(TimeBaseId const &other) = default;

    /// @brief move constructor
    /// @param other an existing rvalue reference.
    TimeBaseId(TimeBaseId &&) noexcept = default;

    /// @brief '=' operator overload
    /// @param other an existing rvalue reference.
    /// @return reference to class object
    TimeBaseId &operator=(TimeBaseId &&) &noexcept = default;

    /// @brief generate time base ID
    /// @param domainId - time domain ID
    /// @param tbIndex - index number
    /// @return generated time base ID
    static std::uint16_t Make(internal::TimeDomainId const &domainId, std::uint8_t const &tbIndex) noexcept
    {
        /// 15         7        0
        /// | domainId | tbIndex  |
        std::uint32_t newDomainId{domainId.ToUint8()};
        newDomainId = newDomainId << kTS_NUM_8;
        return static_cast< std::uint16_t >(newDomainId | static_cast< std::uint32_t >(tbIndex));
    }

    /// @brief get time domain ID
    /// @return time domain ID
    internal::TimeDomainId GetDomainId() const noexcept
    {
        constexpr std::uint8_t kNum{8};
        internal::TimeDomainId domainId{static_cast< std::uint8_t >((id_ >> kNum) & kByte0Flag)};
        return domainId;
    }

    /// @brief get index number
    /// @return index number
    std::uint8_t GetIndex() const noexcept { return static_cast< std::uint8_t >(id_ & kByte0Flag); }

    /// @brief verify whether the time base ID is valid
    /// @return true - valid
    /// @return false - invalid
    bool IsValid() const noexcept
    {
        internal::TimeDomainId const domainId{GetDomainId()};
        if (domainId.IsValid()) {
            return true;
        }
        return false;
    }

    /// @brief get time base ID
    /// @return time base ID
    std::uint16_t ToUint16() const noexcept { return id_; }

    /// @brief whether it is a synchronization time base
    /// @return yes/no
    bool IsSyncTimeBase() const noexcept
    {
        internal::TimeDomainId const domainId{GetDomainId()};
        if (domainId.IsSyncDomain()) {
            return true;
        }
        return false;
    }

    /// @brief whether it is an offset time domain
    /// @return yes/no
    bool IsOffsetTimeBase() const noexcept
    {
        internal::TimeDomainId const domainId{GetDomainId()};
        if (domainId.IsOffsetDomain()) {
            return true;
        }
        return false;
    }

    /// @brief operator overload, comparison function
    /// @param lid - object reference
    /// @param rid - object reference
    /// @return true - equal
    /// @return false - not equal
    friend bool operator==(TimeBaseId const &lid, TimeBaseId const &rid) noexcept
    {
        if (lid.id_ == rid.id_) {
            return true;
        }
        return false;
    }

    /// @brief operator overload, comparison function
    /// @param lid - object reference
    /// @param rid - object reference
    /// @return false - equal
    /// @return true - not equal
    friend bool operator!=(TimeBaseId const &lid, TimeBaseId const &rid) noexcept { return !(lid == rid); }

private:
    /// @name id_ - time base ID
    std::uint16_t id_;

};  /// class TimeBaseId

}  // namespace resource
}  // namespace timebase
}  // namespace internal
}  // namespace tsync
}  // namespace ara

#endif  /// ARA_TSYNC_INTERNAL_TIMEBASEID_H_
