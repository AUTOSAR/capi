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
/// @file       semaphore_lock.cpp
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
/// @unit_name = semaphore_lock
/// @unit_description=Dlt lib's implemented log sinker, for locking when writing to files
/// @endcode
///
/// ================================================================

#include "semaphore_lock.h"

#include <fcntl.h>
#include <semaphore.h>

#include <cassert>
#include <cerrno>
#include <chrono>
#include <cstring>
#include <iostream>
#include <string>
#include <thread>

#include "Utils/src/private_log.h"
namespace ara {
namespace log {
namespace internal {
/// @brief
/// @return

SemaphoreLock::~SemaphoreLock() noexcept { Destroy(); }

std::int32_t SemaphoreLock::Init(std::string const &pathName) noexcept
{
    if (this->initOk_) {
        return 0;
    }
    std::string fullSemName{this->semName_ + pathName};
    LOGVERBOSE("enter :SemaphoreLock::init ") << ",semName_ " << fullSemName;
    std::int32_t const maskInt{0644};
#if 0
    semMutex_ = sem_open(semName_.c_str(), O_RDONLY);
    if (semMutex_ == SEM_FAILED) {
          LOGVERBOSE("SemaphoreLock::init ") << " : semMutex_ == SEM_FAILED   ";
        first_ = true;
        semMutex_ = sem_open(semName_.c_str(), O_CREAT | O_EXCL, maskInt, 1);
    } else {
        LOGVERBOSE("SemaphoreLock::init ") << " : sem_open   ";
        sem_close(semMutex_);
        semMutex_ = sem_open(semName_.c_str(), O_CREAT | O_RDWR, maskInt, 1);
    }
#else

    semMutex_ = sem_open(fullSemName.c_str(), O_CREAT | O_RDWR | O_EXCL, maskInt, 1);
    if (semMutex_ == SEM_FAILED) {
        std::int32_t const localError{errno};
        if (localError == EEXIST) {
            semMutex_ = sem_open(fullSemName.c_str(), O_RDWR);
            if (semMutex_ == SEM_FAILED) {
                LOGERROR("SemaphoreLock::init open existing sem failed: ") << semName_ << ", errno: " << errno;
                return -1;
            }
            first_ = false;
        } else {
            // Unexpected failure: failed to create a new semaphore (not EEXIST)
            LOGERROR("SemaphoreLock::init create new sem failed: ")
                << semName_ << ", errno: " << localError << " (" << std::strerror(localError) << ")";
            return -1;
        }
    } else {
        first_ = true;
    }
#endif

    if (semMutex_ == SEM_FAILED) {
        LOGVERBOSE("SemaphoreLock::init ") << fullSemName << " sem_open failed";
        return -1;
    }

    initOk_        = true;
    this->semName_ = fullSemName;
    LOGVERBOSE("SemaphoreLock::init ") << "first: " << this->first_ << fullSemName
                                       << " value: " << this->CurrentValue();
    return 0;
}
/// @brief
void SemaphoreLock::Lock() noexcept
{
    if (initOk_) {
        std::int32_t value = this->CurrentValue();
        LOGVERBOSE("SemaphoreLock::Lock before lock value= ") << value;
        this->_wait();
        value = this->CurrentValue();
        LOGVERBOSE("SemaphoreLock::Lock after lock value= ") << value;
    }
}

/// @brief
void SemaphoreLock::UnLock() noexcept
{
    if (initOk_) {
        std::int32_t value = this->CurrentValue();
        LOGVERBOSE("SemaphoreLock::UnLock before lock value= ") << value;
        this->_post();
        value = this->CurrentValue();
        LOGVERBOSE("SemaphoreLock::UnLock after lock value= ") << value;
    }
}
/// @brief
void SemaphoreLock::Destroy() noexcept
{
    std::ignore = LOGVERBOSE(" SemaphoreLock::Destroy");
    if (initOk_) {
        if (sem_close(semMutex_) == -1) {
            LOGERROR("SemaphoreLock::Destroy sem_close fail, errno=") << errno;
        }
        if (first_) {
            if (sem_unlink(semName_.c_str()) == -1) {
                LOGERROR("SemaphoreLock::Destroy sem_unlink fail, errno=") << errno;
            }
        }
    } else {
        LOGERROR("SemaphoreLock:: init error : ") << semName_ << __func__;
    }
    semMutex_ = SEM_FAILED;  // Mark the handle as closed
    initOk_   = false;
}
/// @brief
void SemaphoreLock::_wait() noexcept
{
    if (this->initOk_ == false) {
        LOGERROR("SemaphoreLock::_wait not init : ") << semName_ << __func__;
        return;
    }
    if (sem_wait(semMutex_) == -1) {
        LOGERROR("SemaphoreLock::_wait sem_wait fail, errno=") << errno;
    }
}
/// @brief
void SemaphoreLock::_post() noexcept
{
    if (this->initOk_ == false) {
        LOGERROR("SemaphoreLock::_post not init : ") << semName_ << __func__;
        return;
    }
    if (sem_post(semMutex_) == -1) {
        LOGERROR("SemaphoreLock::_post sem_post fail, errno=") << errno;
    }
}

std::int32_t SemaphoreLock::CurrentValue() noexcept
{
    if (this->initOk_ == false) {
        LOGERROR("SemaphoreLock::CurrentValue not init : ") << semName_ << __func__;
        return -1;
    }
    std::int32_t value{-1};
    if (sem_getvalue(semMutex_, &value) == -1) {
        LOGVERBOSE("SemaphoreLock::value ") << " : sem_getvalue error";
    }
    return value;
}

}  // namespace internal
}  // namespace log
}  // namespace ara
