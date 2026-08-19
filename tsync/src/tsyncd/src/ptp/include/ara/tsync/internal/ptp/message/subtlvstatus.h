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
/// @file       subtlvstatus.h
/// @brief
/// @details
/// @date       2022-04-18
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#ifndef _ARA_TSYNC_INTERNAL_PTP_SUBTLVSTATUS_H_
#define _ARA_TSYNC_INTERNAL_PTP_SUBTLVSTATUS_H_
#include "isoft/util/mix.h"
#include "subtlv.h"

namespace ara {
namespace tsync {
namespace internal {
namespace ptp {
namespace message {

// TLV: SubTlvType/Length/Value combination.

#pragma pack(push, 1)

/// @traceid{PRS_TS_00076}  The multiplicity of Status SubTLV is 1, and it is set to Secured or Not Secured according to CRC protection needs.
/// @brief  SubTlvStatus
class SubTlvStatus : public SubTlv
{
public:
    /// @brief constructor
    SubTlvStatus() = default;

    /// @brief destructor
    ~SubTlvStatus() = default;

    /// @brief copy constructor
    /// @param    other
    /// @returns object reference
    SubTlvStatus &operator=(SubTlvStatus const &other) = default;

    /// @brief move constructor
    /// @param    other
    /// @returns object reference
    SubTlvStatus &operator=(SubTlvStatus &&other) = default;

    /// @brief copy constructor
    /// @param other - other object
    SubTlvStatus(SubTlvStatus const &other) = default;

    /// @brief move constructor
    /// @param other an existing rvalue reference.
    SubTlvStatus(SubTlvStatus &&) noexcept = default;

    /// @brief initialize
    void Init() noexcept
    {
        SetType(SubTlvType::kStatus);
        SetLength((sizeof(*this)) - (sizeof(SubTlv)));
    }

    /// @brief set to Secured
    void SetSecured() noexcept { SetType(SubTlvType::kStatusSecured); }

    /// @brief check whether it is Secured
    /// @returns true, Secured.
    bool IsSecured() const noexcept
    {
        if (SubTlvType::kStatusSecured == GetType()) {
            return true;
        }
        return false;
    }

    /// @brief get status
    /// @returns status
    std::uint8_t GetStatus() const noexcept { return status_; }

    /// @brief set status
    /// @param s status
    void SetStatus(std::uint8_t const s) noexcept { status_ = s; }

    /// @brief set crc verification
    /// @param dataId
    void CrcSet(std::uint8_t const dataId) noexcept
    {
        std::uint8_t const crc{isoft::util::Crc8(reinterpret_cast< std::uint8_t const * >(this), (sizeof(*this)) - 1U)};
        crcStatus_ = isoft::util::Crc8Update(crc, &dataId, 1U);
    }

    /// @brief check crc verification
    /// @param dataId
    /// @returns true, verification passed; false, verification failed.
    /// @traceid{PRS_TS_00115}  crcStatus calculation method.
    bool CrcCheck(std::uint8_t const dataId) const noexcept
    {
        std::uint8_t const crc{isoft::util::Crc8(reinterpret_cast< std::uint8_t const * >(this), (sizeof(*this)) - 1U)};
        return isoft::util::Crc8Update(crc, &dataId, 1U) == crcStatus_;
    }

private:
    /// @name status_ // 0 SyncToGTM; 1 SyncToSubDomain
    std::uint8_t status_{0U};
    /// @name crcStatus_
    std::uint8_t crcStatus_{0U};
};

#pragma pack(pop)

}  // namespace message
}  // namespace ptp
}  // namespace internal
}  // namespace tsync
}  // namespace ara
#endif  //_ARA_TSYNC_INTERNAL_PTP_SUBTLVSTATUS_H_
