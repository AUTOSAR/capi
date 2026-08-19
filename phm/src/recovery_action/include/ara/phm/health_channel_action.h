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
/// @file       health_channel_action.h
/// @brief      action when the health channel status need recover.
/// @details
/// @date       2024-06-20
/// @author     wangyanlong
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/PlatformHealthManagement/HealthChannelAction
/// @unit_description=action when the health channel status need recover.
/// @trace_id_sr=SR_PHM_01028
/// @unit_name=HealthChannelAction
/// @interface_level=software
/// @endcode
///
/// ================================================================

#ifndef ARA_PHM_HEALTH_CHANNEL_ACTION_H_
#define ARA_PHM_HEALTH_CHANNEL_ACTION_H_

#include <ara/core/instance_specifier.h>
#include <ara/core/result.h>
#include <ara/core/string.h>

#include "ara/phm/internal/com/hca_server.h"
#include "ara/phm/internal/com/hcaext_client.h"
#include "ara/phm/internal/phm_log.h"
#include "ara/phm/internal/types.h"
#include "ara/phm/phm_error_domain.h"

namespace ara {
namespace phm {

/// @brief Provide a virtual function RecoveryHandler, implemented by SM,
/// called when the supervision need recover
/// @tparam EnumT enum of HealthStatus
/// @trace_id_sr=SR_PHM_01028
/// @needwork = ad
template < typename EnumT >
class HealthChannelAction
{
public:
    /// @brief Constructor.
    /// @param instance instance specifier to the PPortPrototype of a Phm HealthChannelActionInterface.
    /// @trace_id_sr=SR_PHM_01028
    /// @needwork = ad
    explicit HealthChannelAction(const ara::core::InstanceSpecifier &instance) : instance_{instance}  //NOLINT
    {
        actionServer_ = ara::phm::internal::hcacom::Server::GetInstanceShared();
        if (actionServer_ == nullptr) {
            std::cout << instance_.ToString().cbegin() << "actionServer_ is null.\n";
            std::terminate();
        }

        communicationClient_ = ara::phm::internal::hcaextcom::Client::GetInstanceShared();
        if (communicationClient_ == nullptr) {
            std::cout << instance_.ToString().cbegin() << "communicationClient_ is null.\n";
            std::terminate();
        }
    }

    /// @brief Destructor.
    /// @trace_id_sr=SR_PHM_01028
    /// @needwork = ad
    virtual ~HealthChannelAction() noexcept
    {
        // Prevent RecoveryHandler from being called after RecoveryAction is destroyed
        if (actionServer_) {
            actionServer_->StopOffer(static_cast< ara::core::String >(instance_.ToString().begin()));
            if (actionServer_.use_count() == 2) {
                LOG_INFO << "HealthChannelAction, close actionServer_";
                actionServer_->Close();
            }
            actionServer_.reset();
        }

        if (communicationClient_) {
            if (communicationClient_.use_count() == 2) {
                LOG_INFO << "HealthChannelAction, close communicationClient_";
                communicationClient_->Close();
            }
            communicationClient_.reset();
        }
    }

    /// @brief The copy constructor shall not be used.
    /// @param The object to be copied.
    /// @trace_id_sr=SR_PHM_01028
    /// @needwork = ad
    HealthChannelAction(const HealthChannelAction &) = delete;

    /// @brief The copy assignment operator shall not be used.
    /// @param other The object to be copied.
    /// @return HealthChannelAction& The copied object.
    /// @trace_id_sr=SR_PHM_01028
    /// @needwork = ad
    HealthChannelAction &operator=(const HealthChannelAction &) = delete;

    /// @brief Move constructor.
    /// @param hca The object to be moved.
    /// @trace_id_sr=SR_PHM_01028
    /// @needwork = ad
    HealthChannelAction(HealthChannelAction &&hca) noexcept
        : instance_{std::move(hca.instance_)}
        , actionServer_{std::move(hca.actionServer_)}
        , communicationClient_{std::move(hca.communicationClient_)} {};

