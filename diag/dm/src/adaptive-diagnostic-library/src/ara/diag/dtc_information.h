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
/// @brief      This file provides the definitions of DTCInformation and related types.
/// @details
/// @date       2021-11-23
/// @author     jiawei
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_DIAG_DTC_INFORMATION_H_
#define ARA_DIAG_DTC_INFORMATION_H_

#include <ara/core/instance_specifier.h>
#include <ara/core/result.h>
#include <ara/core/vector.h>

#include <cstdint>
#include <functional>
#include <memory>

namespace isoft {
namespace dm {
namespace dic {
class DtcInformationAgent;
}  // namespace dic
}  // namespace dm
}  // namespace isoft

namespace ara {
namespace diag {

/// @brief UDS DTC status bits according to ISO 14229-1
///
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
///
/// @traceid{SWS_DM_00658}@tracestatus{draft}
enum class UdsDtcStatusBitType : std::uint8_t
{
    kTestFailed                         = 0x01,  ///< bit 0: TestFailed
    kTestFailedThisOperationCycle       = 0x02,  ///< bit 1: TestFailedThisOperationCycle
    kPendingDTC                         = 0x04,  ///< bit 2: PendingDTC
    kConfirmedDTC                       = 0x08,  ///< bit 3: ConfirmedDTC
    kTestNotCompletedSinceLastClear     = 0x10,  ///< bit 4: TestNotCompletedSinceLastClear
    kTestFailedSinceLastClear           = 0x20,  ///< bit 5: TestFailedSinceLastClear
    kTestNotCompletedThisOperationCycle = 0x40,  ///< bit 6: TestNotCompletedThisOperationCycle
    kWarningIndicatorRequested          = 0x80   ///< bit 7: WarningIndicatorRequested
};

/// @brief Type for ControlDTCStatus status as requested by UDS service 0x85 ControlDTCSetting.
///
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
///
/// @traceid{SWS_DM_00663}@tracestatus{draft}
enum class ControlDtcStatusType : uint8_t
{
    kDTCSettingOn  = 0x00,  ///<  Updating of diagnostic trouble code status bits is under normal operating conditions
    kDTCSettingOff = 0x01   ///<  Updating of diagnostic trouble code status bits is stopped
};

namespace api {
/// @brief Declare DTCInformationProxyWrapper
class DTCInformationProxyWrapper;
}  // namespace api

/// @brief Class to implement operations on DTC informations per configured DiagnosticMemoryDestination.
///
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
///
/// @traceid{SWS_DM_00657}@tracestatus{draft}
class DTCInformation
{
public:
    /// @brief Type for UDS DTC status byte.
    ///
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    ///
    /// @traceid{SWS_DM_00659}@tracestatus{draft}
    struct UdsDtcStatusByteType
    {
        std::uint8_t udsDtcStatusByteType_{0U};  /// NOLINT
    };

    /// @brief Type for SnapshotDataIdentifierType status
    ///
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    ///
    /// @traceid{SWS_DM_00660}@tracestatus{draft}
    struct SnapshotDataIdentifierType
    {
        /// @name dataIdentifier
        std::uint16_t dataIdentifier;
        ara::core::Vector< std::uint8_t >
            /// @name data
            data;
    };

    /// @brief Type for SnapshotDataRecordType status
    ///
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    ///
    /// @traceid{SWS_DM_00661}@tracestatus{draft}
    struct SnapshotDataRecordType
    {
        /// @name snapshotRecordNumber
        std::uint8_t snapshotRecordNumber;
        ara::core::Vector< SnapshotDataIdentifierType >
            /// @name snapshotDataIdentifiers
            snapshotDataIdentifiers;
    };

    /// @brief Type for SnapshotRecordUpdatedType status
    ///
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    ///
    /// @traceid{SWS_DM_00662}@tracestatus{draft}
    struct SnapshotRecordUpdatedType
    {
        /// @name DTC
        /// DTC which is changed
        uint32_t DTC;  /// NOLINT
        ara::core::Vector< SnapshotDataRecordType >
            /// @name ToBeReplaced
            ToBeReplaced;  // NOLINT
    };

