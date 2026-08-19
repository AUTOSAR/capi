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
/// @file       pdelayresp.h
/// @brief
/// @details
/// @date       2021-10-18
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#ifndef _ARA_TSYNC_INTERNAL_PTP_PDELAY_RESP_MESSAGE_H_
#define _ARA_TSYNC_INTERNAL_PTP_PDELAY_RESP_MESSAGE_H_

#include <chrono>
#include <cstdint>
#include <iomanip>

#include "ara/tsync/internal/ptp/message/header.h"

namespace ara {
namespace tsync {
namespace internal {
namespace ptp {
namespace message {

#pragma pack(push, 1)
/// @brief PdelayResp
class PdelayResp final : public Header
{
public:
    /// @brief constructor
    PdelayResp() = default;

    /// @brief destructor
    ~PdelayResp() = default;

    /// @brief copy constructor is prohibited
    /// @param other - other object
    PdelayResp(PdelayResp const &other) = delete;

    /// @brief copy is prohibited
    /// @param other - other object
    /// @return reference to class object
    PdelayResp &operator=(PdelayResp const &other) = delete;

    /// @brief move constructor
    /// @param other an existing rvalue reference.
    PdelayResp(PdelayResp &&) noexcept = default;

    /// @brief '=' operator overload
    /// @param other an existing rvalue reference.
    /// @return reference to class object
    PdelayResp &operator=(PdelayResp &&) &noexcept = default;

    /// @brief initialize
    void Init() noexcept
    {
        SetTransportSpecific();
        SetType(Type::kPdelayResp);
        SetMessageLength(sizeof(*this));
        // set twoStepFlag, only for Sync and PdelayResp
        SetFlagField(1U << 1U, 0U);
        SetControlField(ControlFieldType::kCtrlAllOther);
    }

    /// @brief
    /// @param ts
    void SetRequestReceiptTimeStamp(internal::TimeValue const &ts) noexcept { requestReceiptTimeStamp_ = ts; }

    /// @brief
    /// @return
    internal::TimeValue const &GetRequestReceiptTimeStamp() const noexcept { return requestReceiptTimeStamp_; }

    /// @brief
    /// @param portIdentity
    void SetRequestingPortIdentity(struct PortIdentity const &portIdentity) noexcept
    {
        requestingPortIdentity_ = portIdentity;
    }

    /// @brief
    /// @return
    const struct PortIdentity &GetRequestingPortIdentity() const noexcept { return requestingPortIdentity_; }

    /// @brief print debug information
    void Debug() const noexcept
    {
#if ARA_TSYNC_DEBUG
        ;
        Header::HeaderDebug();
        std::int32_t const tsNsec{
            static_cast< std::int32_t >(GetRequestReceiptTimeStamp().ToChrono().count() % 1000000000)};
        std::cout << "RequestReceiptTimestamp:" << std::dec
                  << std::chrono::duration_cast< std::chrono::seconds >(GetRequestReceiptTimeStamp().ToChrono()).count()
                  << ".";
        std::cout << std::right << std::setw(kTS_NUM_INT_9) << std::setfill(kTS_FILL_0) << std::dec << tsNsec
                  << std::endl;
#endif
    }

private:
    /// @name requestReceiptTimeStamp_
    internal::TimeValue requestReceiptTimeStamp_{};
    /// @name requestingPortIdentity_
    struct PortIdentity requestingPortIdentity_
    {
    };

};  // class PdelayResp
#pragma pack(pop)

}  // namespace message
}  // namespace ptp
}  // namespace internal
}  // namespace tsync
}  // namespace ara
#endif  //_ARA_TSYNC_INTERNAL_PTP_PDELAY_RESP_MESSAGE_H_
