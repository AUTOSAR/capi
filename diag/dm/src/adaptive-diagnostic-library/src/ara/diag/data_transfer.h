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
/// @file       data_transfer.h
/// @brief      This file provides the definitions of DataTransfer and related types.
/// @details
/// @date       2023-08-23
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_DIAG_DATA_TRANSFER_H__
#define ARA_DIAG_DATA_TRANSFER_H__

#include <ara/core/future.h>
#include <ara/core/instance_specifier.h>
#include <ara/core/result.h>
#include <ara/core/span.h>
#include <ara/core/utility.h>
#include <ara/core/vector.h>

#include "cancellation_handler.h"
#include "meta_info.h"
#include "release_handler.h"

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

/// @brief Determines transfer exit signal type
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
/// @vpublic
/// @traceid{SWS_DM_01538}@tracestatus{draft}
enum class DataTransferExitType : std::uint8_t
{
    kAcknowledge,  //!< The file transfer finished
    kAbort         //!< The file transfer has been aborted
};

/// @brief Handles data transfers initiated by AddFile, ResumeFile or ReplaceFile ModeOfOperation handling per
///       requested RequestWriteFile
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
/// @vpublic
/// @traceid{SWS_DM_01539}@tracestatus{draft}
class DataTransferWriteHandler
{
public:
protected:
    /// @brief Constructs an instance of this class
    /// @pre -
    /// @reentrant FALSE
    /// @traceid{SWS_DM_01540}@tracestatus{draft}
    DataTransferWriteHandler() = default;

public:
    ///@brief Destructs an instance of this class
    ///@details An instance of this class must not be destroyed before ExitWrite() is called
    ///@pre -
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    ///@reentrant FALSE
    ///@traceid{SWS_DM_01541}@tracestatus{draft}
    virtual ~DataTransferWriteHandler() noexcept = default;

    /// @brief Move constructs an instance of this class
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[out] other The other object
    /// @return Reference to self
    /// @pre -
    /// @reentrant FALSE
    /// @traceid{SWS_DM_01542}@tracestatus{draft}
    DataTransferWriteHandler(DataTransferWriteHandler&& other) noexcept = default;

    /// @brief Move assigns an instance of this class
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[out] other The other object
    /// @return Reference to self
    /// @pre -
    /// @reentrant FALSE
    /// @traceid{SWS_DM_01543}@tracestatus{draft}
    auto operator=(DataTransferWriteHandler&& other) & noexcept -> DataTransferWriteHandler& = default;

    /// @brief Handlers shall not be copiable since only one way usage i.e. per single file transfer session
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @traceid{SWS_DM_01544}@tracestatus{draft}
    DataTransferWriteHandler(DataTransferWriteHandler const&) = delete;

    /// @brief Handlers shall not be copiable since only one way usage i.e. per single file transfer session
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @traceid{SWS_DM_01545}@tracestatus{draft}
    auto operator=(DataTransferWriteHandler const&) -> DataTransferWriteHandler& = delete;

    /// @brief Appends a data chunk into application memory
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] requestData The first or subsequent file content chunk received from the UDS client to be written
    /// into
    ///  the file.
    /// @param[in] metaInfo Contains additional meta information
    /// @param[in] cancellationHandler  Informs if the current conversation is canceled
    /// @return A Future, which either is of type void or readied with ErrorCode from DiagUdsNrcErrc (for an
    ///          negative response message).
    /// @error Any Application responded with a UDS NRC value from DiagUdsNrcErrc
    /// @pre -
    /// @reentrant FALSE
    /// @traceid{SWS_DM_01546}@tracestatus{draft}
    virtual auto Write(ara::core::Span< ara::core::Byte > requestData,
                       MetaInfo const& metaInfo,
                       CancellationHandler cancellationHandler) noexcept -> ara::core::Future< void > = 0;

    /// @brief Exits an ongoing data transfer session
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] exitType Specifies the exit reason
    /// @param[in] transferRequestParameterRecord  This parameter record contains parameter(s), which are required by
    /// the server to support the transfer of data. Format and length of this parameter(s) are vehicle manufacturer
    /// specific.
    /// @param[in] metaInfo  contains additional meta information
    /// @param[in] cancellationHandler informs if the current conversation is canceled
    /// @return returns a Future, which either gets readied to OperationOutput (transferResponseParameterRecord
    /// for a positive response message) or readied with ErrorCode from DiagUdsNrcErrc (for an negative
    /// response message) Data in OperationOutput.response_data will be placed after SID as
    /// transferResponseParameterRecord in the positive response.
    /// @error Any  Application responded with a UDS NRC value from DiagUdsNrcErrc
    /// @pre -
    /// @reentrant FALSE
    /// @traceid{SWS_DM_01547}@tracestatus{draft}
    virtual auto ExitWrite(DataTransferExitType exitType,
                           ara::core::Span< ara::core::Byte > transferRequestParameterRecord,
                           MetaInfo const& metaInfo,
                           CancellationHandler cancellationHandler) noexcept
        -> ara::core::Future< ara::core::Vector< ara::core::Byte > > = 0;
};

