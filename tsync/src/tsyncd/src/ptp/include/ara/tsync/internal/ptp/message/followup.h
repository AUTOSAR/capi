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
/// @file       followup.h
/// @brief
/// @details
/// @date       2022-04-18
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#ifndef _ARA_TSYNC_INTERNAL_PTP_FOLLOW_UP_MESSAGE_H_
#define _ARA_TSYNC_INTERNAL_PTP_FOLLOW_UP_MESSAGE_H_

#include <chrono>
#include <cstdint>
// clang-format off
#include "ara/core/vector.h"
#include "ara/tsync/internal/ptp/configure.h"
#include "ara/tsync/internal/ptp/ptpcontext.h"                  // NOLINT
#include "ara/tsync/internal/ptp/message/header.h"              // NOLINT
#include "ara/tsync/internal/ptp/message/subtlvofs.h"           // NOLINT
#include "ara/tsync/internal/ptp/message/subtlvstatus.h"        // NOLINT
#include "ara/tsync/internal/ptp/message/subtlvtimesecured.h"   // NOLINT
#include "ara/tsync/internal/ptp/message/subtlvuserdata.h"      // NOLINT
#include "ara/tsync/internal/ptp/message/tlvautosar.h"          // NOLINT
#include "ara/tsync/internal/ptp/message/tlvieee802.h"          // NOLINT
// clang-format on
#if ARA_TSYNC_DEBUG
    #include <iomanip>
#endif

namespace ara {
namespace tsync {
namespace internal {
namespace ptp {
namespace message {

#pragma pack(push, 1)

/// @brief Followup
class Followup final : public Header
{
    /// @brief kFollowupFlagFieldByte1 constant declaration
    static constexpr std::uint8_t kFollowupFlagFieldByte1{0x08U};

    /// @brief kOfsTimeNSecMax constant declaration
    static constexpr std::int32_t kOfsTimeNSecMax{999999999};

public:
    /// @brief constructor
    /// @return
    Followup() = default;

    /// @brief destructor
    ~Followup() = default;

    /// @brief copy constructor
    /// @param    other
    /// @returns object reference
    Followup &operator=(Followup const &other) = default;

    /// @brief move constructor
    /// @param    other
    /// @returns object reference
    Followup &operator=(Followup &&other) = default;

    /// @brief copy constructor
    /// @param other - other object
    Followup(Followup const &other) = default;

    /// @brief move constructor
    /// @param other an existing rvalue reference.
    Followup(Followup &&) noexcept = default;

    /// @brief
    /// @return
    void Init() noexcept
    {
        _InitHeader(Type::kFollowup);
        // set twoStepFlag, only for Sync and PdelayResp
        SetFlagField(0x0U, kFollowupFlagFieldByte1);
        SetControlField(ControlFieldType::kCtrlFollowup);
        std::ignore = memset(static_cast< std::uint8_t * >(&tlvBuffer_[0]), 0, sizeof(tlvBuffer_));
    }

    /// @brief
    /// @param ts
    void SetPreciseOriginTimeStamp(internal::TimeValue const &ts) noexcept { preciseOriginTimetamp_ = ts; }

    /// @brief
    /// @return
    internal::TimeValue const &GetPreciseOriginTimeStamp() const noexcept { return preciseOriginTimetamp_; }

    /// @brief verify message according to PTP configuration.
    /// @param ptpContext PTP configuration.
    /// @returns true, verification passed; false, verification failed.
    bool Validate(std::shared_ptr< ptp::PtpContext > const &ptpContext) const noexcept;

    /// @brief assemble message according to PTP configuration.
    /// @param ptpContext PTP configuration.
    void AssembleTLV(std::shared_ptr< ptp::PtpContext > const &ptpContext) noexcept;

    /// @brief parse the Followup message, return the start position of each SubTLV.
    /// @returns list of SubTLV start addresses.
    ara::core::Vector< SubTlv const * > UnAssembleTLV() const noexcept;

    /// @brief
    /// @return
    SubTlvUserData const *GetSubTlvUserData() const noexcept;

    /// @brief
    /// @return
    SubTlvStatus const *GetSubTlvStatus() const noexcept;

    /// @brief
    /// @return
    SubTlvOFS const *GetSubTlvOFS() const noexcept;

    /// @brief
    /// @return
    SubTlvTimeSecured const *GetSubTlvTimeSecured() const noexcept;

    /// @brief print debug information
    void Debug() const noexcept
    {
#if ARA_TSYNC_DEBUG
        ;
        Header::HeaderDebug();
        std::int64_t const tsNsec{GetPreciseOriginTimeStamp().ToChrono().count() % 1000000000};
        std::cout << "PreciseOriginTimeStamp:" << std::dec
                  << std::chrono::duration_cast< std::chrono::seconds >(GetPreciseOriginTimeStamp().ToChrono()).count()
                  << "+";
        std::cout << std::right << std::setw(kTS_NUM_INT_9) << std::setfill(kTS_FILL_0) << std::dec << tsNsec
                  << std::endl;
#endif
    }

private:
    /// @name preciseOriginTimetamp_
    internal::TimeValue preciseOriginTimetamp_{};

    /// @name tlvBuffer_
    std::uint8_t tlvBuffer_[sizeof(TLVIEEE802) + sizeof(TLVAutoSar)]{};

};  // class Message
#pragma pack(pop)

}  // namespace message
}  // namespace ptp
}  // namespace internal
}  // namespace tsync
}  // namespace ara
#endif  //_ARA_TSYNC_INTERNAL_PTP_FOLLOW_UP_MESSAGE_H_
