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
/// @file       raw_data_stream.h
/// @brief      Raw data stream header file
/// @details
/// @date       2021-12-02
/// @author     cuijiusen
/// @version    1.2.0
///
/// ================================================================
///
/// @ref [SWS_CM_01020] -- Folder structure
/// @ref [SWS_CM_10488] -- Raw data stream header file existence
/// @ref [SWS_CM_10490] -- Data Type declarations in Raw data stream header file
///
/// ================================================================

#ifndef ARA_COM_RAW_RAW_DATA_STREAM_H
#define ARA_COM_RAW_RAW_DATA_STREAM_H

#include <chrono>
#include <cstddef>
#include <cstdint>

#include "ara/com/internal/raw/raw_base.h"
#include "ara/com/internal/raw/raw_stream_ethernet.h"
#include "ara/com/types.h"
#include "ara/core/instance_specifier.h"
#include "ara/core/result.h"

namespace ara {
namespace com {
namespace raw {
/// @brief Raw byte stream return result
/// @code{.isoft}
/// export_level=/COM/Raw/RAW
/// @endcode
/// @ref [SWS_CM_11300]
/// @ref [RS_CM_00410]
/// @ref [RS_CM_00411]
/// @ref [RS_CM_00412]
struct ReadDataResult
{
    /// @brief Read data pointer
    /// @ref [SWS_CM_11301]
    /// @ref [RS_CM_00410]
    /// @ref [RS_CM_00411]
    /// @ref [RS_CM_00412]
    ara::com::SamplePtr< uint8_t > data;

    /// @brief Number of bytes read
    /// @ref [SWS_CM_11302]
    /// @ref [RS_CM_00410]
    /// @ref [RS_CM_00411]
    /// @ref [RS_CM_00412]
    size_t numberOfBytes;
};

/// @brief Raw client class
/// @code{.isoft}
/// export_level=/COM/Raw/RAW
/// @endcode
/// @ref [SWS_CM_10481]
/// @ref [RS_CM_00410]
/// @ref [RS_CM_00411]
class RawDataStreamClient final
{
public:
    /// @brief Constructor
    /// @code{.isoft}
    /// export_level=/COM/Raw/RAW
    /// @endcode
    /// @param[in] instance The instance specifier for the instance.
    /// @ref [SWS_CM_10482]
    /// @ref [RS_CM_00410]
    /// @ref [RS_CM_00411]
    /// @ref [RS_CM_00412]
    explicit RawDataStreamClient(ara::core::InstanceSpecifier const& instance) noexcept;

    /// @brief Destructor
    /// @ref [SWS_CM_10483]
    /// @ref [RS_CM_00410]
    /// @ref [RS_CM_00411]
    /// @ref [RS_CM_00412]
    ~RawDataStreamClient() noexcept = default;

    /// @brief Copy constructor
    /// @param[in] other The RawDataStreamClient object to be copy.
    /// @ref [SWS_CM_11303]
    /// @ref [RS_CM_00410]
    /// @ref [RS_CM_00411]
    /// @ref [RS_CM_00412]
    RawDataStreamClient(RawDataStreamClient const& other) noexcept = delete;

    /// @brief Copy assignment function
    /// @param[in] other The RawDataStreamClient object to be copy.
    /// @return RawDataStreamClient&
    /// @ref [SWS_CM_11304]
    /// @ref [RS_CM_00410]
    /// @ref [RS_CM_00411]
    /// @ref [RS_CM_00412]
    RawDataStreamClient& operator=(RawDataStreamClient const& other) noexcept = delete;

    /// @brief Move constructor
    /// @param[in] other The RawDataStreamClient object to be moved.
    /// @ref [SWS_CM_11305]
    /// @ref [RS_CM_00410]
    /// @ref [RS_CM_00411]
    /// @ref [RS_CM_00412]
    RawDataStreamClient(RawDataStreamClient&& other) noexcept;

    /// @brief Move assignment function
    /// @param[in] other The RawDataStreamClient object to be moved.
    /// @return RawDataStreamClient&
    /// @ref [SWS_CM_11306]
    /// @ref [RS_CM_00410]
    /// @ref [RS_CM_00411]
    /// @ref [RS_CM_00412]
    RawDataStreamClient& operator=(RawDataStreamClient&& other) & noexcept;