/// @brief Handles data transfers initiated by RequestReadFile/-Directory with shared data from the implementation
/// @vpublic
/// @traceid{SWS_DM_01497}@tracestatus{draft}
class DataTransferReadSharedDataHandler
{
public:
    /// @brief Constructs an instance of this class
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @pre -
    /// @reentrant FALSE
    /// @traceid{SWS_DM_01498}@tracestatus{draft}
    DataTransferReadSharedDataHandler() noexcept = default;

    /// @brief Destructs an instance of this class
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @details An instance of this class must not be destroyed before ExitRead() is called
    /// @pre -
    /// @reentrant FALSE
    /// @traceid{SWS_DM_01499}@tracestatus{draft}
    virtual ~DataTransferReadSharedDataHandler() noexcept = default;

    /// @brief Move constructs an instance of this class
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[out] other The other object
    /// @return Reference to self
    /// @pre -
    /// @reentrant FALSE
    /// @traceid{SWS_DM_01500}@tracestatus{draft}
    DataTransferReadSharedDataHandler(DataTransferReadSharedDataHandler&& other) noexcept = default;

    /// @brief Move assigns an instance of this class
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[out] other  The other object
    /// @return Reference to self
    /// @pre -
    /// @reentrant FALSE
    /// @traceid{SWS_DM_01502}@tracestatus{draft}
    auto operator                             =(DataTransferReadSharedDataHandler&& other) & noexcept
        -> DataTransferReadSharedDataHandler& = default;

    /// @brief Handlers shall not be copiable since only one way usage i.e. per single file transfer session
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @traceid{SWS_DM_01501}@tracestatus{draft}
    DataTransferReadSharedDataHandler(DataTransferReadSharedDataHandler const&) = delete;

    /// @brief Handlers shall not be copiable since only one way usage i.e. per single file transfer session
    /// @traceid{SWS_DM_01503}@tracestatus{draft}
    auto operator=(DataTransferReadSharedDataHandler const&) -> DataTransferReadSharedDataHandler& = delete;

    /// @brief Provides thw whole data to be read out from the application in form of a shared data view
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @details The whole file/directory info content to be sent back to the UDS client will be allocated by
    /// the
    ///            application and its shared ownership (view over the memory location) is passed as a result
    ///            to AraDiag. The allocated memory must be kept until the ReleaseHandler's notifier is
    ///            invoked or the status polled, signalling that the memory addressed by the Span now can
    ///            safely be released.
    /// @param[in] releaseHandler The release shared resource handler to be stored and used by the
    /// application until a new one is passed for the same meta_info (UDS client)
    /// @param[in] metaInfo Contains additional meta information
    /// @param[in] cancellationHandler Informs if the current conversation is canceled
    /// @return A Future, which either gets readied to ReadSharedOutput (transferResponseParameterRecord for
    /// a
    ///          positive response message) or readied with ErrorCode from DiagUdsNrcErrc (for an negative
    ///          response message). Data in ReleaseSharedHandler.responseData will be placed after
    ///          blockSequenceCounter as transferResponseParameterRecord in the positive response.
    /// @error Any Application responded with a UDS NRC value from DiagUdsNrcErrc
    /// @pre -
    /// @reentrant FALSE
    /// @traceid{SWS_DM_01504}@tracestatus{draft}
    virtual auto Read(ReleaseHandler releaseHandler,
                      MetaInfo const& metaInfo,
                      CancellationHandler cancellationHandler) noexcept
        -> ara::core::Future< ara::core::Span< ara::core::Byte > > = 0;

