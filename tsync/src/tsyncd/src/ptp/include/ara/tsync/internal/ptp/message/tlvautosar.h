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
/// @file       tlvautosar.h
/// @brief
/// @details
/// @date       2022-04-18
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#ifndef _ARA_TSYNC_INTERNAL_PTP_TLVAUTOSAR_H_
#define _ARA_TSYNC_INTERNAL_PTP_TLVAUTOSAR_H_

#include <ara/core/vector.h>

#include <chrono>
#include <cstdint>

#include "ara/tsync/internal/ptp/message/subtlvofs.h"
#include "ara/tsync/internal/ptp/message/subtlvstatus.h"
#include "ara/tsync/internal/ptp/message/subtlvtimesecured.h"
#include "ara/tsync/internal/ptp/message/subtlvuserdata.h"
#include "ara/tsync/internal/ptp/message/tlv.h"
#include "ara/tsync/internal/ptp/ptpcontext.h"

namespace ara {
namespace tsync {
namespace internal {
namespace ptp {
namespace message {

// TLV: SubTlvType/Length/Value combination.

#pragma pack(push, 1)

/// @brief Followup
class Followup;

/// @brief TLVAutoSar AutoSar format TLV
class TLVAutoSar final : public TLV
{
public:
    /// @brief constructor
    TLVAutoSar() noexcept : TLV{} { Init(); }

    /// @brief destructor
    ~TLVAutoSar() = default;

    /// @brief copy constructor
    /// @param    other
    /// @returns object reference
    TLVAutoSar &operator=(TLVAutoSar const &other) = default;

    /// @brief move constructor
    /// @param    other
    /// @returns object reference
    TLVAutoSar &operator=(TLVAutoSar &&other) = default;

    /// @brief copy constructor
    /// @param other - other object
    TLVAutoSar(TLVAutoSar const &other) = default;

    /// @brief move constructor
    /// @param other an existing rvalue reference.
    TLVAutoSar(TLVAutoSar &&) noexcept = default;

    /// @brief initialize
    void Init() noexcept
    {
        TLV::TlvInit(Organization::OrgType::kAUTOSAR);
        SetLength(0U);
    }

    /// @brief check whether it contains SubTlv
    /// @returns true, contains; false, does not.
    bool HaveSubTlv() const noexcept
    {
        // > 6
        if (GetLength() > sizeof(TLV::Organization)) {
            return true;
        }
        return false;
    }

    /// @brief check whether SubTlv is configured
    /// @param    ptpCfg  ptp configuration
    /// @returns true, contains; false, does not.
    static bool HaveConfigSubTlv(ara::tsync::internal::ptp::Configure const &ptpCfg) noexcept
    {
        if ((ptpCfg.domainPtp.tlvFollowupTimeSubTLV && ptpCfg.domainPtp.globalTimeTxCrcSecured)
            || ptpCfg.domainPtp.tlvFollowupStatusSubTLV || ptpCfg.domainPtp.tlvFollowupUserDataSubTLV
            || ptpCfg.domainPtp.tsynTLVFollowupOFSSubTLV) {
            return true;
        }
        return false;
    }
    /// @brief set SubTlv length
    /// @param    ptpCfg  ptp configuration
    /// @returns SubTlv length
    void SetSubTlvLength(ara::tsync::internal::ptp::Configure const &ptpCfg) noexcept
    {
        std::uint32_t subTlvLengthCounter{0U};
        if (ptpCfg.domainPtp.tlvFollowupTimeSubTLV && ptpCfg.domainPtp.globalTimeTxCrcSecured) {
            subTlvLengthCounter += sizeof(SubTlvTimeSecured);
        }
        if (ptpCfg.domainPtp.tlvFollowupStatusSubTLV) {
            subTlvLengthCounter += sizeof(SubTlvStatus);
        }
        if (ptpCfg.domainPtp.tlvFollowupUserDataSubTLV) {
            subTlvLengthCounter += sizeof(SubTlvUserData);
        }
        if (ptpCfg.domainPtp.tsynTLVFollowupOFSSubTLV) {
            subTlvLengthCounter += sizeof(SubTlvOFS);
        }
        SetLength(static_cast< std::uint8_t >(sizeof(TLV::Organization) + subTlvLengthCounter));
    }
    /// @brief get SubTlv length
    /// @returns SubTlv length
    std::uint8_t GetSubTlvLength() const noexcept
    {
        return static_cast< std::uint8_t >(TLV::GetLength() - sizeof(TLV::Organization));
    }

    /// @brief encapsulate SubTlv
    /// @param ptpCtx PTP configuration
    /// @param mesg message
    void Assemble(std::shared_ptr< ara::tsync::internal::ptp::PtpContext > const &ptpCtx,
                  Followup const *const mesg) noexcept;

    /// @brief parse SubTlv
    /// @returns SubTlv.
    ara::core::Vector< SubTlv const * > UnAssemble() const noexcept;

private:
    /// @brief AllSubTlv
    struct AllSubTlv final
    {
        /// @name scurTme
        SubTlvTimeSecured scurTme; /* 5 */
        /// @name status
        SubTlvStatus status; /* 4 */
        /// @name userData
        SubTlvUserData userData; /* 7 */
        /// @name ofs
        SubTlvOFS ofs; /* 19*/
    };
    /// @name subTlvBuffer_
    std::uint8_t subTlvBuffer_[sizeof(AllSubTlv)]{0U};
};

#pragma pack(pop)

}  // namespace message
}  // namespace ptp
}  // namespace internal
}  // namespace tsync
}  // namespace ara
#endif  //_ARA_TSYNC_INTERNAL_PTP_TLVAUTOSAR_H_