    /// @brief Type for DTC Suppression.
    ///
    /// @artraceid{SWS_DM_01294}@artracestatus{draft}
    /// @aruptrace{RS_Diag_04220}
    enum class DtcSuppressionType
    {
        kDTCSuppressionOff = 0x00,  ///<  suppression status off
        kDTCSuppressionOn  = 0x01,  ///<  suppression status on
    };

    /// @brief Constructor for a DTCInformation instance which allows for DTC related operation per
    /// DiagnosticMemoryDestination.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] specifier InstanceSpecifier to an PortPrototype of an DiagnosticDTCInformationInterface
    /// @throws on overflow
    ///
    /// @traceid{SWS_DM_00664}@tracestatus{draft}
    explicit DTCInformation(ara::core::InstanceSpecifier const& specifier);

    /// @brief Destructor of class DTCInformation
    ///
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    ///
    /// @traceid{SWS_DM_00665}@tracestatus{draft}
    ~DTCInformation() noexcept = default;

    /// @brief copy constructor
    /// @param other
    DTCInformation(DTCInformation const& other) noexcept = default;
    /// @brief copy assignment operator
    /// @param other
    /// @return DTCInformation&
    DTCInformation& operator=(DTCInformation const& other) noexcept = default;
    /// @brief move constructor
    /// @param other
    DTCInformation(DTCInformation&& other) noexcept = default;
    /// @brief move assignment operator
    /// @param other
    /// @return DTCInformation&
    DTCInformation& operator=(DTCInformation&& other) noexcept = default;

    /// @brief Retrieves the current UDS DTC status byte of the given DTC identifier.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] dtc DTC indentifier for which the status should be retrieved.
    /// @return the current UDS DTC status byte of the given DTC identifier.
    /// error code: kNoSuchDtc
    ///
    ///
    /// @traceid{SWS_DM_00666}@tracestatus{draft}
    ara::core::Result< UdsDtcStatusByteType > GetCurrentStatus(std::uint32_t dtc);

    /// @brief Register a notifier function which is called if a UDS DTC status is changed.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] notifier The function to be called if a DTC status has changed.
    /// @return ara::core::Result<void> error code: kNoSuchDtc, kInvalidArgument
    /// @throws on overflow
    ///
    /// @traceid{SWS_DM_00667}@tracestatus{draft}
    ara::core::Result< void > SetDTCStatusChangedNotifier(
        std::function< void(std::uint32_t dtc,
                            UdsDtcStatusByteType udsStatusByteOld,
                            UdsDtcStatusByteType udsStatusByteNew) > notifier);

    /// @brief Register a notifier function which is called if the SnapshotRecord is changed.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] notifier The function to be called if the SnapshotRecord is changed.
    /// @return ara::core::Result<void>  error code: kInvalidArgument
    /// @throws on overflow
    ///
    /// @traceid{SWS_DM_00668}@tracestatus{draft}
    ara::core::Result< void > SetSnapshotRecordUpdatedNotifier(
        std::function< void(SnapshotRecordUpdatedType) > notifier);

    /// @brief Contains the number of currently stored fault memory entries.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @return Number of currently stored fault memory entries.
    ///
    ///
    /// @traceid{SWS_DM_00669}@tracestatus{draft}
    ara::core::Result< std::uint32_t > GetNumberOfStoredEntries();

    /// @brief Register a notifier function which is called if the number of currently stored fault memory entries
    /// changed.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] notifier The function to be called if the number of entries for this diagnostic event memory instance
    /// has changed.
    /// @return ara::core::Result<void> error code: kInvalidArgument
    /// @throws on overflow
    ///
    /// @traceid{SWS_DM_00670}@tracestatus{draft}
    ara::core::Result< void > SetNumberOfStoredEntriesNotifier(std::function< void(std::uint32_t) > notifier);

