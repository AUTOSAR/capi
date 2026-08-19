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
/// @file       file_transfer_agent.h
/// @brief      This file provides the definition of the CommunicationControlAgent interface class.
/// @details
/// @date       2024-12-23
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================

#ifndef __SERVICEAGENT_FILETRANSFERAGENT_H_
#define __SERVICEAGENT_FILETRANSFERAGENT_H_

#include <functional>
#include <future>
#include <memory>

#include "ara/core/future.h"
#include "file_transfer_proxy.h"

namespace isoft {
namespace dm {
namespace dic {

class FileTransferServiceAgent
{
public:
    explicit FileTransferServiceAgent(uint16_t const& instanceId, uint32_t const& serviceInstanceId);

    ara::core::Result< FileSizes > RequestReadFile(ara::core::String fileName,
                                                   std::uint8_t dataFormatIdentifier,
                                                   MetaInfoMap& metaInfo,
                                                   std::shared_ptr< CancellationEvent >& cancellationEventPtr) noexcept;

    ///  @brief Called for RequestFileTransfer with ModeOfOperation ReadDir
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    ///  @param[in] directoryName Path including name of the directory to read
    ///  @param[in] metaInfo Contains additional meta information
    ///  @param[in] cancellationHandler Informs if the current conversation is canceled
    ///  @return A future with either the access strategy to be used during reading and the directory
    ///  information size to be reported to the UDS client for a positive response message) or an UDS NRC value (for an
    ///  negative response message)
    ///  @error Any Application responded with a UDS NRC value from DiagUdsNrcErrc
    ///  @pre             -
    ///  @reentrant       FALSE
    ///  @traceid{SWS_DM_01332}@tracestatus{draft}
    ara::core::Result< RequestFileResponse > RequestReadDirectory(
        ara::core::String directoryName,
        MetaInfoMap& metaInfo,
        std::shared_ptr< CancellationEvent >& cancellationEventPtr) noexcept;

    ///  @brief Called for RequestFileTransfer with ModeOfOperation WriteFile
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    ///  @param[in] fileName Path including name of the file to read
    ///  @param[in] dataFormatIdentifier UDS dataFormat Identifier
    ///  @param[in] fileSizes  The compressed/uncompressed files sizes
    ///  @param[in] mode The file replacement mode
    ///  @param[in] metaInfo Contains additional meta information
    ///  @param[in] cancellationHandler Informs if the current conversation is canceled
    ///  @return A future with either an instance of the file writing session or an UDS NRC value (for an negative
    ///  response message)
    ///  @error Any Application responded with a UDS NRC value from DiagUdsNrcErrc
    ///  @pre -
    ///  @reentrant FALSE
    ///  @traceid{SWS_DM_01333}@tracestatus{draft}
    ara::core::Result< ara::core::Vector< uint8_t > > RequestWriteFile(
        ara::core::String fileName,
        std::uint8_t dataFormatIdentifier,
        FileSizes fileSizes,
        WriteFileMode mode,
        MetaInfoMap& metaInfo,
        std::shared_ptr< CancellationEvent >& cancellationEventPtr) noexcept;

    ///  @brief Called for RequestFileTransfer with ModeOfOperation ResumeFile
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    ///  @param[in] fileName  Path including name of the file to read
    ///  @param[in] dataFormatIdentifier UDS dataFormat Identifier
    ///  @param[in] fileSizes The compressed/uncompressed files sizes
    ///  @param[in] metaInfo Contains additional meta information
    ///  @param[in] cancellationHandler Informs if the current conversation is canceled
    ///  @return A future with either an instance of the file writing session and the byte position the
    ///  client shall start resuming from or an UDS NRC value (for an negative response message)
    ///  @error Any Application responded with a UDS NRC value from DiagUdsNrcErrc
    ///  @pre -
    ///  @reentrant FALSE
    ///  @traceid{SWS_DM_01334}@tracestatus{draft}
    ara::core::Result< RequestFileResponse > RequestResumeWriteFile(
        ara::core::String fileName,
        std::uint8_t dataFormatIdentifier,
        FileSizes fileSizes,
        MetaInfoMap& metaInfo,
        std::shared_ptr< CancellationEvent >& cancellationEventPtr) noexcept;

    ///  @brief Called for RequestFileTransfer with ModeOfOperation DeleteFile
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    ///  @details This method is the complete operation of deleting a file
    ///  @param[in] fileName Path including name of the file to read
    ///  @param[in] metaInfo Contains additional meta information
    ///  @param[in] cancellationHandler Informs if the current conversation is canceled
    ///  @return A future with either a void data result or an UDS NRC value (for an negative response message)
    ///  @error Any Application responded with a UDS NRC value from DiagUdsNrcErrc
    ///  @pre -
    ///  @reentrant FALSE
    ///  @traceid{SWS_DM_01335}@tracestatus{draft}
    ara::core::Result< ara::core::Vector< uint8_t > > DeleteFile(
        ara::core::String fileName,
        MetaInfoMap& metaInfo,
        std::shared_ptr< CancellationEvent >& cancellationEventPtr) noexcept;

    ara::core::Result< ara::core::Vector< std::uint8_t > > Read(
        std::size_t numBytesToReturn,
        MetaInfoMap& metaInfo,
        std::shared_ptr< CancellationEvent >& cancellationEventPtr) noexcept;

    ara::core::Result< ara::core::Vector< std::uint8_t > > ExitRead(
        DataTransferExitType exitType,
        ara::core::Vector< std::uint8_t > transferRequestParameterRecord,
        MetaInfoMap& metaInfo,
        std::shared_ptr< CancellationEvent >& cancellationEventPtr) noexcept;

    ara::core::Result< void > Write(ara::core::Vector< std::uint8_t > requestData,
                                    MetaInfoMap& metaInfo,
                                    std::shared_ptr< CancellationEvent >& cancellationEventPtr) noexcept;

    ara::core::Result< ara::core::Vector< std::uint8_t > > ExitWrite(
        DataTransferExitType exitType,
        ara::core::Vector< std::uint8_t > transferRequestParameterRecord,
        MetaInfoMap& metaInfo,
        std::shared_ptr< CancellationEvent >& cancellationEventPtr) noexcept;

private:
    std::unique_ptr< FileTransferServiceProxy > proxy_{nullptr};
};

}  // namespace dic
}  // namespace dm
}  // namespace isoft

#endif  // __SERVICEAGENT_FILETRANSFERAGENT_H_