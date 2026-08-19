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
/// @file       dtc_information.h
/// @brief      This file provides the definition of DtcInformation and related types.
/// @details
/// @date       2024-12-09
/// @author     zhanglipeng
/// @version    1.2.0
///
/// ================================================================

#ifndef __ARA_DIAG_FAULT_DTC_INFORMATION_H__
#define __ARA_DIAG_FAULT_DTC_INFORMATION_H__
#include <isoft/uds/fault_management/dtc_information_interface.h>

#include "gen_code/dtc_information/serviceAgent/dtc_information_agent.h"

namespace ara {
namespace diag {
namespace dmd {

/// @brief monitor interface
class DtcInformation : public isoft::uds::server::DtcInformationInterface
{
public:
    DtcInformation(DtcInformation const&) = delete;
    DtcInformation(DtcInformation&&)      = delete;
    DtcInformation& operator=(DtcInformation const&) = delete;
    DtcInformation& operator=(DtcInformation&&) = delete;
    explicit DtcInformation(uint32_t const& serviceInstanceId) noexcept;
    ~DtcInformation() noexcept override = default;

public:
    /// @brief Notify DTC status changes
    /// @param dtcStatus DTC status
    /// @return Returns 0 for success
    std::int32_t NotifyDTCStatusChanged(isoft::uds::server::DTCStatusChangedType const& dtcStatus) override;

    /// @brief Notify snapshot data changes
    /// @param dtcSnapshot Snapshot
    /// @return Returns 0 for success
    std::int32_t NotifySnapshotRecordUpdated(isoft::uds::server::SnapshotRecordUpdatedType const& dtcSnapshot) override;

    /// @brief Notify changes in the number of entities stored in the database
    /// @param numberOfStoredEntries Number of entities
    /// @return Returns 0 for success
    std::int32_t NotifyNumberOfStoredEntries(std::uint32_t const numberOfStoredEntries) override;

    /// @brief Notify whether the database has overflowed
    /// @param isOverflow Overflow flag
    /// @return Returns 0 for success
    std::int32_t NotifyEventMemoryOverflow(bool const isOverflow) override;

    /// @brief Notify changes in the flag controlling DTC
    /// @param dtcStatusType Flag
    /// @return Returns 0 for success
    std::int32_t NotifyControlDtcStatus(isoft::uds::server::ControlDtcStatusType const dtcStatusType) override;

    /// @brief Register callback for AA side to get the number of entities stored in the database
    /// @param callBack Callback function
    /// @return Returns 0 for success
    std::int32_t RegisterGetNumberOfStoredEntries(
        isoft::uds::server::GetNumberOfStoredEntriesCallBack const& callBack) override;

    /// @brief Register callback for AA side to clear DTC
    /// @param callBack Callback function
    /// @return Returns 0 for success
    std::int32_t RegisterClear(isoft::uds::server::ClearCallBack const& callBack) override;

    /// @brief Register callback for AA side to get the control status of DTC
    /// @param callBack Callback function
    /// @return Returns 0 for success
    std::int32_t RegisterGetControlDTCStatus(isoft::uds::server::GetControlDTCStatusBack const& callBack) override;

    /// @brief Register callback for AA side to restore DTC status changes
    /// @param callBack Callback function
    /// @return Returns 0 for success
    std::int32_t RegisterEnableControlDtc(isoft::uds::server::EnableControlDtcCallBack const& callBack) override;

    /// @brief Register callback for AA side to get the current status code of DTC
    /// @param callBack Callback function
    /// @return Returns 0 for success
    std::int32_t RegisterGetCurrentStatus(isoft::uds::server::GetCurrentStatusCallBack const& callBack) override;

    /// @brief Register callback for AA side to get the database overflow flag
    /// @param callBack Callback function
    /// @return Returns 0 for success
    std::int32_t RegisterGetEventMemoryOverflow(
        isoft::uds::server::GetEventMemoryOverflowCallBack const& callBack) override;

    /// @brief Register callback for AA side to set DTC suppression status
    /// @param callBack Callback function
    /// @return
    std::int32_t RegisterSetDtcSuppression(isoft::uds::server::SetDtcSuppressionCallback const& callBack) override;

    /// @brief Register callback for AA side to get DTC suppression status
    /// @param callBack
    /// @return
    std::int32_t RegisterGetDtcSuppression(isoft::uds::server::GetDtcSuppressionCallback const& callBack) override;

private:
    std::unique_ptr< isoft::dm::dis::DtcInformationAgent > dtcInformationAgent_;

    isoft::uds::server::GetNumberOfStoredEntriesCallBack getNumberOfStoredEntriesCallBack_{nullptr};
    isoft::uds::server::ClearCallBack clearCallBack_{nullptr};
    isoft::uds::server::GetControlDTCStatusBack getControlDTCStatusBack_{nullptr};
    isoft::uds::server::EnableControlDtcCallBack enableControlDtcCallBack_{nullptr};
    isoft::uds::server::GetCurrentStatusCallBack getCurrentStatusCallBack_{nullptr};
    isoft::uds::server::GetEventMemoryOverflowCallBack getEventMemoryOverflowCallBack_{nullptr};
    isoft::uds::server::SetDtcSuppressionCallback setDtcSuppressionCallback_{nullptr};
    isoft::uds::server::GetDtcSuppressionCallback getDtcSuppressionCallback_{nullptr};
};

}  // namespace dmd
}  // namespace diag
}  // namespace ara

#endif  // DM_UDS_GENERIC_UDS_SERVICE_H_