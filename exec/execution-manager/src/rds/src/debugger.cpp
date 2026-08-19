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
/// @file       debugger.cpp
/// @brief      Debugger class definition file
/// @details
/// @date       2023-11-22
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#include "ara/exec/internal/rds/debugger.h"

#include <nai/io/nai_io.h>
#include <nai/os/nai_file.h>
#include <nai/runtime/nai_errno.h>

#include "ara/exec/internal/rds/dcp.h"
#include "isoft/osi/network/socket_stream.h"

namespace ara {
namespace exec {
namespace internal {
namespace rds {
namespace {
/// @brief Get value of nai_errno
/// @return Value of nai_errno
int32_t GetNaiErrno() noexcept { return nai_errno; }  // PRQA MS "nai_errno" 3224
}  // namespace

/// @brief Listen event handler function
/// @param stream Data stream
/// @param events Events
/// @return Processing result
int32_t Debugger::NaiListenEventHandler(nai_stream_t *const stream, int32_t const events) noexcept
{
    if (nullptr == stream) {
        std::cerr << "NaiListenEventHandler(): nullptr == stream" << std::endl;
        return -1;
    }

    Debugger *const pDebugger{nai_containof(stream, Debugger, listenSockStream_)};  // NOLINT

 // Error event
    if (static_cast< uint32_t >(NAI_EV_ERROR)
        == (static_cast< uint32_t >(events) & static_cast< uint32_t >(NAI_EV_ERROR))) {
        return GetNaiErrno();
    }

 // Read event
    if (static_cast< uint32_t >(NAI_EV_READ)
        == (static_cast< uint32_t >(events) & static_cast< uint32_t >(NAI_EV_READ))) {
        int32_t const dataSockFd{pDebugger->socketServer_.WaitConnect()};
        if (0 > dataSockFd) {
            std::cerr << "sock.Wait4Connect()" << std::endl;
            return -1;
        }

        int32_t flags{fcntl(dataSockFd, F_GETFD)};
        if (flags != -1) {
            flags |= FD_CLOEXEC;
            if (fcntl(dataSockFd, F_SETFD, flags) < 0) {
                std::cout << "Failed to set FD_CLOEXEC !!!" << std::endl;
            }
        }

        static bool s_HasClientConnected{false};
        if (s_HasClientConnected) {
            std::cout << "New Connection arrived, close the previous connection !!!" << std::endl;
            if (nai_stream_close(&pDebugger->dataSockStream_) < 0) {
                std::cerr << "Failed to close previous connection !!!" << std::endl;
            }
        }

        if (0 != pDebugger->_OpenNaiStream(pDebugger->dataSockStream_, dataSockFd, &NaiDataEventHandler)) {
            std::cerr << "ERROR on InitNaiStream()" << std::endl;
            return -1;
        }
        // only for QAC
        if (nullptr == pDebugger->dataSockStream_.cb) {
        }
        s_HasClientConnected = true;
        std::cout << "Debugger: client connected." << std::endl;

#if 0 // < Remove multi-connection restriction to facilitate reconnection after network loss, close listening socket stream to prevent multiple IDE connections
        if (nai_stream_close(&pDebugger->listenSockStream_) != 0) {
            std::cerr << "ERROR on InitNaiStream()" << std::endl;
            return -1;
        }
#endif
    }  ///< if (events & NAI_EV_READ).

 // Write event
    if (NAI_EV_WRITE == (events & NAI_EV_WRITE)) {
        ;
    }

    return 0;
}

/// @brief Data event handler function
/// @param stream Data stream
/// @param events Events
/// @return Processing result
int32_t Debugger::NaiDataEventHandler(nai_stream_t *const stream, int32_t const events) noexcept
{
    if (nullptr == stream) {
        std::cerr << "NaiDataEventHandler(): nullptr == stream" << std::endl;
        return -1;
    }

    // get object from stream
    Debugger *const pDebugger{nai_containof(stream, Debugger, dataSockStream_)};  // NOLINT

 // Error event
    if (NAI_EV_ERROR == (events & NAI_EV_ERROR)) {
        return GetNaiErrno();
    }

 // Read event
    if (NAI_EV_READ == (events & NAI_EV_READ)) {
        std::ignore = pDebugger->_ParseRequest();
    }  // if (events & NAI_EV_READ).

 // Write event
    if (NAI_EV_WRITE == (events & NAI_EV_WRITE)) {
        ;
    }

    return 0;
}

/// @brief Parse request
/// @return 0 success; <0 failure
int32_t Debugger::_ParseRequest() noexcept
{
    static constexpr std::size_t kReservedSize{8192U};
    static constexpr uint16_t kIncreasedSize{1024U};
    static ara::core::Vector< Char8_t > s_Buffer(kReservedSize);
 static std::size_t s_Count{0U}; ///< Record the number of bytes already read
 static std::size_t s_ContentSize{0U}; ///< Record message body size
 static bool s_IsReadingContent{false}; ///< Whether reading content part

 // Continuously read until empty, because a single event may receive many packets
    while (true) {
 while (true) { ///< Replace goto operation
 /// If the content part has not been completely received, skip the header part
            if (s_IsReadingContent) {
                break;
            }
 /// Read packet header
            Char8_t *headerPos{s_Buffer.data()};  // PRQA S ALL
            while (true) {
                if (s_Count >= DCP::Header::GetHeaderSize()) {
                    break;
                }
                int32_t const r{static_cast< int32_t >(
                    nai_stream_read(&dataSockStream_, s_Buffer.data(), DCP::Header::GetHeaderSize() - s_Count))};
                if (0 > r) {
                    if (NAI_EAGAIN != GetNaiErrno()) {
                        std::cerr << "ParseRequest(): Read dcp header error !!!" << std::endl;
                        s_Count = 0U;
                        return GetNaiErrno();
                    }
                    std::cout << "ParseRequest(): Waiting for the next dcp header !!!" << std::endl;
                    return 0;
                }

 /// Record the read information
                headerPos += r;
                s_Count += static_cast< uint32_t >(r);
            }

            DCP::Header *const dcpHeader{reinterpret_cast< DCP::Header * >(s_Buffer.data())};  // PRQA S 3049
            if (!dcpHeader->Validate()) {
                s_Count = 0U;
                std::cerr << "ParseRequest(): true != dcpHeader->Validate() !!!" << std::endl;
                return -1;
            }

            s_ContentSize = dcpHeader->GetContentSize();
            if (s_ContentSize <= 0U) {
                s_Count = 0U;
                std::cerr << "ParseRequest(): ContentSize <= 0 !!!" << std::endl;
                return -1;
            }

 /// Adjust buffer size based on the read packet header information
            if (s_Buffer.size() < dcpHeader->GetSize()) {
 ///FIXME: When buffer.resize is set to FrameSize, subsequent operations will crash
                s_Buffer.resize(dcpHeader->GetSize() + kIncreasedSize);
            }

            s_Count            = 0U;
 s_IsReadingContent = true; ///< Start reading message body
            break;
        }

 /// Read packet content
        Char8_t *contentPos{s_Buffer.data() + DCP::Header::GetHeaderSize() + s_Count};  // PRQA S 4151
        while (true) {
            if (s_Count >= s_ContentSize) {
                break;
            }
            int32_t const r{
                static_cast< int32_t >(nai_stream_read(&dataSockStream_, contentPos, s_ContentSize - s_Count))};
            if (0 > r) {
                if (NAI_EAGAIN != GetNaiErrno()) {
                    s_Count            = 0U;
                    s_IsReadingContent = false;
                    std::cout << "ParseRequest(): Read dcp body error !!!" << std::endl;
                    return GetNaiErrno();
                }
                std::cout << "ParseRequest(): Waiting for the remain dcp body (" << s_ContentSize << " - " << s_Count
                          << ") !!!" << std::endl;
                return 0;
            }

            if (r == 0) {
                std::cout << "ParseRequest(): No content to read !!!" << std::endl;
                return 0;
            }

            contentPos += r;
            s_Count += static_cast< uint32_t >(r);
        }

 /// After message body reading is complete, reset state information
        s_Count            = 0U;
        s_ContentSize      = 0U;
        s_IsReadingContent = false;

 /// Parse packet content
        DCP dcp{s_Buffer};
        DCP::Type const dcpType{dcp.GetType()};
        switch (dcpType) {
            case DCP::Type::kTraceRequest: {
                ara::core::Vector< ara::core::String > tracedProcessList;
                std::ignore = dcp.GetValue4TraceRequest(tracedProcessList);
                dcp.Debug();
                if (requestCb_) {
                    requestCb_(tracedProcessList);
                }
                break;
            }
            case DCP::Type::kTraceCancel: {
                ara::core::Vector< ara::core::String > canceledProcessList;
                std::ignore = dcp.GetValue4TraceCancel(canceledProcessList);
                dcp.Debug();
                if (cancelCb_) {
                    cancelCb_(canceledProcessList);
                }
                break;
            }
            case DCP::Type::kRequestFunctionGroup: {
                ara::core::String fgName;
                if (!dcp.GetValue4RequestFunctionGroup(fgName)) {
                    std::cerr << "ParseRequest(): GetValue4RequestFunctionGroup error !!!" << std::endl;
                    return -1;
                }

                if (getFunctionGroupInfoCb_) {
                    FunctionGroupInfo fgInfo;
                    if (!getFunctionGroupInfoCb_(fgName, fgInfo)) {
                        std::cerr << "ParseRequest(): GetFunctionGroupInfo( " << fgName << " ) failed !!!" << std::endl;
                    }
                    std::ignore = dcp.Pack4ReponseFunctionGroup(fgInfo);
                    dcp.Debug();
                    std::size_t const buffSize{s_Buffer.size()};
                    if (0 >= nai_stream_send(&dataSockStream_, static_cast< void * >(s_Buffer.data()), buffSize, 0)) {
                        std::cerr << "ParseRequest(): Send fgInfo error !!!" << std::endl;
                        return -1;
                    }
                }
                break;
            }
            case DCP::Type::kRequestProcess: {
                ara::core::String procName;
                if (!dcp.GetValue4RequestProcess(procName)) {
                    std::cerr << "ParseRequest(): GetValue4RequestProcess error !!!" << std::endl;
                    return -1;
                }
                if (getProcessInfoCb_) {
                    ProcessInfo procInfo;
                    if (!getProcessInfoCb_(procName, procInfo)) {
                        std::cerr << "ParseRequest(): true != getProcessInfoCb_(" << procName << ")" << std::endl;
                    }
                    std::ignore = dcp.Pack4ResponseProcess(procInfo);
                    dcp.Debug();
                    std::size_t const buffSize{s_Buffer.size()};
                    if (0 >= nai_stream_send(&dataSockStream_, static_cast< void * >(s_Buffer.data()), buffSize, 0)) {
                        std::cerr << "ParseRequest(): Send procInfo error !!!" << std::endl;
                        return -1;
                    }
                }
                break;
            }
            default: {
                dcp.Debug();
                break;
            }
        }
    }

    return 0;
}

/// @brief Initialize data stream
/// @return 0 success; <0 failure
int32_t Debugger::_InitNaiStream() noexcept
{
 /// Initialize NAI receiver stream
    if (0 != nai_stream_init(&listenSockStream_)) {
        return -1;
    }

 /// Initialize NAI receiver stream
    if (0 != nai_stream_init(&dataSockStream_)) {
        return -1;
    }

    return 0;
}

/// @brief Open NAI stream handle
/// @param stream Stream handle reference
/// @param fd File descriptor
/// @param cb Callback function
/// @return 0 success; <0 failure
int32_t Debugger::_OpenNaiStream(nai_stream_t &stream, int32_t const fd, nai_iobase_cb_f const cb) noexcept
{
 /// Set file descriptor for stream object
    if (0 > nai_stream_set_fd(&stream, fd, NAI_FD_TYPE_SOCK)) {
        return -1;
    }

 // Set the file descriptor to be owned by the stream object, so that fd can be automatically closed when the stream closes
    if (0 > nai_stream_set_fdown(&stream, 1)) {
        return -1;
    }

 /// Set event callback function
    if (0 > nai_stream_set_cb(&stream, cb)) {
        return -1;
    }

 /// Open receive stream, start receiving event loop
    if (0 > nai_stream_open(&stream, mainLoop_->GetRawEvLoop())) {
        return -1;
    }

    return 0;
}

/// @brief Open debugger
/// @param mainLoop Main loop
/// @param serverPort Service port
/// @return Open result
int32_t Debugger::Open(std::shared_ptr< isoft::naicpp::EvLoop > mainLoop, uint16_t const serverPort) noexcept
{
    if (mainLoop == nullptr) {
        std::cerr << "nullptr == mainLoop" << std::endl;
        return -1;
    }

    if (0 != socketServer_.Open(serverPort)) {
        perror("sock.Open()");
        return -1;
    }

    if (0 != _InitNaiStream()) {
        return -1;
    }

    mainLoop_ = std::move(mainLoop);
    if (0 != _OpenNaiStream(listenSockStream_, socketServer_.GetFd(), &NaiListenEventHandler)) {
        mainLoop_ = nullptr;
        return -1;
    }

    return 0;
}

/// @brief Close debugger
/// @return 0 success; <0 failure
int32_t Debugger::Close() noexcept
{
    std::ignore = nai_stream_close(&dataSockStream_);
    std::ignore = nai_stream_close(&listenSockStream_);
    std::ignore = socketServer_.Close();
    mainLoop_   = nullptr;

    return 0;
}

/// @brief Send function group state change notification
/// @param fgInfo Function group information
void Debugger::NotifyFunctionGroupStateChanged(FunctionGroupInfo const &fgInfo) noexcept
{
    ara::core::Vector< Char8_t > buffer;
    DCP dcp{buffer};

    if (!IsServiceOn()) {
        return;
    }

    if (!nai_stream_is_opened(&dataSockStream_)) {  // PRQA S 3124
        return;
    }

    std::ignore = dcp.Pack4ReponseFunctionGroup(fgInfo);
    dcp.Debug();
    std::size_t const buffSize{buffer.size()};
    if (0 >= nai_stream_send(&dataSockStream_, static_cast< void * >(buffer.data()), buffSize, 0)) {
        /// LOG
        return;
    }
}

/// @brief Send process state change notification
/// @param procInfo Process information
void Debugger::NotifyProcessStateChanged(ProcessInfo const &procInfo) noexcept
{
    ara::core::Vector< Char8_t > buffer;
    DCP dcp{buffer};

    if (!IsServiceOn()) {
        return;
    }

    if (!nai_stream_is_opened(&dataSockStream_)) {  // PRQA S 3124
        return;
    }

    std::ignore = dcp.Pack4ResponseProcess(procInfo);
    dcp.Debug();
    size_t const buffSize{buffer.size()};
    if (0 >= nai_stream_send(&dataSockStream_, static_cast< void * >(buffer.data()), buffSize, 0)) {
        /// LOG
        return;
    }
}

/// @brief Send process trace ready notification
/// @param procInfo Process information
void Debugger::NotifyProcessTraceReady(ProcessInfo const &procInfo) noexcept
{
    ara::core::Vector< Char8_t > buffer;
    DCP dcp{buffer};

    if (!IsServiceOn()) {
        return;
    }

    if (!nai_stream_is_opened(&dataSockStream_)) {  // PRQA S 3124
        return;
    }

    std::ignore = dcp.Pack4TraceResponse(procInfo);
    dcp.Debug();
    std::size_t const buffSize{buffer.size()};
    if (0 >= nai_stream_send(&dataSockStream_, static_cast< void * >(buffer.data()), buffSize, 0)) {
        /// LOG
        return;
    }
}

}  // namespace rds
}  // namespace internal
}  // namespace exec
}  // namespace ara