    /// @brief Client connection function
    /// @code{.isoft}
    /// export_level=/COM/Raw/RAW
    /// @endcode
    /// @return void if successful, otherwise an error code indicating the error.
    /// @error ara::com::raw::RawErrc::kConnectionRefused -- The connection was refused by target.
    /// @error ara::com::raw::RawErrc::kAddressNotAvailable -- The specified address is not available from the local machine.
    /// @error ara::com::raw::RawErrc::kStreamAlreadyConnected -- The specified connection is already connected.
    /// @error ara::com::raw::RawErrc::kPeerUnreachable -- The peer is unreachable by the network.
    /// @error ara::com::raw::RawErrc::kInterruptedBySignal -- The operation was interrupted by the system.
    /// @ref [SWS_CM_10484]
    /// @ref [RS_CM_00410]
    /// @ref [RS_CM_00411]
    /// @ref [RS_CM_00412]
    ara::core::Result< void > Connect() noexcept;

    /// @brief Client connection function with timeout
    /// @code{.isoft}
    /// export_level=/COM/Raw/RAW
    /// @endcode
    /// @param[in] timeout Timeout value for this operation.
    /// @return void if successful, otherwise an error code indicating the error.
    /// @error ara::com::raw::RawErrc::kConnectionRefused -- The connection was refused by target.
    /// @error ara::com::raw::RawErrc::kAddressNotAvailable -- The specified address is not available from the local machine.
    /// @error ara::com::raw::RawErrc::kCommunicationTimeout -- The connect operation timed out.
    /// @error ara::com::raw::RawErrc::kStreamAlreadyConnected -- The specified connection is already connected.
    /// @error ara::com::raw::RawErrc::kPeerUnreachable -- The peer is unreachable by the network.
    /// @error ara::com::raw::RawErrc::kInterruptedBySignal -- The operation was interrupted by the system.
    /// @ref [SWS_CM_11307]
    /// @ref [RS_CM_00410]
    /// @ref [RS_CM_00411]
    /// @ref [RS_CM_00412]
    ara::core::Result< void > Connect(std::chrono::milliseconds timeout) noexcept;

    /// @brief Close connection
    /// @code{.isoft}
    /// export_level=/COM/Raw/RAW
    /// @endcode
    /// @return void if successful, otherwise an error code indicating the error
    /// @error ara::com::raw::RawErrc::kStreamNotConnected -- Trying to shutdown a RawDataStream without an established connection.
    /// @error ara::com::raw::RawErrc::kInterruptedBySignal -- The operation was interrupted by the system.
    /// @ref [SWS_CM_10485]
    /// @ref [RS_CM_00410]
    /// @ref [RS_CM_00411]
    /// @ref [RS_CM_00412]
    ara::core::Result< void > Shutdown() noexcept;

    /// @brief Close connection with timeout
    /// @code{.isoft}
    /// export_level=/COM/Raw/RAW
    /// @endcode
    /// @param[in] timeout Timeout value for this operation.
    /// @return void if successful, otherwise an error code indicating the error.
    /// @error ara::com::raw::RawErrc::kStreamNotConnected -- Trying to shutdown a RawDataStream without an established connection.
    /// @error ara::com::raw::RawErrc::kCommunicationTimeout -- The operation dis not finish until the timeout expired.
    /// @error ara::com::raw::RawErrc::kInterruptedBySignal -- The operation was interrupted by the system.
    /// @ref [SWS_CM_11308]
    /// @ref [RS_CM_00410]
    /// @ref [RS_CM_00411]
    /// @ref [RS_CM_00412]
    ara::core::Result< void > Shutdown(std::chrono::milliseconds timeout) noexcept;

    /// @brief Raw byte stream read data
    /// @code{.isoft}
    /// export_level=/COM/Raw/RAW
    /// @endcode
    /// @param[in] maxLength The requested number of bytes to read from the stream.
    /// @return a struct of type ReadDataResult if succesful, otherwise an error code indicating the error.
    /// @error ara::com::raw::RawErrc::kStreamNotConnected -- Trying to read from a stream without an established connection.
    /// @error ara::com::raw::RawErrc::kInterruptedBySignal -- The operation was interrupted by the system.
    /// @ref [SWS_CM_10486]
    /// @ref [RS_CM_00410]
    /// @ref [RS_CM_00411]
    /// @ref [RS_CM_00412]
    ara::core::Result< ReadDataResult > ReadData(size_t maxLength) noexcept;