    /// @brief Exits an ongoing data transfer session
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] exitType Specifies the exit reason
    /// @param[in] transferRequestParameterRecord This parameter record contains parameter(s), which are
    /// required by
    /// the server to support the transfer of data. Format and length of this parameter(s) are vehicle
    /// manufacturer specific.
    /// @param[in] metaInfo  contains additional meta information
    /// @param[in] cancellationHandler informs if the current conversation is canceled
    /// @return returns a Future, which either gets readied to OperationOutput
    /// (transferResponseParameterRecord
    ///       for a positive response message) or readied with ErrorCode from DiagUdsNrcErrc (for an negative
    ///       response message) Data in OperationOutput.response_data will be placed after SID as
    ///       transferResponseParameterRecord in the positive response.
    /// @error Any  Application responded with a UDS NRC value from DiagUdsNrcErrc
    /// @pre -
    /// @reentrant FALSE
    /// @traceid{SWS_DM_01505}@tracestatus{draft}
    virtual auto ExitRead(DataTransferExitType exitType,
                          ara::core::Span< ara::core::Byte > transferRequestParameterRecord,
                          MetaInfo const& metaInfo,
                          CancellationHandler cancellationHandler) noexcept
        -> ara::core::Future< ara::core::Vector< ara::core::Byte > > = 0;
};

/// @brief Handles data transfers initiated by RequestReadFile/-Directory with data pulled by AraDiag from the
/// implementation
/// @vpublic
/// @traceid{SWS_DM_01506}@tracestatus{draft}
class DataTransferReadByPullHandler
{
protected:
    /// @brief  Constructs an instance of this class
    /// @pre -
    /// @reentrant FALSE
    /// @traceid{SWS_DM_01507}@tracestatus{draft}
    DataTransferReadByPullHandler() noexcept = default;

public:
    /// @brief Move constructs an instance of this class
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[out] other The other object
    /// @return Reference to self
    /// @pre -
    /// @reentrant FALSE
    /// @traceid{SWS_DM_01508}@tracestatus{draft}
    DataTransferReadByPullHandler(DataTransferReadByPullHandler&& other) noexcept = default;

    /// @brief Handlers shall not be copiable since only one way usage i.e. per single file transfer session
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @traceid{SWS_DM_01509}@tracestatus{draft}
    DataTransferReadByPullHandler(DataTransferReadByPullHandler const&) = delete;

    /// @brief Destructs an instance of this class
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @details An instance of this class must not be destroyed before ExitRead() is called
    /// @pre -
    /// @reentrant FALSE
    /// @traceid{SWS_DM_01510}@tracestatus{draft}
    virtual ~DataTransferReadByPullHandler() noexcept = default;

    /// @brief Move assigns an instance of this class
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[out] other The other object
    /// @return Reference to self
    /// @pre -
    /// @reentrant FALSE
    /// @traceid{SWS_DM_01511}@tracestatus{draft}
    auto operator=(DataTransferReadByPullHandler&& other) & noexcept -> DataTransferReadByPullHandler& = default;

    /// @brief Handlers shall not be copiable since only one way usage i.e. per single file transfer session
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @traceid{SWS_DM_01512}@tracestatus{draft}
    auto operator=(DataTransferReadByPullHandler const&) -> DataTransferReadByPullHandler& = delete;

    /// @brief Reads data chunk from the application
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @details The first or subsequent file/directory info content chunk to be sent back to the UDS client
    /// will
    ///  be composed by the application and copied into the provided by AraDiag Span with up to the
    ///  requested Span size.
    /// @param[in] responseData  The view over a pre-allocated by AraDiag memory to write in this data
    /// chunk.
    /// @param[in] metaInfo Contains additional meta information
    /// @param[in] cancellationHandler  Informs if the current conversation is canceled
    /// @return A Future, which either gets readied to actual amount of data provided in the Span (for a
    /// positive
    /// response message) or readied with ErrorCode from DiagUdsNrcErrc (for an negative response
    /// message). Data in response_data will be placed after blockSequenceCounter as
    /// transferResponseParameterRecord in the positive response.
    /// @error Any  Application responded with a UDS NRC value from DiagUdsNrcErrc
    /// @pre -
    /// @reentrant FALSE
    /// @traceid{SWS_DM_01513}@tracestatus{draft}
    virtual auto Read(ara::core::Span< ara::core::Byte > responseData,
                      MetaInfo const& metaInfo,
                      CancellationHandler cancellationHandler) noexcept -> ara::core::Future< std::uint32_t > = 0;

