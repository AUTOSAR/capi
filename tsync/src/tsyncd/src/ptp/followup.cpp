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
/// @file       followup.cpp
/// @brief      PTP module followup message class
/// @details
/// @date       2023-01-03
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#include "ara/tsync/internal/ptp/message/followup.h"

#include <cstdint>
#include <vector>

#include "ara/tsync/internal/common.h"
#include "ara/tsync/internal/ptp/ptpcontext.h"
#include "isoft/util/type_cast.h"

namespace ara {
namespace tsync {
namespace internal {
namespace ptp {
namespace message {

////////////////// Message assembly //////////////////////
///////////////////// Sender's CRC check ///////////////////////////
/// @traceid{PRS_TS_00097}  CRC calculation always needs to combine DataId, DataId = DataIDList[Followup.sequenceId % 16],
/// DataIDList is given by the Followup configuration.
/// @traceid{PRS_TS_00182}  When CRC calculation involves multi-byte values, they should be performed in MSB order.
/// @traceid{PRS_TS_00184}  When CRC calculation involves multi-byte data, they should be performed in low address first order.
/// The descriptions of PRS_TS_00182/PRS_TS_184 are the same, so network byte order is itself MSB, therefore regardless of the data, as long as it is multi-byte, it should be low address first.
/// @brief assemble message according to PTP configuration.
/// @param ptpContext PTP configuration.
void Followup::AssembleTLV(std::shared_ptr< PtpContext > const &ptpContext) noexcept
{
    if (nullptr == ptpContext) {
        return;
    }

    ptp::Configure const &ptpCfg{ptpContext->PtpConfig()};
    char8_t *const ptr{reinterpret_cast< char8_t * >(std::addressof(*this))};
    std::ptrdiff_t const offset{reinterpret_cast< char8_t * >(&(this->tlvBuffer_)) - ptr};
    // IEEE 802.1as message
    /// @traceid{PRS_TS_00061}   If messageCompliance is true, SyncFollowup messages should comply with the IEEE802.1AS standard according to the configuration.
    /// Confirmed with CP, the vector is the same; AUTOSAR follow-up is a standard 802.1as follow-up message + autosar-followup tlv
    ///  autosar follow-up contains 802.1as follow-up tlv

    {
        TLVIEEE802 *const tlvIEEE{
            isoft::util::PointerCast< TLVIEEE802, std::uint8_t >(static_cast< std::uint8_t * >(&tlvBuffer_[0]))};
        tlvIEEE->Init();
        // 76
        // 44 + 32
        std::size_t const msgLen{static_cast< std::size_t >(offset) + (sizeof(TLVIEEE802))};
        SetMessageLength(static_cast< std::uint16_t >(msgLen));
        SetTransportSpecific();

        // Unit is seconds
        SetLogMessageInterval(static_cast< std::int8_t >(log2(ptpCfg.domainPtp.globalTimeTxPeriod)));
    }
    if (!ptpCfg.domainPtp.messageCompliance) {
        // AUTOSAR message
        /// @traceid{PRS_TS_00062}   If messageCompliance is false, SyncFollowup messages should comply with the AUTOSAR standard according to the configuration.
        /// @traceid{PRS_TS_00092}   If messageCompliance is false, the Master should add AUTOSAR TLV to the Followup.
        /// @traceid{PRS_TS_00091}   If messageCompliance is false, CRC support should be considered.
        /// @traceid{PRS_TS_00063}   If messageCompliance is false, Followup messages should contain AUTOSAR TLV according to the configuration.
        /// @traceid{PRS_TS_00070}   If SubTLVs exist, they should be placed after the AUTOSAR TLV.
        /// @traceid{PRS_TS_00071}   If multiple SubTLVs exist, they should be placed consecutively with no gaps.
        /// @traceid{PRS_TS_00072}   If multiple SubTLVs exist, their order is arbitrary.
        TLVAutoSar *const tlvAutoSar{
            isoft::util::PointerCast< TLVAutoSar, std::uint8_t >(tlvBuffer_ + sizeof(TLVIEEE802))};
        tlvAutoSar->Init();

        /// FIXME: Need to resolve the conflict with {PRS_TS_00092}. When SubTLV does not exist, must AUTOSAR TLV be included?
        /// @traceid{PRS_TS_00068}   If AUTOSAR TLV exists, there should be at least one SubTLV.
        if (!TLVAutoSar::HaveConfigSubTlv(ptpContext->PtpConfig())) {
            // The calculated message size does not include the TLV, indicating no TLV
            SetMessageLength(static_cast< std::uint16_t >(static_cast< std::size_t >(offset) + sizeof(TLVIEEE802)));
        } else {
            /// @traceid{PRS_TS_00066}  The messageLength in the Followup message header should include all TLVs.
            tlvAutoSar->SetSubTlvLength(ptpContext->PtpConfig());
            std::size_t const messageLength{
                sizeof(TLVIEEE802) + static_cast< std::size_t >(offset) /* 44 */
                + sizeof(TLV)                                           /* 10 */
                + tlvAutoSar->GetSubTlvLength()                         /* SUM(SubTLVs), max 35 */
            };
            SetMessageLength(static_cast< std::uint16_t >(messageLength));
        }
        /// Need to set the length first before encapsulation; CRC check requires the length value
        tlvAutoSar->Assemble(ptpContext, this);
    }
    return;
}

/// @brief parse the Followup message, return the start position of each SubTLV.
/// @returns list of SubTLV start addresses.
std::vector< SubTlv const * > Followup::UnAssembleTLV() const noexcept
{
    TLV const *const tlv{isoft::util::PointerCast< TLV, std::uint8_t >(tlvBuffer_ + sizeof(TLVIEEE802))};

    if (true != tlv->IsOrganizationAUTOSAR()) {
        return std::vector< SubTlv const * >{};
    }

    TLVAutoSar const *const tlvAutoSar{
        isoft::util::PointerCast< TLVAutoSar, std::uint8_t >(tlvBuffer_ + sizeof(TLVIEEE802))};
    if (!tlvAutoSar->HaveSubTlv()) {
        return std::vector< SubTlv const * >{};
    }

    return tlvAutoSar->UnAssemble();
}

///////////////////////// Receiver's CRC check //////////////////////
/// @traceid{PRS_TS_00112}  DataId = DataIDList[Followup.sequenceId % 16], DataIDList is given by the Followup configuration.
/// @traceid{PRS_TS_00183}  When CRC calculation involves multi-byte values, they should be performed in MSB order.
/// @traceid{PRS_TS_00185}  When CRC calculation involves multi-byte data, they should be performed in low address first order.
/// The descriptions of PRS_TS_00183/PRS_TS_185 are the same, so network byte order is itself MSB, therefore regardless of the data, as long as it is multi-byte, it should be low address first.
/// @brief verify message according to PTP configuration.
/// @param ptpContext PTP configuration.
/// @returns true, verification passed; false, verification failed.
bool Followup::Validate(std::shared_ptr< ptp::PtpContext > const &ptpContext) const noexcept
{
    if (nullptr == ptpContext) {
        return false;
    }

    ptp::Configure const &ptpCfg{ptpContext->PtpConfig()};
    SubTlvTimeSecured const *tlvTimeSec{nullptr};
    SubTlvStatus const *tlvStatus{nullptr};
    SubTlvUserData const *tlvUserData{nullptr};
    SubTlvOFS const *tlvOfs{nullptr};

    /// @traceid{PRS_TS_00119} Validation rules:
    /// TODO(person in charge): implement domain validation

    /// @traceid{PRS_TS_00105} If MessageCompliance is FALSE, then rxCrcValidated should be considered.
    /// @traceid{PRS_TS_00107} According to the rxCrcValidated flag and the TLV type, the CRC of the TLV in Followup should be verified.
    /// @traceid{PRS_TS_00108} If the rxCrcValidated flag is CRC_IGNORED, the CRC should be ignored.
    /// @traceid{PRS_TS_00109} If the rxCrcValidated flag is CRC_OPTIONAL, choose whether to verify based on the security type.

    /// @traceid{PRS_TS_00157}
    /// @traceid{PRS_TS_00113}
    /// @traceid{PRS_TS_00114}
    /// FIXME:.
    /// The above three RSs have issues. Which items need to participate in CRC calculation when assembling a message is determined by the crcTimeFlags from ptpConfig.globalTimeTxCrcSecured and included in the mesg.
    /// However, during reception verification, they are determined by the crc-related flags in ptpConfig respectively. If these two sides differ, verification will fail.
    if (true == ptpCfg.domainPtp.messageCompliance) {
        return true;
    }

    switch (ptpCfg.domainPtp.rxCrcValidated) {
        /// In the case of kCRC_IGNORED, regardless of the security type, all CRC verifications are ignored.
        case internal::config::GlobalTimeCrcValidation::kCrcIgnord: {
            break;
        }

            /// FIXME: Is kCRC_OPTIONAL the same as kCRC_NOT_VALIDATED?
            /// In the case of kCRC_NOT_VALIDATED, if it is a security type, an error is reported.
        case internal::config::GlobalTimeCrcValidation::kCrcNotValidated: {
            tlvTimeSec = GetSubTlvTimeSecured();
            if (nullptr != tlvTimeSec) {
                return false;
            }

            tlvStatus = GetSubTlvStatus();
            if (nullptr != tlvStatus) {
                if (true == tlvStatus->IsSecured()) {
                    return false;
                }
            }

            tlvUserData = GetSubTlvUserData();
            if (nullptr != tlvUserData) {
                if (true == tlvUserData->IsSecured()) {
                    return false;
                }
            }

            tlvOfs = GetSubTlvOFS();
            if (nullptr != tlvOfs) {
                if (true == tlvOfs->IsSecured()) {
                    return false;
                }
            }
            break;
        }

            /// In the case of kCRC_OPTIONAL, if it is a security type, verify; otherwise, do not verify.
        case internal::config::GlobalTimeCrcValidation::kCrcOptional: {
            tlvTimeSec = GetSubTlvTimeSecured();
            if (nullptr != tlvTimeSec) {
                // If verification fails, discard the packet.
                if (true != tlvTimeSec->CrcCheck(ptpContext->GetDataID(), this)) {
                    return false;
                }
            }

            tlvStatus = GetSubTlvStatus();
            if (nullptr != tlvStatus) {
                // Verify only if it is a security type.
                if (true != tlvStatus->IsSecured()) {
                    break;
                }
                // If verification fails, discard the packet.
                if (true != tlvStatus->CrcCheck(ptpContext->GetDataID())) {
                    return false;
                }
            }

            tlvUserData = GetSubTlvUserData();
            if (nullptr != tlvUserData) {
                // Verify only if it is a security type.
                if (true != tlvUserData->IsSecured()) {
                    break;
                }
                // If verification fails, discard the packet.
                if (true != tlvUserData->CrcCheck(ptpContext->GetDataID())) {
                    return false;
                }
            }

            tlvOfs = GetSubTlvOFS();
            if (nullptr != tlvOfs) {
                // Verify only if it is a security type.
                if (true != tlvOfs->IsSecured()) {
                    break;
                }
                // If verification fails, discard the packet.
                if (true != tlvOfs->CrcCheck(ptpContext->GetDataID())) {
                    return false;
                }
            }
            break;
        }

            /// In the case of kCRC_VALIDATED, if it is not a security type, an error is reported.
        case internal::config::GlobalTimeCrcValidation::kCrcValidated: {
            tlvTimeSec = GetSubTlvTimeSecured();
            if (nullptr != tlvTimeSec) {
                // If verification fails, discard the packet.
                if (true != tlvTimeSec->CrcCheck(ptpContext->GetDataID(), this)) {
                    return false;
                }
            }

            tlvStatus = GetSubTlvStatus();
            if (nullptr != tlvStatus) {
                // If it is not a security type, consider verification failed and discard the packet.
                if (true != tlvStatus->IsSecured()) {
                    return false;
                }
                // If verification fails, discard the packet.
                if (true != tlvStatus->CrcCheck(ptpContext->GetDataID())) {
                    return false;
                }
            }

            tlvUserData = GetSubTlvUserData();
            if (nullptr != tlvUserData) {
                // If it is not a security type, consider verification failed and discard the packet.
                if (true != tlvUserData->IsSecured()) {
                    return false;
                }
                // If verification fails, discard the packet.
                if (true != tlvUserData->CrcCheck(ptpContext->GetDataID())) {
                    return false;
                }
            }

            tlvOfs = GetSubTlvOFS();
            if (nullptr != tlvOfs) {
                // If it is not a security type, consider verification failed and discard the packet.
                if (true != tlvOfs->IsSecured()) {
                    return false;
                }
                // If verification fails, discard the packet.
                if (true != tlvOfs->CrcCheck(ptpContext->GetDataID())) {
                    return false;
                }
            }
            break;
        }

        default: {
            break;
        }
    }

    /// @traceid{PRS_TS_00119} OfsTimeNSec range 0..999999999
    tlvOfs = GetSubTlvOFS();
    if (nullptr != tlvOfs) {
        if (tlvOfs->GetOffsetTS().GetNanoSecond() > kOfsTimeNSecMax) {
            return false;
        }
    }
    return true;
}

/// @brief
/// @return
SubTlvUserData const *Followup::GetSubTlvUserData() const noexcept
{
    std::vector< SubTlv const * > subTlvList;
    SubTlvUserData const *sub{nullptr};

    subTlvList = UnAssembleTLV();
    for (auto const &itm : subTlvList) {
        bool const bUserDataSecured{SubTlv::SubTlvType::kUserDataSecured == itm->GetType()};
        if ((SubTlv::SubTlvType::kUserData == itm->GetType()) || (bUserDataSecured)) {
            sub = isoft::util::PointerCast< SubTlvUserData, SubTlv >(itm);
            break;
        }
    }

    return sub;
}

/// @brief
/// @return
SubTlvStatus const *Followup::GetSubTlvStatus() const noexcept
{
    std::vector< SubTlv const * > subTlvList;
    SubTlvStatus const *sub{nullptr};

    subTlvList = UnAssembleTLV();
    for (auto const &itm : subTlvList) {
        bool const bStatusSecured{SubTlv::SubTlvType::kStatusSecured == itm->GetType()};
        if ((SubTlv::SubTlvType::kStatus == itm->GetType()) || (bStatusSecured)) {
            sub = isoft::util::PointerCast< SubTlvStatus, SubTlv >(itm);
            break;
        }
    }

    return sub;
}

/// @brief
/// @return
SubTlvOFS const *Followup::GetSubTlvOFS() const noexcept
{
    std::vector< SubTlv const * > subTlvList;
    SubTlvOFS const *sub{nullptr};

    subTlvList = UnAssembleTLV();
    for (std::vector< SubTlv const * >::const_iterator itm{subTlvList.cbegin()}; itm != subTlvList.cend(); ++itm) {
        bool const bOfsSecured{SubTlv::SubTlvType::kOfsSecured == (*itm)->GetType()};
        if ((SubTlv::SubTlvType::kOfs == (*itm)->GetType()) || (bOfsSecured)) {
            sub = isoft::util::PointerCast< SubTlvOFS, SubTlv >((*itm));
            break;
        }
    }

    return sub;
}

/// @brief
/// @return
SubTlvTimeSecured const *Followup::GetSubTlvTimeSecured() const noexcept
{
    std::vector< SubTlv const * > subTlvList;
    SubTlvTimeSecured const *sub{nullptr};

    subTlvList = UnAssembleTLV();
    for (std::vector< SubTlv const * >::const_iterator itm{subTlvList.cbegin()}; itm != subTlvList.cend(); ++itm) {
        if (SubTlv::SubTlvType::kTimeSecured == (*itm)->GetType()) {
            sub = isoft::util::PointerCast< SubTlvTimeSecured, SubTlv >((*itm));
            break;
        }
    }

    return sub;
}

}  // namespace message
}  // namespace ptp
}  // namespace internal
}  // namespace tsync
}  // namespace ara