    /// @brief Raw byte stream read data with timeout
    /// @code{.isoft}
    /// export_level=/COM/Raw/RAW
    /// @endcode
    /// @param[in] maxLength The number of bytes to read from the stream.
    /// @param[in] timeout Timeout value for this operation.
    /// @return a struct of type ReadDataResult if succesful, otherwise an error code indicating the error.
    /// @error ara::com::raw::RawErrc::kStreamNotConnected -- Trying to read from a stream without an established connection.
    /// @error ara::com::raw::RawErrc::kCommunicationTimeout -- No data was read until the timeout expired.
    /// @error ara::com::raw::RawErrc::kInterruptedBySignal -- The operation was interrupted by the system.
    /// @ref [SWS_CM_11309]
    /// @ref [RS_CM_00410]
    /// @ref [RS_CM_00411]
    /// @ref [RS_CM_00412]
    ara::core::Result< ReadDataResult > ReadData(size_t maxLength, std::chrono::milliseconds timeout) noexcept;

    /// @brief Raw byte stream write data
    /// @code{.isoft}
    /// export_level=/COM/Raw/RAW
    /// @endcode
    /// @param[in] data pointer to the byte array to send. A SamplePtr is used to get std::unique_ptr semantics.
    /// @param[in] maxLength The number of bytes to write to the stream.
    /// @return the actual number of bytes written if succesful, otherwise an error code indicating the error.
    /// @error ara::com::raw::RawErrc::kStreamNotConnected -- Trying to write to a stream without an established connection.
    /// @error ara::com::raw::RawErrc::kConnectionClosedByPeer -- The established connection has been shut down during writing.
    /// @error ara::com::raw::RawErrc::kInterruptedBySignal -- The operation was interrupted by the system.
    /// @ref [SWS_CM_10487]
    /// @ref [RS_CM_00410]
    /// @ref [RS_CM_00411]
    /// @ref [RS_CM_00412]
    ara::core::Result< size_t > WriteData(ara::com::SamplePtr< uint8_t > data, size_t maxLength) noexcept;

    /// @brief Raw byte stream write data with timeout
    /// @code{.isoft}
    /// export_level=/COM/Raw/RAW
    /// @endcode
    /// @param[in] data pointer to the byte array to send. A SamplePtr is used to get std::unique_ptr semantics.
    /// @param[in] maxLength The number of bytes to write to the stream.
    /// @param[in] timeout Timeout value for this operation.
    /// @return the actual number of bytes written if succesful, otherwise an error code indicating the error.
    /// @error ara::com::raw::RawErrc::kStreamNotConnected -- Trying to write to a stream without an established connection.
    /// @error ara::com::raw::RawErrc::kCommunicationTimeout -- No data was written until the timeout expired.
    /// @error ara::com::raw::RawErrc::kConnectionClosedByPeer -- The established connection has been shut down during writing.
    /// @error ara::com::raw::RawErrc::kInterruptedBySignal -- The operation was interrupted by the system.
    /// @ref [SWS_CM_11310]
    /// @ref [RS_CM_00410]
    /// @ref [RS_CM_00411]
    /// @ref [RS_CM_00412]
    ara::core::Result< size_t > WriteData(ara::com::SamplePtr< uint8_t > data,
                                          size_t maxLength,
                                          std::chrono::milliseconds timeout) noexcept;

private:
    /// @brief client connentction object
    std::unique_ptr< ara::com::internal::raw::BaseRaw > clientConn_{};
};

/// @brief Raw server class
/// @code{.isoft}
/// export_level=/COM/Raw/RAW
/// @endcode
/// @ref [SWS_CM_11311]
/// @ref [RS_CM_00410]
/// @ref [RS_CM_00411]
class RawDataStreamServer final
{
public:
    /// @brief Constructor
    /// @code{.isoft}
    /// export_level=/COM/Raw/RAW
    /// @endcode
    /// @param[in] instance The instance specifier for the instance.
    /// @ref [SWS_CM_11312]
    /// @ref [RS_CM_00410]
    /// @ref [RS_CM_00411]
    /// @ref [RS_CM_00412]
    explicit RawDataStreamServer(ara::core::InstanceSpecifier const& instance) noexcept;

    /// @brief Destructor
    /// @ref [SWS_CM_11313]
    /// @ref [RS_CM_00410]
    /// @ref [RS_CM_00411]
    /// @ref [RS_CM_00412]
    ~RawDataStreamServer() noexcept;

    /// @brief Copy constructor
    /// @ref [SWS_CM_11314]
    /// @ref [RS_CM_00410]
    /// @ref [RS_CM_00411]
    /// @ref [RS_CM_00412]
    RawDataStreamServer(RawDataStreamServer const&) noexcept = delete;

