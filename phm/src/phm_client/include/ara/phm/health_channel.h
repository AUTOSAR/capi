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
/// @file       health_channel.h
/// @brief      used to report health status.
/// @details
/// @date       2024-06-20
/// @author     wangyanlong
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/PlatformHealthManagement/HealthChannel
/// @unit_description=used to report health status.
/// @trace_id_sr=SR_PHM_01024
/// @unit_name=HealthChannel
/// @interface_level=software
/// @endcode
///
/// ================================================================

#ifndef ARA_PHM_HEALTH_CHANNEL_H_
#define ARA_PHM_HEALTH_CHANNEL_H_

#include <ara/core/instance_specifier.h>
#include <ara/core/result.h>

#include <cstdint>
#include <utility>

#include "ara/phm/internal/com/hc_client.h"
#include "ara/phm/internal/phm_log.h"
#include "ara/phm/internal/types.h"
#include "ara/phm/phm_error_domain.h"

namespace ara {
namespace phm {

/// @brief HealthChannel provide client to report health status.
/// @tparam EnumT Health status enum.
/// @trace_id_sr=SR_PHM_01024
/// @needwork = ad
template < typename EnumT >
class HealthChannel
{
public:
    /// @brief Constructor.
    /// @param instance instance specifier of the health channel.
    /// @trace_id_sr=SR_PHM_01024
    /// @needwork = ad
    explicit HealthChannel(const ara::core::InstanceSpecifier& instance) : instance_(instance)  //NOLINT
    {
        communicationClient_ = ara::phm::internal::hccom::Client::GetInstanceShared();
        if (communicationClient_ == nullptr) {
            std::terminate();
        }
    }

    /// @brief Reports a Health Status.
    /// @param healthStatusId The identifier representing the Health Status. The mapping is implementation specific.
    /// @return ara::core::Result<void> A Result, being either empty or containing an implementation specific error
    /// code.
    /// @trace_id_sr=SR_PHM_01024
    /// @needwork = ad
    ara::core::Result< void > ReportHealthStatus(EnumT healthStatusId)
    {
        if (!communicationClient_->IsOpened()) {
            if (communicationClient_->Open() < 0) {
                LOG_FATAL << instance_
                          << " HealthChannel::ReportHealthStatus, open communication "
                             "client failed.";
                return ara::core::Result< void >::FromError(PhmErrc::kOfferFailed);
            }
        }

        if (communicationClient_->ReportHealthStatus(static_cast< ara::core::String >(instance_.ToString().begin()),
                                                     static_cast< ara::phm::HealthStatus >(healthStatusId))
            < 0) {
            return ara::core::Result< void >::FromError(PhmErrc::kOfferFailed);
        }
        return ara::core::Result< void >::FromValue();
    }

    /// @brief Destructor.
    /// @trace_id_sr=SR_PHM_01024
    /// @needwork = ad
    ~HealthChannel() noexcept
    {
        if (communicationClient_.use_count() == 2) {
            LOG_INFO << "HealthChannel, close communicationClient_";
            communicationClient_->Close();
        }
        communicationClient_.reset();
    }

    /// @brief The copy constructor shall not be used.
    /// @param channel The object to be copied.
    /// @trace_id_sr=SR_PHM_01024
    /// @needwork = ad
    HealthChannel(const HealthChannel& channel) = delete;

    /// @brief The copy assignment shall not be used.
    /// @param channel The object to be copied.
    /// @return HealthChannel& The copied object
    /// @trace_id_sr=SR_PHM_01024
    /// @needwork = ad
    HealthChannel& operator=(const HealthChannel& channel) = delete;

    /// @brief The move constructor.
    /// @param channel The object to be moved.
    /// @trace_id_sr=SR_PHM_01024
    /// @needwork = ad
    HealthChannel(HealthChannel&& channel) noexcept
        : instance_{std::move(channel.instance_)}, communicationClient_{std::move(channel.communicationClient_)}
    {
    }

    /// @brief The move assignment.
    /// @param channel The object to be moved.
    /// @return HealthChannel& The moved object
    /// @trace_id_sr=SR_PHM_01024
    /// @needwork = ad
    HealthChannel& operator=(const HealthChannel&& channel) noexcept
    {
        instance_            = std::move(channel.instance_);
        communicationClient_ = std::move(channel.communicationClient_);
        return *this;
    }

private:
    /// @brief instance specifier of this health channel
    /// @trace_id_sr=SR_PHM_01024
    /// @needwork = dda
    ara::core::InstanceSpecifier instance_;

    /// @brief used to communicate with PHMD.
    /// @trace_id_sr=SR_PHM_01024
    /// @needwork = dda
    std::shared_ptr< ara::phm::internal::hccom::Client > communicationClient_;
};

}  // namespace phm
}  // namespace ara

#endif  // ARA_PHM_HEALTH_CHANNEL_H_
