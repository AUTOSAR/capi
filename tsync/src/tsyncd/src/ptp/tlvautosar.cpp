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
/// @file       tlvautosar.cpp
/// @brief      PTP module followup message TLV class
/// @details
/// @date       2023-01-03
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#include <cstdint>
#include <vector>
// clang-format off
#include "ara/tsync/internal/ptp/ptpcontext.h"  //NOLINT
#include "ara/tsync/internal/ptp/message/followup.h"
#include "ara/tsync/internal/ptp/message/tlvautosar.h"
#include "isoft/util/mix.h"
// clang-format on
namespace ara {
namespace tsync {
namespace internal {
namespace ptp {
namespace message {

/// @brief encapsulate SubTlv
/// @param ptpCtx PTP context
/// @param mesg message
void TLVAutoSar::Assemble(std::shared_ptr< PtpContext > const &ptpCtx, Followup const *const mesg) noexcept
{
    std::uint32_t subTlvLengthCounter{0U};

    if (nullptr == ptpCtx) {
        return;
    }

    ptp::Configure const &ptpCfg{ptpCtx->PtpConfig()};
    std::shared_ptr< timedomain::TDContext > const timeDomainCtx{ptpCtx->GetTimeDomainContext()};

    /// @traceid{PRS_TS_00093}  Depending on the globalTimeTxCrcSecured configuration item, treat CRC support differently.
    /// @traceid{PRS_TS_00074}  The multiplicity of TimeSecured SubTLV is 1, and it is only available when CRC protection is needed.
    /// @traceid{PRS_TS_00075}   If messageCompliance is false and tlvFollowupTimeSubTLV is true,
    /// then the Master should include the TimeSecured SubTLV in the Followup message.
    if (ptpCfg.domainPtp.tlvFollowupTimeSubTLV) {
        if (ptpCfg.domainPtp.globalTimeTxCrcSecured) {
            SubTlvTimeSecured *const timsecSubTlv{new (static_cast< std::uint8_t * >(&subTlvBuffer_[0]))
                                                      SubTlvTimeSecured()};
            timsecSubTlv->Init();
            subTlvLengthCounter += sizeof(SubTlvTimeSecured);

            std::uint8_t const crcTimeFlag{ptpCfg.domainPtp.crcTimeFlagsTxSecured};
            /// @traceid{PRS_TS_00098}  Write ptpConfig.crcTimeFlagsTxSecured into crcTimeFlags.
            timsecSubTlv->SetCRCTimeFlags(crcTimeFlag);
            timsecSubTlv->CrcSet(ptpCtx->GetDataID(), mesg);
        }
    }

    /// @traceid{PRS_TS_00076}  The multiplicity of Status SubTLV is 1, and it is set to Secured or Not Secured according to CRC protection needs.
    /// @traceid{PRS_TS_00077}   If messageCompliance is false and tlvFollowupStatusSubTLV is true,
    /// then the Master should include the Status SubTLV in the Followup message.
    if (ptpCfg.domainPtp.tlvFollowupStatusSubTLV) {
        SubTlvStatus *const subTlv{new (subTlvBuffer_ + subTlvLengthCounter) SubTlvStatus()};
        subTlvLengthCounter += sizeof(SubTlvStatus);

        subTlv->Init();
        // set Status
        if (timeDomainCtx->IsStatusSynchronized()) {
            subTlv->SetStatus(0U);
        } else {
            subTlv->SetStatus(1U);
        }

        if (ptpCfg.domainPtp.globalTimeTxCrcSecured) {
            subTlv->SetSecured();
            /// @traceid{PRS_TS_00101}   If globalTimeTxCrcSecured is CRC_SUPPORTED, then CRC needs to be calculated combined with status_ and DataId.
            subTlv->CrcSet(ptpCtx->GetDataID());
        }
    }

    /// @traceid{PRS_TS_00078}  The multiplicity of UserData SubTLV is 1, and it is set to Secured or Not Secured according to CRC protection needs.
    /// @traceid{PRS_TS_00079}   If messageCompliance is false and tlvFollowupUserDataSubTLV is true,
    /// then the Master should include the UserData SubTLV in the Followup message.
    /// @traceid{PRS_TS_00081}  The UserData data should be consistent with the current read message.
    /// @traceid{PRS_TS_00082}  The UserData data should be consistently written into the next outgoing message.
    if (ptpCfg.domainPtp.tlvFollowupUserDataSubTLV) {
        SubTlvUserData *const subTlv{new (subTlvBuffer_ + subTlvLengthCounter) SubTlvUserData()};
        subTlvLengthCounter += sizeof(SubTlvUserData);
        subTlv->Init();
        // set user data
        subTlv->SetUserData(timeDomainCtx->GetUserData());

        if (ptpCfg.domainPtp.globalTimeTxCrcSecured) {
            subTlv->SetSecured();
            /// @traceid{PRS_TS_00102}
            /// If globalTimeTxCrcSecured is CRC_SUPPORTED, then CRC needs to be calculated combined with UserData related data and DataId.
            // Calculate and set crc
            subTlv->CrcSet(ptpCtx->GetDataID());
        }
    }

    /// @traceid{PRS_TS_00084}  The multiplicity of OFS SubTLV is 1, and it is set to Secured or Not Secured according to CRC protection needs.
    /// @traceid{PRS_TS_00086}   If messageCompliance is false and tsyncTLVFollowupOFSSubTLV is true,
    /// then the Master should include OFS SubTLV in the Followup message.
    if (ptpCfg.domainPtp.tsynTLVFollowupOFSSubTLV) {
        SubTlvOFS *const subTlv{new (subTlvBuffer_ + subTlvLengthCounter) SubTlvOFS()};
        subTlvLengthCounter += sizeof(SubTlvOFS);

        subTlv->Init();
        // Set OFS info
        /// @traceid{PRS_TS_00085}  OFS SubTLV should contain offset time domain Id, and its range should be 16 - 31.
        subTlv->SetOffsetDomainId(timeDomainCtx->GetDomainId());
        /// @traceid{PRS_TS_00088}  The UserData in OFS should be consistent with the current read message.
        /// @traceid{PRS_TS_00089}  The UserData in OFS should be consistently written into the next outgoing message.
        subTlv->SetUserData(timeDomainCtx->GetUserData());

        /// @traceid{PRS_TS_00094} status
        if (timeDomainCtx->IsStatusSynchronized()) {
            subTlv->SetStatus(0U);
        } else {
            subTlv->SetStatus(1U);
        }

        /// @traceid{PRS_TS_00095} Offset TimeStamp
        subTlv->SetOffsetTS(timeDomainCtx->GetOffsetTS());

        if (ptpCfg.domainPtp.globalTimeTxCrcSecured) {
            subTlv->SetSecured();
            /// @traceid{PRS_TS_00103}
            /// If globalTimeTxCrcSecured is CRC_SUPPORTED, then CRC needs to be calculated combined with OFS related data and DataId.
            subTlv->CrcSet(ptpCtx->GetDataID());
        }
    }

    /// @traceid{PRS_TS_00069}   If AutoSarTLV exists, the length of the TLV should include the size of all SubTlvs.
    // 6 + SUM(Sub-TLVs).
    SetLength(static_cast< std::uint8_t >(sizeof(TLV::Organization) + subTlvLengthCounter));

    return;
}

/// @traceid{PRS_TS_00118}   If the SubTlv Type cannot be recognized, it should be ignored and the next one parsed.
/// Implementation method: read all SubTlvs at once and put them in a list. Take whatever type is needed.
/// @brief parse SubTlv
/// @returns SubTlv.
std::vector< SubTlv const * > TLVAutoSar::UnAssemble() const noexcept
{
    std::vector< SubTlv const * > subTlvList;

    std::uint8_t const *head{static_cast< std::uint8_t const * >(subTlvBuffer_)};
    std::uint8_t const *const tail{static_cast< std::uint8_t const * >(subTlvBuffer_) + GetLength()
                                   - sizeof(TLV::Organization)};

    while (head < tail) {
        SubTlv const *subTlv{reinterpret_cast< SubTlv const * >(head)};
        subTlvList.emplace_back(subTlv);
        head += static_cast< std::size_t >(subTlv->GetLength()) + (sizeof(SubTlv));
    }

    return subTlvList;
}

}  // namespace message
}  // namespace ptp
}  // namespace internal
}  // namespace tsync
}  // namespace ara
