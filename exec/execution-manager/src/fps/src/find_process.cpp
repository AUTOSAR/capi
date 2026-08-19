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
/// @file       find_process.cpp
/// @brief      Find process class definition
/// @details
/// @date       2023-04-01
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/ExecutionManagement/FPS
/// @unit_name=FindProcess
/// @unit_description=Used to find and manage processes in the execution management system.
/// @interface_level=module
/// @endcode
///
/// ================================================================

#include "ara/exec/internal/fps/find_process.h"

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include "ara/exec/internal/log/log.h"
#include "isoft/ara_fsh/filesystem_hierarchy.h"
#include "isoft/utils/error.h"

#ifdef LOG
    #undef LOG
#endif

/// @brief Process find client log macro definition
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
#define LOG() ara::exec::internal::log1::Log< FpLogInfo >()

namespace ara {
namespace exec {
namespace internal {
namespace fps {
namespace {

/// @brief FindProcess server log macro definition
class FpLogInfo
{
public:
    /// @brief char type redefinition
    using Char8_t = char;

    /// @brief Get the log context ID
    /// @return Log context ID
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    static constexpr Char8_t const *GetLogCtxId() noexcept { return "FPS_"; }

    /// @brief Get the log context description
    /// @return Log context description
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    static constexpr Char8_t const *GetLogCtxDesc() noexcept { return "The Find Process Service"; }
};

}  // namespace

/// @brief Constructor
/// @param name Process FQN
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_ad=AD_EM_00096
/// @trace_id_dd=DD_EM_00788
/// @needwork = dda
/// @endcode
FindProcess::InfoNode::InfoNode(ara::core::StringView const &name) noexcept
    : nameLen_{static_cast< uint16_t >(name.size())}, name_{'\0'}
{
    std::ignore = memmove(static_cast< Char8_t * >(name_), name.data(), static_cast< size_t >(nameLen_));
}

/// @brief Get the node size
/// @return Bytes
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_ad=AD_EM_00096
/// @trace_id_dd=DD_EM_00790
/// @needwork = dda
/// @endcode
uint16_t FindProcess::InfoNode::GetSize() const noexcept
{
    uint16_t const trueSize{static_cast< uint16_t >(sizeof(nameLen_) + nameLen_)};
    if (trueSize > sizeof(FindProcess::InfoNode)) {
        return trueSize;
    }

    return sizeof(FindProcess::InfoNode);
}

/// @brief Calculate the node size based on the process name
/// @param name Process FQN
/// @return Bytes
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_ad=AD_EM_00096
/// @trace_id_dd=DD_EM_00791
/// @needwork = dda
/// @endcode
uint16_t FindProcess::InfoNode::CalculateNodeSize(ara::core::StringView const &name) noexcept
{
    std::size_t const nameLen{name.size()};
    std::size_t const trueSize{sizeof(nameLen_) + nameLen};
    if (trueSize > sizeof(FindProcess::InfoNode)) {
        return static_cast< uint16_t >(trueSize);
    }

    return sizeof(FindProcess::InfoNode);
}

/// @brief Create the finder, this function will create based on the processes added by AddProcess
/// @return 0 success; <0 failure
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_EM_00096
/// @trace_id_dd=DD_EM_00308
/// @needwork = dda
/// @endcode
int32_t FindProcess::Create() noexcept
{
    if (procList_.empty()) {
        LOGE() << "procList_ is empty, can not create FindProcess";
        return -1;
    }

    /// Calculate the shared memory size
    rwMutex_.lock_shared();
    std::size_t const nodeHeadOffset{sizeof(Header::memSize) + sizeof(Header::procNumber)
                                     + sizeof(Header::nodeHeadOffset) + sizeof(InfoTable) * procList_.size()};
    uint32_t memSize{static_cast< uint32_t >(nodeHeadOffset)};
    for (auto &p : procList_) {
        memSize += InfoNode::CalculateNodeSize(p.name);
    }
    rwMutex_.unlock_shared();

    if (0 != _ShareMemoryCreate(memSize)) {
        return -1;
    }

    std::unique_lock< std::shared_timed_mutex > const rwLock{rwMutex_};
    Header *const h{reinterpret_cast< Header * >(mem_)};
    h->memSize        = memSize;
    h->procNumber     = static_cast< uint32_t >(procList_.size());
    h->nodeHeadOffset = static_cast< uint32_t >(nodeHeadOffset);

    int32_t tableIndex{0};
    /// Pointer to the node to be written
    uint32_t offset{static_cast< uint32_t >(nodeHeadOffset)};
    for (auto &p : procList_) {
        InfoTable &t{h->tables[tableIndex]};  // NOLINT
        tableIndex++;
        /// Fill the table
        t.pid    = 0U;
        t.proc   = p.proc;
        t.offset = offset;
        /// Fill the node
        InfoNode *const node{new (mem_ + offset) InfoNode(p.name)};

        /// Update the offset
        offset += node->GetSize();
    }

    // Do not clear, considering that it may be needed during reload
    return 0;
}

/// @brief Create shared memory
/// @param size Bytes
/// @return 0 success; <0 failure
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_ad=AD_EM_00096
/// @trace_id_dd=DD_EM_00322
/// @needwork = dda
/// @endcode
int32_t FindProcess::_ShareMemoryCreate(uint32_t const size) noexcept
{
    std::unique_lock< std::shared_timed_mutex > const rwLock{rwMutex_};

    int32_t shmfd{0};
    int32_t const openFlag{O_CREAT | O_RDWR};  // PRQA S 3600
    int32_t const protFlag{PROT_READ | PROT_WRITE};

    memSize_ = size;
    ara::core::String const shmFile{ara::core::String(GetSharedMemoryFile()) + "_"
                                    + isoft::ara_fsh::Platform().GetSysrootMd5Str().c_str()};

    shmfd = shm_open(shmFile.c_str(), openFlag, S_IRUSR | S_IWUSR);  // PRQA S 3143,3600
    if (shmfd < 0) {
        LOGE() << "shm_open(" << shmFile << "): " << isoft::utils::StrError();  // NOLINT
        return -1;
    }

    if (ftruncate(shmfd, static_cast< off_t >(memSize_)) < 0) {
        LOGE() << "ftruncate(): " << isoft::utils::StrError();  // NOLINT
        std::ignore = close(shmfd);
        return -1;
    }

    mem_
        = static_cast< uint8_t * >(mmap(nullptr, static_cast< std::size_t >(memSize_), protFlag, MAP_SHARED, shmfd, 0));
    if (mem_ == MAP_FAILED) {  // NOLINT
        LOGE() << "mmap(): " << isoft::utils::StrError();
        std::ignore = close(shmfd);
        return -1;
    }

    if (close(shmfd) != 0) {
        LOGE() << "close(): " << isoft::utils::StrError();
        return -1;
    }

    return 0;
}

/// @brief Destroy shared memory
/// @return 0 success; <0 failure
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_ad=AD_EM_00096
/// @trace_id_dd=DD_EM_00323
/// @needwork = dda
/// @endcode
int32_t FindProcess::_SharedMemoryDestroy() noexcept
{
    int32_t r{0};

    if (0 != _SharedMemoryClose()) {
        r = -1;
    }

    ara::core::String shmFile{GetSharedMemoryFile() + ara::core::String{"_"}};
    isoft::ara_fsh::Platform const fsh;
    shmFile += fsh.GetSysrootMd5Str().c_str();

    std::unique_lock< std::shared_timed_mutex > const rwLock{rwMutex_};
    if (0 != shm_unlink(shmFile.c_str())) {
        if (ENOENT != isoft::utils::GetErrNo()) {
            LOGE() << "shm_unlink(" << shmFile << "): " << isoft::utils::StrError();
            r = -1;
        }
    }
    return r;
}

/// @brief Open shared memory
/// @return 0 success; <0 failure
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_ad=AD_EM_00096
/// @trace_id_dd=DD_EM_00324
/// @needwork = dda
/// @endcode
int32_t FindProcess::_ShareMemoryOpen() noexcept
{
    std::unique_lock< std::shared_timed_mutex > const rwLock{rwMutex_};
    int32_t shmfd{-1};
    int32_t const openFlag{O_RDWR};
    int32_t const protFlag{PROT_READ | PROT_WRITE};

    ara::core::String shmFile{GetSharedMemoryFile() + ara::core::String{"_"}};
    isoft::ara_fsh::Platform const fsh;
    shmFile += fsh.GetSysrootMd5Str().c_str();
    shmfd = shm_open(shmFile.c_str(), openFlag, S_IRUSR);
    if (-1 == shmfd) {
        LOGE() << "shm_open(" << shmFile << "): " << isoft::utils::StrError();
        return -1;
    }

    /// GetMemSize
    struct stat stBuf
    {
    };
    if (0 > fstat(shmfd, &stBuf)) {
        LOGE() << "fstate(): " << isoft::utils::StrError();
        return -1;
    }

    memSize_ = static_cast< uint32_t >(stBuf.st_size);

    mem_ = static_cast< uint8_t * >(mmap(nullptr, static_cast< size_t >(memSize_), protFlag, MAP_SHARED, shmfd, 0));
    if (mem_ == MAP_FAILED) {  // NOLINT
        LOGE() << "mmap()";
        return -1;
    }

    if (0 != close(shmfd)) {
        LOGE() << "close(): " << isoft::utils::StrError();
        return -1;
    }

    return 0;
}

/// @brief Close shared memory
/// @return 0 success; <0 failure
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_ad=AD_EM_00096
/// @trace_id_dd=DD_EM_00325
/// @needwork = dda
/// @endcode
int32_t FindProcess::_SharedMemoryClose() noexcept
{
    std::unique_lock< std::shared_timed_mutex > const rwLock{rwMutex_};
    if (nullptr == mem_) {
        return 0;
    }

    if (0 > munmap(mem_, static_cast< std::size_t >(memSize_))) {
        LOGE() << "munmap(): " << isoft::utils::StrError();
        return -1;
    }
    mem_     = nullptr;
    memSize_ = 0U;

    return 0;
}

/// @brief Remap shared memory
/// @return 0 success; <0 failure
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_ad=AD_EM_00096
/// @trace_id_dd=DD_EM_00326
/// @needwork = dda
/// @endcode
int32_t FindProcess::_ShareMemoryRemap() noexcept
{
    if (0 != _SharedMemoryClose()) {
        return -1;
    }
    if (0 != _ShareMemoryOpen()) {
        return -1;
    }
    return 0;
}

/// @brief Find the process node by pid
/// @param pid Process PID
/// @return !nullptr process information node; nullptr not found
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_ad=AD_EM_00096
/// @trace_id_dd=DD_EM_00318
/// @needwork = dda
/// @endcode
FindProcess::InfoTable *FindProcess::_FindByPid(uint32_t const pid) noexcept
{
    if ((nullptr == mem_) || (0U == memSize_)) {
        return nullptr;
    }

    Header *const h{reinterpret_cast< Header * >(mem_)};
    if (h->memSize != memSize_) {
        if (0 != _ShareMemoryRemap()) {
            return nullptr;
        }
    }

    std::shared_lock< std::shared_timed_mutex > const lock{rwMutex_};
    for (uint16_t i{0U}; i < h->procNumber; i++) {
        InfoTable &t{h->tables[static_cast< uint32_t >(i)]};  // NOLINT
        if (t.pid == pid) {
            return &t;
        }
    }
    return nullptr;
}

/// @brief Find the process index table entry by process object address
/// @param proc Process address
/// @return !nullptr process information node; nullptr not found
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_ad=AD_EM_00096
/// @trace_id_dd=DD_EM_00319
/// @needwork = dda
/// @endcode
FindProcess::InfoTable *FindProcess::_FindByProc(void const *const proc) noexcept
{
    if ((nullptr == mem_) || (0U == memSize_) || (nullptr == proc)) {
        return nullptr;
    }

    Header *const h{reinterpret_cast< Header * >(mem_)};
    if (h->memSize != memSize_) {
        if (0 != _ShareMemoryRemap()) {
            return nullptr;
        }
    }

    std::shared_lock< std::shared_timed_mutex > const rwLock{rwMutex_};
    for (uint32_t i{0U}; i < h->procNumber; i++) {
        InfoTable &t{h->tables[i]};  // NOLINT
        if (t.proc == proc) {
            return &t;
        }
    }
    return nullptr;
}

/// @brief Get the process name
/// @param pid Process PID
/// @param name Process FQN
/// @return 0 success; <0 failure
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_EM_00096
/// @trace_id_dd=DD_EM_00314
/// @needwork = dda
/// @endcode
int32_t FindProcess::GetNameByPid(uint32_t const pid, ara::core::String &name) noexcept
{
    if ((pid == 0U) || (nullptr == mem_) || (0U == memSize_)) {
        return -1;
    }

    InfoTable *const t{_FindByPid(pid)};
    if (nullptr == t) {
        return -1;
    }

    std::shared_lock< std::shared_timed_mutex > const rwLock{rwMutex_};
    InfoNode *const node{_Table2Node(t)};
    if (nullptr == node) {
        return -1;
    }
    name = node->GetName();
    return 0;
}

/// @brief Get the pid by process name
/// @param name Process FQN
/// @param pid Process PID
/// @return 0 success; <0 failure
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_EM_00096
/// @trace_id_dd=DD_EM_00315
/// @needwork = dda
/// @endcode
int32_t FindProcess::GetPidByName(ara::core::String const &name, uint32_t &pid) noexcept
{
    std::ignore = pid;
    if (name.empty() || (nullptr == mem_) || (0U == memSize_)) {
        return -1;
    }

    Header *const h{reinterpret_cast< Header * >(mem_)};
    if (h->memSize != memSize_) {
        if (0 != _ShareMemoryRemap()) {
            return -1;
        }
    }

    std::shared_lock< std::shared_timed_mutex > const lock{rwMutex_};
    for (uint32_t i{0U}; i < h->procNumber; i++) {
        InfoTable const &t{h->tables[i]};  // NOLINT
        InfoNode *const in{_Table2Node(&t)};
        if (in->GetName() == name) {
            pid = t.pid;
            return 0;
        }
    }

    return -1;
}

/// @brief Debug, print all process information
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_EM_00096
/// @trace_id_dd=DD_EM_00316
/// @needwork = dda
/// @endcode
void FindProcess::Debug() const noexcept
{
#if ARA_EXEC_DEBUG
    std::cout << "+++ FindProcess::Debug() +++" << std::endl;
    std::cout << "PID            NAME" << std::endl;
    if ((nullptr == mem_) || (0U == memSize_)) {
        return;
    }

    Header *const h{reinterpret_cast< Header * >(mem_)};
    for (uint32_t i{0U}; i < h->procNumber; i++) {
        InfoTable const &tab{h->tables[i]};  // NOLINT
        InfoTable const *const t{&tab};
        InfoNode const *const n{_Table2Node(t)};
        std::cout << tab.pid << "\t" << n->GetName() << std::endl;
    }
    std::cout << "--- FindProcess::Debug() ---" << std::endl;
#endif
}

}  // namespace fps
}  // namespace internal
}  // namespace exec
}  // namespace ara
