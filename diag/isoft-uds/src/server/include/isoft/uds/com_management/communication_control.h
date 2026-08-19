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
/// @file       communication_control.h
/// @brief      This file provides communication_control functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/isoft/uds/com_management
/// @interface_level=software
/// @trace_id_sr=aad26a33-c1f3-47dd-81c4-96862b8bf1df
/// @unit_name=CommunicationControl
/// @unit_description=This file provides communication_control functionality
/// @endcode
///
/// ================================================================

#ifndef ISOFT_UDS_SERVER_COMMUNICATIONCONTROL_H_
#define ISOFT_UDS_SERVER_COMMUNICATIONCONTROL_H_
#include <functional>
#include <vector>

#include "isoft/uds/cancellation_handler.h"
#include "isoft/uds/common.h"
#include "isoft/uds/result.h"
namespace isoft {
namespace uds {
namespace server {
/// @brief Communication control interface
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = ad
/// @endcode
class CommunicationControlInterface
{
public:
    /// @param
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00954
    /// @needwork = dda
    /// @endcode
    CommunicationControlInterface(CommunicationControlInterface const&) = delete;
    /// @param
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00955
    /// @needwork = dda
    /// @endcode
    CommunicationControlInterface& operator=(CommunicationControlInterface const&) = delete;
    /// @param
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00956
    /// @needwork = dda
    /// @endcode
    CommunicationControlInterface(CommunicationControlInterface&&) = delete;
    /// @param
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00957
    /// @needwork = dda
    /// @endcode
    CommunicationControlInterface& operator=(CommunicationControlInterface&&) = delete;

public:
    /// @brief Communication control request parameter type
    /// @param[in] controlType Control type
    /// @param[in] communicationType Communication type
    /// @param[in] nodeIdentificationNumber Node identification number
    /// @code{.isoft}
    ///
    /// export_level=/Diagnostics
    /// @interface_level=unit
    /// @needwork = ad
    /// @trace_id_sws={SWS_DM_00805}@tracestatus{draft}
    /// @endcode
    struct ComCtrlRequestParamsType
    {
        /// @brief Control type
        std::uint8_t controlType{0U};
        /// @brief Communication type
        std::uint8_t communicationType{0U};
        /// @brief Node identification number
        std::uint16_t nodeIdentificationNumber{0U};
    };
    /// @brief Constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00958
    /// @needwork = dda
    /// @endcode
    CommunicationControlInterface() = default;
    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00959
    /// @needwork = dda
    /// @endcode
    virtual ~CommunicationControlInterface() noexcept = default;

    /// @brief. Typically provider of this interface is considered as part of the state management.
    /// @param[in] controlType All UDS request parameters packed into one structure because it contains optional elements
    /// @param[in] metaInfo Contains additional meta information
    /// @param[in] cancellationHandler Notifies whether the current session is cancelled
    /// @return ara::core::Future<void> Error codes: kSubfunctionNotSupported, kIncorrectMessageLengthOrInvalidFormat,
    /// kConditionsNotCorrect, kRequestOutOfRange
    /// @throws on overflow
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = ad
    /// @trace_id_sws={SWS_DM_00808}@tracestatus{draft}
    /// @endcode
    virtual Result< void > CommCtrlRequest(ComCtrlRequestParamsType controlType,
                                           MetaInfoMap& metaInfo,
                                           CancellationHandler cancellationHandler) noexcept = 0;

private:
};

}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  //ISOFT_UDS_SERVER_SERVICE_COMMUNICATIONCONTROL_H_