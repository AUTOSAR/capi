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
/// @file       supervised_entity.h
/// @brief      used to report Checkpoints or to query the status of a SupervisedEntity.
/// @details
/// @date       2024-06-20
/// @author     wangyanlong
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/PlatformHealthManagement/SupervisedEntity
/// @unit_description=used to report Checkpoints or to query the status of a SupervisedEntity.
/// @trace_id_sr=SR_PHM_01001,SR_PHM_01018,SR_PHM_01019,SR_PHM_01020
/// @unit_name=SupervisedEntity
/// @interface_level=software
/// @endcode
///
/// ================================================================

#ifndef ARA_PHM_SUPERVISED_ENTITY_H_
#define ARA_PHM_SUPERVISED_ENTITY_H_

#include <ara/core/instance_specifier.h>
#include <ara/core/result.h>

#include "ara/phm/internal/com/sv_client.h"
#include "ara/phm/internal/phm_log.h"
#include "ara/phm/internal/types.h"
#include "ara/phm/phm_error_domain.h"

namespace ara {
namespace phm {

/// @brief Local Supervision Status.
/// @trace_id_sr=SR_PHM_01001,SR_PHM_01018,SR_PHM_01019,SR_PHM_01020
/// @needwork = ad
enum class LocalSupervisionStatus : ara::phm::internal::SupervisionStatus
{
    kDeactivated = 0,  ///< Local supervision not activated, all base supervisions it referenced is not active.
    kOK = 1,  ///< Local supervision is activated and there is no error, all base supervisions it referenced is CORRECT.
    kFailed = 2,  ///< Local supervision is activated and there is some error.
                  ///< 1、All deadline supervisions and logical supervisions it referenced are CORRECT.
                  ///< And
                  ///< 2、Some alive superviusion is INCORRECT and failed count is less than the
                  ///< failedSupervisionCyclesTolerance.
    kExpired = 3  ///< Local supervision is activated and the alive supervision failed count exceed the
                  ///< failedSupervisionCyclesTolerance, Or there is deadline supervision or logical supervision is
                  ///< INCORRECT
};

/// @brief Global Supervision Status, the worst of local supervision status it referenced.
/// @trace_id_sr=SR_PHM_01001,SR_PHM_01018,SR_PHM_01019,SR_PHM_01020
/// @needwork = ad
enum class GlobalSupervisionStatus : ara::phm::internal::SupervisionStatus
{
    kDeactivated = 0,  ///< Global supervision not activated,
                       ///< all local supervisions it referenced is in kDeactivated.
    kOK = 1,           ///< Global supervision is activated and all local supervisions it referenced is in kOK.

    kFailed = 2,   ///< Global supervision is activated and status is kFailed.
                   ///< At least One local supervision is kFailed and no local
                   ///< supervision is kExpired.
    kExpired = 3,  ///< Global supervision is activated and status is kExpired.
                   ///< At least One local supervision is kExpired，and not exceed
                   ///< expired tolerance.
    kStopped = 4   ///< Global supervision is activated and status is kStopped.
                   ///< At least One local supervision is kExpired，and exceed
                   ///< expired tolerance.
};

/// @brief Provide Client to report checkpoint,
///        get local supervision status and get global supervision status.
/// @tparam EnumT Checkpoint enum
/// @trace_id_sr=SR_PHM_01001,SR_PHM_01018,SR_PHM_01019,SR_PHM_01020
/// @needwork = ad
template < typename EnumT >
class SupervisedEntity
{
public:
    /// @brief Constructor.
    /// @param instance Instance specifier of the supervised entity.
    /// @trace_id_sr=SR_PHM_01001,SR_PHM_01018,SR_PHM_01019,SR_PHM_01020
    /// @needwork = ad
    explicit SupervisedEntity(const ara::core::InstanceSpecifier &instance) noexcept : instance_(instance)  //NOLINT
    {
        communicationClient_ = ara::phm::internal::svcom::Client::GetInstanceShared();
        if (communicationClient_ == nullptr) {
            std::cout << instance_.ToString().cbegin() << "communicationClient_ is null.\n";
            std::terminate();
        }
    }

    /// @brief Reports an occurrence of a Checkpoint.
    /// @param checkpointId checkpoint id.
    /// @return ara::core::Result<void> A Result, being either empty or containing an implementation specific error
    /// code.
    /// @trace_id_sr=SR_PHM_01001,SR_PHM_01018,SR_PHM_01019,SR_PHM_01020
    /// @needwork = ad
    ara::core::Result< void > ReportCheckpoint(EnumT checkpointId)
    {
        if (!communicationClient_->IsOpened()) {
            if (communicationClient_->Open() < 0) {
                LOG_FATAL << instance_
                          << " SupervisedEntity::ReportCheckpoint, open communication "
                             "client failed.";
                return ara::core::Result< void >::FromError(PhmErrc::kOfferFailed);
            }
        }

        if (communicationClient_->ReportCheckpoint(static_cast< ara::core::String >(instance_.ToString().begin()),
                                                   static_cast< uint32_t >(checkpointId))
            < 0) {
            return ara::core::Result< void >::FromError(PhmErrc::kOfferFailed);
        }
        return ara::core::Result< void >::FromValue();
    }

