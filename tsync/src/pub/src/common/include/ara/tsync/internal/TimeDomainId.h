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
/// @file       TimeDomainId.h
/// @brief      time domain ID definition
/// @details
/// @date       2023-02-09
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_TSYNC_INTERNAL_TIME_DOMAIN_ID_H_
#define ARA_TSYNC_INTERNAL_TIME_DOMAIN_ID_H_

#include <cstdint>

namespace ara {
namespace tsync {
namespace internal {

/// @brief time domain ID
class TimeDomainId final
{
public:
    /// @brief default constructor
    TimeDomainId() noexcept : id_{kInvalidId} {}

    /// @brief constructor with parameters
    /// @param id - time domain ID
    explicit TimeDomainId(std::uint8_t const id) noexcept : id_{id} {}

    /// @brief move constructor is prohibited
    /// @param id - time domain ID
    TimeDomainId(TimeDomainId &&id) = default;

    /// @brief copy constructor
    /// @param id - time domain ID
    TimeDomainId(TimeDomainId const &id) noexcept = default;

    /// @brief move assignment is prohibited
    /// @param id - time domain ID
    /// @return reference to self
    TimeDomainId &operator=(TimeDomainId &&id) = default;

    /// @brief copy assignment
    /// @param other - another object
    /// @return reference to self
    TimeDomainId &operator=(TimeDomainId const &other) &noexcept = default;

    /// @brief default destructor
    ~TimeDomainId() = default;

    /// @brief set time domain ID
    /// @param id - time domain ID
    void FromUint8(std::uint8_t const id) noexcept { id_ = id; }

    /// @brief get time domain ID
    /// @return time domain ID
    std::uint8_t ToUint8() const noexcept { return id_; }

    /// @brief whether it is a synchronization time domain
    /// @return yes/no
    bool IsSyncDomain() const noexcept
    {
        if ((id_ >= kMinSyncId) && (id_ <= kMaxSyncId)) {
            return true;
        }
        return false;
    }

    /// @brief whether it is an offset time domain
    /// @return yes/no
    bool IsOffsetDomain() const noexcept
    {
        if ((id_ >= kMinOffsetId) && (id_ <= kMaxOffsetId)) {
            return true;
        }
        return false;
    }

    /// @brief whether it is a valid time domain ID
    /// @return yes/no
    bool IsValid() const noexcept
    {
        if (this->id_ <= kMaxOffsetId) {
            return true;
        }
        return false;
    }

    /// @brief get the maximum valid time domain ID
    /// @return maximum time domain ID
    static std::uint8_t GetMaxNumber() noexcept { return kMaxOffsetId; }

    /// @brief operator overload, comparison function
    /// @param lhs - object reference
    /// @param rhs - object reference
    /// @return true - equal
    /// @return false - not equal
    friend bool operator==(TimeDomainId const &lhs, TimeDomainId const &rhs) noexcept { return lhs.id_ == rhs.id_; }

    /// @brief operator overload, comparison function
    /// @param lhs - object reference
    /// @param rhs - object reference
    /// @return true - not equal
    /// @return false - equal
    friend bool operator!=(TimeDomainId const &lhs, TimeDomainId const &rhs) noexcept { return !(lhs == rhs); }

    /// @brief operator overload, increment
    void operator++() noexcept { this->id_++; }

    /// @brief operator overload, decrement
    void operator--() noexcept { this->id_--; }

    /// @brief operator overload, compound assignment
    /// @param id - reference to other object
    /// @return false - equal
    /// @return true - not equal
    TimeDomainId &operator+=(std::int8_t const &id) &noexcept
    {
        std::int8_t tmp{0};
        /// TODO(zhoubo): Conversion may lose precision, this->id_ is uint8_t
        tmp       = static_cast< std::int8_t >(this->id_) + id;  //NOLINT
        this->id_ = static_cast< std::uint8_t >(tmp);
        return *this;
    }

private:
    /// @brief invalid value
    constexpr static std::uint8_t kInvalidId{static_cast< std::uint8_t >(0xFF)};
    /// @brief minimum synchronization time domain ID
    constexpr static std::uint8_t kMinSyncId{static_cast< std::uint8_t >(0)};
    /// @brief maximum synchronization time domain ID
    constexpr static std::uint8_t kMaxSyncId{static_cast< std::uint8_t >(15)};
    /// @brief minimum offset time domain ID
    constexpr static std::uint8_t kMinOffsetId{static_cast< std::uint8_t >(16)};
    /// @brief maximum offset time domain ID
    constexpr static std::uint8_t kMaxOffsetId{static_cast< std::uint8_t >(31)};

    /// @name id_ - time domain ID
    std::uint8_t id_;

};  /// class TimeDomainId

}  // namespace internal
}  // namespace tsync
}  // namespace ara

#endif  /// ARA_TSYNC_INTERNAL_TIME_DOMAIN_ID_H_
