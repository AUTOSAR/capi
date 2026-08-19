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
/// @brief      This file provides the definitions of FileTransfer and related types.
/// @details
/// @date       2023-08-23
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_DIAG_FILE_TRANSFER_H__
#define ARA_DIAG_FILE_TRANSFER_H__

#include <ara/core/future.h>
#include <ara/core/instance_specifier.h>
#include <ara/core/result.h>
#include <ara/core/string.h>
#include <ara/core/utility.h>
#include <ara/core/vector.h>

#include "cancellation_handler.h"
#include "data_transfer.h"
#include "reentrancy.h"

namespace isoft {
namespace dm {
namespace dis {
/// @brief FileTransferServiceAgent
class FileTransferServiceAgent;
}  // namespace dis
}  // namespace dm
}  // namespace isoft

namespace ara {
namespace diag {

namespace api {
class FileTransferServiceSkeleton;
}  // namespace api

///  @brief File Transfer service interface
///
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
///
///  @traceid{SWS_DM_01320}@tracestatus{draft}
///  @uptrace{RS_Diag_04135}
class FileTransferService
{
public:
    ///  @brief Definition of total file sizes
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    ///  @traceid{SWS_DM_01321}@tracestatus{draft}
    struct FileSizes
    {
        ///  @brief Specifies the uncompressed file size in bytes
        /// @code{.isoft}
        /// export_level=/Diagnostics
        /// @endcode
        ///  @traceid{SWS_DM_01322}@tracestatus{draft}
        std::uint64_t uncompressed_size;  /// NOLINT

        ///  @brief Specifies the compressed file size in bytes
        /// @code{.isoft}
        /// export_level=/Diagnostics
        /// @endcode
        ///  @traceid{SWS_DM_01323}@tracestatus{draft}
        std::uint64_t compressed_size;  /// NOLINT
    };

    ///  @brief Determines the write file operation mode
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    ///  @traceid{SWS_DM_01324}@tracestatus{draft}
    enum class WriteFileMode : std::uint8_t
    {
        kAdd,     ///< The file shall be added only if not existing
        kReplace  ///< The file shall be added in any case, if already exists, will
                  ///< be replaced
    };

    ///  @brief Constructor for FileTransferService (inherited)
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    ///  @param[in] instanceSpecifier InstanceSpecifier to an PortPrototype of an
    ///  DownloadServiceInterface
    ///  @param[in] reentrancyType Specifies if interface is callable fully- or
    ///  non-reentrant
    ///  @pre -
    ///  @reentrant FALSE
    ///  @traceid{SWS_DM_01325}@tracestatus{draft}
    FileTransferService(ara::core::InstanceSpecifier const &instanceSpecifier, ReentrancyType reentrancyType) noexcept;

    ///  @brief Destructor of class FileTransferService
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    ///  @pre -
    ///  @reentrant FALSE
    ///  @traceid{SWS_DM_01326}@tracestatus{draft}
    virtual ~FileTransferService() noexcept = default;

protected:  /// NOLINT
    ///  @brief FileTransferService shall be a single not copy-able instance.
    ///  @traceid{SWS_DM_01327}@tracestatus{draft}
    FileTransferService(FileTransferService const &) = delete;  // NOLINT

    ///  @brief Move constructs an instance of FileTransferService
    ///  @param[out] other The other object
    ///  @pre -
    ///  @reentrant FALSE
    ///  @traceid{SWS_DM_01328}@tracestatus{draft}
    FileTransferService(FileTransferService &&other) noexcept;  // NOLINT

    ///  @brief FileTransferService shall be a single not assignable instance
    ///  @param[out] other The other object
    ///  @return Reference to self
    ///  @pre -
    ///  @reentrant FALSE
    ///  @traceid{SWS_DM_01329}@tracestatus{draft}
    auto operator                =(FileTransferService const &)  // NOLINT
        -> FileTransferService & = delete;                       // NOLINT

    ///  @brief  Move assigns an instance of FileTransferService
    ///  @param[out] other The other object
    ///  @return Reference to self
    ///  @pre -
    ///  @reentrant FALSE
    ///  @traceid{SWS_DM_01330}@tracestatus{draft}
    auto operator=(FileTransferService &&other) &noexcept -> FileTransferService &;  // NOLINT

public:
    ///  @brief Called for RequestFileTransfer with ModeOfOperation ReadFile
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
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
    ///  @error Any Application responded with a UDS NRC value from DiagUdsNrcErrc
    ///  @pre -
    ///  @reentrant FALSE
    ///  @traceid{SWS_DM_01331}@tracestatus{draft}
    virtual auto RequestReadFile(ara::core::String fileName,
                                 ara::core::Byte dataFormatIdentifier,
                                 MetaInfo const &metaInfo,
                                 CancellationHandler cancellationHandler) noexcept
        -> ara::core::Future< std::tuple< DataTransferReadSession, FileSizes > > = 0;