    /// @brief Move assignment operator.
    /// @param hca The object to be moved.
    /// @return HealthChannelAction& The moved object.
    /// @trace_id_sr=SR_PHM_01028
    /// @needwork = ad
    HealthChannelAction &operator=(HealthChannelAction &&hca) &noexcept
    {
        instance_            = std::move(hca.instance_);
        actionServer_        = std::move(hca.actionServer_);
        communicationClient_ = std::move(hca.communicationClient_);
        return *this;
    };

    /// @brief RecoveryHandler to be invoked by PHM.
    ///        The handler invocation needs to be enabled before by a call of HealthChannelAction::Offer.
    /// @param healthStatusId The identifier representing the Health Status. The mapping is implementation specific.
    /// @trace_id_sr=SR_PHM_01028
    /// @needwork = ad
    virtual void RecoveryHandler(EnumT healthStatusId) = 0;

    /// @brief Enables potential invocations of recovery handler.
    /// @return ara::core::Result<void> A Result, being either empty or containing any of the errors defined below.
    /// @trace_id_sr=SR_PHM_01028
    /// @needwork = ad
    ara::core::Result< void > Offer()
    {
        if (!actionServer_->IsOpened()) {
            if (actionServer_->Open() < 0) {
                LOG_FATAL << "HealthChannelAction::HealthChannelAction, open action "
                             "server failed.";
                return ara::core::Result< void >::FromError(PhmErrc::kOfferFailed);
            }
        }
        actionServer_->RegisterRecoveryHandler(
            static_cast< ara::core::String >(instance_.ToString().begin()),
            // TODO(wangyanlong): std:bind is not recommended in QAC
            std::bind(&HealthChannelAction::_recoveryHandlerWrapper, this, std::placeholders::_1));
        actionServer_->Offer(static_cast< ara::core::String >(instance_.ToString().begin()));

        if (!communicationClient_->IsOpened()) {
            if (communicationClient_->Open() < 0) {
                LOG_FATAL << "HealthChannelAction::HealthChannelAction, open "
                             "communication client failed.";
                return ara::core::Result< void >::FromError(PhmErrc::kOfferFailed);
            }
        }
        if (communicationClient_->Offer(static_cast< ara::core::String >(instance_.ToString().begin())) < 0) {
            return ara::core::Result< void >::FromError(PhmErrc::kOfferFailed);
        }

        return ara::core::Result< void >::FromValue();
    }

    /// @brief Disables invocations of recovery handler.
    /// @trace_id_sr=SR_PHM_01028
    /// @needwork = ad
    void StopOffer()
    {
        if (!actionServer_->IsOpened()) {
            if (actionServer_->Open() < 0) {
                LOG_FATAL << "HealthChannelAction::HealthChannelAction, open action "
                             "server failed.";
                return;
            }
        }
        actionServer_->StopOffer(static_cast< ara::core::String >(instance_.ToString().begin()));

        if (!communicationClient_->IsOpened()) {
            if (communicationClient_->Open() < 0) {
                LOG_FATAL << "HealthChannelAction::HealthChannelAction, open "
                             "communication client failed.";
                return;
            }
        }
        communicationClient_->StopOffer(static_cast< ara::core::String >(instance_.ToString().begin()));
    }

private:
    /// @brief Used to wrap recovery handler, wrapper is used to be registered to action server.
    /// @param healthStatusId id of health status.
    /// @trace_id_sr=SR_PHM_01028
    /// @needwork = dda
    void _recoveryHandlerWrapper(ara::phm::HealthStatus healthStatusId)
    {
        RecoveryHandler(static_cast< EnumT >(healthStatusId));
    }

private:
    /// @brief instance specifier of this health channel action.
    /// @trace_id_sr=SR_PHM_01028
    /// @needwork = dda
    ara::core::InstanceSpecifier instance_;

    /// @brief the ipc server run in SM
    /// @trace_id_sr=SR_PHM_01028
    /// @needwork = dda
    std::shared_ptr< ara::phm::internal::hcacom::Server > actionServer_;

    /// @brief communication client for Offer/StopOffer.
    /// @trace_id_sr=SR_PHM_01028
    /// @needwork = dda
    std::shared_ptr< ara::phm::internal::hcaextcom::Client > communicationClient_;
};

}  // namespace phm
}  // namespace ara

#endif  // ARA_PHM_HEALTH_CHANNEL_ACTION_H_
