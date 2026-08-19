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
/// @file       subtlv.h
/// @brief
/// @details
/// @date       2022-04-18
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#ifndef _ARA_TSYNC_INTERNAL_PTP_SUBTLV_H_
#define _ARA_TSYNC_INTERNAL_PTP_SUBTLV_H_

#include <chrono>
#include <cstdint>

namespace ara {
namespace tsync {
namespace internal {
namespace ptp {
namespace message {

// TLV: SubTlvType/Length/Value combination.

#pragma pack(push, 1)

/// @brief SubTlv tlv header, protocol, cannot contain virtual
class SubTlv
{
public:
    // Secured and Not Secured differ in whether CRC check is enabled
    /// @brief SubTlvType
    enum class SubTlvType : std::uint8_t
    {
        kTimeSecured     = 0x28,
        kOfs             = 0x34,
        kOfsSecured      = 0x44,
        kStatusSecured   = 0x50,
        kStatus          = 0x51,
        kUserDataSecured = 0x60,
        kUserData        = 0x61,
    };

protected:
    /// @brief constructor
    SubTlv() = default;

    /// @brief destructor  protocol, cannot contain virtual
    ~SubTlv() = default;

    /// @brief copy constructor
    /// @param    other
    /// @returns object reference
    SubTlv &operator=(SubTlv const &other) = default;

    /// @brief move constructor
    /// @param    other
    /// @returns object reference
    SubTlv &operator=(SubTlv &&other) = default;

    /// @brief copy constructor
    /// @param other - other object
    SubTlv(SubTlv const &other) = default;

    /// @brief move constructor
    /// @param other an existing rvalue reference.
    SubTlv(SubTlv &&) noexcept = default;

public:
    /// @brief set type
    /// @param type type
    void SetType(SubTlvType const type) noexcept { type_ = type; }

    /// @brief set length
    /// @param len length
    void SetLength(std::uint8_t const len) noexcept { length_ = len; }

    /// @brief get length
    /// @returns length.
    std::uint8_t GetLength() const noexcept { return length_; }

    /// @brief get type
    /// @returns type.
    SubTlvType GetType() const noexcept { return type_; }

private:
    /// @name type_
    SubTlvType type_{SubTlvType::kTimeSecured};
    /// @name length_
    std::uint8_t length_{0U};
};
#pragma pack(pop)

}  // namespace message
}  // namespace ptp
}  // namespace internal
}  // namespace tsync
}  // namespace ara
#endif  //_ARA_TSYNC_INTERNAL_PTP_SUBTLV_H_