    /// @brief Exits an ongoing data transfer session
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] exitType Specifies the exit reason
    /// @param[in] transferRequestParameterRecord This parameter record contains parameter(s), which are
    /// required by
    /// the server to support the transfer of data. Format and length of this parameter(s) are vehicle
    /// manufacturer specific.
    /// @param[in] metaInfo contains additional meta information
    /// @param[in] cancellationHandler informs if the current conversation is canceled
    /// @return returns a Future, which either gets readied to OperationOutput
    /// (transferResponseParameterRecord
    ///          for a positive response message) or readied with ErrorCode from DiagUdsNrcErrc (for an
    ///          negative response message) Data in OperationOutput.response_data will be placed after SID as
    /// transferResponseParameterRecord in the positive response.
    /// @error Any  Application responded with a UDS NRC value from DiagUdsNrcErrc
    /// @pre -
    /// @reentrant FALSE
    /// @traceid{SWS_DM_01514}@tracestatus{draft}
    virtual auto ExitRead(DataTransferExitType exitType,
                          ara::core::Span< ara::core::Byte > transferRequestParameterRecord,
                          MetaInfo const& metaInfo,
                          CancellationHandler cancellationHandler) noexcept
        -> ara::core::Future< ara::core::Vector< ara::core::Byte > > = 0;
};

/// @brief Handles data transfers initiated by RequestReadFile/-Directory with data pushed by the
/// implementation
/// @vpublic
/// @traceid{SWS_DM_01515}@tracestatus{draft}
class DataTransferReadByPushHandler
{
protected:
    /// @brief Constructs an instance of this class
    /// @pre -
    /// @reentrant FALSE
    /// @traceid{SWS_DM_01516}@tracestatus{draft}
    DataTransferReadByPushHandler() noexcept = default;

public:
    /// @brief Move constructs an instance of this class
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[out] other The other object
    /// @return Reference to self
    /// @pre -
    /// @reentrant FALSE
    /// @traceid{SWS_DM_01517}@tracestatus{draft}
    DataTransferReadByPushHandler(DataTransferReadByPushHandler&& other) noexcept = default;

    /// @brief Handlers shall not be copiable since only one way usage i.e. per single file transfer session
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @traceid{SWS_DM_01518}@tracestatus{draft}
    DataTransferReadByPushHandler(DataTransferReadByPushHandler const&) = delete;

    /// @brief Destructs an instance of this class
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @details An instance of this class must not be destroyed before ExitRead() is called
    /// @pre -
    /// @reentrant FALSE
    /// @traceid{SWS_DM_01519}@tracestatus{draft}
    virtual ~DataTransferReadByPushHandler() noexcept = default;

    /// @brief Move assigns an instance of this class
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[out] other The other object
    /// @return Reference to self
    /// @pre -
    /// @reentrant FALSE
    /// @traceid{SWS_DM_01520}@tracestatus{draft}
    auto operator=(DataTransferReadByPushHandler&& other) & noexcept -> DataTransferReadByPushHandler& = default;

    /// @brief Handlers shall not be assignable since only one way usage i.e. per single file transfer
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// session
    /// @traceid{SWS_DM_01521}@tracestatus{draft}
    auto operator=(DataTransferReadByPushHandler const&) -> DataTransferReadByPushHandler& = delete;

    /// @brief Reads data chunk from the application
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @details The first or subsequent file/directory info content chunk to be sent back to the UDS client
    /// will be composed by the application and its ownership is passed as a result to AraDiag.
    /// @param[in] recommended_number_bytes_to_return The recommended number of bytes to send back to DM in
    /// order to
    /// get optimum data throughput
    /// @param[in] meta_info Contains additional meta information
    /// @param[in] cancellation_handler Informs if the current conversation is canceled
    /// @return A Future, which either gets readied to ReadPushOutput (transferResponseParameterRecord for a
    ///         positive response message) or readied with ErrorCode from DiagUdsNrcErrc (for an negative
    ///         response message). Data in ReadPushOutput.responseData will be placed after
    ///         blockSequenceCounter as transferResponseParameterRecord in the positive response.
    /// @error Any  Application responded with a UDS NRC value from DiagUdsNrcErrc
    /// @pre -
    /// @reentrant FALSE
    /// @traceid{SWS_DM_01522}@tracestatus{draft}
    virtual auto Read(std::size_t recommendedNumberBytesToReturn,
                      MetaInfo const& metaInfo,
                      CancellationHandler cancellationHandler) noexcept
        -> ara::core::Future< ara::core::Vector< ara::core::Byte > > = 0;