    ///  @brief Called for RequestFileTransfer with ModeOfOperation ReadDir
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    ///  @param[in] directoryName Path including name of the directory to read
    ///  @param[in] metaInfo Contains additional meta information
    ///  @param[in] cancellationHandler Informs if the current conversation is
    ///  canceled
    ///  @return A future with either the access strategy to be used during
    ///  reading and the directory information size to be reported to the UDS
    ///  client for a positive response message) or an UDS NRC value (for an
    ///  negative response message)
    ///  @error Any Application responded with a UDS NRC value from DiagUdsNrcErrc
    ///  @pre             -
    ///  @reentrant       FALSE
    ///  @traceid{SWS_DM_01332}@tracestatus{draft}
    virtual auto RequestReadDirectory(ara::core::String directoryName,
                                      MetaInfo const &metaInfo,
                                      CancellationHandler cancellationHandler) noexcept
        -> ara::core::Future< std::tuple< DataTransferReadSession, std::uint64_t > > = 0;

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
    ///  @error Any Application responded with a UDS NRC value from DiagUdsNrcErrc
    ///  @pre -
    ///  @reentrant FALSE
    ///  @traceid{SWS_DM_01333}@tracestatus{draft}
    virtual auto RequestWriteFile(ara::core::String fileName,
                                  ara::core::Byte dataFormatIdentifier,
                                  FileSizes fileSizes,
                                  WriteFileMode mode,
                                  MetaInfo const &metaInfo,
                                  CancellationHandler cancellationHandler) noexcept
        -> ara::core::Future< DataTransferWriteSession > = 0;

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
    ///  @error Any Application responded with a UDS NRC value from DiagUdsNrcErrc
    ///  @pre -
    ///  @reentrant FALSE
    ///  @traceid{SWS_DM_01334}@tracestatus{draft}
    virtual auto RequestResumeWriteFile(ara::core::String fileName,
                                        ara::core::Byte dataFormatIdentifier,
                                        FileSizes fileSizes,
                                        MetaInfo const &metaInfo,
                                        CancellationHandler cancellationHandler) noexcept
        -> ara::core::Future< std::tuple< DataTransferWriteSession, std::uint64_t > > = 0;

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
    ///  @error Any Application responded with a UDS NRC value from DiagUdsNrcErrc
    ///  @pre -
    ///  @reentrant FALSE
    ///  @traceid{SWS_DM_01335}@tracestatus{draft}
    virtual auto DeleteFile(ara::core::String fileName,
                            MetaInfo const &metaInfo,
                            CancellationHandler cancellationHandler) noexcept -> ara::core::Future< void > = 0;

    ///  @brief This Offer will enable the DM to forward request messages to this
    ///  handler
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    ///  @return Positive result if service was offered, error if offer failed
    ///  @error DiagLibErrc::kConfigurationMismatch if handle is an invalid handle
    ///  @error DiagLibErrc::kAlreadyOffered if service has been offered already
    ///  @pre             -
    ///  @reentrant       FALSE
    ///  @traceid{SWS_DM_01336}@tracestatus{draft}
    auto Offer() noexcept -> ara::core::Result< void >;

    ///  @brief This StopOffer will disable the forwarding of request messages
    ///  from DM
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    ///  @pre -
    ///  @reentrant  FALSE
    ///  @traceid{SWS_DM_01337}@tracestatus{draft}
    void StopOffer() noexcept;

    /// @brief AA side to set the MD5 of the transmitted file and return it to the diagnostic management through all responses of FileTransferService, then return it to the tester side through UDS
    /// @param vecMD5 MD5 to be set
    void SetMD5(ara::core::Vector< uint8_t > &&vecMD5) noexcept;

    /// @brief Get MD5
    /// @return MD5
    ara::core::Vector< uint8_t > GetMD5() noexcept { return vecMD5_; }

private:
    ara::core::InstanceSpecifier specifier_;
    ReentrancyType reentrancyType_;
    std::shared_ptr< isoft::dm::dis::FileTransferServiceAgent > skeleton_;
    ara::core::Vector< uint8_t > vecMD5_;
};

}  // namespace diag
}  // namespace ara

#endif  // ARA_DIAG_FILE_TRANSFER_H__