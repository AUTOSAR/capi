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
/// @file       upload.h
/// @brief      This file provides upload functionality
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
/// @trace_id_sr=92c527c8-027c-40ae-92d2-d72d389dab38
/// @unit_name=upload
/// @unit_description=This file provides upload functionality
/// @endcode
///
/// ================================================================

#ifndef ISOFT_UDS_SERVER_SERVICE_UPLOAD_H_
#define ISOFT_UDS_SERVER_SERVICE_UPLOAD_H_
#include <functional>
#include <vector>

#include "isoft/uds/cancellation_handler.h"
#include "isoft/uds/common.h"
#include "isoft/uds/result.h"
namespace isoft {
namespace uds {
namespace server {
/// @brief Upload interface
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01328
/// @trace_id_dd=DD_UDS_02620
/// @needwork = ad
/// @endcode
class UploadInterface
{
public:
    /// @param
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00938
    /// @needwork = dda
    /// @endcode
    UploadInterface(UploadInterface const&) = delete;
    /// @param
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00939
    /// @needwork = dda
    /// @endcode
    UploadInterface& operator=(UploadInterface const&) = delete;
    /// @param
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00940
    /// @needwork = dda
    /// @endcode
    UploadInterface(UploadInterface&&) = delete;
    /// @param
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00941
    /// @needwork = dda
    /// @endcode
    UploadInterface& operator=(UploadInterface&&) = delete;

public:
    /// @brief Constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01037
    /// @trace_id_dd=DD_UDS_02151
    /// @needwork = ad
    /// @endcode
    UploadInterface(/* args */) = default;
    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01038
    /// @trace_id_dd=DD_UDS_02152
    /// @needwork = ad
    /// @endcode
    virtual ~UploadInterface() noexcept = default;

    /// @brief Request to upload data
    /// @param[in] dataFormatIdentifier Data format identifier
    /// @param[in] addressAndLengthFormatIdentifier Address and length format identifier
    /// @param[in] memoryAddressAndSize Memory address and size
    /// @param[in] metaInfo Additional meta information
    /// @param[in] cancellationHandler Cancellation handler
    /// @return Returns void on success, UDS NRC error code on failure
    /// @throws Throws exception on overflow
    /// @code{.isoft}
    /// @retval kIncorrectMessageLengthOrInvalidFormat Message length error or invalid format
    /// @retval kConditionsNotCorrect Condition not correct
    /// @retval kRequestOutOfRange Request out of range
    /// @retval kSecurityAccessDenied Security access denied
    /// @retval kUploadDownloadNotAccepted Upload/Download not accepted
    /// @interface_level=unit
    /// @needwork=ad
    /// @trace_id_sws={SWS_DM_00799}@tracestatus{draft}
    /// @endcode
    virtual Result< void > RequestUpload(std::uint8_t dataFormatIdentifier,
                                         std::uint8_t addressAndLengthFormatIdentifier,
                                         std::vector< std::uint8_t > memoryAddressAndSize,
                                         MetaInfoMap& metaInfo,
                                         CancellationHandler cancellationHandler) noexcept = 0;

    /// @brief Upload data
    /// @param[in] blockSequenceCounter Block sequence counter
    /// @param[in] numBytesToReturn Number of bytes DM can accept for this data block (based on its internal buffer)
    /// @param[in] metaInfo Additional meta information
    /// @param[in] cancellationHandler Cancellation handler
    /// @return Returns data vector on success, UDS NRC error code on failure
    /// @code{.isoft}
    /// @retval kIncorrectMessageLengthOrInvalidFormat Message length error or invalid format
    /// @retval kRequestSequenceError Request sequence error
    /// @retval kRequestOutOfRange Request out of range
    /// @retval kTransferDataSuspended Transfer data suspended
    /// @retval kGeneralProgrammingFailure General programming failure
    /// @retval kWrongBlockSequenceCounter Wrong block sequence counter
    /// @retval kVoltageTooHigh Voltage too high
    /// @retval kVoltageTooLow Voltage too low
    /// @interface_level=unit
    /// @needwork=ad
    /// @trace_id_sws={SWS_DM_00800}@tracestatus{draft}
    /// @endcode
    virtual Result< std::vector< std::uint8_t > > UploadData(std::uint8_t blockSequenceCounter,
                                                             std::size_t numBytesToReturn,
                                                             MetaInfoMap& metaInfo,
                                                             CancellationHandler cancellationHandler) noexcept = 0;

    /// @brief Request to exit upload
    /// @param[in] transferRequestParameterRecord Transfer request parameter record, containing parameters required for server to support data transfer, format and length defined by vehicle manufacturer
    /// @param[in] metaInfo Additional meta information
    /// @param[in] cancellationHandler Cancellation handler
    /// @return Returns transfer response parameter record data on success, UDS NRC error code on failure
    /// @throws Throws exception on overflow
    /// @code{.isoft}
    /// @retval kIncorrectMessageLengthOrInvalidFormat Message length error or invalid format
    /// @retval kRequestSequenceError Request sequence error
    /// @retval kRequestOutOfRange Request out of range
    /// @retval kGeneralProgrammingFailure General programming failure
    /// @interface_level=unit
    /// @needwork=ad
    /// @trace_id_sws={SWS_DM_00801}@tracestatus{draft}
    /// @endcode
    virtual Result< std::vector< std::uint8_t > > RequestUploadExit(
        std::vector< std::uint8_t > transferRequestParameterRecord,
        MetaInfoMap& metaInfo,
        CancellationHandler cancellationHandler) noexcept = 0;
};

}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  //ISOFT_UDS_SERVER_SERVICE_UPLOAD_H_