    /// @brief Copy assignment function
    /// @return RawDataStreamServer&
    /// @ref [SWS_CM_11315]
    /// @ref [RS_CM_00410]
    /// @ref [RS_CM_00411]
    /// @ref [RS_CM_00412]
    RawDataStreamServer& operator=(RawDataStreamServer const&) noexcept = delete;

    /// @brief Move constructor
    /// @param[in] other The RawDataStreamServer object to be moved.
    /// @ref [SWS_CM_11316]
    /// @ref [RS_CM_00410]
    /// @ref [RS_CM_00411]
    /// @ref [RS_CM_00412]
    RawDataStreamServer(RawDataStreamServer&& other) noexcept;

    /// @brief Move assignment function
    /// @param[in] other The RawDataStreamServer object to be moved.
    /// @return RawDataStreamServer&
    /// @ref [SWS_CM_11317]
    /// @ref [RS_CM_00410]
    /// @ref [RS_CM_00411]
    /// @ref [RS_CM_00412]
    RawDataStreamServer& operator=(RawDataStreamServer&& other) & noexcept;

    /// @brief Server wait for connection function
    /// @code{.isoft}
    /// export_level=/COM/Raw/RAW
    /// @endcode
    /// @return void if successful, otherwise an error code indicating the error.
    /// @error ara::com::raw::RawErrc::kAddressNotAvailable -- The specified address is not available from the local machine.
    /// @error ara::com::raw::RawErrc::kStreamAlreadyConnected -- The specified connection is already connected.
    /// @error ara::com::raw::RawErrc::kConnectionAborted -- The incoming connection was aborted by the network.
    /// @error ara::com::raw::RawErrc::kInterruptedBySignal -- The operation was interrupted by the system.
    /// @ref [SWS_CM_11318]
    /// @ref [RS_CM_00410]
    /// @ref [RS_CM_00411]
    /// @ref [RS_CM_00412]
    ara::core::Result< void > WaitForConnection() noexcept;

    /// @brief Server wait for connection function with timeout
    /// @code{.isoft}
    /// export_level=/COM/Raw/RAW
    /// @endcode
    /// @param[in] timeout Timeout value for this operation.
    /// @return void if successful, otherwise an error code indicating the error.
    /// @error ara::com::raw::RawErrc::kAddressNotAvailable -- The specified address is not available from the local machine.
    /// @error ara::com::raw::RawErrc::kCommunicationTimeout -- The WaitForConnection operation timed out.
    /// @error ara::com::raw::RawErrc::kStreamAlreadyConnected -- The specified connection is already connected.
    /// @error ara::com::raw::RawErrc::kConnectionAborted -- The incoming connection was aborted by the network.
    /// @error ara::com::raw::RawErrc::kInterruptedBySignal -- The operation was interrupted by the system
    /// @ref [SWS_CM_11319]
    /// @ref [RS_CM_00410]
    /// @ref [RS_CM_00411]
    /// @ref [RS_CM_00412]
    ara::core::Result< void > WaitForConnection(std::chrono::milliseconds timeout) noexcept;

    /// @brief Server close connection
    /// @code{.isoft}
    /// export_level=/COM/Raw/RAW
    /// @endcode
    /// @return void if successful, otherwise an error code indicating the error.
    /// @error ara::com::raw::RawErrc::kStreamNotConnected -- Trying to shutdown a RawDataStream without an established connection.
    /// @error ara::com::raw::RawErrc::kInterruptedBySignal -- The operation was interrupted by the system.
    /// @ref [SWS_CM_11320]
    /// @ref [RS_CM_00410]
    /// @ref [RS_CM_00411]
    /// @ref [RS_CM_00412]
    ara::core::Result< void > Shutdown() noexcept;

    /// @brief Server close connection with timeout
    /// @code{.isoft}
    /// export_level=/COM/Raw/RAW
    /// @endcode
    /// @param[in] timeout Parameter to assign a timeout for this operation.
    /// @return void if successful, otherwise an error code indicating the error.
    /// @error ara::com::raw::RawErrc::kStreamNotConnected -- Trying to shutdown a RawDataStream without an established connection.
    /// @error ara::com::raw::RawErrc::kCommunicationTimeout -- The operation dis not finish until the timeout expired.
    /// @error ara::com::raw::RawErrc::kInterruptedBySignal -- The operation was interrupted by the system.
    /// @ref [SWS_CM_11321]
    /// @ref [RS_CM_00410]
    /// @ref [RS_CM_00411]
    /// @ref [RS_CM_00412]
    ara::core::Result< void > Shutdown(std::chrono::milliseconds timeout) noexcept;

