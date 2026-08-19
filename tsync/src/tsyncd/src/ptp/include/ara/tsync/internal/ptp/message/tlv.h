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
/// @file       tlv.h
/// @brief
/// @details
/// @date       2022-04-18
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#ifndef _ARA_TSYNC_INTERNAL_PTP_TLV_H_
#define _ARA_TSYNC_INTERNAL_PTP_TLV_H_

#include <arpa/inet.h>

#include <chrono>
#include <cstdint>

#include "ara/tsync/internal/common.h"

namespace ara {
namespace tsync {
namespace internal {
namespace ptp {
namespace message {

// TLV: SubTlvType/Length/Value combination.

#pragma pack(push, 1)

/// @brief TLV base class
class TLV
{
    /// @brief kAutoSarOrgByte0 constant declaration
    constexpr static std::uint8_t kAutoSarOrgByte0{0x1AU};

    /// @brief kAutoSarOrgByte1 constant declaration
    constexpr static std::uint8_t kAutoSarOrgByte1{0x75U};

    /// @brief kAutoSarOrgByte2 constant declaration
    constexpr static std::uint8_t kAutoSarOrgByte2{0xFBU};

    /// @brief kAutoSarOrgSubByte0 constant declaration
    constexpr static std::uint8_t kAutoSarOrgSubByte0{0x60U};

    /// @brief kAutoSarOrgSubByte1 constant declaration
    constexpr static std::uint8_t kAutoSarOrgSubByte1{0x56U};

    /// @brief kAutoSarOrgSubByte2 constant declaration
    constexpr static std::uint8_t kAutoSarOrgSubByte2{0x76U};

    /// @brief kASOrg8021Byte0 constant declaration
    constexpr static std::uint8_t kASOrg8021Byte0{0x00U};

    /// @brief kASOrg8021Byte1 constant declaration
    constexpr static std::uint8_t kASOrg8021Byte1{0x80U};

    /// @brief kASOrg8021Byte2 constant declaration
    constexpr static std::uint8_t kASOrg8021Byte2{0xC2U};

    /// @brief kASOrgSub8021Byte0 constant declaration
    constexpr static std::uint8_t kASOrgSub8021Byte0{0x00U};

    /// @brief kASOrgSub8021Byte1 constant declaration
    constexpr static std::uint8_t kASOrgSub8021Byte1{0x00U};

    /// @brief kASOrgSub8021Byte2 constant declaration
    constexpr static std::uint8_t kASOrgSub8021Byte2{0x01U};

protected:
    /// @brief constructor
    TLV() = default;

    /// @brief destructor
    ~TLV() = default;

    /// @brief copy constructor
    /// @param    other
    /// @returns object reference
    TLV &operator=(TLV const &other) = default;

    /// @brief move constructor
    /// @param    other
    /// @returns object reference
    TLV &operator=(TLV &&other) = default;

    /// @brief copy constructor
    /// @param other - other object
    TLV(TLV const &other) = default;

    /// @brief move constructor
    /// @param other an existing rvalue reference.
    TLV(TLV &&) noexcept = default;

public:
    /// @brief TLV header
    struct TlvHeader final
    {
        // fixed to 3
        /// @name type
        std::uint16_t type{3};
        /// header_.length value calculation does not include header_.type and header_.length themselves in the TLV header.
        /// header_.length = sizeof(Organization)        /* 6 */.
        ///           + SUM(Sub-TLVs)
        /// @name length
        std::uint16_t length{0};
    };

    /// @brief TLV format autosar or IEEE802
    struct Organization final
    {
        /// @brief OrgType
        enum class OrgType : std::uint8_t
        {
            kIEEE802 = 1,
            kAUTOSAR = 2,
        };
        /// @name id
        std::uint8_t id[kTS_NUM_3]{0U};
        /// @name subType
        std::uint8_t subType[kTS_NUM_3]{0U};
    };

    /// @brief initialize
    /// @param org type
    void TlvInit(Organization::OrgType const org) noexcept
    {
        header_.type   = htons(kTS_NUM_3);
        header_.length = htons(0U);

        switch (org) {
            case Organization::OrgType::kAUTOSAR: {
                // 0x1A75FB  AUTOSAR
                organization_.id[0]                   = kAutoSarOrgByte0;
                organization_.id[1]                   = kAutoSarOrgByte1;
                organization_.id[internal::kTS_NUM_2] = kAutoSarOrgByte2;
                // 0x605676  BCD coded GlobalTimeEthTSyn
                organization_.subType[0]                   = kAutoSarOrgSubByte0;
                organization_.subType[1]                   = kAutoSarOrgSubByte1;
                organization_.subType[internal::kTS_NUM_2] = kAutoSarOrgSubByte2;
                break;
            }
            case Organization::OrgType::kIEEE802: {
                // 0x0080C2  IEEE802.1AS
                organization_.id[0]                   = kASOrg8021Byte0;
                organization_.id[1]                   = kASOrg8021Byte1;
                organization_.id[internal::kTS_NUM_2] = kASOrg8021Byte2;
                // 1
                organization_.subType[0]                   = kASOrgSub8021Byte0;
                organization_.subType[1]                   = kASOrgSub8021Byte1;
                organization_.subType[internal::kTS_NUM_2] = kASOrgSub8021Byte2;
                break;
            }
        }
    }

    /// @brief set length
    /// @param len length
    void SetLength(std::uint8_t const len) noexcept { header_.length = htons(static_cast< uint16_t >(len)); }

    /// @brief get length
    /// @returns length.
    std::uint8_t GetLength() const noexcept { return static_cast< std::uint8_t >(ntohs(header_.length)); }

    /// @brief get type
    /// @returns type
    std::uint8_t GetType() const noexcept { return static_cast< std::uint8_t >(ntohs(header_.type)); }

    /// @brief check whether it is autosar format TLV
    /// @returns true, autosar format TLV.
    bool IsOrganizationAUTOSAR() const noexcept
    {
        if ((organization_.id[0] == kAutoSarOrgByte0) && (organization_.id[1] == kAutoSarOrgByte1)
            && (organization_.id[internal::kTS_NUM_2] == kAutoSarOrgByte2)
            && (organization_.subType[0] == kAutoSarOrgSubByte0) && (organization_.subType[1] == kAutoSarOrgSubByte1)
            && (organization_.subType[internal::kTS_NUM_2] == kAutoSarOrgSubByte2)) {
            return true;
        }
        return false;
    }

    /// @brief check whether it is IEEE802 format TLV
    /// @returns true, IEEE802 format TLV.
    bool IsOrganizationIEEE802() const noexcept
    {
        if ((organization_.id[0] == kASOrg8021Byte0) && (organization_.id[1] == kASOrg8021Byte1)
            && (organization_.id[internal::kTS_NUM_2] == kASOrg8021Byte2)
            && (organization_.subType[0] == kASOrgSub8021Byte0) && (organization_.subType[1] == kASOrgSub8021Byte1)
            && (organization_.subType[internal::kTS_NUM_2] == kASOrgSub8021Byte2)) {
            return true;
        }
        return false;
    }

private:
    /// @name header_
    struct TlvHeader header_; /* 4 */
    /// @name organization_
    struct Organization organization_; /* 6 */
};

#pragma pack(pop)

}  // namespace message
}  // namespace ptp
}  // namespace internal
}  // namespace tsync
}  // namespace ara
#endif  //_ARA_TSYNC_INTERNAL_PTP_TLV_H_
