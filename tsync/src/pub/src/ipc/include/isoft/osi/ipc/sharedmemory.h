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
/// @file       sharedmemory.h
/// @brief      OSI inter-process communication shared memory class
/// @details
/// @date       2023-01-09
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#ifndef ISOFT_OSI_IPC_SHARED_MEMORY_H_
#define ISOFT_OSI_IPC_SHARED_MEMORY_H_

#include <cstdint>
#include <string>

#include "ara/tsync/internal/common.h"

namespace isoft {
namespace osi {
namespace tsipc {

/// @brief shared memory mode 0600U
static constexpr std::uint32_t kTS_SHARE_MEM__MODE{0X180U};

/// @brief shared memory operation class
class SharedMemory final
{
public:
    /// @brief create and map shared memory
    /// @param shmName - shared memory file name
    /// @param shmSize - shared memory size
    /// @return 0 - success
    /// @return <0 - failure, and set errno
    std::int32_t Create(ara::tsync::internal::char8_t const *const shmName, std::size_t const shmSize) noexcept;

    /// @brief unmap and destroy shared memory
    /// @return 0 - success
    /// @return <0 - failure, and set errno
    std::int32_t Destroy() noexcept;

    /// @brief map an existing shared memory
    /// @param shmName - shared memory file name
    /// @return 0 - success
    /// @return <0 - failure, and set errno
    std::int32_t Map(ara::tsync::internal::char8_t const *const shmName) noexcept;

    /// @brief unmap the currently mapped shared memory
    /// @return 0 - success
    /// @return <0 - failure, and set errno
    std::int32_t Unmap() noexcept;

    /// @brief remap a previously mapped shared memory; the memory size may change
    /// @return 0 - success
    /// @return <0 - failure, and set errno
    std::int32_t Remap() noexcept;

    /// @brief get shared memory address
    /// @returns shared memory address, nullptr indicates failure
    std::uint8_t *Get() const noexcept { return shmAddr_; }

    /// @brief get shared memory size
    /// @returns shared memory size in bytes
    std::uint32_t Size() const noexcept { return shmSize_; }

    /// @brief get the saved shared memory file name
    /// @returns file name, empty indicates not opened or mapped
    std::string const &Name() const noexcept { return shmName_; }

private:
    /// @name shmName_ - shared memory file name
    std::string shmName_;
    /// @name shmSize_ - shared memory size
    std::uint32_t shmSize_{0};
    /// @name shmAddr_ - shared memory mapped address
    std::uint8_t *shmAddr_{nullptr};
};

}  // namespace tsipc
}  // namespace osi
}  // namespace isoft

#endif  /// ISOFT_OSI_IPC_SHARED_MEMORY_H_
