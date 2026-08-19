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
/// @file       ecu_reset_request_impl.h
/// @brief      A implementation of EcuResetRequest.
/// @details
/// @date       2024-06-25
/// @author     lianglongxiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/StateManagement/DMComm
/// @unit_name=EcuResetRequestImpl
/// @interface_level=uint
/// @unit_description=A implementation of EcuResetRequest.
/// @trace_id_sr=SR_SM_03002
/// @endcode
///
/// ================================================================

#ifndef ECU_RESET_REQUEST_IMPL_H_
#define ECU_RESET_REQUEST_IMPL_H_

#include <ara/core/promise.h>
#include <ara/diag/ecu_reset_request.h>
#include <ara/log/logger.h>

#include "event.h"

namespace ara {
namespace sm {
namespace dm_comm {

/// @brief Specific implementation of diag::EcuResetRequest
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_SM_03002
/// @trace_id_ad=AD_SM_08057
/// @trace_id_dd=DD_SM_08149
/// @needwork = ad
/// @endcode
class EcuResetRequestImpl : public diag::EcuResetRequest
{
public:
    /// @brief Abstract skeleton class which is implemented here.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = no
    /// @endcode
    using EcuResetRequest = diag::EcuResetRequest;

    /// @brief Constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_03002
    /// @trace_id_ad=AD_SM_00147
    /// @trace_id_dd=DD_SM_00147
    /// @needwork = ad
    /// @endcode
    explicit EcuResetRequestImpl(core::InstanceSpecifier const &specifier) noexcept;

    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_03002
    /// @trace_id_ad=AD_SM_00148
    /// @trace_id_dd=DD_SM_00148
    /// @needwork = ad
    /// @endcode
    ~EcuResetRequestImpl() noexcept override;

    /// @brief deleted copy constructor function
    /// @param other The EcuResetRequestImpl instance to be copyed
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_03002
    /// @trace_id_ad=AD_SM_00149
    /// @trace_id_dd=DD_SM_00149
    /// @needwork = ad
    /// @endcode
    EcuResetRequestImpl(EcuResetRequestImpl const &other) = delete;

    /// @brief deleted copy assignment function
    /// @param other The EcuResetRequestImpl instance to be copyed
    /// @return the assigned EcuResetRequestImpl instance itself
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_03002
    /// @trace_id_ad=AD_SM_00150
    /// @trace_id_dd=DD_SM_00150
    /// @needwork = ad
    /// @endcode
    EcuResetRequestImpl &operator=(EcuResetRequestImpl const &other) = delete;

    /// @brief Move constructor function
    /// @param other The EcuResetRequestImpl instance to be moved
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_03002
    /// @trace_id_ad=AD_SM_00151
    /// @trace_id_dd=DD_SM_00151
    /// @needwork = ad
    /// @endcode
    EcuResetRequestImpl(EcuResetRequestImpl &&other) = delete;

    /// @brief Move assignment function
    /// @param other The EcuResetRequestImpl instance to be moved
    /// @return the assigned EcuResetRequestImpl instance itself
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_03002
    /// @trace_id_ad=AD_SM_00152
    /// @trace_id_dd=DD_SM_00152
    /// @needwork = ad
    /// @endcode
    EcuResetRequestImpl &operator=(EcuResetRequestImpl &&other) = delete;

    /// @brief Register the callback function for appending events
    /// @param appendEventHandler
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_03002
    /// @trace_id_ad=AD_SM_00153
    /// @trace_id_dd=DD_SM_00153
    /// @needwork = ad
    /// @endcode
    void RegisterAppendEventHandler(std::function< void(common::Event &&) > const &appendEventHandler) noexcept;

    /// @brief Get the last reset cause
    /// @return The type of the last machine reset. error code: kRequestFailed
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_03002
    /// @trace_id_ad=AD_SM_00154
    /// @trace_id_dd=DD_SM_00154
    /// @needwork = ad
    /// @endcode
    core::Result< diag::LastResetType > GetLastResetCause() noexcept override;

    /// @brief interface for subFunction En-/DisableRapidShutdown
    /// @param enable when enable is set to true the rapid shutdown will be enabled, setting enable to false will disable rapid shutdown
    /// @param metaInfo MetaInfo of the request.
    /// @param cancellationHandler Set if the current conversation is canceled.
    /// @return core::Future<void> error code: kRejected, kResetTypeNotSupported
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_03002
    /// @trace_id_ad=AD_SM_00155
    /// @trace_id_dd=DD_SM_00155
    /// @needwork = ad
    /// @endcode
    core::Future< void > EnableRapidShutdown(bool enable,
                                             diag::MetaInfo const &metaInfo,
                                             diag::CancellationHandler cancellationHandler) noexcept override;

    /// @brief Called for any EcuRest subFunction, except En-/DisableRapidShutdown.
    /// StateManagement needs to evalute carefully if the request to restart parts or the whole machine. Once the request to reset is accepted, the
    /// StateManagement has to rely on this decision for the ExecuteReset() trigger.
    /// @param resetType Type of the requested reset.
    /// @param id id of the custom reset type. Will only be evaluated when resetType is "custom"
    /// @param metaInfo MetaInfo of the request.
    /// @param cancellationHandler Set if the current conversation is canceled.
    /// @return core::Future<void> error code : kRejected, kRequestFailed, kCustomResetTypeNotSupported, kResetTypeNotSupported
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_03002
    /// @trace_id_ad=AD_SM_00156
    /// @trace_id_dd=DD_SM_00156
    /// @needwork = ad
    /// @endcode
    core::Future< void > RequestReset(diag::ResetRequestType resetType,
                                      core::Optional< std::uint8_t > id,
                                      diag::MetaInfo const &metaInfo,
                                      diag::CancellationHandler cancellationHandler) noexcept override;

    /// @brief StateManagement has to execute the requested reset.
    /// Called after DM sent the response message to tester.
    /// @param metaInfo  MetaInfo of the request.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_03002
    /// @trace_id_ad=AD_SM_00157
    /// @trace_id_dd=DD_SM_00157
    /// @needwork = ad
    /// @endcode
    void ExecuteReset(diag::MetaInfo metaInfo) noexcept override;

private:
    /// @brief Convert diag reset type to SM internal reset type
    /// @param resetType Diag reset type
    /// @return SM internal reset type
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_03002
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08150
    /// @needwork = dda
    /// @endcode
    common::DiagResetRequestInternalType _toInternalType(diag::ResetRequestType const resetType) const noexcept;

    /// @brief Convert SM internal reset type to diag reset type
    /// @param resetType SM internal reset type
    /// @return Diag reset type
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_03002
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08151
    /// @needwork = dda
    /// @endcode
    diag::ResetRequestType _toDiagType(common::DiagResetRequestInternalType const resetType) const noexcept;

    /// @brief Function handle for publishing events to EventManager
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_03002
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08152
    /// @needwork = dda
    /// @endcode
    std::function< void(common::Event &&) > appendEventHandler_;

    /// @brief Logger instance
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_03002
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08153
    /// @needwork = dda
    /// @endcode
    log::Logger &log_;

    /// @brief Hexadecimal
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_03002
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08154
    /// @needwork = dda
    /// @endcode
    int32_t const kHex;
};
}  // namespace dm_comm
}  // namespace sm
}  // namespace ara

#endif  // ECU_RESET_REQUEST_IMPL_H_
