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
/// @brief      This file provides file_transfer functionality
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
/// @trace_id_sr=1cbac7a9-54af-477a-b06e-bda112c7e458
/// @unit_name=FileTransfer
/// @unit_description=This file provides file_transfer functionality
/// @endcode
///
/// ================================================================

#ifndef ISOFT_UDS_SERVER_SERVICE_FILETRANSFER_H_
#define ISOFT_UDS_SERVER_SERVICE_FILETRANSFER_H_
#include <functional>
#include <vector>

#include "isoft/uds/cancellation_handler.h"
#include "isoft/uds/common.h"
#include "isoft/uds/result.h"
namespace isoft {
namespace uds {
namespace server {
/// @brief Data transfer exit type
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01329
/// @trace_id_dd=DD_UDS_02621
/// @needwork = ad
/// @endcode
enum class DataTransferExitType : std::uint8_t
{
    kAcknowledge,  //!< The file transfer finished
    kAbort         //!< The file transfer has been aborted
};
/// @brief File transfer interface
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01330
/// @trace_id_dd=DD_UDS_02622
/// @needwork = ad
/// @endcode
class FileTransferInterface
{
public:
    /// @param
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00942
    /// @needwork = dda
    /// @endcode
    FileTransferInterface(FileTransferInterface const&) = delete;
    /// @param
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00943
    /// @needwork = dda
    /// @endcode
    FileTransferInterface& operator=(FileTransferInterface const&) = delete;
    /// @param
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00944
    /// @needwork = dda
    /// @endcode
    FileTransferInterface(FileTransferInterface&&) = delete;
    /// @param
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00945
    /// @needwork = dda
    /// @endcode
    FileTransferInterface& operator=(FileTransferInterface&&) = delete;

public:
    /// @brief File total size definition
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=ad
    /// @trace_id_sws={SWS_DM_01321}@tracestatus{draft}
    /// @endcode
    struct FileSizes
    {
        /// @brief Uncompressed size definition
        /// @code{.isoft}
        /// @interface_level=unit
        /// @needwork=ad
        /// @trace_id_sws={SWS_DM_01322}@tracestatus{draft}
        /// @endcode
        std::uint64_t uncompressedSize{0U};

        /// @brief Compressed size definition
        /// @code{.isoft}
        /// @interface_level=unit
        /// @needwork=ad
        /// @trace_id_sws={SWS_DM_01323}@tracestatus{draft}
        /// @endcode
        std::uint64_t compressedSize{0U};

        std::vector< uint8_t > vecMD5{};
    };

    /// @brief Request file response
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=ad
    /// @trace_id_sws={SWS_DM_01324}@tracestatus{draft}
    /// @endcode
    struct RequestFileResponse
    {
        /// @brief File size definition
        /// @code{.isoft}
        /// @interface_level=unit
        /// @needwork=ad
        /// @trace_id_sws={SWS_DM_01324}@tracestatus{draft}
        /// @endcode
        std::uint64_t fileSize{0U};
        /// @brief File MD5 value definition
        /// @code{.isoft}
        /// @interface_level=unit
        /// @needwork=ad
        /// @trace_id_sws={SWS_DM_01324}@tracestatus{draft}
        /// @endcode
        std::vector< uint8_t > vecMD5{};
    };

    /// @brief Write file operation mode
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=ad
    /// @trace_id_sws={SWS_DM_01324}@tracestatus{draft}
    /// @endcode
    enum class WriteFileMode : std::uint8_t
    {
        kAdd,     ///< The file shall be added only if not existing
        kReplace  ///< The file shall be added in any case, if already exists, will be replaced
    };

    /// @brief Constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=ad
    /// @trace_id_sws={SWS_DM_01324}@tracestatus{draft}
    /// @endcode
    FileTransferInterface(/* args */) = default;
    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=ad
    /// @trace_id_sws={SWS_DM_01324}@tracestatus{draft}
    /// @endcode
    virtual ~FileTransferInterface() noexcept = default;

