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
/// @file       file_transfer.h
/// @brief
/// @details
/// @date       2024-10-12
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================

#ifndef ADAPTIVEDIAGNOSTICMANAGER_SRC_UDSADAPTER_FILETRANSFER_H_
#define ADAPTIVEDIAGNOSTICMANAGER_SRC_UDSADAPTER_FILETRANSFER_H_
#include <isoft/uds/transfer_managment/file_transfer.h>

#include "cancellation/cancellation_event_manager.h"
#include "gen_code/file_transfer/serviceAgent/file_transfer_agent.h"
namespace ara {
namespace diag {
namespace dmd {

class FileTransfer : public isoft::uds::server::FileTransferInterface
{
public:
    using MetaInfoMap         = isoft::uds::server::MetaInfoMap;
    using CancellationHandler = isoft::uds::server::CancellationHandler;
    using FileSizes           = isoft::uds::server::FileTransferInterface::FileSizes;
    using RequestFileResponse = isoft::uds::server::FileTransferInterface::RequestFileResponse;
    FileTransfer(uint16_t const &instanceId, uint32_t const &serviceInstanceId);
    ~FileTransfer() override = default;

    /// @brief copy constructor
    /// @param other
    FileTransfer(FileTransfer const &other) noexcept = delete;
    /// @brief copy assignment operator
    /// @param other
    /// @return FileTransfer&
    FileTransfer &operator=(FileTransfer const &other) noexcept = delete;
    /// @brief move constructor
    /// @param other
    FileTransfer(FileTransfer &&other) noexcept = delete;
    /// @brief move assignment operator
    /// @param other
    /// @return FileTransfer&
    FileTransfer &operator=(FileTransfer &&other) noexcept = delete;

    ///  @brief Called for RequestFileTransfer with ModeOfOperation ReadFile
    ///  @code{.isoft}
    ///  export_level=/Diagnostics
    ///  @endcode
    ///  @param[in] fileName Path including name of the file to read
    ///  @param[in] dataFormatIdentifier UDS dataFormat Identifier
    ///  @param[in] metaInfo Contains additional meta information
    ///  @param[in] cancellationHandler Informs if the current conversation is
    ///  canceled
    ///  @return A future with either the access strategy to be used during
    ///  reading and the file sizes to be
    ///          reported to the UDS client for a positive response message) or an
    ///          UDS NRC value (for an negative
    ///        response message)
    ///  @return uncompressed size: If set to 0 - the parameter will not be
    ///  reported to the diagnostic client
    ///  @return compressed size: If set to 0 - the parameter will not be reported
    ///  to the diagnostic client. If no
    /// compression is used, the value shall be zero.
    ///  @error Any Application responded with a UDS NRC value from NrcErrc
    ///  @pre -
    ///  @reentrant FALSE
    ///  @traceid{SWS_DM_01331}@tracestatus{draft}
    isoft::uds::Result< FileSizes > RequestReadFile(std::string fileName,
                                                    std::uint8_t dataFormatIdentifier,
                                                    MetaInfoMap const &metaInfo,
                                                    CancellationHandler cancellationHandler) noexcept override;

    ///  @brief Called for RequestFileTransfer with ModeOfOperation ReadDir
    ///  @code{.isoft}
    ///  export_level=/Diagnostics
    ///  @endcode
    ///  @param[in] directoryName Path including name of the directory to read
    ///  @param[in] metaInfo Contains additional meta information
    ///  @param[in] cancellationHandler Informs if the current conversation is
    ///  canceled
    ///  @return A future with either the access strategy to be used during
    ///  reading and the directory information size to be reported to the UDS
    ///  client for a positive response message) or an UDS NRC value (for an
    ///  negative response message)
    ///  @error Any Application responded with a UDS NRC value from NrcErrc
    ///  @pre             -
    ///  @reentrant       FALSE
    ///  @traceid{SWS_DM_01332}@tracestatus{draft}
    isoft::uds::Result< RequestFileResponse > RequestReadDirectory(
        std::string directoryName,
        MetaInfoMap const &metaInfo,
        CancellationHandler cancellationHandler) noexcept override;

