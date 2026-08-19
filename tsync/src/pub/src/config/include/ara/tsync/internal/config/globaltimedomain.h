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
/// @file       globaltimedomain.h
/// @brief      global time domain configuration management class
/// @details
/// @date       2023-01-09
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// export_level=/TimeSync/Config
/// module_path=/TimeSync/Config
/// @endcode
///
/// ================================================================

#ifndef ARA_TSYNC_INTERNAL_CONFIG_GLOBALTIMEDOMAIN_H_
#define ARA_TSYNC_INTERNAL_CONFIG_GLOBALTIMEDOMAIN_H_

#include <ara/core/string.h>
#include <ara/core/string_view.h>
#include <isoft/manifestreader/manifest_node.h>

#include <cstdint>
#include <cstring>
// clang-format off
#include "ara/tsync/internal/TimeDomainId.h"
#include "ara/tsync/internal/config/common.h"
#include "ara/tsync/internal/config/crcflags.h"
#include "ara/tsync/internal/config/globaltimecorrectionprops.h"
#include "ara/tsync/internal/config/managedcouplingport.h"          // NOLINT
#include "ara/tsync/internal/config/globaltimedomainproperty.h"     // NOLINT
#include "ara/tsync/internal/config/globaltimegateway.h"            // NOLINT
#include "ara/tsync/internal/config/subtlvconfig.h"                 // NOLINT
#include "ara/tsync/internal/config/globaltimemaster.h"             // NOLINT
#include "ara/tsync/internal/config/globaltimeslave.h"              // NOLINT
// clang-format on
#include "ara/tsync/internal/common.h"

namespace ara {
namespace tsync {
namespace internal {

namespace config {

/// @brief GlobalTimeDomain
class GlobalTimeDomain final
{
public:
    /// @brief read configuration
    /// @param node - configuration node
    /// @return 0 - success
    std::int32_t ManifestLoader(isoft::manifestreader::ManifestNode const &node) noexcept;

    /// @brief print debug information
    void Debug() const noexcept;

    /// @brief get time domain name
    /// @returns time domain name
    ara::core::String const &GetName() const noexcept { return name_; }

    /// @brief get master time domain configuration
    /// @returns reference to master time domain configuration
    GlobalTimeMaster const &GetMaster() const noexcept { return globalTimeMaster_; }

    /// @brief get slave time domain configuration
    /// @param slaveName - slave time name
    /// @returns reference to slave time domain configuration
    GlobalTimeSlave const *GetSlave(ara::core::StringView const &slaveName) const noexcept
    {
        GlobalTimeSlave const *sp{nullptr};
        for (ara::core::Vector< GlobalTimeSlave >::const_iterator itm{slave_.cbegin()}; itm != slave_.cend(); ++itm) {
            ara::core::StringView const sv{itm->GetName().data(), itm->GetName().size()};
            if (slaveName == sv) {
                sp = &(*itm);
                break;
            }
        }
        return sp;
    }

    /// @brief get slave time domain configuration
    /// @param slaveName - slave time name
    /// @returns reference to slave time domain configuration
    GlobalTimeSlave const *GetSlave(ara::core::String const &slaveName) const noexcept
    {
        ara::core::StringView const sv{slaveName.data(), slaveName.size()};
        return GetSlave(sv);
    }

    /// @brief get whether Master is configured on the current machine
    /// @returns whether Master is configured on the current machine
    bool GetIsMasterOnThisMachine() const noexcept { return isMasterOnThisMachine_; }

    /// @brief minimum interval between two Sync packets
    /// @returns minimum interval between two Sync packets
    double GetDebounceTime() const noexcept { return debounceTime_; }

    /// @brief get domainid
    /// @returns domainid
    TimeDomainId const &GetDomainId() const noexcept { return domainId_; }

    /// @brief get global time correction parameters
    /// @returns global time correction parameters
    GlobalTimeCorrectionProps const &GetGlobalTimeCorrectionProps() const noexcept
    {
        return globalTimeCorrectionProps_;
    }

    /// @brief get global time domain properties
    /// @returns global time domain properties
    GlobalTimeDomainProperty const &GetGlobalTimeDomainProperty() const noexcept { return globalTimeDomainProperty_; }

    /// @brief get time domain master configuration
    /// @returns time domain master configuration
    GlobalTimeMaster const &GetGlobalTimeMaster() const noexcept { return globalTimeMaster_; }

    /// @brief get offset domain name
    /// @returns offset domain name
    ara::core::String const &GetOffsetTimeDomain() const noexcept { return offsetTimeDomain_; }

    /// @brief get slave configuration
    /// @returns slave configuration
    ara::core::Vector< GlobalTimeSlave > const &GetSlave() const noexcept { return slave_; }

    /// @brief get timeout for detecting sync packet loss on slave side
    /// @returns timeout for detecting sync packet loss on slave side
    double GetSyncLossTimeout() const noexcept { return syncLossTimeout_; }

    /// @brief set whether Master is configured on the current machine
    /// @param ismaster - whether Master is configured on the current machine
    void SetIsMasterOnThisMachine(bool const ismaster) noexcept { isMasterOnThisMachine_ = ismaster; }

    /// @brief get timestamp type
    /// @returns timestamp type
    TSTimeStampTypeEnum GetTimeStampType() const noexcept { return timeStampType_; }

