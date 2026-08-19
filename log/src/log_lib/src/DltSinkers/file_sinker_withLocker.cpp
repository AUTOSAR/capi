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
/// @file       file_sinker_withLocker.cpp
/// @brief      Template class for encapsulating log parameters
/// @details
/// @date       2024-06-28
/// @author     yangjinbiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/LOG/DltSinkers
/// @interface_level = unit
/// @trace_id_sr=LOG_SR_00007
/// @unit_name = file_sinker_withLocker
/// @unit_description=Dlt lib's implemented log sinker for writing to files
/// @endcode
///
/// ================================================================

#include "file_sinker_withLocker.h"

#include <sys/mman.h>
#include <sys/time.h>

#include <cmath>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <list>
#include <stdexcept>
#include <string>
#include <tuple>
#include <vector>

#include "Utils/src/private_log.h"
#include "common.h"

namespace ara {
namespace log {
namespace internal {
using Char8_T = char;

/// @brief FileSinkerWithLocker
FileSinkerWithLocker::~FileSinkerWithLocker() noexcept
{
    if (this->inited_) {
        std::ignore = FlushBuffer();
        std::ignore = _destroy();
    }
}
/// @brief
/// @param[in]  ecuId
/// @param[in]  filename
/// @param[in]  singleFilesize
/// @param[in]  maxfileCount
/// @param[in]  buffersize
/// @return
bool FileSinkerWithLocker::Init(std::string const& ecuId,
                                std::string const& filename,
                                std::size_t const& singleFilesize,
                                std::size_t const& maxfileCount,
                                std::size_t const& buffersize) noexcept
{
    ecuId_          = ecuId;
    filename_       = filename;
    singleFileSize_ = singleFilesize;
    loopFileCount_  = maxfileCount;
    bufferSize_     = buffersize;

    std::size_t found{filename_.find_last_of('/')};
    if (std::string::npos != found) {
        logDir_ = filename_.substr(0U, found + 1U);
    } else {
        logDir_ = "./";
    }

    bool fileInit{fileSinkerImpl_.Init(ecuId, filename, singleFilesize, maxfileCount, buffersize)};
    if (fileInit == false) {
        LOGERROR(__func__) << ", open file failed ";
        return false;
    }

    std::string const firstSearch{"/"};
    std::string const secondSearch{"."};
    std::string const toReplace{"_"};
    std::string const firstStr{_replaceStr(filename_, firstSearch, toReplace)};
    std::string const uniqPathforOpen{_replaceStr(firstStr, secondSearch, toReplace)};
    LOGVERBOSE("uniqPathforOpen: ") << uniqPathforOpen;
    processLocker_ = std::make_unique< ProcessThreadLock >(uniqPathforOpen);

    if (processLocker_ == nullptr) {
        LOGERROR(__func__) << ", processLocker_ init failed";
        return false;
    }
    LOGVERBOSE(__func__) << ", processLocker_ init success ";

    if (ipc_.OpenForFile(uniqPathforOpen, logDir_) == false) {
        std::ignore = LOGVERBOSE("oepn file sinker shm failed");
        fileSinkerImpl_.CloseFile();
        return false;
    }

    inited_ = true;
    LOGVERBOSE(__func__) << ",FileSinkerWithLocker init success ";
    return true;
}
int32_t FileSinkerWithLocker::SaveLogWithTimeStamp(std::uint8_t* buffer1,
                                                   std::size_t const& len1,
                                                   std::uint8_t* buffer2,
                                                   std::size_t const& len2,
                                                   DltTimeStamp const& dltTimestamp) noexcept
{
    if (inited_ == false) {
        return -1;
    }
    _checkWithLocker();
    return fileSinkerImpl_.SaveLogWithTimeStampNocheck(buffer1, len1, buffer2, len2, dltTimestamp);
}

int32_t FileSinkerWithLocker::SavePlainTextLog(std::uint8_t* buffer1, std::size_t const& len1) noexcept
{
    if (inited_ == false) {
        return -1;
    }
    _checkWithLocker();
    return fileSinkerImpl_.SavePlainTextLogNocheck(buffer1, len1);
}

/// @brief
/// @return
bool FileSinkerWithLocker::FlushBuffer() noexcept
{
    if (inited_ == false) {
        LOGERROR(__func__) << ", not intied ";
        return false;
    }
    return fileSinkerImpl_.FlushBuffer();
}

/// @brief check
/// @return
bool FileSinkerWithLocker::_checkWithLocker() noexcept
{
    LOGVERBOSE(__func__) << ": enter ";
    if (inited_ == false) {
        LOGVERBOSE(__func__) << ": leave ";
        return false;
    }

    ScopedFLock guard(*processLocker_);
    if (ipc_.IsChanged()) {
        ipc_.SyncLocalFromShared();
        fileSinkerImpl_.CloseFile();
        fileSinkerImpl_.OpenFile();
    } else {
        bool changed{fileSinkerImpl_.CheckFileOverSize()};
        if (changed) {
            LOGVERBOSE(__func__) << ": leave after check file oversize ";
            ipc_.IncrementIndex();
        }
    }
    LOGVERBOSE(__func__) << ": leave";
    return true;
}

/// @brief
/// @return
int32_t FileSinkerWithLocker::_destroy() noexcept
{
    fileSinkerImpl_.CloseFile();
    ipc_.CloseShm();
    return 0;
}

/// @brief

}  // namespace internal
}  // namespace log

}  // namespace ara
