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
/// @file       sharedmemory.cpp
/// @brief      OSI inter-process communication shared memory class
/// @details
/// @date       2023-01-09
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#include "isoft/osi/ipc/sharedmemory.h"

#include "ara/tsync/internal/common.h"

namespace isoft {
namespace osi {
namespace tsipc {

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

/// @brief create and map shared memory
/// @param shmName - shared memory file name
/// @param shmSize - shared memory size
/// @return 0 - success
/// @return <0 - failure, and set errno
std::int32_t SharedMemory::Create(ara::tsync::internal::char8_t const *const shmName,
                                  std::size_t const shmSize) noexcept
{
    /// @name shmfd - shared memory file descriptor
    std::int32_t shmfd{0};
    /// @name openFlag - shared memory creation option, create | read/write | error if exists
    std::int32_t const openFlag{O_CREAT | O_RDWR | O_EXCL};
    /// @name portFlag - shared memory mapping option, readable | writable
    std::int32_t const protFlag{PROT_READ | PROT_WRITE};
    /// @name return value
    std::int32_t retCode{0};

    shmName_ = shmName;
    shmSize_ = static_cast< std::uint32_t >(shmSize);

    /// instead of goto
    while (true) {
        if (0 != shm_unlink(shmName_.c_str())) {
            if (ENOENT != ara::tsync::internal::GetErrNo()) {
                retCode = ara::tsync::internal::kRET_E5;
                break;
            }
        }

        shmfd = shm_open(shmName, openFlag, kTS_SHARE_MEM__MODE);
        if (-1 == shmfd) {
            retCode = ara::tsync::internal::kRET_E1;
            break;
        }

        if (0 > ftruncate(shmfd, static_cast< off_t >(shmSize_))) {
            retCode = ara::tsync::internal::kRET_E2;
            break;
        }

        shmAddr_ = static_cast< std::uint8_t * >(
            mmap(nullptr, static_cast< std::size_t >(shmSize_), protFlag, MAP_SHARED, shmfd, 0));
        /// TODO(zhoubo): integer to pointer conversion is not recommended
        if (reinterpret_cast< void * >(-1) == shmAddr_) {  //NOLINT
            retCode = ara::tsync::internal::kRET_E3;
            break;
        }

        if (0 != close(shmfd)) {
            retCode = ara::tsync::internal::kRET_E4;
            break;
        }

        /// instead of goto
        break;
    }  /// while(true)

    return retCode;
}

/// @brief unmap and destroy shared memory
/// @return 0 - success
/// @return <0 - failure, and set errno
std::int32_t SharedMemory::Destroy() noexcept
{
    /// @name return value
    std::int32_t retCode{0};

    if (nullptr == shmAddr_) {
        return 0;
    }

    if (0 != Unmap()) {
        retCode = ara::tsync::internal::kRET_E1;
    }

    if (0 != shm_unlink(shmName_.c_str())) {
        if (ENOENT != ara::tsync::internal::GetErrNo()) {
            retCode = ara::tsync::internal::kRET_E2;
        }
    }

    return retCode;
}

/// @brief map an existing shared memory
/// @param shmName - shared memory file name
/// @return 0 - success
/// @return <0 - failure, and set errno
std::int32_t SharedMemory::Map(ara::tsync::internal::char8_t const *const shmName) noexcept
{
    /// @name shmfd - shared memory file descriptor
    std::int32_t shmfd{0};
    /// @name openFlag - shared memory open option, read/write
    std::int32_t const openFlag{O_RDWR};
    /// @name portFlag - shared memory mapping option, readable | writable
    std::int32_t const protFlag{PROT_READ | PROT_WRITE};
    /// @name return value
    std::int32_t retCode{0};

    shmName_ = shmName;

    /// instead of goto
    while (true) {
        shmfd = shm_open(shmName, openFlag, kTS_SHARE_MEM__MODE);
        if (-1 == shmfd) {
            retCode = ara::tsync::internal::kRET_E1;
            break;
        }

        /// GetMemSize
        struct stat stBuf
        {
        };
        if (0 > fstat(shmfd, &stBuf)) {
            retCode = ara::tsync::internal::kRET_E2;
            break;
        }

        shmSize_ = static_cast< std::uint32_t >(stBuf.st_size);

        shmAddr_ = static_cast< std::uint8_t * >(
            mmap(nullptr, static_cast< std::size_t >(shmSize_), protFlag, MAP_SHARED, shmfd, 0));
        /// TODO(zhoubo): integer to pointer conversion is not recommended
        if (reinterpret_cast< void * >(-1) == shmAddr_) {  //NOLINT
            retCode = ara::tsync::internal::kRET_E3;
            break;
        }

        if (0 != close(shmfd)) {
            retCode = ara::tsync::internal::kRET_E4;
            break;
        }
        /// instead of goto
        break;
    }  /// while(true)

    return retCode;
}

/// @brief unmap the currently mapped shared memory
/// @return 0 - success
/// @return <0 - failure, and set errno
std::int32_t SharedMemory::Unmap() noexcept
{
    /// @name return value
    std::int32_t retCode{0};

    if ((nullptr != shmAddr_) && (shmSize_ > 0U)) {
        if (0 > munmap(shmAddr_, static_cast< std::size_t >(shmSize_))) {
            retCode = ara::tsync::internal::kRET_E1;
        }
    }

    shmAddr_ = nullptr;
    shmSize_ = 0U;

    return retCode;
}

/// @brief remap a previously mapped shared memory; the memory size may change
/// @return 0 - success
/// @return <0 - failure, and set errno
std::int32_t SharedMemory::Remap() noexcept
{
    /// @name return value
    std::int32_t retCode{0};

    /// instead of goto
    while (true) {
        if (0 != Unmap()) {
            retCode = ara::tsync::internal::kRET_E1;
            break;
        }
        if (0 != Map(shmName_.c_str())) {
            retCode = ara::tsync::internal::kRET_E2;
            break;
        }
        /// instead of goto
        break;
    }  /// while(true)

    return retCode;
}

}  // namespace tsipc
}  // namespace osi
}  // namespace isoft