    /// @brief Exits an ongoing data transfer session
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] exitType  Specifies the exit reason
    /// @param[in] transferRequestParameterRecord This parameter record contains parameter(s), which are
    /// required by the server to support the transfer of data. Format and length of this parameter(s) are
    /// vehicle manufacturer specific.
    /// @param[in] metaInfo contains additional meta information
    /// @param[in] cancellationHandler informs if the current conversation is canceled
    /// @return returns a Future, which either gets readied to OperationOutput
    /// (transferResponseParameterRecord
    ///       for a positive response message) or readied with ErrorCode from DiagUdsNrcErrc (for an negative
    ///       response message) Data in OperationOutput.response_data will be placed after SID as
    ///       transferResponseParameterRecord in the positive response.
    /// @error Any Application responded with a UDS NRC value from DiagUdsNrcErrc
    /// @pre -
    /// @reentrant FALSE
    /// @traceid{SWS_DM_01523}@tracestatus{draft}
    virtual auto ExitRead(DataTransferExitType exitType,
                          ara::core::Span< ara::core::Byte > transferRequestParameterRecord,
                          MetaInfo const& metaInfo,
                          CancellationHandler cancellationHandler) noexcept
        -> ara::core::Future< ara::core::Vector< ara::core::Byte > > = 0;
};

/// @brief Encapsulates all data transfer reading variants
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
/// @traceid{SWS_DM_01548}@tracestatus{draft}

class DataTransferReadSession final
{
public:
    /// @brief No default construction allowed
    /// @traceid{SWS_DM_01549}@tracestatus{draft}
    DataTransferReadSession() = delete;

    /// @brief Vendor specific implementation
    template < typename ReadHandler,
               typename std::enable_if_t< std::is_base_of< DataTransferReadByPullHandler, ReadHandler >::value
                                              && std::is_rvalue_reference< ReadHandler&& >::value,
                                          int > = 0 >
    explicit DataTransferReadSession(ReadHandler&& readHandler) noexcept  /// NOLINT
        : byPullHandler_{new ReadHandler(std::move(readHandler))}
    {
    }  // NOLINT[runtime/explicit]

    /// @brief Vendor specific implementation
    template < typename ReadHandler,
               typename std::enable_if_t< std::is_base_of< DataTransferReadByPushHandler, ReadHandler >::value
                                              && std::is_rvalue_reference< ReadHandler&& >::value,
                                          int > = 0 >
    explicit DataTransferReadSession(ReadHandler&& readHandler) noexcept  /// NOLINT
        : byPushHandler_{new ReadHandler(std::move(readHandler))}
    {
    }  // NOLINT[runtime/explicit]

    /// @brief Vendor specific implementation
    template < typename ReadHandler,
               typename std::enable_if_t< std::is_base_of< DataTransferReadSharedDataHandler, ReadHandler >::value
                                              && std::is_rvalue_reference< ReadHandler&& >::value,
                                          int > = 0 >
    explicit DataTransferReadSession(ReadHandler&& readHandler) noexcept  ///NOLINT
        : sharedDataHandler_{new ReadHandler(std::move(readHandler))}
    {
    }  // NOLINT[runtime/explicit]

    /// @brief Vendor specific implementation
    template < typename ReadHandler,
               typename std::enable_if_t< std::is_base_of< DataTransferReadByPullHandler, ReadHandler >::value
                                              && std::is_lvalue_reference< ReadHandler& >::value,
                                          int > = 0 >
    DataTransferReadSession(ReadHandler& read_handler) noexcept = delete;  // NOLINT[runtime/explicit]

    /// @brief Vendor specific implementation
    template < typename ReadHandler,
               typename std::enable_if_t< std::is_base_of< DataTransferReadByPushHandler, ReadHandler >::value
                                              && std::is_lvalue_reference< ReadHandler& >::value,
                                          int > = 0 >
    DataTransferReadSession(ReadHandler& read_handler) noexcept = delete;  // NOLINT[runtime/explicit]

    /// @brief Vendor specific implementation
    template < typename ReadHandler,
               typename std::enable_if_t< std::is_base_of< DataTransferReadSharedDataHandler, ReadHandler >::value
                                              && std::is_lvalue_reference< ReadHandler& >::value,
                                          int > = 0 >
    DataTransferReadSession(ReadHandler& read_handler) noexcept = delete;  // NOLINT[runtime/explicit]