    /// @brief Returns the local supervision status that the supervised entity
    /// belongs to.
    /// @return ara::core::Result<LocalSupervisionStatus> the local supervision
    /// status.
    /// @trace_id_sr=SR_PHM_01001,SR_PHM_01018,SR_PHM_01019,SR_PHM_01020
    /// @needwork = ad
    ara::core::Result< LocalSupervisionStatus > GetLocalSupervisionStatus() const
    {
        if (!communicationClient_->IsOpened()) {
            if (communicationClient_->Open() < 0) {
                LOG_FATAL << instance_
                          << " SupervisedEntity::GetLocalSupervisionStatus, open "
                             "communication client failed.";
                return ara::core::Result< LocalSupervisionStatus >::FromError(PhmErrc::kOfferFailed);
            }
        }

        ara::phm::internal::SupervisionStatus const status{communicationClient_->GetLocalSupervisionStatus(
            static_cast< ara::core::String >(instance_.ToString().begin()))};
        if (status == ara::phm::internal::kInvalidSupervisionStatus) {
            return ara::core::Result< LocalSupervisionStatus >::FromError(PhmErrc::kOfferFailed);
        }
        return ara::core::Result< LocalSupervisionStatus >::FromValue(static_cast< LocalSupervisionStatus >(status));
    }

    /// @brief Returns the status of global supervision that the supervised entity belongs to.
    /// @return ara::core::Result<GlobalSupervisionStatus> the global supervision status.
    /// @trace_id_sr=SR_PHM_01001,SR_PHM_01018,SR_PHM_01019,SR_PHM_01020
    /// @needwork = ad
    ara::core::Result< GlobalSupervisionStatus > GetGlobalSupervisionStatus() const
    {
        if (!communicationClient_->IsOpened()) {
            if (communicationClient_->Open() < 0) {
                LOG_FATAL << instance_
                          << " SupervisedEntity::GetGlobalSupervisionStatus, open "
                             "communication client failed.";
                return ara::core::Result< GlobalSupervisionStatus >::FromError(PhmErrc::kOfferFailed);
            }
        }

        ara::phm::internal::SupervisionStatus const status{communicationClient_->GetGlobalSupervisionStatus(
            static_cast< ara::core::String >(instance_.ToString().begin()))};
        if (status == ara::phm::internal::kInvalidSupervisionStatus) {
            return ara::core::Result< GlobalSupervisionStatus >::FromError(PhmErrc::kOfferFailed);
        }
        return ara::core::Result< GlobalSupervisionStatus >::FromValue(static_cast< GlobalSupervisionStatus >(status));
    }

    /// @brief Destructor.
    /// @trace_id_sr=SR_PHM_01001,SR_PHM_01018,SR_PHM_01019,SR_PHM_01020
    /// @needwork = ad
    ~SupervisedEntity() noexcept
    {
        // use count is 2, indicating it can be released
        // communicationClient_ references once, static variable in the function obtaining the singleton references once
        if (communicationClient_.use_count() == 2) {
            LOG_INFO << "SupervisedEntity, close communicationClient_";
            communicationClient_->Close();
        }
        communicationClient_.reset();
    }

    /// @brief The copy constructor shall not be used.
    /// @param se The object to be copied.
    /// @trace_id_sr=SR_PHM_01001,SR_PHM_01018,SR_PHM_01019,SR_PHM_01020
    /// @needwork = ad
    SupervisedEntity(const SupervisedEntity &se) = delete;

    /// @brief The copy assignment operator shall not be used.
    /// @param se The object to be copied.
    /// @return SupervisedEntity& The copied object.
    /// @trace_id_sr=SR_PHM_01001,SR_PHM_01018,SR_PHM_01019,SR_PHM_01020
    /// @needwork = ad
    SupervisedEntity &operator=(const SupervisedEntity &se) = delete;

    /// @brief Move constructor.
    /// @param se The object to be moved.
    /// @trace_id_sr=SR_PHM_01001,SR_PHM_01018,SR_PHM_01019,SR_PHM_01020
    /// @needwork = ad
    SupervisedEntity(SupervisedEntity &&se) noexcept
        : instance_{std::move(se.instance_)}, communicationClient_{std::move(se.communicationClient_)}
    {
    }

    /// @brief Move assignment operator.
    /// @param se The object to be moved.
    /// @return SupervisedEntity& The moved object.
    /// @trace_id_sr=SR_PHM_01001,SR_PHM_01018,SR_PHM_01019,SR_PHM_01020
    /// @needwork = ad
    SupervisedEntity &operator=(SupervisedEntity &&se) noexcept
    {
        instance_            = std::move(se.instance_);
        communicationClient_ = std::move(se.communicationClient_);
        return *this;
    }

private:
    /// @brief instance specifier of this supervised entity
    /// @trace_id_sr=SR_PHM_01001,SR_PHM_01018,SR_PHM_01019,SR_PHM_01020
    /// @needwork = dda
    ara::core::InstanceSpecifier instance_;

    /// @brief used to communicate with PHMD.
    /// @trace_id_sr=SR_PHM_01001,SR_PHM_01018,SR_PHM_01019,SR_PHM_01020
    /// @needwork = dda
    std::shared_ptr< ara::phm::internal::svcom::Client > communicationClient_;
};

}  // namespace phm
}  // namespace ara

#endif  // ARA_PHM_SUPERVISED_ENTITY_H_