    ///  @brief Called for RequestFileTransfer with ModeOfOperation WriteFile
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    ///  @param[in] fileName Path including name of the file to read
    ///  @param[in] dataFormatIdentifier UDS dataFormat Identifier
    ///  @param[in] fileSizes  The compressed/uncompressed files sizes
    ///  @param[in] mode The file replacement mode
    ///  @param[in] metaInfo Contains additional meta information
    ///  @param[in] cancellationHandler Informs if the current conversation is
    ///  canceled
    ///  @return A future with either an instance of the file writing session or
    ///  an UDS NRC value (for an negative response message)
    ///  @error Any Application responded with a UDS NRC value from NrcErrc
    ///  @pre -
    ///  @reentrant FALSE
    ///  @traceid{SWS_DM_01333}@tracestatus{draft}
    isoft::uds::Result< std::vector< uint8_t > > RequestWriteFile(
        std::string fileName,
        std::uint8_t dataFormatIdentifier,
        FileSizes fileSizes,
        WriteFileMode mode,
        MetaInfoMap const &metaInfo,
        CancellationHandler cancellationHandler) noexcept override;

    ///  @brief Called for RequestFileTransfer with ModeOfOperation ResumeFile
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    ///  @param[in] fileName  Path including name of the file to read
    ///  @param[in] dataFormatIdentifier UDS dataFormat Identifier
    ///  @param[in] fileSizes The compressed/uncompressed files sizes
    ///  @param[in] metaInfo Contains additional meta information
    ///  @param[in] cancellationHandler Informs if the current conversation is
    ///  canceled
    ///  @return A future with either an instance of the file writing session and
    ///  the byte position the client shall start resuming from or an UDS NRC
    ///  value (for an negative response message)
    ///  @error Any Application responded with a UDS NRC value from NrcErrc
    ///  @pre -
    ///  @reentrant FALSE
    ///  @traceid{SWS_DM_01334}@tracestatus{draft}
    isoft::uds::Result< RequestFileResponse > RequestResumeWriteFile(
        std::string fileName,
        std::uint8_t dataFormatIdentifier,
        FileSizes fileSizes,
        MetaInfoMap const &metaInfo,
        CancellationHandler cancellationHandler) noexcept override;

    ///  @brief Called for RequestFileTransfer with ModeOfOperation DeleteFile
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    ///  @details This method is the complete operation of deleting a file
    ///  @param[in] fileName Path including name of the file to read
    ///  @param[in] metaInfo Contains additional meta information
    ///  @param[in] cancellationHandler Informs if the current conversation is
    ///  canceled
    ///  @return A future with either a void data result or an UDS NRC value (for
    ///  an negative response message)
    ///  @error Any Application responded with a UDS NRC value from NrcErrc
    ///  @pre -
    ///  @reentrant FALSE
    ///  @traceid{SWS_DM_01335}@tracestatus{draft}
    isoft::uds::Result< std::vector< uint8_t > > DeleteFile(std::string fileName,
                                                            MetaInfoMap const &metaInfo,
                                                            CancellationHandler cancellationHandler) noexcept override;

    /// @brief Reads data chunk from the application
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @details The first or subsequent file/directory info content chunk to be
    /// sent back to the UDS client will
    ///  be composed by the application and copied into the provided by AraDiag
    ///  Span with up to the requested Span size.
    /// @param[in] response_data  The view over a pre-allocated by AraDiag memory
    /// to write in this data chunk.
    /// @param[in] meta_info Contains additional meta information
    /// @param[in] cancellation_handler  Informs if the current conversation is
    /// canceled
    /// @return A Future, which either gets readied to actual amount of data
    /// provided in the Span (for a positive response message) or readied with
    /// ErrorCode from NrcErrc (for an negative response message). Data in
    /// response_data will be placed after blockSequenceCounter as
    /// transferResponseParameterRecord in the positive response.
    /// @error Any  Application responded with a UDS NRC value from NrcErrc
    /// @pre -
    /// @reentrant FALSE
    /// @traceid{SWS_DM_01513}@tracestatus{draft}
    isoft::uds::Result< std::vector< std::uint8_t > > Read(std::uint8_t blockSequenceCounter,
                                                           std::size_t numBytesToReturn,
                                                           MetaInfoMap const &metaInfo,
                                                           CancellationHandler cancellationHandler) noexcept override;

