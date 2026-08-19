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
/// @file       subtlvtimesecured.h
/// @brief
/// @details
/// @date       2022-04-18
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#ifndef _ARA_TSYNC_INTERNAL_PTP_SUBTLVTIMESECURED_H_
#define _ARA_TSYNC_INTERNAL_PTP_SUBTLVTIMESECURED_H_

#include <chrono>
#include <cstdint>

#include "subtlv.h"

namespace ara {
namespace tsync {
namespace internal {
namespace ptp {
namespace message {

#pragma pack(push, 1)

/// @brief Followup
class Followup;

/// @traceid{PRS_TS_00074}  The multiplicity of TimeSecured SubTLV is 1, and it is only available when CRC protection is needed.
/// @brief SubTlvTimeSecured
class SubTlvTimeSecured : public SubTlv
{
    /// @brief kTS_PTP_CRC_02U constant declaration
    static constexpr std::uint8_t kTS_PTP_CRC_02U{0x02U};

    /// @brief kTS_PTP_CRC_08U constant declaration
    static constexpr std::uint8_t kTS_PTP_CRC_08U{0x08U};

    /// @brief kTS_PTP_CRC_20U constant declaration
    static constexpr std::uint8_t kTS_PTP_CRC_20U{0x20U};

    /// @brief kTS_PTP_CRC_04U constant declaration
    static constexpr std::uint8_t kTS_PTP_CRC_04U{0x04U};

    /// @brief kTS_PTP_CRC_10U constant declaration
    static constexpr std::uint8_t kTS_PTP_CRC_10U{0x10U};

public:
    /// @brief constructor
    SubTlvTimeSecured() = default;

    /// @brief destructor
    ~SubTlvTimeSecured() = default;

    /// @brief copy constructor
    /// @param    other
    /// @returns object reference
    SubTlvTimeSecured &operator=(SubTlvTimeSecured const &other) = default;

    /// @brief move constructor
    /// @param    other
    /// @returns object reference
    SubTlvTimeSecured &operator=(SubTlvTimeSecured &&other) = default;

    /// @brief copy constructor
    /// @param other - other object
    SubTlvTimeSecured(SubTlvTimeSecured const &other) = default;

    /// @brief move constructor
    /// @param other an existing rvalue reference.
    SubTlvTimeSecured(SubTlvTimeSecured &&) noexcept = default;

    /// @brief initialize
    void Init() noexcept
    {
        SetType(SubTlvType::kTimeSecured);
        SetLength((sizeof(*this)) - (sizeof(SubTlv)));
    }

    /// @brief set Crc flag
    /// @param flag Crc flag
    void SetCRCTimeFlags(std::uint8_t const flag) noexcept { crcTimeFlags_ = flag; }

    /// @brief get Crc flag
    /// @returns Crc flag
    std::uint8_t GetCRCTimeFlags() const noexcept { return crcTimeFlags_; }

    /// @brief check Crc
    /// @param dataId
    /// @param mesg
    /// @returns true, verification passed; false, verification failed.
    /// @traceid{PRS_TS_00207}
    /// @traceid{PRS_TS_00208} Repeatedly and separately mention the verification of the correctionField field.
    bool CrcCheck(std::uint8_t const dataId, Followup const *const mesg) const noexcept;

    /// @brief set Crc
    /// @param dataId
    /// @param mesg
    void CrcSet(std::uint8_t const dataId, Followup const *const mesg) noexcept;

private:
    // crcTimeFlags_ value comes from the PTP configuration item crcTimeFlagsTxSecured and remains consistent.
    // BitMask 0x01  length of the message
    // BitMask 0x02  domainNumber
    // BitMask 0x04  correctionField
    // BitMask 0x08  sourcePortIdentity
    // BitMask 0x10  sequenceId
    // BitMask 0x20  preciseOriginTimetamp
    // BitMask 0x40  reserved
    // BitMask 0x80  reserved
    /// @name crcTimeFlags_
    std::uint8_t crcTimeFlags_{0U};

    /// @traceid{PRS_TS_00099}  crcTime0 calculation rule.
    /// @name crcTime0_
    std::uint8_t crcTime0_{0U};
    /// @traceid{PRS_TS_00100}  crcTime1 calculation rule.
    /// @name crcTime1_
    std::uint8_t crcTime1_{0U};
};
#pragma pack(pop)

}  // namespace message
}  // namespace ptp
}  // namespace internal
}  // namespace tsync
}  // namespace ara
#endif  //_ARA_TSYNC_INTERNAL_PTP_SUBTLVTIMESECURED_H_