    /// @brief Request to read file
    /// @param[in] fileName File name including file path
    /// @param[in] dataFormatIdentifier UDS data format identifier
    /// @param[in] metaInfo Contains additional meta information
    /// @param[in] cancellationHandler Notifies whether the current session is cancelled
    /// @return Returns file size information (FileSizes) on success, UDS NRC error code on failure
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=ad
    /// @retval FileSizes::uncompressedSize If set to 0, this parameter will not be reported to the diagnostic client
    /// @retval FileSizes::compressedSize If set to 0, this parameter will not be reported to the diagnostic client; if compression is not used, this value should be zero
    /// @error Any UDS NRC error code from NrcErrc
    /// @pre -
    /// @reentrant FALSE
    /// @trace_id_sws={SWS_DM_01331}@tracestatus{draft}
    /// @endcode
    virtual Result< FileSizes > RequestReadFile(std::string fileName,
                                                std::uint8_t dataFormatIdentifier,
                                                MetaInfoMap const& metaInfo,
                                                CancellationHandler cancellationHandler) noexcept = 0;

    /// @brief Request to read directory
    /// @param[in] directoryName Directory name including path
    /// @param[in] metaInfo Contains additional meta information
    /// @param[in] cancellationHandler Notifies whether the current session is cancelled
    /// @return Returns file response information (RequestFileResponse) on success, UDS NRC error code on failure
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=ad
    /// @error Any UDS NRC error code from NrcErrc
    /// @pre -
    /// @reentrant FALSE
    /// @trace_id_sws={SWS_DM_01332}@tracestatus{draft}
    /// @endcode
    virtual Result< RequestFileResponse > RequestReadDirectory(std::string directoryName,
                                                               MetaInfoMap const& metaInfo,
                                                               CancellationHandler cancellationHandler) noexcept = 0;

    /// @brief Request to write file
    /// @param[in] fileName File name including file path
    /// @param[in] dataFormatIdentifier UDS data format identifier
    /// @param[in] fileSizes Uncompressed/compressed file sizes
    /// @param[in] mode File replacement mode
    /// @param[in] metaInfo Contains additional meta information
    /// @param[in] cancellationHandler Notifies whether the current session is cancelled
    /// @return Returns file write session instance on success, UDS NRC error code on failure
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=ad
    /// @error Any UDS NRC error code from NrcErrc
    /// @pre -
    /// @reentrant FALSE
    /// @trace_id_sws={SWS_DM_01333}@tracestatus{draft}
    /// @endcode
    virtual Result< std::vector< uint8_t > > RequestWriteFile(std::string fileName,
                                                              std::uint8_t dataFormatIdentifier,
                                                              FileSizes fileSizes,
                                                              WriteFileMode mode,
                                                              MetaInfoMap const& metaInfo,
                                                              CancellationHandler cancellationHandler) noexcept = 0;

    /// @brief Request to resume writing file
    /// @param[in] fileName File name including file path
    /// @param[in] dataFormatIdentifier UDS data format identifier
    /// @param[in] fileSizes Uncompressed/compressed file sizes
    /// @param[in] metaInfo Contains additional meta information
    /// @param[in] cancellationHandler Notifies whether the current session is cancelled
    /// @return Returns file write session instance and byte position where client should resume writing on success, UDS NRC error code on failure
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=ad
    /// @error Any UDS NRC error code from NrcErrc
    /// @pre -
    /// @reentrant FALSE
    /// @trace_id_sws={SWS_DM_01334}@tracestatus{draft}
    /// @endcode
    virtual Result< RequestFileResponse > RequestResumeWriteFile(std::string fileName,
                                                                 std::uint8_t dataFormatIdentifier,
                                                                 FileSizes fileSizes,
                                                                 MetaInfoMap const& metaInfo,
                                                                 CancellationHandler cancellationHandler) noexcept = 0;

    /// @brief Request to delete file
    /// @param[in] fileName File name including file path
    /// @param[in] metaInfo Contains additional meta information
    /// @param[in] cancellationHandler Notifies whether the current session is cancelled
    /// @return Returns void on success, UDS NRC error code on failure
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=ad
    /// @details This method performs the complete operation of deleting a file
    /// @error Any UDS NRC error code from NrcErrc
    /// @pre -
    /// @reentrant FALSE
    /// @trace_id_sws={SWS_DM_01335}@tracestatus{draft}
    /// @endcode
    virtual Result< std::vector< uint8_t > > DeleteFile(std::string fileName,
                                                        MetaInfoMap const& metaInfo,
                                                        CancellationHandler cancellationHandler) noexcept = 0;