    /// @brief Destructs an instance of this class
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @pre -
    /// @reentrant FALSE
    /// @traceid{SWS_DM_01550}@tracestatus{draft}
    ~DataTransferReadSession() noexcept = default;

    /// @brief Move constructs an instance of this class
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[out] other  The other object
    /// @return Reference to self
    /// @pre -
    /// @reentrant FALSE
    /// @traceid{SWS_DM_01551}@tracestatus{draft}
    DataTransferReadSession(DataTransferReadSession&& other) noexcept = default;

    /// @brief Move assigns an instance of this class
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[out] other The other object
    /// @return Reference to self
    /// @pre -
    /// @reentrant FALSE
    /// @traceid{SWS_DM_01552}@tracestatus{draft}
    auto operator=(DataTransferReadSession&& other) & noexcept -> DataTransferReadSession& = default;

    /// @brief DataTransferReadSession shall be a single not copy-able instance.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @traceid{SWS_DM_01553}@tracestatus{draft}
    DataTransferReadSession(DataTransferReadSession const&) = delete;

    /// @brief DataTransferReadSession shall be a single not assignable instance.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @traceid{SWS_DM_01554}@tracestatus{draft}
    auto operator=(DataTransferReadSession const&) -> DataTransferReadSession& = delete;

private:
    std::unique_ptr< DataTransferReadByPullHandler > byPullHandler_;
    std::unique_ptr< DataTransferReadByPushHandler > byPushHandler_;
    std::unique_ptr< DataTransferReadSharedDataHandler > sharedDataHandler_;
    friend isoft::dm::dis::FileTransferServiceAgent;
};

/// @brief Encapsulates all data transfer writing variants
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
/// @traceid{SWS_DM_01555}@tracestatus{draft}
class DataTransferWriteSession final
{
public:
    /// @brief No default construction allowed
    /// @traceid{SWS_DM_01339}@tracestatus{draft}
    DataTransferWriteSession() = delete;

    /// @brief Vendor specific implementation
    template < typename WriteHandler,
               typename std::enable_if_t< std::is_base_of< DataTransferWriteHandler, WriteHandler >::value
                                              && std::is_rvalue_reference< WriteHandler&& >::value,
                                          int > = 0 >
    explicit DataTransferWriteSession(WriteHandler&& writeHandler) noexcept  /// NOLINT
        : handler_{new WriteHandler(std::move(writeHandler))}
    {
    }  // NOLINT[runtime/explicit]

    /// @brief Vendor specific implementation
    /// @note This interface is not implemented
    template < typename WriteHandler,
               typename std::enable_if_t< std::is_base_of< DataTransferWriteHandler, WriteHandler >::value
                                              && std::is_lvalue_reference< WriteHandler& >::value,
                                          int > = 0 >
    DataTransferWriteSession(WriteHandler& writeHandler) noexcept = delete;  // NOLINT[runtime/explicit]

    /// @brief Destructs an instance of this class
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @pre -
    /// @reentrant FALSE
    /// @traceid{SWS_DM_01556}@tracestatus{draft}
    ~DataTransferWriteSession() noexcept = default;

    /// @brief Move constructs an instance of this class
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[out] other The other object
    /// @return Reference to self
    /// @pre -
    /// @reentrant FALSE
    /// @traceid{SWS_DM_01557}@tracestatus{draft}
    DataTransferWriteSession(DataTransferWriteSession&& other) noexcept = default;

    /// @brief Move assigns an instance of this class
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[out] other The other object
    /// @return Reference to self
    /// @pre -
    /// @reentrant FALSE
    /// @traceid{SWS_DM_01558}@tracestatus{draft}
    auto operator=(DataTransferWriteSession&& other) & noexcept -> DataTransferWriteSession& = default;

    /// @brief DataTransferWriteSession shall be a single not copy-able instance.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @traceid{SWS_DM_01559}@tracestatus{draft}
    DataTransferWriteSession(DataTransferWriteSession const&) = delete;

    /// @brief DataTransferWriteSession shall be a single not assignable instance.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @traceid{SWS_DM_01560}@tracestatus{draft}
    auto operator=(DataTransferWriteSession const&) -> DataTransferWriteSession& = delete;

private:
    std::unique_ptr< DataTransferWriteHandler > handler_;
    friend isoft::dm::dis::FileTransferServiceAgent;
};

}  // namespace diag
}  // namespace ara

#endif  // ARA_DIAG_DATA_TRANSFER_H__