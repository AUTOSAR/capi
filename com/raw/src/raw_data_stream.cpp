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
/// @file       raw_data_stream.cpp
/// @brief
/// @details
/// @date       2021-12-02
/// @author     cuijiusen
/// @version    1.2.0
///
/// ================================================================

#include "ara/com/raw/raw_data_stream.h"

#include "ara/com/internal/raw/raw_runtime.h"
#include "ara/com/internal/raw/raw_stream_ethernet.h"
namespace ara {
namespace com {
namespace raw {

/// @brief Raw byte stream client constructor
/// @param[in] instance The instance specifier for the instance.
RawDataStreamClient::RawDataStreamClient(ara::core::InstanceSpecifier const& instance) noexcept
    : clientConn_{ara::com::internal::raw::RawRuntime::CreateInstance(instance)}
{
}

/// @brief Raw byte stream client move constructor
/// @param[in] other The RawDataStreamClient object to be moved.
RawDataStreamClient::RawDataStreamClient(RawDataStreamClient&& other) noexcept
    : clientConn_{std::move(other.clientConn_)}
{
}

/// @brief Raw byte stream client move assignment function
/// @param[in] other The RawDataStreamClient object to be moved.
/// @ref [SWS_CM_11306]
/// @ref [RS_CM_00410]
/// @ref [RS_CM_00411]
/// @ref [RS_CM_00412]
RawDataStreamClient& RawDataStreamClient::operator=(RawDataStreamClient&& other) & noexcept
{
    clientConn_ = std::move(other.clientConn_);
    return *this;
}

/// @brief Raw byte stream client connection function
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
ara::core::Result< void > RawDataStreamClient::Connect() noexcept
{
    if (nullptr == clientConn_.get()) {
        return ara::core::Result< void >::FromError(ara::com::raw::RawErrc::kAddressNotAvailable);
    }
    return clientConn_.get()->Connect();
}
/// @brief Raw byte stream client connection function with timeout

/// @param[in] timeout Timeout value for this operation.
/// @return void if successful, otherwise an error code indicating the error.
/// @error ara::com::raw::RawErrc::kConnectionRefused -- The connection was refused by target.
/// @error ara::com::raw::RawErrc::kAddressNotAvailable -- The specified address is not available from the local machine.
/// @error ara::com::raw::RawErrc::kCommunicationTimeout -- The connect operation timed out.
/// @error ara::com::raw::RawErrc::kStreamAlreadyConnected -- The specified connection is already connected.
/// @error ara::com::raw::RawErrc::kPeerUnreachable -- The peer is unreachable by the network.
/// @error ara::com::raw::RawErrc::kInterruptedBySignal -- The operation was interrupted by the system.
/// @exception
/// @ref [SWS_CM_11307]
/// @ref [RS_CM_00410]
/// @ref [RS_CM_00411]
/// @ref [RS_CM_00412]
ara::core::Result< void > RawDataStreamClient::Connect(std::chrono::milliseconds timeout) noexcept
{
    if (nullptr == clientConn_.get()) {
        return ara::core::Result< void >::FromError(ara::com::raw::RawErrc::kAddressNotAvailable);
    }
    return clientConn_.get()->Connect(timeout.count());
}

/// @brief Raw byte stream client close connection function
/// @return void if successful, otherwise an error code indicating the error
/// @error ara::com::raw::RawErrc::kStreamNotConnected -- Trying to shutdown a RawDataStream without an established connection.
/// @error ara::com::raw::RawErrc::kInterruptedBySignal -- The operation was interrupted by the system.
/// @ref [SWS_CM_10485]
/// @ref [RS_CM_00410]
/// @ref [RS_CM_00411]
/// @ref [RS_CM_00412]
ara::core::Result< void > RawDataStreamClient::Shutdown() noexcept
{
    if (nullptr == clientConn_.get()) {
        return ara::core::Result< void >::FromError(ara::com::raw::RawErrc::kAddressNotAvailable);
    }
    return clientConn_.get()->Shutdown();
}

/// @brief Raw byte stream client close connection function with timeout
/// @param[in] timeout Timeout value for this operation.
/// @return void if successful, otherwise an error code indicating the error.
/// @error ara::com::raw::RawErrc::kStreamNotConnected -- Trying to shutdown a RawDataStream without an established connection.
/// @error ara::com::raw::RawErrc::kCommunicationTimeout -- The operation dis not finish until the timeout expired.
/// @error ara::com::raw::RawErrc::kInterruptedBySignal -- The operation was interrupted by the system.
/// @exception
/// @ref [SWS_CM_11308]
/// @ref [RS_CM_00410]
/// @ref [RS_CM_00411]
/// @ref [RS_CM_00412]
ara::core::Result< void > RawDataStreamClient::Shutdown(std::chrono::milliseconds timeout) noexcept
{
    if (nullptr == clientConn_.get()) {
        return ara::core::Result< void >::FromError(ara::com::raw::RawErrc::kAddressNotAvailable);
    }
    return clientConn_.get()->Shutdown(timeout.count());
}

/// @brief Raw byte stream client read data function
/// @param[in] maxLength The requested number of bytes to read from the stream.
/// @return a struct of type ReadDataResult if succesful, otherwise an error code indicating the error.
/// @error ara::com::raw::RawErrc::kStreamNotConnected -- Trying to read from a stream without an established connection.
/// @error ara::com::raw::RawErrc::kInterruptedBySignal -- The operation was interrupted by the system.
/// @ref [SWS_CM_10486]
/// @ref [RS_CM_00410]
/// @ref [RS_CM_00411]
/// @ref [RS_CM_00412]
ara::core::Result< ReadDataResult > RawDataStreamClient::ReadData(size_t maxLength) noexcept
{
    if (nullptr == clientConn_.get()) {
        return ara::core::Result< ReadDataResult >::FromError(ara::com::raw::RawErrc::kAddressNotAvailable);
    }
    return clientConn_.get()->ReadData(maxLength);
}

/// @brief Raw byte stream client read data function with timeout
/// @param[in] maxLength The number of bytes to read from the stream.
/// @param[in] timeout Timeout value for this operation.
/// @return a struct of type ReadDataResult if succesful, otherwise an error code indicating the error.
/// @error ara::com::raw::RawErrc::kStreamNotConnected -- Trying to read from a stream without an established connection.
/// @error ara::com::raw::RawErrc::kCommunicationTimeout -- No data was read until the timeout expired.
/// @error ara::com::raw::RawErrc::kInterruptedBySignal -- The operation was interrupted by the system.
/// @exception
/// @ref [SWS_CM_11309]
/// @ref [RS_CM_00410]
/// @ref [RS_CM_00411]
/// @ref [RS_CM_00412]
ara::core::Result< ReadDataResult > RawDataStreamClient::ReadData(size_t maxLength,
                                                                  std::chrono::milliseconds timeout) noexcept
{
    if (nullptr == clientConn_.get()) {
        return ara::core::Result< ReadDataResult >::FromError(ara::com::raw::RawErrc::kAddressNotAvailable);
    }
    return clientConn_.get()->ReadData(maxLength, timeout.count());
}

/// @brief Raw byte stream client write data function
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
ara::core::Result< size_t > RawDataStreamClient::WriteData(ara::com::SamplePtr< uint8_t > data,
                                                           size_t maxLength) noexcept
{
    if (nullptr == clientConn_.get()) {
        return ara::core::Result< size_t >::FromError(ara::com::raw::RawErrc::kAddressNotAvailable);
    }
    return clientConn_.get()->WriteData(std::move(data), maxLength);
}

/// @brief Raw byte stream client write data function with timeout
/// @param[in] data pointer to the byte array to send. A SamplePtr is used to get std::unique_ptr semantics.
/// @param[in] maxLength The number of bytes to write to the stream.
/// @param[in] timeout Timeout value for this operation.
/// @return the actual number of bytes written if succesful, otherwise an error code indicating the error.
/// @error ara::com::raw::RawErrc::kStreamNotConnected -- Trying to write to a stream without an established connection.
/// @error ara::com::raw::RawErrc::kCommunicationTimeout -- No data was written until the timeout expired.
/// @error ara::com::raw::RawErrc::kConnectionClosedByPeer -- The established connection has been shut down during writing.
/// @error ara::com::raw::RawErrc::kInterruptedBySignal -- The operation was interrupted by the system.
/// @exception
/// @ref [SWS_CM_11310]
/// @ref [RS_CM_00410]
/// @ref [RS_CM_00411]
/// @ref [RS_CM_00412]
ara::core::Result< size_t > RawDataStreamClient::WriteData(ara::com::SamplePtr< uint8_t > data,
                                                           size_t maxLength,
                                                           std::chrono::milliseconds timeout) noexcept
{
    if (nullptr == clientConn_.get()) {
        return ara::core::Result< size_t >::FromError(ara::com::raw::RawErrc::kAddressNotAvailable);
    }
    return clientConn_.get()->WriteData(std::move(data), maxLength, timeout.count());
}

/// @brief Raw byte stream server constructor
/// instance.
/// @param[in] instance The instance specifier for the instance.
/// @ref [SWS_CM_11312]
/// @ref [RS_CM_00410]
/// @ref [RS_CM_00411]
/// @ref [RS_CM_00412]
RawDataStreamServer::RawDataStreamServer(ara::core::InstanceSpecifier const& instance) noexcept
    : serverConn_{ara::com::internal::raw::RawRuntime::CreateInstance(instance)}
{
}

/// @brief Raw byte stream server destructor
/// @ref [SWS_CM_11313]
/// @ref [RS_CM_00410]
/// @ref [RS_CM_00411]
/// @ref [RS_CM_00412]
RawDataStreamServer::~RawDataStreamServer() noexcept { std::ignore = Shutdown(); }

/// @brief Raw byte stream server move constructor
/// @param[in] other The RawDataStreamServer object to be moved.
/// @ref [SWS_CM_11316]
/// @ref [RS_CM_00410]
/// @ref [RS_CM_00411]
/// @ref [RS_CM_00412]
RawDataStreamServer::RawDataStreamServer(RawDataStreamServer&& other) noexcept
    : serverConn_{std::move(other.serverConn_)}
{
}

/// @brief Raw byte stream server copy assignment function
/// @param[in] other The RawDataStreamServer object to be moved.
/// @ref [SWS_CM_11317]
/// @ref [RS_CM_00410]
/// @ref [RS_CM_00411]
/// @ref [RS_CM_00412]
RawDataStreamServer& RawDataStreamServer::operator=(RawDataStreamServer&& other) & noexcept
{
    serverConn_ = std::move(other.serverConn_);
    return *this;
}

/// @brief Raw byte stream server create connection function
/// @return void if successful, otherwise an error code indicating the error.
/// @error ara::com::raw::RawErrc::kAddressNotAvailable -- The specified address is not available from the local machine.
/// @error ara::com::raw::RawErrc::kStreamAlreadyConnected -- The specified connection is already connected.
/// @error ara::com::raw::RawErrc::kConnectionAborted -- The incoming connection was aborted by the network.
/// @error ara::com::raw::RawErrc::kInterruptedBySignal -- The operation was interrupted by the system.
/// @ref [SWS_CM_11318]
/// @ref [RS_CM_00410]
/// @ref [RS_CM_00411]
/// @ref [RS_CM_00412]
ara::core::Result< void > RawDataStreamServer::WaitForConnection() noexcept
{
    if (nullptr == serverConn_.get()) {
        return ara::core::Result< void >::FromError(ara::com::raw::RawErrc::kAddressNotAvailable);
    }
    ara::core::Result< void > const res{serverConn_.get()->WaitForConnection()};
    if (res) {
        return ara::core::Result< void >::FromValue();
    }
    return ara::core::Result< void >::FromError(res.Error());
}

/// @brief Raw byte stream server create connection function with timeout
/// @param[in] timeout Timeout value for this operation.
/// @return void if successful, otherwise an error code indicating the error.
/// @error ara::com::raw::RawErrc::kAddressNotAvailable -- The specified address is not available from the local machine.
/// @error ara::com::raw::RawErrc::kCommunicationTimeout -- The WaitForConnection operation timed out.
/// @error ara::com::raw::RawErrc::kStreamAlreadyConnected -- The specified connection is already connected.
/// @error ara::com::raw::RawErrc::kConnectionAborted -- The incoming connection was aborted by the network.
/// @error ara::com::raw::RawErrc::kInterruptedBySignal -- The operation was interrupted by the system
/// @exception
/// @ref [SWS_CM_11319]
/// @ref [RS_CM_00410]
/// @ref [RS_CM_00411]
/// @ref [RS_CM_00412]
ara::core::Result< void > RawDataStreamServer::WaitForConnection(std::chrono::milliseconds timeout) noexcept
{
    if (nullptr == serverConn_.get()) {
        return ara::core::Result< void >::FromError(ara::com::raw::RawErrc::kAddressNotAvailable);
    }
    return serverConn_.get()->WaitForConnection(timeout.count());
}

/// @brief Raw byte stream server close connection function
/// @return void if successful, otherwise an error code indicating the error.
/// @error ara::com::raw::RawErrc::kStreamNotConnected -- Trying to shutdown a RawDataStream without an established connection.
/// @error ara::com::raw::RawErrc::kInterruptedBySignal -- The operation was interrupted by the system.
/// @ref [SWS_CM_11320]
/// @ref [RS_CM_00410]
/// @ref [RS_CM_00411]
/// @ref [RS_CM_00412]
ara::core::Result< void > RawDataStreamServer::Shutdown() noexcept
{
    if (nullptr == serverConn_.get()) {
        return ara::core::Result< void >::FromError(ara::com::raw::RawErrc::kAddressNotAvailable);
    }
    std::ignore = serverConn_.get()->Shutdown();
    return serverConn_.get()->Shutdown();
}

/// @brief Raw byte stream server close connection function with timeout
/// @param[in] timeout Parameter to assign a timeout for this operation.
/// @return void if successful, otherwise an error code indicating the error.
/// @error ara::com::raw::RawErrc::kStreamNotConnected -- Trying to shutdown a RawDataStream without an established connection.
/// @error ara::com::raw::RawErrc::kCommunicationTimeout -- The operation did not finish until the timeout expired.
/// @error ara::com::raw::RawErrc::kInterruptedBySignal -- The operation was interrupted by the system.
/// @exception
/// @ref [SWS_CM_11321]
/// @ref [RS_CM_00410]
/// @ref [RS_CM_00411]
/// @ref [RS_CM_00412]
ara::core::Result< void > RawDataStreamServer::Shutdown(std::chrono::milliseconds timeout) noexcept
{
    if (nullptr == serverConn_.get()) {
        return ara::core::Result< void >::FromError(ara::com::raw::RawErrc::kAddressNotAvailable);
    }
    std::ignore = serverConn_.get()->Shutdown();
    return serverConn_.get()->Shutdown(timeout.count());
}

/// @brief Raw byte stream server read data function
/// @param[in] maxLength The number of bytes to read from the stream.
/// @return a struct of type ReadDataResult if succesful, otherwise an error code indicating the error.
/// @error ara::com::raw::RawErrc::kStreamNotConnected -- Trying to read from a stream without an established connection.
/// @error ara::com::raw::RawErrc::kInterruptedBySignal -- The operation was interrupted by the system.
/// @ref [SWS_CM_11322]
/// @ref [RS_CM_00410]
/// @ref [RS_CM_00411]
/// @ref [RS_CM_00412]
ara::core::Result< ReadDataResult > RawDataStreamServer::ReadData(size_t maxLength) noexcept
{
    if (nullptr == serverConn_.get()) {
        return ara::core::Result< ReadDataResult >::FromError(ara::com::raw::RawErrc::kAddressNotAvailable);
    }
    return serverConn_.get()->ReadData(maxLength);
}

/// @brief Raw byte stream server read data function with timeout
/// @param[in] maxLength The number of bytes to read from the stream.
/// @param[in] timeout Parameter to assign a timeout for this operation.
/// @return a struct of type ReadDataResult.
/// @error ara::com::raw::RawErrc::kStreamNotConnected -- Trying to read from a stream without an established connection.
/// @error ara::com::raw::RawErrc::kCommunicationTimeout -- No data was read until the timeout expired.
/// @error ara::com::raw::RawErrc::kInterruptedBySignal -- The operation was interrupted by the system.
/// @exception
/// @ref [SWS_CM_11323]
/// @ref [RS_CM_00410]
/// @ref [RS_CM_00411]
/// @ref [RS_CM_00412]
ara::core::Result< ReadDataResult > RawDataStreamServer::ReadData(size_t maxLength,
                                                                  std::chrono::milliseconds timeout) noexcept
{
    if (nullptr == serverConn_.get()) {
        return ara::core::Result< ReadDataResult >::FromError(ara::com::raw::RawErrc::kAddressNotAvailable);
    }
    return serverConn_.get()->ReadData(maxLength, timeout.count());
}

/// @brief Raw byte stream server write data function
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
ara::core::Result< size_t > RawDataStreamServer::WriteData(ara::com::SamplePtr< uint8_t > data,
                                                           size_t maxLength) noexcept
{
    if (nullptr == serverConn_.get()) {
        return ara::core::Result< size_t >::FromError(ara::com::raw::RawErrc::kAddressNotAvailable);
    }
    ara::core::Result< size_t > res{serverConn_.get()->WriteData(std::move(data), maxLength)};
    if (res) {
        return ara::core::Result< size_t >::FromValue(static_cast< size_t >(res.Value()));
    }
    return ara::core::Result< size_t >::FromError(res.Error());
}

/// @brief Raw byte stream server write data function with timeout
/// @param[in] data pointer to the byte array to send. A SamplePtr is used to get std::unique_ptr semantics.
/// @param[in] maxLength The number of bytes to write to the stream.
/// @param[in] timeout Parameter to assign a timeout for this operation.
/// @return the actual number of bytes written if succesful, otherwise an error code indicating the error.
/// @error ara::com::raw::RawErrc::kStreamNotConnected -- Trying to write to a stream without an established connection.
/// @error ara::com::raw::RawErrc::kCommunicationTimeout -- No data was written until the timeout expired.
/// @error ara::com::raw::RawErrc::kConnectionClosedByPeer -- The established connection has been shut down during writing.
/// @error ara::com::raw::RawErrc::kInterruptedBySignal -- The operation was interrupted by the system.
/// @exception
/// @ref [SWS_CM_11325]
/// @ref [RS_CM_00410]
/// @ref [RS_CM_00411]
/// @ref [RS_CM_00412]
ara::core::Result< size_t > RawDataStreamServer::WriteData(ara::com::SamplePtr< uint8_t > data,
                                                           size_t maxLength,
                                                           std::chrono::milliseconds timeout) noexcept
{
    if (nullptr == serverConn_.get()) {
        return ara::core::Result< size_t >::FromError(ara::com::raw::RawErrc::kAddressNotAvailable);
    }
    ara::core::Result< size_t > const res{serverConn_.get()->WriteData(std::move(data), maxLength, timeout.count())};
    if (res) {
        return ara::core::Result< size_t >::FromValue(static_cast< size_t >(res.Value()));
    }
    return ara::core::Result< size_t >::FromError(res.Error());
}

}  // namespace raw
}  // namespace com
}  // namespace ara