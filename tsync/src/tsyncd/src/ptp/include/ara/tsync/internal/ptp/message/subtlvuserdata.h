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
/// @file       subtlvuserdata.h
/// @brief
/// @details
/// @date       2022-04-18
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#ifndef _ARA_TSYNC_INTERNAL_PTP_SUBTLVUSERDATA_H_
#define _ARA_TSYNC_INTERNAL_PTP_SUBTLVUSERDATA_H_

#include <ara/core/vector.h>

#include <chrono>
#include <cstdint>

#include "ara/tsync/internal/ptp/configure.h"
#include "ara/tsync/internal/ptp/message/subtlv.h"
#include "ara/tsync/internal/userdata.h"
#include "isoft/util/mix.h"

namespace ara {
namespace tsync {
namespace internal {
namespace ptp {
namespace message {

// TLV: SubTlvType/Length/Value combination.

#pragma pack(push, 1)

/// @brief Followup
class Followup;

/// @traceid{PRS_TS_00078}  The multiplicity of UserData SubTLV is 1, and it is set to Secured or Not Secured according to CRC protection needs.
/// @brief  SubTlvUserData
class SubTlvUserData : public SubTlv
{
public:
    /// @brief constructor
    SubTlvUserData() = default;

    /// @brief destructor
    ~SubTlvUserData() = default;

    /// @brief copy constructor
    /// @param    other
    /// @returns object reference
    SubTlvUserData &operator=(SubTlvUserData const &other) = default;

    /// @brief move constructor
    /// @param    other
    /// @returns object reference
    SubTlvUserData &operator=(SubTlvUserData &&other) = default;

    /// @brief copy constructor
    /// @param other - other object
    SubTlvUserData(SubTlvUserData const &other) = default;

    /// @brief move constructor
    /// @param other an existing rvalue reference.
    SubTlvUserData(SubTlvUserData &&) noexcept = default;

    /// @brief initialize
    void Init() noexcept
    {
        SetType(SubTlvType::kUserData);
        SetLength((sizeof(*this)) - (sizeof(SubTlv)));
    }

    /// @brief set to Secured
    void SetSecured() noexcept { SetType(SubTlvType::kUserDataSecured); }

    /// @brief check whether it is Secured
    /// @returns true, Secured.
    bool IsSecured() const noexcept
    {
        if (SubTlvType::kUserDataSecured == GetType()) {
            return true;
        }
        return false;
    }

    /// @brief set user data
    /// @param userData - user data
    void SetUserData(internal::UserData const &userData) noexcept
    {
        ara::core::Span< std::uint8_t const > const us{userData.ToSpan()};
        std::size_t i{0U};
        for (std::uint8_t const &d : us) {
            (&userByte0_)[i] = d;
            i++;
        }
        userDataLength_ = static_cast< std::uint8_t >(us.size());
    }

    /// @brief get user data
    /// @return user data
    internal::UserData GetUserData() const noexcept
    {
        internal::UserData ud;
        ara::core::Span< std::uint8_t const > us;
        us = ara::core::MakeSpan(static_cast< std::uint8_t const * >(&userByte0_),
                                 static_cast< std::size_t >(userDataLength_));
        ud.FromSpan(us);
        return ud;
    }

    /// @brief set crc verification
    /// @param dataId
    void CrcSet(std::uint8_t const dataId) noexcept
    {
        std::uint8_t const crc{isoft::util::Crc8(reinterpret_cast< std::uint8_t const * >(this), (sizeof(*this)) - 1U)};
        crcUserData_ = isoft::util::Crc8Update(crc, &dataId, 1U);
    }

    /// @brief check crc verification
    /// @param dataId
    /// @returns true, verification passed; false, verification failed.
    /// @traceid{PRS_TS_00116}  crcUserData calculation method.
    bool CrcCheck(std::uint8_t const dataId) const noexcept
    {
        std::uint8_t const crc{isoft::util::Crc8(reinterpret_cast< std::uint8_t const * >(this), (sizeof(*this)) - 1U)};
        return isoft::util::Crc8Update(crc, &dataId, 1U) == crcUserData_;
    }

private:
    /// @name userDataLength_ // 1 .. 3
    std::uint8_t userDataLength_{1U};
    /// @name userByte0_
    std::uint8_t userByte0_{0U};
    /// @name userByte1_
    std::uint8_t userByte1_{0U};
    /// @name userByte2_
    std::uint8_t userByte2_{0U};
    /// @name crcUserData_
    std::uint8_t crcUserData_{0U};
};
#pragma pack(pop)

}  // namespace message
}  // namespace ptp
}  // namespace internal
}  // namespace tsync
}  // namespace ara
#endif  //_ARA_TSYNC_INTERNAL_PTP_SUBTLVUSERDATA_H_
