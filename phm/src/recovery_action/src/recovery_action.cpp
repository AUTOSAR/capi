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
/// @file       recovery_action.cpp
/// @brief
/// @details
/// @date       2024-06-20
/// @author     wangyanlong
/// @version    1.2.0
///
/// ================================================================

#include "ara/phm/recovery_action.h"

#include "ara/phm/internal/phm_log.h"

namespace ara {
namespace phm {
/// @brief Constructor.
/// @param instance instance specifier to the PPortPrototype of a Phm RecoveryActionInterface.
/// @throws QAC
RecoveryAction::RecoveryAction(ara::core::InstanceSpecifier const &instance)  // NOLINT
    : instance_{instance}, actionServer_{nullptr}, communicationClient_{nullptr}
{
    actionServer_ = ara::phm::internal::racom::Server< TypeOfSupervision >::GetInstanceShared();
    if (actionServer_ == nullptr) {
        std::cout << instance_.ToString().cbegin() << "actionServer_ is null.\n";
        std::terminate();
    }

    communicationClient_ = ara::phm::internal::raextcom::Client::GetInstanceShared();
    if (communicationClient_ == nullptr) {
        std::cout << instance_.ToString().cbegin() << "communicationClient_ is null.";
        std::terminate();
    }
}

/// @brief Destructor for RecoveryAction.
RecoveryAction::~RecoveryAction() noexcept
{
    // Prevent RecoveryHandler from being called after RecoveryAction is destroyed
    // After being moved, this may be empty
    int32_t const useCountWhenNeedReset{2};
    if (actionServer_) {
        actionServer_->StopOffer(static_cast< ara::core::String >(instance_.ToString().cbegin()));
        if (actionServer_.use_count() == useCountWhenNeedReset) {
            LOG_INFO << "RecoveryAction, close actionServer_";
            std::ignore = actionServer_->Close();
        }
        actionServer_.reset();
    }

    if (communicationClient_) {
        if (communicationClient_.use_count() == useCountWhenNeedReset) {
            LOG_INFO << "RecoveryAction, close communicationClient_";
            std::ignore = communicationClient_->Close();
        }
        communicationClient_.reset();
    }
}

/// @brief Move constructor.
/// @param ra The object to be moved.
RecoveryAction::RecoveryAction(RecoveryAction &&ra) noexcept
    : instance_{std::move(ra.instance_)}
    , actionServer_{std::move(ra.actionServer_)}
    , communicationClient_{std::move(ra.communicationClient_)}
{
}

/// @brief  Move assignment operator.
/// @param ra The object to be moved.
/// @return RecoveryAction& The moved object.
RecoveryAction &RecoveryAction::operator=(RecoveryAction &&ra) &noexcept
{
    instance_            = std::move(ra.instance_);
    actionServer_        = std::move(ra.actionServer_);
    communicationClient_ = std::move(ra.communicationClient_);
    std::ignore          = std::move(ra);
    return *this;
}

/// @brief Returns the status of global supervision that the supervised entity
/// belongs to.
/// @throws QAC
ara::core::Result< GlobalSupervisionStatus > RecoveryAction::GetGlobalSupervisionStatus() const
{
    if (!communicationClient_->IsOpened()) {
        if (communicationClient_->Open() < 0) {
            LOG_FATAL << "RecoveryAction::RecoveryAction, open communication client "
                         "failed.";
            return ara::core::Result< GlobalSupervisionStatus >::FromError(PhmErrc::kOfferFailed);
        }
    }

    ara::phm::internal::SupervisionStatus const status{communicationClient_->GetGlobalSupervisionStatus(
        static_cast< ara::core::String >(instance_.ToString().cbegin()))};
    if (status == ara::phm::internal::kInvalidSupervisionStatus) {
        return ara::core::Result< GlobalSupervisionStatus >::FromError(PhmErrc::kOfferFailed);
    }
    return ara::core::Result< GlobalSupervisionStatus >::FromValue(static_cast< GlobalSupervisionStatus >(status));
}

/// @brief Enables potential invocations of RecoveryHandler.
/// @return ara::core::Result<void> A Result, being either empty or containing any of the errors defined below.
/// @throws QAC
ara::core::Result< void > RecoveryAction::Offer()
{
    if (!actionServer_->IsOpened()) {
        if (actionServer_->Open() < 0) {
            LOG_FATAL << "RecoveryAction::RecoveryAction, open action server failed.";
            return ara::core::Result< void >::FromError(PhmErrc::kOfferFailed);
        }
    }
    actionServer_->RegisterRecoveryHandler(
        static_cast< ara::core::String >(instance_.ToString().cbegin()),
        // TODO(wangyanlong): std:bind is not recommended in QAC
        std::bind(&RecoveryAction::RecoveryHandler, this, std::placeholders::_1, std::placeholders::_2));
    actionServer_->Offer(static_cast< ara::core::String >(instance_.ToString().cbegin()));

    if (!communicationClient_->IsOpened()) {
        if (communicationClient_->Open() < 0) {
            LOG_FATAL << "RecoveryAction::RecoveryAction, open communication client "
                         "failed.";
            return ara::core::Result< void >::FromError(PhmErrc::kOfferFailed);
        }
    }
    if (communicationClient_->Offer(ara::core::String(instance_.ToString().cbegin())) < 0) {
        return ara::core::Result< void >::FromError(PhmErrc::kOfferFailed);
    }

    return ara::core::Result< void >::FromValue();
}

/// @brief Disables invocations of RecoveryHandler.
/// @throws QAC
void RecoveryAction::StopOffer()
{
    if (!actionServer_->IsOpened()) {
        if (actionServer_->Open() < 0) {
            LOG_FATAL << "RecoveryAction::RecoveryAction, open action server failed.";
            return;
        }
    }
    actionServer_->StopOffer(static_cast< ara::core::String >(instance_.ToString().cbegin()));

    if (!communicationClient_->IsOpened()) {
        if (communicationClient_->Open() < 0) {
            LOG_FATAL << "RecoveryAction::RecoveryAction, open communication client "
                         "failed.";
            return;
        }
    }
    std::ignore = communicationClient_->StopOffer(static_cast< ara::core::String >(instance_.ToString().cbegin()));
}

}  // namespace phm
}  // namespace ara
