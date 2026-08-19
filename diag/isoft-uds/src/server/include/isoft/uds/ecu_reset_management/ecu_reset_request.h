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
/// @file       ecu_reset_request.h
/// @brief      This file provides ecu_reset_request functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/isoft/uds/ecu_reset_management
/// @interface_level=software
/// @trace_id_sr=12e57faa-6602-4821-b13d-d6a6c7661258
/// @unit_name=EcuResetRequest
/// @unit_description=This file provides ecu_reset_request functionality
/// @endcode
///
/// ================================================================

#ifndef ISOFT_DM_ECURESETREQUEST_H_
#define ISOFT_DM_ECURESETREQUEST_H_

#include <ara/core/optional.h>

#include <cstdint>
#include <memory>

#include "isoft/uds/cancellation_handler.h"
#include "isoft/uds/common.h"
#include "isoft/uds/result.h"

namespace isoft {
namespace uds {
namespace server {
/// @brief Define uint8_t alias for reset request type
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01339
/// @trace_id_dd=DD_UDS_02631
/// @needwork = ad
/// @endcode
using ResetRequestType = std::uint8_t;
/// @brief Soft reset type
/// @code{.isoft}
/// @interface_level=unit
/// @needwork=ad
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_
/// @trace_id_dd=DD_UDS_00969
/// @needwork = dd
/// @endcode
constexpr ResetRequestType kSoftResetType{0};  ///<  softReset.
/// @brief Hard reset type
/// @code{.isoft}
/// @interface_level=unit
/// @needwork=ad
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_
/// @trace_id_dd=DD_UDS_00970
/// @needwork = dd
/// @endcode
constexpr ResetRequestType kHardResetType{1};  ///<  hardReset
/// @brief Key/on/off reset type
/// @code{.isoft}
/// @interface_level=unit
/// @needwork=ad
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_
/// @trace_id_dd=DD_UDS_00971
/// @needwork = dd
/// @endcode
constexpr ResetRequestType kKeyOffOnResetType{2};  ///<  keyOffOnReset
/// @brief Custom reset type
/// @code{.isoft}
/// @interface_level=unit
/// @needwork=ad
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_
/// @trace_id_dd=DD_UDS_00972
/// @needwork = dd
/// @endcode
constexpr ResetRequestType kCustomResetType{3};  ///<  kCustomReset

/// @brief ECU reset request interface
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01340
/// @trace_id_dd=DD_UDS_02632
/// @needwork = ad
/// @endcode
class EcuResetRequestInterface
{
public:
    /// @param
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00973
    /// @needwork = dda
    /// @endcode
    EcuResetRequestInterface(EcuResetRequestInterface const&) = delete;
    /// @param
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00974
    /// @needwork = dda
    /// @endcode
    EcuResetRequestInterface& operator=(EcuResetRequestInterface const&) = delete;
    /// @param
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00975
    /// @needwork = dda
    /// @endcode
    EcuResetRequestInterface(EcuResetRequestInterface&&) = delete;
    /// @param
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00976
    /// @needwork = dda
    /// @endcode
    EcuResetRequestInterface& operator=(EcuResetRequestInterface&&) = delete;

public:
    /// @brief Constructor
    /// @throws on overflow
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01050
    /// @trace_id_dd=DD_UDS_02164
    /// @needwork = ad
    /// @endcode
    explicit EcuResetRequestInterface() = default;

    /// @brief Destructor
    /// @throws on overflow
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01051
    /// @trace_id_dd=DD_UDS_02165
    /// @needwork = ad
    /// @endcode
    virtual ~EcuResetRequestInterface() noexcept = default;

    /// @brief Enable or disable fast shutdown function
    /// @param[in] enable Set to true to enable fast shutdown, set to false to disable fast shutdown
    /// @param[in] metaInfo Request meta information
    /// @param[in] cancellationHandler Set when current session is cancelled
    /// @return Result<void> Error codes: kRejected, kResetTypeNotSupported
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=ad
    /// @retval kRejected Request rejected
    /// @retval kResetTypeNotSupported Reset type not supported
    /// @note This interface is a pure virtual function and needs to be implemented by derived classes
    /// @trace_id_sws={SWS_DM_01012}@tracestatus{draft}
    /// @endcode
    virtual Result< void > EnableRapidShutdown(bool enable,
                                               MetaInfoMap const& metaInfo,
                                               CancellationHandler cancellationHandler)
        = 0;
    /// @brief Request ECU reset
    /// @param[in] resetType Requested reset type
    /// @param[in] id ID for custom reset type, only valid when resetType is "custom"
    /// @param[in] metaInfo Request meta information
    /// @param[in] cancellationHandler Set when current session is cancelled
    /// @return Result<void> Error codes: kRejected, kRequestFailed, kCustomResetTypeNotSupported, kResetTypeNotSupported
    /// @code{.isoft}
    /// @details StateManagement needs to carefully evaluate whether to restart partially or the entire machine. Once the reset request is accepted,
    ///          StateManagement must rely on this decision when ExecuteReset() is triggered.
    /// @interface_level=unit
    /// @needwork=ad
    /// @retval kRejected Request rejected
    /// @retval kRequestFailed Request failed
    /// @retval kCustomResetTypeNotSupported Custom reset type not supported
    /// @retval kResetTypeNotSupported Reset type not supported
    /// @note This interface is a pure virtual function and needs to be implemented by derived classes
    /// @trace_id_sws={SWS_DM_01013}@tracestatus{draft}
    /// @endcode
    virtual Result< void > RequestReset(ResetRequestType resetType,
                                        ara::core::Optional< std::uint8_t > id,
                                        MetaInfoMap const& metaInfo,
                                        CancellationHandler cancellationHandler) noexcept = 0;

    /// @brief Called after DM sends response message to tester
    /// @param[in] metaInfo Request meta information
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=ad
    /// @note This interface is a pure virtual function and needs to be implemented by derived classes
    /// @trace_id_sws={SWS_DM_01014}@tracestatus{draft}
    /// @endcode
    virtual void ExecuteReset(MetaInfoMap metaInfo) noexcept = 0;
};
}  // namespace server
}  // namespace uds
}  // namespace isoft
#endif  // ISOFT_DM_ECURESETREQUEST_H_