    /// @brief Server raw byte stream read data
    /// @code{.isoft}
    /// export_level=/COM/Raw/RAW
    /// @endcode
    /// @param[in] maxLength The number of bytes to read from the stream.
    /// @return a struct of type ReadDataResult if succesful, otherwise an error code indicating the error.
    /// @error ara::com::raw::RawErrc::kStreamNotConnected -- Trying to read from a stream without an established connection.
    /// @error ara::com::raw::RawErrc::kInterruptedBySignal -- The operation was interrupted by the system.
    /// @ref [SWS_CM_11322]
    /// @ref [RS_CM_00410]
    /// @ref [RS_CM_00411]
    /// @ref [RS_CM_00412]
    ara::core::Result< ReadDataResult > ReadData(size_t maxLength) noexcept;

    /// @brief Server raw byte stream read data with timeout
    /// @code{.isoft}
    /// export_level=/COM/Raw/RAW
    /// @endcode
    /// @param[in] maxLength The number of bytes to read from the stream.
    /// @param[in] timeout Parameter to assign a timeout for this operation.
    /// @return a struct of type ReadDataResult.
    /// @error ara::com::raw::RawErrc::kStreamNotConnected -- Trying to read from a stream without an established connection.
    /// @error ara::com::raw::RawErrc::kCommunicationTimeout -- No data was read until the timeout expired.
    /// @error ara::com::raw::RawErrc::kInterruptedBySignal -- The operation was interrupted by the system.
    /// @ref [SWS_CM_11323]
    /// @ref [RS_CM_00410]
    /// @ref [RS_CM_00411]
    /// @ref [RS_CM_00412]
    ara::core::Result< ReadDataResult > ReadData(size_t maxLength, std::chrono::milliseconds timeout) noexcept;

    /// @brief Server raw byte stream write data
    /// @code{.isoft}
    /// export_level=/COM/Raw/RAW
    /// @endcode
    /// @param[in] data pointer to the byte array to send. A SamplePtr is used to get std::unique_ptr semantics.
    /// @param[in] maxLength The number of bytes to write to the stream.
    /// @return the actual number of bytes written if succesful, otherwise an error code indicating the error.
    /// @error ara::com::raw::RawErrc::kStreamNotConnected -- Trying to write to a stream without an established connection.
    /// @error ara::com::raw::RawErrc::kConnectionClosedByPeer -- The established connection has been shut down during writing.
    /// @error ara::com::raw::RawErrc::kInterruptedBySignal -- The operation was interrupted by the system.
    /// @ref [SWS_CM_11324]
    /// @ref [RS_CM_00410]
    /// @ref [RS_CM_00411]
    /// @ref [RS_CM_00412]
    ara::core::Result< size_t > WriteData(ara::com::SamplePtr< uint8_t > data, size_t maxLength) noexcept;

    /// @brief Server raw byte stream write data with timeout
    /// @code{.isoft}
    /// export_level=/COM/Raw/RAW
    /// @endcode
    /// @param[in] data pointer to the byte array to send. A SamplePtr is used to get std::unique_ptr semantics.
    /// @param[in] maxLength The number of bytes to write to the stream.
    /// @param[in] timeout Parameter to assign a timeout for this operation.
    /// @return the actual number of bytes written if succesful, otherwise an error code indicating the error.
    /// @error ara::com::raw::RawErrc::kStreamNotConnected -- Trying to write to a stream without an established connection.
    /// @error ara::com::raw::RawErrc::kCommunicationTimeout -- No data was written until the timeout expired.
    /// @error ara::com::raw::RawErrc::kConnectionClosedByPeer -- The established connection has been shut down during writing.
    /// @error ara::com::raw::RawErrc::kInterruptedBySignal -- The operation was interrupted by the system.
    /// @ref [SWS_CM_11325]
    /// @ref [RS_CM_00410]
    /// @ref [RS_CM_00411]
    /// @ref [RS_CM_00412]
    ara::core::Result< size_t > WriteData(ara::com::SamplePtr< uint8_t > data,
                                          size_t maxLength,
                                          std::chrono::milliseconds timeout) noexcept;

private:
    /// @brief server connection object
    std::unique_ptr< ara::com::internal::raw::BaseRaw > serverConn_{};
};
}  // namespace raw
}  // namespace com
}  // namespace ara
#endif