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
/// @file       pdelayrespfollowup.h
/// @brief
/// @details
/// @date       2021-10-18
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_TSYNC_INTERNAL_PTP_PDELAY_RESP_FUP_MESSAGE_H_
#define ARA_TSYNC_INTERNAL_PTP_PDELAY_RESP_FUP_MESSAGE_H_

#include <chrono>
#include <cstdint>

#include "ara/tsync/internal/ptp/message/header.h"

namespace ara {
namespace tsync {
namespace internal {
namespace ptp {
namespace message {

#pragma pack(push, 1)
/// @brief PdelayRespFollowup
class PdelayRespFollowup final : public Header
{
public:
    /// @brief constructor
    PdelayRespFollowup() = default;

    /// @brief destructor
    ~PdelayRespFollowup() = default;

    /// @brief copy constructor is prohibited
    /// @param other - other object
    PdelayRespFollowup(PdelayRespFollowup const &other) = delete;

    /// @brief copy is prohibited
    /// @param other - other object
    /// @return reference to class object
    PdelayRespFollowup &operator=(PdelayRespFollowup const &other) = delete;

    /// @brief move constructor
    /// @param other an existing rvalue reference.
    PdelayRespFollowup(PdelayRespFollowup &&) noexcept = default;

    /// @brief '=' operator overload
    /// @param other an existing rvalue reference.
    /// @return reference to class object
    PdelayRespFollowup &operator=(PdelayRespFollowup &&) &noexcept = default;

    /// @brief initialize
    void Init() noexcept
    {
        SetTransportSpecific();
        SetType(Type::kPdelayRespFollowup);
        SetMessageLength(sizeof(*this));
        // set twoStepFlag, only for Sync and PdelayResp
        // SetFlagField(1 << 1, 0).
        SetControlField(ControlFieldType::kCtrlAllOther);
    }

    /// @brief
    /// @param ts
    void SetResponseOriginTimeStamp(internal::TimeValue const &ts) noexcept { responseOriginTimeStmap_ = ts; }

    /// @brief
    /// @return
    internal::TimeValue const &GetResponseOriginTimeStamp() const noexcept { return responseOriginTimeStmap_; }

    /// @brief
    /// @param portIdentity
    void SetRequestingPortIdentity(struct PortIdentity const &portIdentity) noexcept
    {
        requestingPortIdentity_ = portIdentity;
    }
    /// @brief
    /// @return
    const struct PortIdentity &GetRequestingPortIdentity() const noexcept { return requestingPortIdentity_; }

private:
    /// @name responseOriginTimeStmap_
    internal::TimeValue responseOriginTimeStmap_{};
    /// @name requestingPortIdentity_
    struct PortIdentity requestingPortIdentity_
    {
    };

};  // class PdelayRespFollowup
#pragma pack(pop)

}  // namespace message
}  // namespace ptp
}  // namespace internal
}  // namespace tsync
}  // namespace ara
#endif  /// ARA_TSYNC_INTERNAL_PTP_PDELAY_RESP_FUP_MESSAGE_H_
