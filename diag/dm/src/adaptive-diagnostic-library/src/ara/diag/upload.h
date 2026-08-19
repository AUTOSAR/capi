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
/// @brief      This file provides the definitions of UploadService and related types.
/// @details
/// @date       2021-11-23
/// @author     jiawei
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_DIAG_UPLOAD_H_
#define ARA_DIAG_UPLOAD_H_

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
/// @brief UploadServiceAgent
class UploadServiceAgent;
}  // namespace dis
}  // namespace dm
}  // namespace isoft
namespace ara {
namespace diag {

/// @brief Upload service interface
///
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
///
/// @traceid{SWS_DM_00794}@tracestatus{draft}
class UploadService
{
public:
    /// @brief Response data of positive respone message
    ///
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    ///
    /// @traceid{SWS_DM_00795}@tracestatus{draft}
    struct OperationOutput
    {
        /// @brief Content of positive respone message (without SID)
        /// Depending on the operation (e.g.: UploadData, RequestUploadExit) the expectation, what
        /// responseData shall contain (where it starts in the positive response) might differ. See
        /// doc of corresponding operation.
        ///
        /// @traceid{SWS_DM_00796}@tracestatus{draft}
        ara::core::Vector< std::uint8_t > responseData;
    };

public:
    /// @brief Class for an UploadService
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] specifier InstanceSpecifier to an PortPrototype of an DownloadServiceInterface
    /// @param[in] reentrancyType specifies if interface is callable fully- or non-reentrant
    /// @throws on overflow
    ///
    /// @traceid{SWS_DM_00797}@tracestatus{draft}
    explicit UploadService(ara::core::InstanceSpecifier const& specifier, ReentrancyType reentrancyType);

    /// @brief Destructor of class UploadService
    ///
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @throws on overflow
    /// @traceid{SWS_DM_00798}@tracestatus{draft}
    virtual ~UploadService() noexcept = default;

    /// @brief copy constructor
    /// @param other
    UploadService(UploadService const& other) noexcept = default;
    /// @brief copy assignment operator
    /// @param other
    /// @return UploadService&
    UploadService& operator=(UploadService const& other) noexcept = default;
    /// @brief move constructor
    /// @param other
    UploadService(UploadService&& other) noexcept = default;
    /// @brief move assignment operator
    /// @param other
    /// @return UploadService&
    UploadService& operator=(UploadService&& other) noexcept = default;

    /// @brief Called for RequestDownload.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] dataFormatIdentifier  UDS dataFormat Identifier
    /// @param[in] addressAndLengthFormatIdentifier UDS addressAndLengthFormatIdentifier
    /// @param[in] memoryAddressAndSize memoryAddress and memorySize part of the request
    /// @param[in] metaInfo contains additional meta information
    /// @param[in] cancellationHandler informs if the current conversation is canceled
    /// @returns a Result with either void (for a positive response message) or an UDS NRC value (for an negative
    /// response message)
    /// error code: kIncorrectMessageLengthOrInvalidFormat, kConditionsNotCorrect, kRequestOutOfRange,
    /// kSecurityAccessDenied, kUploadDownloadNotAccepted
    /// @throws on overflow
    ///
    /// @traceid{SWS_DM_00799}@tracestatus{draft}
    virtual ara::core::Future< void > RequestUpload(std::uint8_t dataFormatIdentifier,
                                                    std::uint8_t addressAndLengthFormatIdentifier,
                                                    ara::core::Span< std::uint8_t > memoryAddressAndSize,
                                                    MetaInfo& metaInfo,
                                                    CancellationHandler cancellationHandler)
        = 0;

    /// @brief Called for TransferData following a previous RequestUpload.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] numBytesToReturn number of bytes DM accepts (due to its internal buffer) for this chunk.
    /// @param[in] metaInfo contains additional meta information
    /// @param[in] cancellationHandler informs if the current conversation is canceled
    /// @returns a Future, which either gets readied to OperationOutput (transferResponseParameterRecord for a positive
    /// response message) or readied with ErrorCode from DiagUdsNrcErrc (for an negative response message). Data in
    /// OperationOutput.responseData will be placed after blockSequenceCounter as transferResponseParameterRecord in the
    /// positive response.
    /// error code : kIncorrectMessageLengthOrInvalidFormat, kRequestSequenceError, kRequestOutOfRange,
    /// kTransferDataSuspended, kGeneralProgrammingFailure, kWrongBlockSequenceCounter, kVoltageTooHigh, kVoltageTooLow
    ///
    ///
    /// @traceid{SWS_DM_00800}@tracestatus{draft}
    virtual ara::core::Future< OperationOutput > UploadData(std::size_t numBytesToReturn,
                                                            MetaInfo& metaInfo,
                                                            CancellationHandler cancellationHandler)
        = 0;

    /// @brief Called for RequestTransferExit.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] transferRequestParameterRecord  This parameter record contains parameter(s), which are required by
    /// the server to support the transfer of data. Format and length of this parameter(s) are vehicle manufacturer
    /// specific.
    /// @param[in] metaInfo contains additional meta information
    /// @param[in] cancellationHandler informs if the current conversation is canceled
    /// @returns a Future, which either gets readied to OperationOutput (transferResponseParameterRecord for a positive
    /// response message) or readied with ErrorCode from DiagUdsNrcErrc (for an negative response message) Data in
    /// OperationOutput.responseData will be placed after SID as transferResponseParameterRecord in the positive
    /// response.
    /// error code : kIncorrectMessageLengthOrInvalidFormat, kRequestSequenceError, kRequestOutOfRange,
    ///> kGeneralProgrammingFailure,
    /// @throws on overflow
    ///
    /// @traceid{SWS_DM_00801}@tracestatus{draft}
    virtual ara::core::Future< OperationOutput > RequestUploadExit(
        ara::core::Span< std::uint8_t > transferRequestParameterRecord,
        MetaInfo& metaInfo,
        CancellationHandler cancellationHandler)
        = 0;

    /// @brief This Offer will enable the DM to forward request messages to this handler
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// error code : kNotOffered, kGenericError, AlreadyOffered
    /// @return ara::core::Result<void>
    /// @throws on overflow
    /// @traceid{SWS_DM_00802}@tracestatus{draft}
    ara::core::Result< void > Offer();

    /// @brief This StopOffer will disable the forwarding of request messages from DM
    ///
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @throws on overflow
    /// @traceid{SWS_DM_00803}@tracestatus{draft}
    void StopOffer();

private:
    /// @name specifier_
    ara::core::InstanceSpecifier specifier_;
    /// @name reentrancyType_
    ReentrancyType reentrancyType_;
    std::shared_ptr< isoft::dm::dis::UploadServiceAgent >
        /// @name skeleton_
        skeleton_{};
};

}  // namespace diag
}  // namespace ara

#endif  // ARA_DIAG_UPLOAD_H_