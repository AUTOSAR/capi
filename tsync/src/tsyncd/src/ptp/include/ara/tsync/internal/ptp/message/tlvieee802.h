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
/// @file       tlvieee802.h
/// @brief
/// @details
/// @date       2022-04-18
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#ifndef _ARA_TSYNC_INTERNAL_PTP_TLVIEEE802_H_
#define _ARA_TSYNC_INTERNAL_PTP_TLVIEEE802_H_

#include <chrono>
#include <cstdint>
#include <cstring>

#include "ara/tsync/internal/ptp/message/tlv.h"

namespace ara {
namespace tsync {
namespace internal {
namespace ptp {
namespace message {

// TLV: SubTlvType/Length/Value combination.

#pragma pack(push, 1)

/// @traceid{PRS_TS_00067} The multiplicity of the AutoSarTLV header format is one.
/// @brief TLVIEEE802 IEEE802 format TLV
class TLVIEEE802 final : public TLV
{
public:
    /// @brief constructor
    TLVIEEE802() = default;

    /// @brief destructor
    ~TLVIEEE802() = default;

    /// @brief copy constructor
    /// @param    other
    /// @returns object reference
    TLVIEEE802 &operator=(TLVIEEE802 const &other) = default;

    /// @brief move constructor
    /// @param    other
    /// @returns object reference
    TLVIEEE802 &operator=(TLVIEEE802 &&other) = default;

    /// @brief copy constructor
    /// @param other - other object
    TLVIEEE802(TLVIEEE802 const &other) = default;

    /// @brief move constructor
    /// @param other an existing rvalue reference.
    TLVIEEE802(TLVIEEE802 &&) noexcept = default;

    /// @brief initialize
    void Init() noexcept
    {
        TLV::TlvInit(Organization::OrgType::kIEEE802);
        // 28
        SetLength((sizeof(*this)) - sizeof(TLV::TlvHeader));
        cumluativeScaledRateOffset_ = 0U;
        gmTimeBaseIndicator_        = 0U;
        static_cast< void >(
            memset(reinterpret_cast< std::uint8_t * >(&lastGmPhaseChange_), 0, sizeof(lastGmPhaseChange_)));
        scaledLastGmFreqChange_ = 0U;
    }

private:
    /// @name cumluativeScaledRateOffset_
    std::uint32_t cumluativeScaledRateOffset_{0U};
    /// @name gmTimeBaseIndicator_
    std::uint16_t gmTimeBaseIndicator_{0U};
    /// @brief protocol header field lastGmPhaseChange
    struct LastGmPhase final
    {
    public:
        /// @name  data
        std::uint8_t data[internal::kTS_NUM_12]{0U};
    };
    /// @name lastGmPhaseChange_
    LastGmPhase lastGmPhaseChange_{};

    /// @name scaledLastGmFreqChange_
    std::uint32_t scaledLastGmFreqChange_{0U};
};

#pragma pack(pop)

}  // namespace message
}  // namespace ptp
}  // namespace internal
}  // namespace tsync
}  // namespace ara
#endif  //_ARA_TSYNC_INTERNAL_PTP_TLVIEEE802_H_
