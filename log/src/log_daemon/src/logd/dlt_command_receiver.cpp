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
/// @file       dlt_command_receiver.cpp
/// @brief      size of receiver buffer
/// @details
/// @date       2024-06-28
/// @author     yangjinbiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/LOG/DltLogd
/// @interface_level = unit
/// @trace_id_sr=LOG_SR_00001,LOG_SR_00003,LOG_SR_00009
/// @unit_name = dlt_command_receiver
/// @unit_description=Backend module of Dlt for receiving client information types of log records sent from AA.
/// @endcode
///
/// ================================================================

#include "dlt_command_receiver.h"

#include <sys/socket.h>
#include <unistd.h>

#include <cerrno>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <string>

#include "Utils/src/private_log.h"
#include "log_lib/include/ara/log/internal/dlt_helpers.h"
namespace ara {
namespace log {
namespace internal {

/// @brief g_Dlt_Serial_Header g_Dlt_Serial_Header
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_LOG_00218
/// @trace_id_dd=DD_LOG_00132
/// @needwork = dd
/// @endcode
static Char8_T g_Dlt_Serial_Header[kDlt_Id_Size]{'D', 'L', 'S', 1};
DltCommandReceiver::DltCommandReceiver(DltCommandReceiver::Listener *const lsner) : mListener_{lsner}
{
    LOGVERBOSE(__func__) << " enter ";
}

/// @brief Destructor, free memory
DltCommandReceiver::~DltCommandReceiver() noexcept
{
    LOGVERBOSE(__func__) << " enter ";
    if (this->mBuffer_ != nullptr) {
        free(this->mBuffer_);
    }

    if (this->backupBuffer_ != nullptr) {
        free(this->backupBuffer_);
    }
    LOGVERBOSE(__func__) << " leave  ";
}

void DltCommandReceiver::Init(std::int32_t const &recvFd,
                              DltReceiverType const &recvType,
                              std::uint32_t const &bufferSize) noexcept
{
    LOGVERBOSE(__func__) << "enter";
    this->fd_   = recvFd;
    this->type_ = recvType;
    if ((nullptr != this->mBuffer_) && (bufferSize != this->mBufferSize_)) {
        free(this->mBuffer_);
        this->mBuffer_ = nullptr;
    }

    if (nullptr == this->mBuffer_) {
        this->lastBytesRcvd_  = 0U;
        this->bytesRcvd_      = 0U;
        this->totalBytesRcvd_ = 0U;
        this->buf_            = nullptr;
        this->backupBuffer_   = nullptr;
        this->mBuffer_        = static_cast< uint8_t * >(calloc(1U, static_cast< std::size_t >(bufferSize)));
        this->mBufferSize_    = bufferSize;
    }

    if (nullptr == this->mBuffer_) {
        return;
    }
    this->buf_ = this->mBuffer_;

    LOGVERBOSE(__func__) << " leave  ";
}

bool DltCommandReceiver::DataToBuffer()
{
    LOGVERBOSE(__func__) << " enter ";

    if (this->mBuffer_ == nullptr) {
        return false;
    }

    this->buf_           = static_cast< uint8_t * >(this->mBuffer_);
    this->lastBytesRcvd_ = this->bytesRcvd_;

    if ((this->lastBytesRcvd_ > 0U) && (this->backupBuffer_ != nullptr)) {
        std::ignore = memcpy(this->buf_, this->backupBuffer_, static_cast< size_t >(this->lastBytesRcvd_));
        free(this->backupBuffer_);
        this->backupBuffer_ = nullptr;
    }

    ssize_t recvBytes{0};
    if (this->type_ == DltReceiverType::kDlt_Receive_Socket) {
        recvBytes
            = recv(static_cast< std::int32_t >(this->fd_), static_cast< void * >(this->buf_ + this->lastBytesRcvd_),
                   static_cast< size_t >(this->mBufferSize_ - this->lastBytesRcvd_), 0);
    }
    if (this->type_ == DltReceiverType::kDlt_Receive_Fd) {
        recvBytes = read(this->fd_, this->buf_ + this->lastBytesRcvd_,
                         static_cast< size_t >(this->mBufferSize_ - this->lastBytesRcvd_));
    }

    if (recvBytes < 0) {
        if ((errno == EAGAIN) || (errno == EWOULDBLOCK) || (errno == EINTR)) {
            LOGVERBOSE(__func__) << " temporary read unavailable, errno: " << errno;
            return true;
        }
        LOGERROR(__func__) << " read failed, errno: " << errno << " " << strerror(errno);
        return false;
    }

    if (recvBytes == 0) {
        LOGVERBOSE(__func__) << " peer closed connection";
        return false;
    }

    this->bytesRcvd_ = static_cast< std::uint32_t >(recvBytes);

    this->totalBytesRcvd_ += this->bytesRcvd_;
    this->bytesRcvd_ += this->lastBytesRcvd_;

    std::uint32_t bytesToBeRemoved{0U};
    /// @brief
    std::uint8_t const dltTypeControl{0x03U};
    std::uint8_t const dltControlRequest{0x01U};
    while (this->MessageRead(&controlMsg_, this->buf_, this->bytesRcvd_, 0) == 0) {
        bool const isUEH{((controlMsg_.standardheader->htyp) & kDlt_Htyp_Ueh) != 0};
        bool const istypeControl{
            static_cast< std::uint8_t >(((controlMsg_.extendedheader->msin) & kDlt_Msin_Mstp) >> kDlt_Msin_Mstp_Shift)
            == dltTypeControl};
        bool const isRequest{(((controlMsg_.extendedheader->msin) & kDlt_Msin_Mtin) >> kDlt_Msin_Mtin_Shift)
                             == dltControlRequest};

        if (isUEH && istypeControl && isRequest) {
            std::ignore = LOGVERBOSE("is command ");
            if (mListener_ != nullptr) {
                std::ignore = LOGVERBOSE("call OnCommandMessage");
                mListener_->OnCommandMessage(this->fd_, &controlMsg_);
            }
        }

        bytesToBeRemoved = controlMsg_.headersize + controlMsg_.datasize;

        if (controlMsg_.foundSerialheader != 0) {
            bytesToBeRemoved += sizeof(g_Dlt_Serial_Header);
        }

        if (controlMsg_.resyncOffset != 0U) {
            bytesToBeRemoved += controlMsg_.resyncOffset;
        }

        if (this->Remove(bytesToBeRemoved) == -1) {
            LOGERROR(__func__) << " control message parse failed";
            return false;
        }
    }
    if (this->MoveToBegin() == -1) {
        LOGERROR(__func__) << " control message parse failed";
        return false;
    }
    LOGVERBOSE(__func__) << " leave  ";
    return true;
}

std::int32_t DltCommandReceiver::MessageRead(DltMessage *const msg,
                                             uint8_t const *buffer,
                                             std::uint32_t length,
                                             std::int32_t const &resync)
{
    LOGVERBOSE(__func__) << " enter ";
    msg->resyncOffset = 0U;

    if (length < kDlt_Id_Size) {
        return -1;
    }

    if (memcmp(buffer, static_cast< void * >(g_Dlt_Serial_Header), kDlt_Id_Size) == 0) {
        msg->foundSerialheader = 1;
        buffer += kDlt_Id_Size;
        length -= kDlt_Id_Size;
    } else {
        msg->foundSerialheader = 0;
        if (resync > 0) {
            msg->resyncOffset = 0U;
            do {
                if (memcmp(buffer + msg->resyncOffset, static_cast< void * >(g_Dlt_Serial_Header), kDlt_Id_Size) == 0) {
                    msg->foundSerialheader = 1;
                    buffer += kDlt_Id_Size;
                    length -= static_cast< std::uint32_t >(kDlt_Id_Size);
                    break;
                }
                msg->resyncOffset++;
            } while ((kDlt_Id_Size + msg->resyncOffset) <= length);
            if (msg->resyncOffset > 0U) {
                buffer += msg->resyncOffset;
                length -= static_cast< std::uint32_t >(msg->resyncOffset);
            }
        }
    }

    if (length < sizeof(DltStandardHeader)) {
        return -1;
    }

    std::ignore         = memcpy(static_cast< void * >(msg->headerbuffer), buffer, sizeof(DltStandardHeader));
    msg->standardheader = reinterpret_cast< DltStandardHeader * >(msg->headerbuffer);

    std::size_t weeidOffset{0U};

    if (DLT_IS_HTYP_UEH(msg->standardheader->htyp)) {
        weeidOffset = sizeof(DltExtendedHeader);
    }

    size_t const extraSize{
        static_cast< uint32_t >(DLT_STANDARD_HEADER_EXTRA_SIZE(msg->standardheader->htyp) + weeidOffset)};
    msg->headersize = static_cast< uint32_t >(sizeof(DltStandardHeader) + extraSize);
    msg->datasize   = static_cast< uint32_t >(be16toh(msg->standardheader->len) - msg->headersize);

    std::int32_t const tempDataSize{be16toh(msg->standardheader->len) - static_cast< int32_t >(msg->headersize)};

    if (tempDataSize < 0) {
        return -1;
    }
    msg->datasize = static_cast< std::uint32_t >(tempDataSize);

    /* load standard header extra parameters and Extended header if used */
    if (extraSize > 0U) {
        if (length < (msg->headersize)) {
            return -1;
        }

        std::ignore
            = memcpy(msg->headerbuffer + sizeof(DltStandardHeader), buffer + sizeof(DltStandardHeader), extraSize);

        /* set extended header ptr and get standard header extra parameters */
        if (DLT_IS_HTYP_UEH(msg->standardheader->htyp)) {
            msg->extendedheader
                = reinterpret_cast< DltExtendedHeader * >(msg->headerbuffer + sizeof(DltStandardHeader)
                                                          + DLT_STANDARD_HEADER_EXTRA_SIZE(msg->standardheader->htyp));
        } else {
            msg->extendedheader = nullptr;
        }

        // Get_Extraparameters(msg);--------------------------------------------

        if (DLT_IS_HTYP_WEID(msg->standardheader->htyp)) {
            std::ignore = memcpy(static_cast< void * >(msg->headerExtra.ecu),
                                 msg->headerbuffer + sizeof(DltStandardHeader), kDlt_Id_Size);
        }

        std::size_t weidOffset{0U};
        std::size_t wsidOffset{0U};
        if (DLT_IS_HTYP_WEID(msg->standardheader->htyp)) {
            weidOffset = kDlt_Size_Weid;
        }
        if (DLT_IS_HTYP_WSID(msg->standardheader->htyp)) {
            wsidOffset = kDlt_Size_Wsid;
        }

        if (DLT_IS_HTYP_WSID(msg->standardheader->htyp)) {
            std::ignore = memcpy(&(msg->headerExtra.seid), msg->headerbuffer + sizeof(DltStandardHeader) + weidOffset,
                                 kDlt_Size_Wsid);
            msg->headerExtra.seid = be32toh(msg->headerExtra.seid);
        }

        if (DLT_IS_HTYP_WTMS(msg->standardheader->htyp)) {
            std::ignore
                = memcpy(&(msg->headerExtra.tmsp),
                         msg->headerbuffer + sizeof(DltStandardHeader) + weidOffset + wsidOffset, kDlt_Size_Wtms);
            msg->headerExtra.tmsp = be32toh(msg->headerExtra.tmsp);
        }
    }

    if (length < (msg->headersize + msg->datasize)) {
        return -1;
    }

    if (msg->databuffer != nullptr) {
        if (msg->datasize > msg->databuffersize) {
            free(msg->databuffer);
            msg->databuffer     = static_cast< uint8_t * >(malloc(msg->datasize));
            msg->databuffersize = msg->datasize;
        }
    } else {
        msg->databuffer     = static_cast< uint8_t * >(malloc(msg->datasize));
        msg->databuffersize = msg->datasize;
    }

    if (msg->databuffer == nullptr) {
        return -1;
    }
    std::ignore = memcpy(msg->databuffer, buffer + (msg->headersize), msg->datasize);
    LOGVERBOSE(__func__) << " leave  ";
    return 0;
}

void DltCommandReceiver::Free() noexcept
{
    LOGVERBOSE(__func__) << " enter ";
    if (this->mBuffer_ != nullptr) {
        free(this->mBuffer_);
    }

    if (this->backupBuffer_ != nullptr) {
        free(this->backupBuffer_);
    }

    this->mBuffer_      = nullptr;
    this->buf_          = nullptr;
    this->backupBuffer_ = nullptr;
    LOGVERBOSE(__func__) << " leave  ";
}

std::int32_t DltCommandReceiver::Remove(std::uint32_t const &size) noexcept
{
    LOGVERBOSE(__func__) << " enter ";
    if ((size > this->bytesRcvd_) || (size <= 0U)) {
        this->buf_       = this->buf_ + this->bytesRcvd_;
        this->bytesRcvd_ = 0U;
        return -1;
    }

    this->bytesRcvd_ = this->bytesRcvd_ - size;
    this->buf_       = this->buf_ + size;
    LOGVERBOSE(__func__) << " leave  ";
    return 0;
}

std::int32_t DltCommandReceiver::MoveToBegin() noexcept
{
    LOGVERBOSE(__func__) << " enter ";
    if ((this->mBuffer_ == nullptr) || (this->buf_ == nullptr)) {
        return -1;
    }
    if ((this->mBuffer_ != this->buf_) && (this->bytesRcvd_ != 0U)) {
        this->backupBuffer_
            = static_cast< uint8_t * >(calloc(static_cast< std::size_t >(this->bytesRcvd_ + 1U), sizeof(Char8_T)));
        if (this->backupBuffer_ == nullptr) {
            // TOOD logverbose
            LOGERROR(__func__) << " backup buffer allocation failed";
        } else {
            std::ignore = memcpy(this->backupBuffer_, this->buf_, static_cast< std::size_t >(this->bytesRcvd_));
        }
    }
    LOGVERBOSE(__func__) << " leave  ";
    return 0;
}

}  // namespace internal
}  // namespace log
}  // namespace ara
