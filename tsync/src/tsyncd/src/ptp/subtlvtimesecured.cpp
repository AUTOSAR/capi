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
/// @file       subtlvtimesecured.cpp
/// @brief      PTP module followup message TLV class
/// @details
/// @date       2023-01-03
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#include <cstdint>
#include <vector>

#include "ara/tsync/internal/ptp/message/followup.h"
#include "isoft/util/mix.h"

namespace ara {
namespace tsync {
namespace internal {
namespace ptp {
namespace message {

/// @brief set Crc
/// @param dataId
/// @param mesg
void SubTlvTimeSecured::CrcSet(std::uint8_t const dataId, Followup const *const mesg) noexcept
{
    /// NOTE: The CRC calculation uses the raw data (network byte order) from the Message frame.
    //
    /// @traceid{PRS_TS_00099}  crcTime0 calculation rule.
    crcTime0_ = isoft::util::Crc8(&crcTimeFlags_, 1U);
    if (kTS_PTP_CRC_02U == (crcTimeFlags_ & kTS_PTP_CRC_02U)) {
        // Cannot call method to return data, because the method returns converted local byte order. AAAAA?
        std::uint8_t const domainId{mesg->GetDomainId().ToUint8()};
        crcTime0_
            = isoft::util::Crc8Update(crcTime0_, static_cast< std::uint8_t const * >(&(domainId)), sizeof(domainId));
    }

    if (kTS_PTP_CRC_08U == (crcTimeFlags_ & kTS_PTP_CRC_08U)) {
        struct PortIdentity sourcePortIdentity;
        sourcePortIdentity.clockIdentity = isoft::util::HtoN64(mesg->GetClockIdentity());
        sourcePortIdentity.portNumber    = isoft::util::HtoN16(mesg->GetSourcePortNumber());
        crcTime0_ = isoft::util::Crc8Update(crcTime0_, reinterpret_cast< std::uint8_t const * >(&(sourcePortIdentity)),
                                            sizeof(sourcePortIdentity));
    }

    if (kTS_PTP_CRC_20U == (crcTimeFlags_ & kTS_PTP_CRC_20U)) {
        internal::TimeValue const &preciseOriginTimetamp{mesg->GetPreciseOriginTimeStamp()};
        crcTime0_
            = isoft::util::Crc8Update(crcTime0_, reinterpret_cast< std::uint8_t const * >(&(preciseOriginTimetamp)),
                                      sizeof(preciseOriginTimetamp));
    }

    crcTime0_ = isoft::util::Crc8Update(crcTime0_, &dataId, sizeof(dataId));

    /// @traceid{PRS_TS_00100}  crcTime1 calculation rule.
    crcTime1_ = isoft::util::Crc8(&crcTimeFlags_, 1U);
    if (0x01U == (crcTimeFlags_ & 0x01U)) {
        // Cannot call method to return data, because the method returns converted local byte order.
        std::uint16_t const mesgLength{isoft::util::HtoN16(mesg->GetMessageLength())};
        crcTime1_ = isoft::util::Crc8Update(crcTime1_, reinterpret_cast< std::uint8_t const * >(&(mesgLength)),
                                            sizeof(mesgLength));
    }

    if (kTS_PTP_CRC_04U == (crcTimeFlags_ & kTS_PTP_CRC_04U)) {
        std::uint64_t const correctionField{isoft::util::HtoN64(mesg->GetCorrectionField())};
        crcTime1_ = isoft::util::Crc8Update(crcTime1_, reinterpret_cast< std::uint8_t const * >(&(correctionField)),
                                            sizeof(correctionField));
    }

    if (kTS_PTP_CRC_10U == (crcTimeFlags_ & kTS_PTP_CRC_10U)) {
        std::uint16_t const sequenceId{isoft::util::HtoN16(mesg->GetSequenceID())};
        crcTime1_ = isoft::util::Crc8Update(crcTime1_, reinterpret_cast< std::uint8_t const * >(&(sequenceId)),
                                            sizeof(sequenceId));
    }

    crcTime1_ = isoft::util::Crc8Update(crcTime1_, &dataId, sizeof(dataId));
}

/// @brief check Crc
/// @param dataId
/// @param mesg
/// @returns true, verification passed; false, verification failed.
bool SubTlvTimeSecured::CrcCheck(std::uint8_t const dataId, Followup const *const mesg) const noexcept
{
    std::uint8_t crc0{0U};
    std::uint8_t crc1{0U};
    /// NOTE: The CRC calculation uses the raw data (network byte order) from the Message frame.
    //
    /// @traceid{PRS_TS_000}    crcTime0 calculation rule.
    crc0 = isoft::util::Crc8(&crcTimeFlags_, 1U);
    if (kTS_PTP_CRC_02U == (crcTimeFlags_ & kTS_PTP_CRC_02U)) {
        // Cannot call method to return data, because the method returns converted local byte order.
        // std::uint8_t domainId = mesg->GetDomainId().
        std::uint8_t const domainId{mesg->GetDomainId().ToUint8()};
        crc0 = isoft::util::Crc8Update(crc0, static_cast< std::uint8_t const * >(&(domainId)), sizeof(domainId));
    }

    if (kTS_PTP_CRC_08U == (crcTimeFlags_ & kTS_PTP_CRC_08U)) {
        struct PortIdentity sourcePortIdentity;
        sourcePortIdentity.clockIdentity = isoft::util::HtoN64(mesg->GetClockIdentity());
        sourcePortIdentity.portNumber    = isoft::util::HtoN16(mesg->GetSourcePortNumber());
        crc0 = isoft::util::Crc8Update(crc0, reinterpret_cast< std::uint8_t const * >(&(sourcePortIdentity)),
                                       sizeof(sourcePortIdentity));
    }

    if (kTS_PTP_CRC_20U == (crcTimeFlags_ & kTS_PTP_CRC_20U)) {
        internal::TimeValue const &preciseOriginTimetamp{mesg->GetPreciseOriginTimeStamp()};
        crc0 = isoft::util::Crc8Update(crc0, reinterpret_cast< std::uint8_t const * >(&(preciseOriginTimetamp)),
                                       sizeof(preciseOriginTimetamp));
    }

    crc0 = isoft::util::Crc8Update(crc0, &dataId, sizeof(dataId));
    if (crcTime0_ != crc0) {
        return false;
    }

    /// @traceid{PRS_TS_00} crcTime1 calculation rule.
    crc1 = isoft::util::Crc8(&crcTimeFlags_, 1U);
    if (0x01U == (crcTimeFlags_ & 0x01U)) {
        // Cannot call method to return data, because the method returns converted local byte order.
        std::uint16_t const mesgLength{isoft::util::HtoN16(mesg->GetMessageLength())};
        crc1 = isoft::util::Crc8Update(crc1, reinterpret_cast< std::uint8_t const * >(&(mesgLength)),
                                       sizeof(mesgLength));
    }

    if (kTS_PTP_CRC_04U == (crcTimeFlags_ & kTS_PTP_CRC_04U)) {
        std::uint64_t const correctionField{isoft::util::HtoN64(mesg->GetCorrectionField())};
        crc1 = isoft::util::Crc8Update(crc1, reinterpret_cast< std::uint8_t const * >(&(correctionField)),
                                       sizeof(correctionField));
    }

    if (kTS_PTP_CRC_10U == (crcTimeFlags_ & kTS_PTP_CRC_10U)) {
        std::uint16_t const sequenceId{isoft::util::HtoN16(mesg->GetSequenceID())};
        crc1 = isoft::util::Crc8Update(crc1, reinterpret_cast< std::uint8_t const * >(&(sequenceId)),
                                       sizeof(sequenceId));
    }

    crc1 = isoft::util::Crc8Update(crc1, &dataId, sizeof(dataId));
    if (crcTime1_ != crc1) {
        return false;
    }
    return true;
}

}  // namespace message
}  // namespace ptp
}  // namespace internal
}  // namespace tsync
}  // namespace ara
