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
/// @file       download.h
/// @brief      This file provides download functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/isoft/uds/transfer_managment
/// @interface_level=software
/// @trace_id_sr=439a48c1-5528-4596-9ce4-c505850f15fc
/// @unit_name=download
/// @unit_description=This file provides download functionality
/// @endcode
///
/// ================================================================

#ifndef ISOFT_UDS_SERVER_SERVICE_DOWNLOAD_H_
#define ISOFT_UDS_SERVER_SERVICE_DOWNLOAD_H_
#include <functional>
#include <vector>

#include "isoft/uds/cancellation_handler.h"
#include "isoft/uds/common.h"
#include "isoft/uds/result.h"
namespace isoft {
namespace uds {
namespace server {
/// @brief Download interface
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01331
/// @trace_id_dd=DD_UDS_02623
/// @needwork = ad
/// @endcode
class DownloadInterface
{
public:
    /// @param
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00946
    /// @needwork = dda
    /// @endcode
    DownloadInterface(DownloadInterface const&) = delete;
    /// @param
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00947
    /// @needwork = dda
    /// @endcode
    DownloadInterface& operator=(DownloadInterface const&) = delete;
    /// @param
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00948
    /// @needwork = dda
    /// @endcode
    DownloadInterface(DownloadInterface&&) = delete;
    /// @param
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00949
    /// @needwork = dda
    /// @endcode
    DownloadInterface& operator=(DownloadInterface&&) = delete;

public:
    /// @brief Constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01039
    /// @trace_id_dd=DD_UDS_02153
    /// @needwork = ad
    /// @endcode
    DownloadInterface(/* args */) = default;
    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01040
    /// @trace_id_dd=DD_UDS_02154
    /// @needwork = ad
    /// @endcode
    virtual ~DownloadInterface() noexcept = default;

    /// @brief Request to download data
    /// @param[in] dataFormatIdentifier UDS data format identifier
    /// @param[in] addressAndLengthFormatIdentifier UDS address and length format identifier
    /// @param[in] memoryAddressAndSize Memory address and size (request parameter part)
    /// @param[in] metaInfo Contains additional meta information
    /// @param[in] cancellationHandler Used to notify whether the current session is cancelled
    /// @return Returns a Future object, returns void on success (positive response), returns NrcErrc error code on failure (negative response)
    /// @throws Throws exception on overflow
    /// @code{.isoft}
    /// @retval kIncorrectMessageLengthOrInvalidFormat Message length incorrect or format invalid
    /// @retval kConditionsNotCorrect Condition not correct
    /// @retval kRequestOutOfRange Request out of range
    /// @retval kSecurityAccessDenied Security access denied
    /// @retval kUploadDownloadNotAccepted Upload/Download not accepted
    /// @interface_level=unit
    /// @needwork=ad
    /// @trace_id_sws={SWS_DM_00789}@tracestatus{draft}
    /// @endcode
    virtual Result< void > RequestDownload(std::uint8_t dataFormatIdentifier,
                                           std::uint8_t addressAndLengthFormatIdentifier,
                                           std::vector< std::uint8_t > memoryAddressAndSize,
                                           MetaInfoMap& metaInfo,
                                           CancellationHandler cancellationHandler) noexcept = 0;

    /// @brief Download data
    /// @param[in] blockSequenceCounter Block sequence counter
    /// @param[in] transferRequestParameterRecord Data to be transferred (copied/downloaded to ECU/server)
    /// @param[in] metaInfo Contains additional meta information
    /// @param[in] cancellationHandler Used to notify whether the current session is cancelled
    /// @return Returns a Future object, returns void on success (positive response), returns NrcErrc error code on failure (negative response)
    /// @code{.isoft}
    /// @retval kIncorrectMessageLengthOrInvalidFormat Message length incorrect or format invalid
    /// @retval kRequestSequenceError Request sequence error
    /// @retval kRequestOutOfRange Request out of range
    /// @retval kTransferDataSuspended Transfer data suspended
    /// @retval kGeneralProgrammingFailure General programming failure
    /// @retval kWrongBlockSequenceCounter Wrong block sequence counter
    /// @retval kVoltageTooHigh Voltage too high
    /// @retval kVoltageTooLow Voltage too low
    /// @interface_level=unit
    /// @needwork=ad
    /// @trace_id_sws={SWS_DM_00790}@tracestatus{draft}
    /// @endcode
    virtual Result< void > DownloadData(std::uint8_t blockSequenceCounter,
                                        std::vector< std::uint8_t > transferRequestParameterRecord,
                                        MetaInfoMap& metaInfo,
                                        CancellationHandler cancellationHandler) noexcept = 0;

    /// @brief Request to exit download
    /// @param[in] transferRequestParameterRecord Transfer request parameter record, containing parameters required for server to support data transfer, format and length defined by vehicle manufacturer
    /// @param[in] metaInfo Contains additional meta information
    /// @param[in] cancellationHandler Used to notify whether the current session is cancelled
    /// @return Returns a Future object, returns transfer response parameter record on success (positive response), returns NrcErrc error code on failure (negative response)
    /// @throws Throws exception on overflow
    /// @code{.isoft}
    /// @retval kRequestSequenceError Request sequence error
    /// @retval kGeneralProgrammingFailure General programming failure
    /// @interface_level=unit
    /// @needwork=ad
    /// @trace_id_sws={SWS_DM_00791}@tracestatus{draft}
    /// @endcode
    virtual Result< std::vector< std::uint8_t > > RequestDownloadExit(
        std::vector< std::uint8_t > transferRequestParameterRecord,
        MetaInfoMap& metaInfo,
        CancellationHandler cancellationHandler) noexcept = 0;
};

}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  //ISOFT_UDS_SERVER_SERVICE_DOWNLOAD_H_