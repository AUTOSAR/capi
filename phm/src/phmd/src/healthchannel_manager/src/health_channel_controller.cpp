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
/// @file       health_channel_controller.cpp
/// @brief
/// @details
/// @date       2024-06-20
/// @author     wangyanlong
/// @version    1.2.0
///
/// ================================================================

#include "ara/phm/internal/hcmanager/health_channel_controller.h"

#include "ara/phm/internal/phm_log.h"

namespace ara {
namespace phm {
namespace internal {
/// @brief Default deconstructor.
HealthChannelController::~HealthChannelController() noexcept
{
    LOG_INFO << "~HealthChannelController()";
    std::ignore = Stop();
}

/// @brief Init health channel controller.
/// @return 0, success
/// @return < 0, fail
int32_t HealthChannelController::Init() noexcept
{
    LOG_INFO << "HealthChannelController Init start";
    ara::core::String const tempStr{""};
    configManager_ = ConfigManager::GetInstanceShared(tempStr);
    if (configManager_ == nullptr) {
        LOG_ERROR << "ConfigManager is null.";
        return -1;
    }

    // Create resources according to configuration
    ara::core::Vector< HealthChannelEntityConf > const& healthChannelConf{configManager_->GetHealthChannelConf()};
    ara::core::Vector< std::shared_ptr< HealthChannelEntity > > healthChannelEntity{
        MakeHealthChannelEntity(healthChannelConf)};
    for (std::shared_ptr< HealthChannelEntity >& element : healthChannelEntity) {
        healthChannelEntity_[element->GetName()] = element;
    }

    ara::core::Vector< HealthChannelSupervisionConf > const& healthChannelSupervisionConf{
        configManager_->GetHealthChannelSupervisionConf()};
    ara::core::Vector< std::shared_ptr< HealthChannelSupervision > > healthChannelSupervision{
        _makeHealthChannelSupervision(healthChannelSupervisionConf)};
    for (std::shared_ptr< HealthChannelSupervision >& element : healthChannelSupervision) {
        healthChannelSupervision_[element->GetMetaModelIdentifier()]      = element;
        healthChannelSupervisionExt_[element->GetHcInterfaceIdentifier()] = element;
    }

    ara::core::Vector< RecoveryNotificationPortMappingConf > recoveryNotificationMapping{
        configManager_->GetRecoveryNotificationMap()};
    for (RecoveryNotificationPortMappingConf const& element : recoveryNotificationMapping) {
        recoveryActionMap_[element.healthChannelId]          = element.instanceSpecifier;
        recoveryActionReverseMap_[element.instanceSpecifier] = element.healthChannelId;
    }

#ifdef COMPILE_WITH_IAM
    // Create iam control handle
    iamHandle_ = std::make_unique< ara::iam::internal::grant::IAMGrantPhmQuery >();
    if (!iamHandle_->Initialize()) {
        LOG_ERROR << "make ara::iam::grantquery::IAMGrantPhmQuery failed.";
        return -1;
    }
#endif

    if (_makeCommunicationHandles() < 0) {
        LOG_ERROR << "make communication handles error for health channel controller.";
        return -1;
    }

    LOG_INFO << "HealthChannelController Init end.";
    return 0;
}

/// @brief Create handles used to communicate with AA、SM.
/// @return 0, success; < 0, fail.
int32_t HealthChannelController::_makeCommunicationHandles()
{
    /// Create communication handle
    healthChannelServer_ = ara::phm::internal::hccom::Server::GetInstanceUnique(
        // TODO(wangyanlong): std:bind is not recommended in QAC
        std::bind(&HealthChannelController::_healthStatusHandler, this, std::placeholders::_1, std::placeholders::_2,
                  std::placeholders::_3));
    if (healthChannelServer_ == nullptr) {
        LOG_ERROR << "get ara::phm::internal::hccom::Server failed.";
        return -1;
    }
    if (healthChannelServer_->Open() < 0) {
        LOG_ERROR << "open healthChannelServer_ failed";
        return -1;
    }

    // TODO(wangyanlong): std:bind is not recommended in QAC
    healthChannelActionClient_ = ara::phm::internal::hcacom::Client::GetInstanceUnique(std::bind(
        &HealthChannelController::_recoveryResultHandler, this, std::placeholders::_1, std::placeholders::_2));
    if (healthChannelActionClient_ == nullptr) {
        LOG_ERROR << "get ara::phm::internal::hcacom::Client failed.";
        return -1;
    }

    if (healthChannelActionClient_->Open() < 0) {
        LOG_ERROR << "open healthChannelActionClient_ failed.";
        return -1;
    }
    // TODO(wangyanlong): std:bind is not recommended in QAC
    healthChannelActionExtServer_ = ara::phm::internal::hcaextcom::Server::GetInstanceUnique(std::bind(
        &HealthChannelController::_healthChannelExtServerHandler, this, std::placeholders::_1, std::placeholders::_2));
    if (healthChannelActionClient_ == nullptr) {
        LOG_ERROR << "get ara::phm::internal::hcaextcom::Server failed.";
        return -1;
    }
    if (healthChannelActionExtServer_->Open() < 0) {
        LOG_ERROR << "open healthChannelActionExtServer_ failed.";
        return -1;
    }
    return 0;
}

/// @brief Close com handles.
/// @return 0, success
/// @return < 0, fail
int32_t HealthChannelController::Stop() const noexcept
{
    LOG_INFO << "stop health channel maneger";
    if (healthChannelServer_) {
        LOG_INFO << "close healthChannelServer_";
        std::ignore = healthChannelServer_->Close();
    }

    if (healthChannelActionExtServer_) {
        LOG_INFO << "close healthChannelActionExtServer_";
        std::ignore = healthChannelActionExtServer_->Close();
    }

    if (healthChannelActionClient_) {
        LOG_INFO << "close healthChannelActionClient_";
        std::ignore = healthChannelActionClient_->Close();
    }

    return 0;
}

/// @brief Used by PhmContext to register a function, HealthChannelController can use this function to trigger
/// watchdog.
/// @param trigger a callback used by health channel controller to trigger watchdog
void HealthChannelController::SetWatchdogTriggerFunction(std::function< void() > const& trigger) noexcept
{
    LOG_INFO << "HealthChannelController::SetWatchdogTriggerFunction";
    watchdogTriggerFuntion_ = trigger;
}

/// @brief A callback registered to healthChannelServer_ to process health status
/// @throws QAC
/// @param specifierId identifier of health channel.
/// @param processId process id who report health status.
/// @param healthStatusId id of health status.
void HealthChannelController::_healthStatusHandler(Specifier const& specifierId,
                                                   ProcessId const processId,
                                                   HealthStatus const healthStatusId)
{
    LOG_DEBUG << "health status reached, specifier " << specifierId.c_str() << ", pid " << processId
              << ", health status " << healthStatusId;
    if (!_checkAccessByIam(processId, healthStatusId)) {
        LOG_ERROR << "health status check error by iam, specifier " << specifierId.c_str() << ", pid " << processId
                  << ", health status " << healthStatusId;
        return;
    }

    std::shared_ptr< HealthChannelSupervision > const healthChannelSupervision{
        healthChannelSupervisionExt_[specifierId]};
    if (healthChannelSupervision == nullptr) {
        LOG_ERROR << "health status not defined, identifier " << specifierId.c_str() << ", status id "
                  << healthStatusId;
        return;
    }

    if (healthChannelSupervision->IsHealthStatusNeedRecover(healthStatusId)) {
        if (!healthChannelSupervision->IsOffered()) {
            LOG_INFO << "health channel supervision " << specifierId.c_str() << " is not offered.";
            return;
        }

        LOG_INFO << "recovery health status, health channel " << specifierId.c_str() << ", status id "
                 << healthStatusId;
        std::shared_ptr< ara::phm::internal::hcacom::NotifyInfo > notifyInfo{
            std::make_shared< ara::phm::internal::hcacom::NotifyInfo >()};
        notifyInfo->status = healthStatusId;

        // TODO(wangyanlong): The names here are different, between health channel supervision and supervision management they are also different
        notifyInfo->identifier
            = recoveryActionMap_[ara::core::String(healthChannelSupervision->GetMetaModelIdentifier())];
        notifyInfo->waitResponseTimeOutMs = healthChannelSupervision->GetRecoveryNotificationTimeout();
        notifyInfo->maxRetryTimes = static_cast< int32_t >(healthChannelSupervision->GetRecoveryNotificationRetry());
        if (healthChannelActionClient_->Notify(notifyInfo) < 0) {
            LOG_ERROR << "healthChannelActionClient_->Notify error.";
            return;
        }
    }
}

/// @brief Check whether the process can report this health status.
/// @param processId process id who report health status.
/// @param healthStatusId id of health status.
/// @return true, check passed; false，check not passed.
bool HealthChannelController::_checkAccessByIam(ProcessId const processId,
                                                HealthStatus const healthStatusId) const noexcept
{
// true, verification passed; false, verification failed
#ifdef COMPILE_WITH_IAM
    return iamHandle_->HasChannelGrant(processId, healthStatusId);
#else
    std::ignore = processId;
    std::ignore = healthStatusId;
    return true;
#endif
}

/// @brief Create objects of HealthChannelEntity based on the conf from ConfigManager.
/// @param healthChannelEntityConf the conf of HealthChannelEntity from ConfigManager.
/// @return ara::core::Vector<std::shared_ptr<HealthChannelEntity>> objects of health channel entity.
ara::core::Vector< std::shared_ptr< HealthChannelEntity > > HealthChannelController::MakeHealthChannelEntity(
    ara::core::Vector< HealthChannelEntityConf > const& healthChannelEntityConf) noexcept
{
    LOG_INFO << "HealthChannelController::MakeHealthChannelEntity, health channel count "
             << healthChannelEntityConf.size();

    ara::core::Vector< std::shared_ptr< HealthChannelEntity > > ret;
    for (HealthChannelEntityConf const& oneConf : healthChannelEntityConf) {
        // Create the health state owned by the health channel
        ara::core::Vector< std::shared_ptr< HealthChannelStatus > > healthChannelStatus;
        for (HealthStatusConf const& status : oneConf.healthStatus) {
            std::shared_ptr< HealthChannelStatus > const statusObject{std::make_shared< HealthChannelStatus >(
                status.shortName, status.statusId, status.triggersRecoveryNotification)};
            healthChannelStatus.push_back(statusObject);
        }

        std::shared_ptr< HealthChannelEntity > const healthChannelObject{
            std::make_shared< HealthChannelEntity >(oneConf.shortName, healthChannelStatus)};
        ret.push_back(healthChannelObject);
    }

    return ret;
}

/// @brief Create objects of HealthChannelSupervision based on the conf from ConfigManager.
/// @param healthChannelSupervisionConf the conf of HealthChannelSupervision from ConfigManager.
/// @return ara::core::Vector<std::shared_ptr<HealthChannelSupervision>> objects of health channel supervision.
ara::core::Vector< std::shared_ptr< HealthChannelSupervision > > HealthChannelController::_makeHealthChannelSupervision(
    ara::core::Vector< HealthChannelSupervisionConf > const& healthChannelSupervisionConf) noexcept
{
    LOG_INFO << "HealthChannelController::_makeHealthChannelSupervision, health channel supervision count "
             << healthChannelSupervisionConf.size();

    ara::core::Vector< std::shared_ptr< HealthChannelSupervision > > ret;
    for (HealthChannelSupervisionConf const& oneConf : healthChannelSupervisionConf) {
        // TODO(wangyanlong): oneConf->phmHealthChannelInterface, this name needs to be changed
        LOG_INFO << "make health channel supervision " << oneConf.phmHealthChannelInterface.c_str();
        std::shared_ptr< HealthChannelSupervision > const object{std::make_shared< HealthChannelSupervision >(
            oneConf, healthChannelEntity_[oneConf.phmHealthChannelInterface])};
        ret.push_back(object);
    }
    return ret;
}

/// @brief To process the recovery result, this function is registered to healthChannelActionClient_.
/// @throws QAC
/// @param result result of recover.
/// @param instance instance of HealthChannelAction.
void HealthChannelController::_recoveryResultHandler(ara::phm::internal::hcacom::RecoveryResult const result,
                                                     ara::core::String const& instance) const
{
    if (result == ara::phm::internal::hcacom::RecoveryResult::kSuccess) {
        LOG_INFO << instance << " recovery success.";
    } else if (result == ara::phm::internal::hcacom::RecoveryResult::kNotOffered) {
        LOG_INFO << instance << " is not offered, so it is not needed to recovery it.";
    } else {
        LOG_INFO << instance.c_str() << " recovery failed.";
        if (watchdogTriggerFuntion_) {
            watchdogTriggerFuntion_();
        } else {
            LOG_ERROR << "watchdogTriggerFuntion_ is null";
        }
    }
}

/// @brief A callback registered to healthChannelActionExtServer_ to process Offer/StopOffer from SM.
/// @throws QAC
/// @param instance the instance specifier of HealthChannelAction.
/// @param eventType offer or stopoffer.
void HealthChannelController::_healthChannelExtServerHandler(ara::core::String const& instance,
                                                             ara::phm::internal::hcaextcom::EventType const eventType)
{
    LOG_DEBUG << "hc ext server, instance " << instance.c_str() << ". event " << static_cast< uint32_t >(eventType);
    if (healthChannelSupervision_.count(recoveryActionReverseMap_[instance]) == static_cast< size_t >(0)) {
        LOG_WARN << "there is no health channel supervision related with " << instance.c_str()
                 << ", it may be not configured";
        return;
    }

    if (eventType == ara::phm::internal::hcaextcom::EventType::kOffer) {
        healthChannelSupervision_[recoveryActionReverseMap_[instance]]->SetOffer(true);
    } else {
        healthChannelSupervision_[recoveryActionReverseMap_[instance]]->SetOffer(false);
    }
}

}  // namespace internal
}  // namespace phm
}  // namespace ara