    /// @brief get default propagation delay value
    /// @returns default propagation delay value
    double GetGlobalTimePropagationDelay() const noexcept { return globalTimePropagationDelay_; }

    /// @brief get whether Master/Slave conflict detection and notification is enabled
    /// @returns whether Master/Slave conflict detection and notification is enabled
    bool GetMasterSlaveConflictDetection() const noexcept { return masterSlaveConflictDetection_; }

    /// @brief
    /// Get which elements in the Followup message need to participate in CRC calculation when sending, see [PRS_TS_00098]. Controlled by the globalTimeTxCrcSecured master switch.
    /// @returns which elements in the Followup message need to participate in CRC calculation when sending
    std::uint8_t GetCrcTimeFlagsTxSecured() const noexcept { return crcTimeFlagsTxSecured_; }

    /// @brief
    /// Get which elements in the Followup message need to participate in CRC calculation when sending, see [PRS_TS_00098]. Controlled by the globalTimeTxCrcSecured master switch.
    /// @returns which elements in the Followup message need to participate in CRC calculation when sending
    std::uint16_t GetGlobalTimeSequenceCounterJumpWidth() const noexcept { return globalTimeSequenceCounterJumpWidth_; }

    /// @brief get whether configured as a stable clock
    /// @returns whether configured as a stable clock
    bool IsSteadyClock() const noexcept { return steadyClock_; }

    /// @brief whether to synchronize the global clock to the phc clock
    /// @returns whether to synchronize the global clock to the phc clock
    bool IsSyncGlobalToPhc() const noexcept { return syncGlobalToPhc_; }

    /// @brief whether to synchronize the global clock to the system clock
    /// @returns whether to synchronize the global clock to the system clock
    bool IsSyncGlobalToSystem() const noexcept { return syncGlobalToSystem_; }

    /// @brief whether to enable slave side precision measurement
    /// @returns whether to enable slave side precision measurement
    bool IsTestPrecisionMeasure() const noexcept { return testPrecisionMeasure_; }

private:
    /// @name name
    ara::core::String name_;
    /// @name debounceTime - minimum interval between two Sync packets
    double debounceTime_;
    /// @name domainId
    TimeDomainId domainId_;
    /// @name gateway
    ara::core::Vector< GlobalTimeGateWay > gateway_;
    /// @name globalTimeCorrectionProps
    GlobalTimeCorrectionProps globalTimeCorrectionProps_;
    /// @name globalTimeDomainProperty
    GlobalTimeDomainProperty globalTimeDomainProperty_;

    /// @name globalTimeMaster
    GlobalTimeMaster globalTimeMaster_;
    /// @name globalTimeSubDomain
    ara::core::Vector< ara::core::String > globalTimeSubDomain_;
    /// @name networkSegmentId
    std::int32_t networkSegmentId_;
    /// @name offsetTimeDomain
    ara::core::String offsetTimeDomain_;
    /// @name pduTriggering
    ara::core::String pduTriggering_;
    /// @name slave
    ara::core::Vector< GlobalTimeSlave > slave_;
    /// @name syncLossTimeout
    double syncLossTimeout_;

    /// @name isMasterOnThisMachine - whether Master is configured on the current machine
    bool isMasterOnThisMachine_{false};

    /// @name timeStampType - timestamp type
    TSTimeStampTypeEnum timeStampType_{TSTimeStampTypeEnum::kSoftware};

    /// @name globalTimePropagationDelay -
    /// If path delay measurement is enabled, this parameter represents the default propagation delay until the first actual measured value is available. If cyclic propagation delay measurement is disabled, the side uses this parameter as a fixed value.
    /// TODO(zhoubo): magic number should be place a common file
    double globalTimePropagationDelay_{kTS_NUM_0_POINT_001};

    /// @name masterSlaveConflictDetection -
    /// Enable Master/Slave conflict detection and notification function, false means disabled.
    bool masterSlaveConflictDetection_{true};

    /// @name crcTimeFlagsTxSecured -
    /// Controls which elements in the Followup message need to participate in CRC calculation when sending,
    /// see [PRS_TS_00098]. Controlled by the globalTimeTxCrcSecured master switch.
    /// TODO(zhoubo): magic number should be place a common file
    std::uint8_t crcTimeFlagsTxSecured_{kTS_NUM_63};

    /// @name globalTimeSequenceCounterJumpWidth -
    /// Specifies the maximum allowed jump in the sequence counter between two consecutive synchronization messages.
    /// TODO(zhoubo): magic number should be place a common file
    std::uint16_t globalTimeSequenceCounterJumpWidth_{kTS_NUM_10};

    /// @name steadyClock - whether it is a stable clock
    bool steadyClock_{false};

    /// @name syncGlobalToPhc - whether to synchronize the global clock to the phc clock
    bool syncGlobalToPhc_{false};

    /// @name syncGlobalToSystem - whether to synchronize the global clock to the system clock
    bool syncGlobalToSystem_{false};

    /// @name testPrecisionMeasure_ - slave side precision measurement
    bool testPrecisionMeasure_{false};
};

}  // namespace config
}  // namespace internal
}  // namespace tsync
}  // namespace ara
#endif  /// ARA_TSYNC_INTERNAL_CONFIG_GLOBALTIMEDOMAIN_H_