    /// @brief Contains the current event memory overflow status.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @return Current status of event memory overflow.
    ///
    ///
    /// @traceid{SWS_DM_00919}@tracestatus{draft}
    ara::core::Result< bool > GetEventMemoryOverflow();

    /// @brief Register a notifier function which is called if the current event memory overflow status changed.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] notifier The function to be called if the overflow status for this diagnostic event memory instance
    /// has changed.
    /// @return ara::core::Result<void> error code: kInvalidArgument
    /// @throws on overflow
    ///
    /// @traceid{SWS_DM_00918}@tracestatus{draft}
    ara::core::Result< void > SetEventMemoryOverflowNotifier(std::function< void(bool) > notifier);

    /// @brief Method for Clearing a DTC or a group of DTCs.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] dtcGroup DTC group to be cleared.
    /// @return void or errors : kBusy, kFailed, kMemoryError, kWrongDtc
    /// @throws on overflow
    ///
    /// @traceid{SWS_DM_00671}@tracestatus{draft}
    ara::core::Result< void > Clear(std::uint32_t dtcGroup);

    /// @brief Contains the current status of the ControlDTCStatus
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @return The current status of ControlDtcStatus (related to UDS service 0x85)
    ///
    ///
    /// @traceid{SWS_DM_00672}@tracestatus{draft}
    ara::core::Result< ControlDtcStatusType > GetControlDTCStatus();

    /// @brief Registers a notifier function which is called if the control DTC setting is changed.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] notifier The function to be called if the ControlDTCStatus (related to UDS service 0x85) for this
    /// diagnostic memory instance has changed.
    /// @return error code: kInvalidArgument
    /// @throws on overflow
    ///
    /// @traceid{SWS_DM_00673}@tracestatus{draft}
    ara::core::Result< void > SetControlDtcStatusNotifier(std::function< void(ControlDtcStatusType) > notifier);

    /// @brief Enforce restoring ControlDTCStatus setting to enabled in case the monitor has some conditions or states
    /// demands to do so.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @return ara::core::Result<void>
    /// @throws on overflow
    ///
    /// @traceid{SWS_DM_00674}@tracestatus{draft}
    ara::core::Result< void > EnableControlDtc();

    /// @brief Contains the current status of DTC Suppression.
    /// @param[in] dtc  DTC indentifier for which the suppression status should be
    /// retrieved.
    /// @returns the current status of DTC Suppression.
    /// @arerror DiagErrorDomain::DiagErrc::kWrongDtc wrong DTC number requested.
    /// @arerror ara::diag::DiagErrc::kServiceNotAvailable
    ///
    /// @artraceid{SWS_DM_01293}@artracestatus{draft}
    /// @aruptrace{RS_AP_00139}
    /// @aruptrace{RS_Diag_04220}
    ///
    ara::core::Result< DtcSuppressionType > GetDtcSuppression(std::uint32_t dtc);

    /// @brief Set the status of DTC Suppression.
    /// @param[in] dtc  DTC indentifier for which the DTC Suppression status should be
    /// set.
    /// @param[in] suppressionStatus  DTC Suppression status that should be set.
    /// @arerror DiagErrorDomain::DiagErrc::kWrongDtc wrong DTC number requested.
    /// @arerror DiagErrorDomain::DiagErrc::kSuppressionIgnored Suppression request
    /// ignored.
    ///
    /// @artraceid{SWS_DM_01292}@artracestatus{draft}
    /// @aruptrace{RS_AP_00139}
    /// @aruptrace{RS_Diag_04220}
    ///
    ara::core::Result< void > SetDtcSuppression(std::uint32_t dtc, DtcSuppressionType suppressionStatus);

private:
    std::shared_ptr< isoft::dm::dic::DtcInformationAgent >
        /// @name proxy_
        proxy_;
};
}  // namespace diag
}  // namespace ara

#endif  // ARA_DIAG_DTC_INFORMATION_H_