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
/// @file       globaltimedomain.cpp
/// @brief      time domain parsing
/// @details
/// @date       2023-01-11
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#include "ara/tsync/internal/config/globaltimedomain.h"

#include <isoft/define.h>
#include <isoft/manifestreader/manifest_node.h>
#include <isoft/manifestreader/manifest_reader.h>
namespace ara {
namespace tsync {
namespace internal {
namespace config {

/// @brief read configuration
/// @param node - configuration node
/// @return 0 - success
std::int32_t GlobalTimeDomain::ManifestLoader(isoft::manifestreader::ManifestNode const &node) noexcept
{
    /// @name kName
    static constexpr ara::core::StringView kName{"name"};
    /// @name kDebounceTime
    static constexpr ara::core::StringView kDebounceTime{"debounceTime"};
    /// @name kDomainId
    static constexpr ara::core::StringView kDomainId{"domainId"};
    /// @name kGateWay
    static constexpr ara::core::StringView kGateWay{"gateway"};
    /// @name kGlobalTimeCorrectionProps
    static constexpr ara::core::StringView kGlobalTimeCorrectionProps{"globalTimeCorrectionProps"};
    /// @name kGlobalTimeDomainProperty
    static constexpr ara::core::StringView kGlobalTimeDomainProperty{"globalTimeDomainProperty"};
    /// @name kGlobalTimeMaster
    static constexpr ara::core::StringView kGlobalTimeMaster{"globalTimeMaster"};
    /// @name kGlobalTimeSubDomain
    static constexpr ara::core::StringView kGlobalTimeSubDomain{"globalTimeSubDomain"};
    /// @name kNetworkSegmentId
    static constexpr ara::core::StringView kNetworkSegmentId{"networkSegmentId"};
    /// @name kOffsetTimeDomain
    static constexpr ara::core::StringView kOffsetTimeDomain{"offsetTimeDomain"};
    /// @name kPduTriggering
    static constexpr ara::core::StringView kPduTriggering{"pduTriggering"};
    /// @name kSyncLossTimeout
    static constexpr ara::core::StringView kSyncLossTimeout{"syncLossTimeout"};
    /// @name kTimeStampType
    static constexpr ara::core::StringView kTimeStampType{"timeStampType"};
    /// @name kMasterSlaveConflictDetection
    static constexpr ara::core::StringView kMasterSlaveConflictDetection{"masterSlaveConflictDetection"};
    /// @name kGlobalTimePropagationDelay
    static constexpr ara::core::StringView kGlobalTimePropagationDelay{"globalTimePropagationDelay"};
    /// @name kCrcTimeFlagsTxSecured
    static constexpr ara::core::StringView kCrcTimeFlagsTxSecured{"crcTimeFlagsTxSecured"};
    /// @name kGlobalTimeSequenceCounterJumpWidth
    static constexpr ara::core::StringView kGlobalTimeSequenceCounterJumpWidth{"globalTimeSequenceCounterJumpWidth"};
    /// @name kSteadyClock
    static constexpr ara::core::StringView kSteadyClock{"steadyClock"};
    /// @name kSOFTWARE
    static constexpr ara::core::StringView kSOFTWARE{"Software"};
    /// @name kHARDWARE
    static constexpr ara::core::StringView kHARDWARE{"Hardware"};
    /// @name kSyncGlobalToPhc
    static constexpr ara::core::StringView kSyncGlobalToPhc{"syncGlobalToPhc"};
    /// @name kSyncGlobalToSystem
    static constexpr ara::core::StringView kSyncGlobalToSystem{"syncGlobalToSystem"};
    /// @name kTestPrecisionMeasure
    static constexpr ara::core::StringView kTestPrecisionMeasure{"testPrecisionMeasure"};
    /// @name kSlave
    static constexpr ara::core::StringView kSlave{"slave"};
    bool haveError{false};

    this->name_         = node.GetValue(kName, ara::core::String());
    this->debounceTime_ = node.GetValue(kDebounceTime, 0.0);
    std::int16_t const defVal{-1};
    std::int16_t const id{node.GetValue(kDomainId, defVal)};
    if (-1 == id) {
        // domainId must be configured
        LOG().Fatal() << "lack of DomainId";
        haveError = true;
    } else {
        this->domainId_.FromUint8(static_cast< std::uint8_t >(id));
    }

    static_cast< void >(node.Load(kGateWay, this->gateway_));
    static_cast< void >(node.Load(kGlobalTimeCorrectionProps, this->globalTimeCorrectionProps_));
    // messageCompliance must be configured
    if (isoft::kSuccess != node.Load(kGlobalTimeDomainProperty, this->globalTimeDomainProperty_)) {
        haveError = true;
    }
    // The current Machine may have no Master, in which case the Machine acts as a Slave for the current domain
    static_cast< void >(node.Load(kGlobalTimeMaster, this->globalTimeMaster_));
    static_cast< void >(node.Load(kGlobalTimeSubDomain, this->globalTimeSubDomain_));
    this->networkSegmentId_ = node.GetValue(kNetworkSegmentId, 0);
    this->offsetTimeDomain_ = node.GetValue(kOffsetTimeDomain, ara::core::String());
    pduTriggering_          = node.GetValue(kPduTriggering, ara::core::String());
    // The current Machine may have no Slave, in which case the Machine may act as a Master for the current domain
    static_cast< void >(node.Load(kSlave, this->slave_));
    this->syncLossTimeout_ = node.GetValue(kSyncLossTimeout, 0.0);
    ara::core::String const stameType{node.GetValue(kTimeStampType, ara::core::String())};
    ara::core::StringView const svType{stameType.c_str(), stameType.length()};
    if (svType == kSOFTWARE) {
        this->timeStampType_ = TSTimeStampTypeEnum::kSoftware;
    } else if (svType == kHARDWARE) {
        this->timeStampType_ = TSTimeStampTypeEnum::kHardware;
    } else if (0U != stameType.length()) {
        LOG().Warn() << "invalid " << kTimeStampType << "{" << stameType << "}, use Software";
        this->timeStampType_ = TSTimeStampTypeEnum::kSoftware;
    } else {
    }

    /// globalTimePropagationDelay,
    /// double value too small, ara-gen generates scientific notation, change to generate string,
    ara::core::String dealyStr{""};
    dealyStr = node.GetValue(kGlobalTimePropagationDelay, ara::core::String());
    if (dealyStr.empty()) {
        double const defDelay{1e-3};
        this->globalTimePropagationDelay_ = defDelay;
    } else {
        this->globalTimePropagationDelay_ = ara::core::internal::stod(dealyStr);
    }
    // this->globalTimePropagationDelay =
    // node.GetValue(kGlobalTimePropagationDelay, );
    this->masterSlaveConflictDetection_ = node.GetValue(kMasterSlaveConflictDetection, true);
    std::uint8_t const defValSecured{0x3fU};
    this->crcTimeFlagsTxSecured_ = node.GetValue(kCrcTimeFlagsTxSecured, defValSecured);  // 3f
    std::uint16_t const defJumpWidth{10U};
    this->globalTimeSequenceCounterJumpWidth_ = node.GetValue(kGlobalTimeSequenceCounterJumpWidth, defJumpWidth);
    this->steadyClock_                        = node.GetValue(kSteadyClock, false);
    this->syncGlobalToPhc_                    = node.GetValue(kSyncGlobalToPhc, false);
    this->syncGlobalToSystem_                 = node.GetValue(kSyncGlobalToSystem, false);
    this->testPrecisionMeasure_               = node.GetValue(kTestPrecisionMeasure, false);
    LOG().Info() << "GlobalTimeDomain::ManifestLoader syncGlobalToPhc_=" << syncGlobalToPhc_
                 << ", syncGlobalToSystem_=" << syncGlobalToSystem_;
    if (haveError) {
        return kRET_E1;
    }
    return isoft::kSuccess;
}

/// @brief print debug information
void GlobalTimeDomain::Debug() const noexcept
{
#if ARA_TSYNC_DEBUG
    std::cout << "name: " << name_ << std::endl;
    std::cout << "debounceTime: " << debounceTime_ << std::endl;
    std::cout << "domainId: " << static_cast< std::uint16_t >(domainId_.ToUint8()) << std::endl;
    std::cout << "gateway: " << std::endl;
    for (auto const &g : gateway_) {
        g.Debug();
    }
    std::cout << "globalTimeCorreciontProps: " << std::endl;
    globalTimeCorrectionProps_.Debug();

    std::cout << "globalTimeDomainProperty: " << std::endl;
    globalTimeDomainProperty_.Debug();
    std::cout << "globalTimeMaster: " << std::endl;
    globalTimeMaster_.Debug();
    std::cout << "kGlobalTimeSubDomain: " << std::endl;
    for (auto const &s : globalTimeSubDomain_) {
        std::cout << s << " ";
    }
    std::cout << std::endl;
    std::cout << "networkSegmentId: " << networkSegmentId_ << std::endl;
    std::cout << "offsetTimeDomain: " << offsetTimeDomain_ << std::endl;
    std::cout << "pduTriggering: " << pduTriggering_ << std::endl;
    std::cout << "slave:";
    for (auto const &s : slave_) {
        s.Debug();
    }
    std::cout << "syncLossTimeout: " << syncLossTimeout_ << std::endl;
#endif
}

}  // namespace config
}  // namespace internal
}  // namespace tsync
}  // namespace ara