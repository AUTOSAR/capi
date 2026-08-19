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
/// @file       log_trans.cpp
/// @brief      Template class for encapsulating log parameters
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
/// @unit_name = log_trans
/// @unit_description=Backend of Dlt module, used to support log and command forwarding.
/// @endcode
///
/// ================================================================

#include "log_trans.h"

#include <arpa/inet.h>
#include <fcntl.h>
#include <isoft/ara_fsh/platform.h>
#include <sys/epoll.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <thread>

#include "Utils/src/private_log.h"

namespace ara {
namespace log {
namespace internal {
constexpr int32_t kInt32_1000{1000};
/// @brief g_Do_Fifio_Reading Whether to stop reading from fifo
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_LOG_00278
/// @trace_id_dd=DD_LOG_01693
/// @needwork = dda
/// @endcode
static bool g_Do_Fifio_Reading{true};
/// @brief File descriptor of the opened fifo
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_LOG_00275
/// @trace_id_dd=DD_LOG_01705
/// @needwork = dda
/// @endcode
static std::int32_t g_Fifo_Fd{-1};

/// @brief g_Fifo_Read_Thread
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_LOG_00275
/// @trace_id_dd=DD_LOG_01706
/// @needwork = dda
/// @endcode
std::unique_ptr< std::thread > g_Fifo_Read_Thread{nullptr};

std::int32_t LogTrans::Init(Listener* const lis) noexcept
{
    LOGVERBOSE(__func__) << " enter ";
    std::int32_t ret{-1};
    mListener_ = lis;
    // std::string const fullPath{this->mInternalPipeNameLogModule_ + isoft::ara_fsh::Platform().GetSysrootMd5Str()};

    std::string const fullPath{this->mInternalPipeNameLogModule_};

    if (access(fullPath.c_str(), F_OK) == 0) {
        std::ignore = unlink(fullPath.c_str());
    }
    std::uint32_t const maskInt{0666U};
    ret = mkfifo(fullPath.c_str(), maskInt);
    if (ret < 0) {
        LOGERROR("backend   mkfifo  ") << fullPath;
        return ret;
    }

    ret = open(fullPath.c_str(), O_RDONLY | O_NONBLOCK | O_CLOEXEC);

    if (ret == -1) {
        if (errno == ENXIO) {
            LOGERROR("No writer connected!");
        } else {
            LOGERROR("open failed");
        }
        return ret;
    }

    LOGERROR("backend pip open ok: ") << fullPath;
    g_Fifo_Fd = ret;

    // int flags = fcntl(g_Fifo_Fd, F_GETFL);
    // fcntl(g_Fifo_Fd, F_SETFL, flags & ~O_NONBLOCK);  // Clear O_NONBLOCK

    if (fcntl(g_Fifo_Fd, F_SETPIPE_SZ, kFIFO_PIPE_SIZE) == -1) {
        LOGERROR("fcntl F_SETPIPE_SZ failed : ") << __func__;
    }
    if (g_Fifo_Read_Thread == nullptr) {
        g_Fifo_Read_Thread = std::make_unique< std::thread >(&LogTrans::_StartListenForAALog, this);
    }
    LOGVERBOSE(__func__) << " leave  ";
    return g_Fifo_Fd;
}
/// @brief
/// @return
std::int32_t LogTrans::Destroy() noexcept
{
    LOGVERBOSE(__func__) << " LogTrans enter ";
    g_Do_Fifio_Reading = false;

    bool const joinOK{g_Fifo_Read_Thread->joinable()};
    if (g_Fifo_Read_Thread != nullptr && joinOK) {
        LOGVERBOSE(__func__) << " waiting for reading end  ";
        g_Fifo_Read_Thread->join();
    }
    LOGVERBOSE(__func__) << " LogTrans end join ";

    g_Fifo_Read_Thread = nullptr;

    LOGVERBOSE(__func__) << " LogTrans leave  ";
    return 0;
}

bool LogTrans::SetStoping() noexcept
{
    LOGVERBOSE(__func__) << " LogTrans   ";
    g_Do_Fifio_Reading = false;
    return true;
}

bool LogTrans::StopWaiting() noexcept { return false; }

bool LogTrans::WaitSeconds(std::int32_t const& secs) noexcept
{
    LOGVERBOSE(__func__) << " LogTrans  enter ";
    std::int32_t flags = fcntl(g_Fifo_Fd, F_GETFL);
    if (flags == -1) {
        return false;  // Failed to get
    }
    flags |= O_NONBLOCK;
    std::int32_t ctlRet{fcntl(g_Fifo_Fd, F_SETFL, flags)};
    if (ctlRet < 0) {
        LOGERROR(__func__) << "set nonblocking failed";
    }
    LOGERROR(__func__) << "start waiting ";
    std::this_thread::sleep_for(std::chrono::milliseconds(kInt32_1000 * secs));
    LOGERROR(__func__) << "stop waiting ";
    ////  read until end
    std::unique_ptr< uint8_t[] > const tmpfifoBUffer{std::make_unique< uint8_t[] >(kFIFOBUFFERSIZE * 600)};
    std::int64_t const tempret{read(g_Fifo_Fd, tmpfifoBUffer.get(), kFIFOBUFFERSIZE * 100)};
    LOGERROR(__func__) << " no use message :  " << tempret;

    LOGVERBOSE(__func__) << " LogTrans  leave  ";
    return true;
}

bool LogTrans::ClosePipe() noexcept
{
    if (g_Fifo_Fd > 0) {
        std::ignore = close(g_Fifo_Fd);
        g_Fifo_Fd   = 0;
    }
    std::string const fullPath{this->mInternalPipeNameLogModule_};
    std::ignore = unlink(fullPath.c_str());
    return true;
}

/// TODO Remove thread, use underlying NAI
/// @brief
/// @return
std::int32_t LogTrans::_StartListenForAALog() noexcept
{
    LOGVERBOSE(__func__) << " enter ";

    while (g_Do_Fifio_Reading) {
        std::unique_ptr< uint8_t[] > const fifoBUffer{std::make_unique< uint8_t[] >(kFIFOBUFFERSIZE)};
        fd_set fds;
        FD_ZERO(&fds);  // NOLINT
        FD_SET(g_Fifo_Fd, &fds);
        struct timeval timeout
        {
        };
        timeout.tv_sec  = 1;
        timeout.tv_usec = 0;
        std::int32_t const ready{select(g_Fifo_Fd + 1, &fds, nullptr, nullptr, &timeout)};
        if (ready == -1) {
            LOGVERBOSE(__func__) << "ListenForAALog ready -1";
            break;
        }
        if (ready == 0) {
            LOGVERBOSE(__func__) << "ListenForAALog ready 0";
            continue;
        }
        if (g_Do_Fifio_Reading == true) {
            std::int64_t const ret{read(g_Fifo_Fd, fifoBUffer.get(), kFIFOBUFFERSIZE)};
            if (ret > 0) {
                if (mListener_ != nullptr && g_Do_Fifio_Reading == true) {
                    mListener_->OnLogBuffer(fifoBUffer.get(), ret);
                } else {
                    LOGVERBOSE(__func__) << "readbuffer no sending   false";
                }
            } else if (ret == 0) {
                LOGVERBOSE(__func__) << "read ret 0";
                continue;
            } else {
                LOGVERBOSE(__func__) << "read ret -1";
            }
        }

        if (g_Do_Fifio_Reading == false) {
            LOGVERBOSE(__func__) << "LogTrans g_Do_Fifio_Reading = false ";
            break;
        }
    }
    LOGVERBOSE(__func__) << "  LogTrans leave  ";
    return 0;
}

}  // namespace internal

}  // namespace log

}  // namespace ara