    /// @brief Exits an ongoing data transfer session
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] exit_type Specifies the exit reason
    /// @param[in] transfer_request_parameter_record This parameter record
    /// contains parameter(s), which are required by the server to support the
    /// transfer of data. Format and length of this parameter(s) are vehicle
    /// manufacturer specific.
    /// @param[in] meta_info contains additional meta information
    /// @param[in] cancellation_handler informs if the current conversation is
    /// canceled
    /// @return returns a Future, which either gets readied to OperationOutput
    /// (transferResponseParameterRecord
    ///          for a positive response message) or readied with ErrorCode from
    ///          NrcErrc (for an negative response message) Data in
    ///          OperationOutput.response_data will be placed after SID as
    /// transferResponseParameterRecord in the positive response.
    /// @error Any  Application responded with a UDS NRC value from NrcErrc
    /// @pre -
    /// @reentrant FALSE
    /// @traceid{SWS_DM_01514}@tracestatus{draft}
    isoft::uds::Result< std::vector< std::uint8_t > > ExitRead(
        isoft::uds::server::DataTransferExitType exitType,
        std::vector< std::uint8_t > transferRequestParameterRecord,
        MetaInfoMap const &metaInfo,
        CancellationHandler cancellationHandler) noexcept override;

    /// @brief Appends a data chunk into application memory
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] requestData The first or subsequent file content chunk received
    /// from the UDS client to be written into
    ///  the file.
    /// @param[in] metaInfo Contains additional meta information
    /// @param[in] cancellationHandler  Informs if the current conversation is
    /// canceled
    /// @return A Future, which either is of type void or readied with ErrorCode
    /// from NrcErrc (for an
    ///          negative response message).
    /// @error Any Application responded with a UDS NRC value from NrcErrc
    /// @pre -
    /// @reentrant FALSE
    /// @traceid{SWS_DM_01546}@tracestatus{draft}
    isoft::uds::Result< void > Write(std::uint8_t blockSequenceCounter,
                                     std::vector< std::uint8_t > requestData,
                                     MetaInfoMap const &metaInfo,
                                     CancellationHandler cancellationHandler) noexcept override;

    /// @brief Exits an ongoing data transfer session
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] exitType Specifies the exit reason
    /// @param[in] transferRequestParameterRecord  This parameter record contains
    /// parameter(s), which are required by the server to support the transfer of
    /// data. Format and length of this parameter(s) are vehicle manufacturer
    /// specific.
    /// @param[in] metaInfo  contains additional meta information
    /// @param[in] cancellationHandler informs if the current conversation is
    /// canceled
    /// @return returns a Future, which either gets readied to OperationOutput
    /// (transferResponseParameterRecord for a positive response message) or
    /// readied with ErrorCode from NrcErrc (for an negative response message)
    /// Data in OperationOutput.response_data will be placed after SID as
    /// transferResponseParameterRecord in the positive response.
    /// @error Any  Application responded with a UDS NRC value from NrcErrc
    /// @pre -
    /// @reentrant FALSE
    /// @traceid{SWS_DM_01547}@tracestatus{draft}
    isoft::uds::Result< std::vector< std::uint8_t > > ExitWrite(
        isoft::uds::server::DataTransferExitType exitType,
        std::vector< std::uint8_t > transferRequestParameterRecord,
        MetaInfoMap const &metaInfo,
        CancellationHandler cancellationHandler) noexcept override;

private:
    std::unique_ptr< isoft::dm::dic::FileTransferServiceAgent > agentPtr_;
    isoft::dm::CancellationEventManager cancellationEventManager_{};
};
}  // namespace dmd
}  // namespace diag
}  // namespace ara

#endif  /// ADAPTIVEDIAGNOSTICMANAGER_SRC_UDSADAPTER_FILETRANSFER_H_