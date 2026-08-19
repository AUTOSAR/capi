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
/// @brief      This file provides the definitions of DownloadService and related types.
/// @details
/// @date       2021-11-23
/// @author     jiawei
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_DIAG_DOWNLOAD_H_
#define ARA_DIAG_DOWNLOAD_H_

#include <ara/core/future.h>
#include <ara/core/instance_specifier.h>
#include <ara/core/result.h>
#include <ara/core/span.h>
#include <ara/core/vector.h>

#include <cstdint>
#include <memory>

#include "cancellation_handler.h"
#include "meta_info.h"
#include "reentrancy.h"

namespace isoft {
namespace dm {
namespace dis {
/// @brief Declare DownloadServiceAgent
class DownloadServiceAgent;
}  // namespace dis
}  // namespace dm
}  // namespace isoft

namespace ara {
namespace diag {

namespace api {
/// @brief Declare DownloadServiceSkeleton
class DownloadServiceSkeleton;
}  // namespace api

/// @brief Download service interface
///
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
///
/// @traceid{SWS_DM_00784}@tracestatus{draft}
class DownloadService
{
public:
    /// @brief Response data of positive respone message
    ///
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    ///
    /// @traceid{SWS_DM_00785}@tracestatus{draft}
    struct OperationOutput
    {
        /// @brief Content of positive respone message (without SID)
        /// Depending on the operation (e.g.: DownloadData, RequestDownloadExit) the expectation, what
        /// responseData shall contain (where it starts in the positive response) might differ. See
        /// doc of corresponding operation.
        ///
        /// @traceid{SWS_DM_00786}@tracestatus{draft}
        ara::core::Vector< std::uint8_t > responseData;
    };

public:
    /// @brief copy constructor
    /// @param other
    DownloadService(DownloadService const& other) noexcept = default;
    /// @brief copy assignment operator
    /// @param other
    /// @return DownloadService&
    DownloadService& operator=(DownloadService const& other) noexcept = default;
    /// @brief move constructor
    /// @param other
    DownloadService(DownloadService&& other) noexcept = default;
    /// @brief move assignment operator
    /// @param other
    /// @return DownloadService&
    DownloadService& operator=(DownloadService&& other) noexcept = default;

public:
    /// @brief Class for an DownloadService
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] specifier InstanceSpecifier to an PortPrototype of an DownloadServiceInterface
    /// @param[in] reentrancyType specifies if interface is callable fully- or non-reentrant
    /// @throws on overflow
    ///
    /// @traceid{SWS_DM_00787}@tracestatus{draft}
    explicit DownloadService(ara::core::InstanceSpecifier const& specifier, ReentrancyType reentrancyType);

    /// @brief Destructor of class DownloadService
    ///
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    ///
    /// @traceid{SWS_DM_00788}@tracestatus{draft}
    virtual ~DownloadService() noexcept = default;

    /// @brief Called for RequestDownload.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] dataFormatIdentifier UDS dataFormat Identifier
    /// @param[in] addressAndLengthFormatIdentifier UDS addressAndLengthFormatIdentifier
    /// @param[in] memoryAddressAndSize memoryAddress and memorySize part of the request
    /// @param[in] metaInfo contains additional meta information
    /// @param[in] cancellationHandler informs if the current conversation is canceled
    /// @return a Future, which either gets readied to void (for a positive response message) or readied with ErrorCode
    /// from DiagUdsNrcErrc (for an negative response message)
    /// error code: kIncorrectMessageLengthOrInvalidFormat, kConditionsNotCorrect, kRequestOutOfRange,
    /// kSecurityAccessDenied kUploadDownloadNotAccepted
    /// @throws on overflow
    ///
    /// @traceid{SWS_DM_00789}@tracestatus{draft}
    virtual ara::core::Future< void > RequestDownload(std::uint8_t dataFormatIdentifier,
                                                      std::uint8_t addressAndLengthFormatIdentifier,
                                                      ara::core::Span< std::uint8_t > memoryAddressAndSize,
                                                      MetaInfo& metaInfo,
                                                      CancellationHandler cancellationHandler)
        = 0;

    /// @brief Called for TransferData following a previous RequestDownload.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] transferRequestParameterRecord data to be transferred (copied/downloaded to the ECU/server).
    /// @param[in] metaInfo contains additional meta information
    /// @param[in] cancellationHandler informs if the current conversation is canceled
    /// @return a Future, which either gets readied to OperationOutput (transferResponseParameterRecord for a positive
    /// response message) or readied with ErrorCode from DiagUdsNrcErrc (for an negative response message). Data in
    /// OperationOutput.responseData will be placed after blockSequenceCounter as transferResponseParameterRecord in the
    /// positive response.
    /// error code: kIncorrectMessageLengthOrInvalidFormat, kRequestSequenceError, kRequestOutOfRange,
    /// kTransferDataSuspended kGeneralProgrammingFailure, kWrongBlockSequenceCounter, kVoltageTooHigh, kVoltageTooLow
    ///
    ///
    /// @traceid{SWS_DM_00790}@tracestatus{draft}
    virtual ara::core::Future< OperationOutput > DownloadData(
        ara::core::Span< std::uint8_t > transferRequestParameterRecord,
        MetaInfo& metaInfo,
        CancellationHandler cancellationHandler)
        = 0;

    /// @brief Called for RequestTransferExit.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] transferRequestParameterRecord This parameter record contains parameter(s), which are required by the
    /// server to support the transfer of data. Format and length of this parameter(s) are vehicle manufacturer
    /// specific.
    /// @param[in] metaInfo contains additional meta information
    /// @param[in] cancellationHandler informs if the current conversation is canceled
    /// @return a Future, which either gets readied to OperationOutput (transferResponseParameterRecord for a positive
    /// response message) or readied with ErrorCode from DiagUdsNrcErrc (for an negative response message) Data in
    /// OperationOutput.responseData will be placed after SID as transferResponseParameterRecord in the positive
    /// response.
    /// @throws on overflow
    ///
    /// @traceid{SWS_DM_00791}@tracestatus{draft}
    virtual ara::core::Future< OperationOutput > RequestDownloadExit(
        ara::core::Span< std::uint8_t > transferRequestParameterRecord,
        MetaInfo& metaInfo,
        CancellationHandler cancellationHandler)
        = 0;

    /// @brief This Offer will enable the DM to forward request messages to this handler
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @return ara::core::Result<void> error code: kNotOffered, kGenericError, kAlreadyOffered
    /// @throws on overflow
    ///
    /// @traceid{SWS_DM_00792}@tracestatus{draft}
    ara::core::Result< void > Offer();

    /// @brief This StopOffer will disable the forwarding of request messages from DM
    ///
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @throws on overflow
    /// @traceid{SWS_DM_00793}@tracestatus{draft}
    void StopOffer();

private:
    /// @name specifier_
    ara::core::InstanceSpecifier specifier_;
    /// @name reentrancyType_
    ReentrancyType reentrancyType_;

    std::shared_ptr< isoft::dm::dis::DownloadServiceAgent >
        /// @name skeleton_
        skeleton_{};
};

}  // namespace diag
}  // namespace ara

#endif  // ARA_DIAG_DOWNLOAD_H_