    /// @brief Send first or subsequent data block of file or directory information to UDS client
    /// @param[in] blockSequenceCounter Block sequence counter
    /// @param[in] numBytesToReturn Number of bytes requested by client to return
    /// @param[in] responseData Pre-allocated memory view for writing data blocks
    /// @param[in] metaInfo Additional meta information
    /// @param[in] cancellationHandler Whether the current session is cancelled
    /// @return Returns actual number of bytes written on success, UDS NRC error code on failure
    /// @code{.isoft}
    /// @details Application writes data to responseData, UDS server copies it to response message
    /// @error Any UDS NRC error code from NrcErrc
    /// @interface_level=unit
    /// @needwork=ad
    /// @pre -
    /// @reentrant FALSE
    /// @trace_id_sws={SWS_DM_01513}@tracestatus{draft}
    /// @endcode
    virtual Result< std::vector< std::uint8_t > > Read(std::uint8_t blockSequenceCounter,
                                                       std::size_t numBytesToReturn,
                                                       MetaInfoMap const& metaInfo,
                                                       CancellationHandler cancellationHandler) noexcept = 0;

    /// @brief Exit read operation
    /// @param[in] exitType Specifies exit reason
    /// @param[in] transferRequestParameterRecord Contains parameters required for server to support data transfer, format and length defined by vehicle manufacturer
    /// @param[in] metaInfo Contains additional meta information
    /// @param[in] cancellationHandler Notifies whether the current session is cancelled
    /// @return Returns transfer response parameter record on success, UDS NRC error code on failure
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=ad
    /// @error Any UDS NRC error code from NrcErrc
    /// @pre -
    /// @reentrant FALSE
    /// @trace_id_sws={SWS_DM_01514}@tracestatus{draft}
    /// @endcode
    virtual Result< std::vector< std::uint8_t > > ExitRead(DataTransferExitType exitType,
                                                           std::vector< std::uint8_t > transferRequestParameterRecord,
                                                           MetaInfoMap const& metaInfo,
                                                           CancellationHandler cancellationHandler) noexcept = 0;

    /// @brief Write file data block
    /// @param[in] blockSequenceCounter Block sequence counter
    /// @param[in] requestData File content block (first or subsequent) received from UDS client to be written to file
    /// @param[in] metaInfo Contains additional meta information
    /// @param[in] cancellationHandler Notifies whether the current session is cancelled
    /// @return Returns void on success, UDS NRC error code on failure
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=ad
    /// @error Any UDS NRC error code from NrcErrc
    /// @pre -
    /// @reentrant FALSE
    /// @trace_id_sws={SWS_DM_01546}@tracestatus{draft}
    /// @endcode
    virtual Result< void > Write(std::uint8_t blockSequenceCounter,
                                 std::vector< std::uint8_t > requestData,
                                 MetaInfoMap const& metaInfo,
                                 CancellationHandler cancellationHandler) noexcept = 0;

    /// @brief Exit write operation
    /// @param[in] exitType Specifies exit reason
    /// @param[in] transferRequestParameterRecord This parameter record contains parameters required for server to support data transfer, format and length defined by vehicle manufacturer
    /// @param[in] metaInfo Contains additional meta information
    /// @param[in] cancellationHandler Notifies whether the current session is cancelled
    /// @return Returns transfer response parameter record on success, UDS NRC error code on failure
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork=ad
    /// @error Any UDS NRC error code from NrcErrc
    /// @pre -
    /// @reentrant FALSE
    /// @trace_id_sws={SWS_DM_01547}@tracestatus{draft}
    /// @endcode
    virtual Result< std::vector< std::uint8_t > > ExitWrite(DataTransferExitType exitType,
                                                            std::vector< std::uint8_t > transferRequestParameterRecord,
                                                            MetaInfoMap const& metaInfo,
                                                            CancellationHandler cancellationHandler) noexcept = 0;
};

}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  // ISOFT_UDS_SERVER_SERVICE_FILETRANSFER_H_