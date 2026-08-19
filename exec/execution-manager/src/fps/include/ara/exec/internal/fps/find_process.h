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
/// @file       find_process.h
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

#ifndef ARA_EXEC_INTERNAL_FIND_PROCESS_H_
#define ARA_EXEC_INTERNAL_FIND_PROCESS_H_

#include <isoft/core/list.h>

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <functional>
#include <mutex>
#include <shared_mutex>
#include <type_traits>

#include "ara/core/string.h"

namespace ara {
namespace exec {
namespace internal {
namespace fps {

/// @brief Process finder
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_EM_10008
/// @trace_id_ad=AD_EM_00096
/// @trace_id_dd=DD_EM_00302
/// @needwork = ad
/// @endcode
class FindProcess  // PRQA S 5215
{
public:
    /// @brief char type redefinition
    using Char8_t = char;

    /// @brief Process information node, saves all information of a single process
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_EM_10008
    /// @trace_id_ad=AD_EM_00096
    /// @trace_id_dd=DD_EM_00303
    /// @needwork = dda
    /// @endcode
    class InfoNode
    {
    public:
        /// @brief Constructor
        /// @param name Process FQN
        /// @code{.isoft}
        /// @interface_level=none
        /// @trace_id_ad=AD_EM_00096
        /// @trace_id_dd=DD_EM_00788
        /// @needwork = dda
        /// @endcode
        explicit InfoNode(ara::core::StringView const &name) noexcept;

        /// @brief Get the process name
        /// @return Process name
        /// @code{.isoft}
        /// @interface_level=none
        /// @trace_id_ad=AD_EM_00096
        /// @trace_id_dd=DD_EM_00789
        /// @needwork = dda
        /// @endcode
        ara::core::StringView GetName() const noexcept
        {
            return ara::core::StringView{static_cast< Char8_t const * >(name_), static_cast< std::size_t >(nameLen_)};
        }

        /// @brief Get the node size
        /// @return Bytes
        /// @code{.isoft}
        /// @interface_level=none
        /// @trace_id_ad=AD_EM_00096
        /// @trace_id_dd=DD_EM_00790
        /// @needwork = dda
        /// @endcode
        uint16_t GetSize() const noexcept;

        /// @brief Calculate the node size based on the process name
        /// @param name Process FQN
        /// @return Bytes
        /// @code{.isoft}
        /// @interface_level=none
        /// @trace_id_ad=AD_EM_00096
        /// @trace_id_dd=DD_EM_00791
        /// @needwork = dda
        /// @endcode
        static uint16_t CalculateNodeSize(ara::core::StringView const &name) noexcept;

    private:
        /// @brief Total length of the name
        /// @code{.isoft}
        /// @interface_level=none
        /// @trace_id_ad=AD_EM_00096
        /// @trace_id_dd=DD_EM_00792
        /// @needwork = dda
        /// @endcode
        uint16_t nameLen_;
        /// @brief Process FQN (Full Qualified Name)
        /// @code{.isoft}
        /// @interface_level=none
        /// @trace_id_ad=AD_EM_00096
        /// @trace_id_dd=DD_EM_00793
        /// @needwork = dda
        /// @endcode
        Char8_t name_[1];  // PRQA S 4151
    };

    /// @brief Index table, placed at the beginning of the shared memory, convenient for quickly locating the Node
    /// @note  This is intended to keep as many hits as possible within one cache line, preventing performance loss caused by TLB flushing
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_EM_10008
    /// @trace_id_ad=AD_EM_00096
    /// @trace_id_dd=DD_EM_00304
    /// @needwork = dda
    /// @endcode
    struct InfoTable
    {
        /// @brief Process pid
        /// @code{.isoft}
        /// @interface_level=none
        /// @trace_id_ad=AD_EM_00096
        /// @trace_id_dd=DD_EM_00794
        /// @needwork = dda
        /// @endcode
        uint32_t pid;
        /// @brief Process object address, convenient for execution management to find the corresponding process
        /// @code{.isoft}
        /// @interface_level=none
        /// @trace_id_ad=AD_EM_00096
        /// @trace_id_dd=DD_EM_00795
        /// @needwork = dda
        /// @endcode
        void *proc;
        /// @brief Process node offset address
        /// @code{.isoft}
        /// @interface_level=none
        /// @trace_id_ad=AD_EM_00096
        /// @trace_id_dd=DD_EM_00796
        /// @needwork = dda
        /// @endcode
        uint32_t offset;
    };

    /// @brief Shared memory header information
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_EM_10008
    /// @trace_id_ad=AD_EM_00096
    /// @trace_id_dd=DD_EM_00796
    /// @needwork = dda
    /// @endcode
    struct Header
    {
        /// @brief The first four bytes describe the size of the shared memory. If the client detects that the size is inconsistent with the local cache, it indicates a change, and remapping is required
        /// @code{.isoft}
        /// @interface_level=none
        /// @trace_id_ad=AD_EM_00096
        /// @trace_id_dd=DD_EM_00797
        /// @needwork = dda
        /// @endcode
        uint32_t memSize;
        /// @brief Number of processes
        /// @code{.isoft}
        /// @interface_level=none
        /// @trace_id_ad=AD_EM_00096
        /// @trace_id_dd=DD_EM_00798
        /// @needwork = dda
        /// @endcode
        uint32_t procNumber;
        /// @brief Node offset position, relative to the start of the shared memory
        /// @code{.isoft}
        /// @interface_level=none
        /// @trace_id_ad=AD_EM_00096
        /// @trace_id_dd=DD_EM_00799
        /// @needwork = dda
        /// @endcode
        uint32_t nodeHeadOffset;
        /// @brief Table start position
        /// @code{.isoft}
        /// @interface_level=none
        /// @trace_id_ad=AD_EM_00096
        /// @trace_id_dd=DD_EM_00800
        /// @needwork = dda
        /// @endcode
        InfoTable tables[1U];
    };

public:
    /// @brief Add a process for the FindProcess object, must be called before Create/Update
    /// @tparam T Process type, must be a type inherited from ara::exec::Process
    /// @param name Process FQN
    /// @param proc Process instance address
    /// @exception Exception safety guarantee, no exception thrown
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00096
    /// @trace_id_dd=DD_EM_00305
    /// @needwork = dda
    /// @endcode
    template < typename T >
    void AddProcess(ara::core::String const &name, T *const proc) noexcept(noexcept(ara::core::String(name)))
    {
        if (name.empty() || (nullptr == proc)) {
            return;
        }

        std::unique_lock< std::shared_timed_mutex > const rwLock{rwMutex_};
        for (auto &ln : procList_) {
            if (name == ln.name) {
                ln.proc = proc;
                return;
            }
        }
        ListNode ln{static_cast< void * >(proc), name};
        std::ignore = procList_.emplace(procList_.cbegin(), ln);
    }

    /// @brief Delete a process from the FindProcess object, deletion will take effect after Create
    /// @param name Process FQN
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00096
    /// @trace_id_dd=DD_EM_00306
    /// @needwork = dda
    /// @endcode
    void DelProcess(ara::core::String const &name) noexcept
    {
        std::unique_lock< std::shared_timed_mutex > const rwLock{rwMutex_};
        procList_.remove_if([&name](ListNode const &node) noexcept { return node.name == name; });
    }

    /// @brief Clear the process list
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00096
    /// @trace_id_dd=DD_EM_00307
    /// @needwork = dda
    /// @endcode
    void ClearProcess() noexcept
    {
        std::unique_lock< std::shared_timed_mutex > const rwLock{rwMutex_};
        procList_.clear();
    }

    /// @brief Create the finder, this function will create based on the processes added by AddProcess
    /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00096
    /// @trace_id_dd=DD_EM_00308
    /// @needwork = dda
    /// @endcode
    int32_t Create() noexcept;

    /// @brief Update the finder, recalculate the process list and open new shared memory, this function will update based on the processes added by AddProcess
    /// @tparam ProcessType Process type, must be a type inherited from ara::exec::Process
    /// @return 0 success; <0 failure
    /// @exception Exception safety guarantee, no exception thrown
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00096
    /// @trace_id_dd=DD_EM_00309
    /// @needwork = dda
    /// @endcode
    template < typename ProcessType >
    int32_t Update()
    {
        std::ignore = Close();

        /// Recalculate the shared memory size
        if (Create() < 0) {
            return -1;
        }

        /// Update the process PID in the finder, because all process PIDs (including existing ones) will be cleared after Create
        _EachFor([this](void *const p, uint32_t) noexcept {
            if (p != nullptr) {
                ProcessType *const proc{reinterpret_cast< ProcessType * >(p)};  // PRQA S 3049
                SetPid(p, static_cast< uint32_t >(proc->GetPid()));
            }
        });

        return 0;  // Return success.
    }

    /// @brief Destroy the finder
    /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00096
    /// @trace_id_dd=DD_EM_00310
    /// @needwork = dda
    /// @endcode
    int32_t Destroy() noexcept { return _SharedMemoryDestroy(); }

    /// @brief Open the finder for read access, with no write permissions
    /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00096
    /// @trace_id_dd=DD_EM_00311
    /// @needwork = dda
    /// @endcode
    int32_t Open() noexcept { return _ShareMemoryOpen(); }

    /// @brief Close the finder, only closes the current process's access entry, does not destroy
    /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00096
    /// @trace_id_dd=DD_EM_00312
    /// @needwork = dda
    /// @endcode
    int32_t Close() noexcept { return _SharedMemoryClose(); }

    /// @brief Update the PID
    /// @tparam T Process type, must be a type inherited from ara::exec::Process
    /// @param proc Process address
    /// @param pid PID
    /// @exception Exception safety guarantee, no exception thrown
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00096
    /// @trace_id_dd=DD_EM_00313
    /// @needwork = dda
    /// @endcode
    template < typename T >
    void SetPid(T const *const proc, uint32_t const pid)
    {
        InfoTable *const t{_FindByProc(proc)};
        if (t == nullptr) {
            return;
        }
        std::unique_lock< std::shared_timed_mutex > const rwLock{rwMutex_};
        t->pid = pid;
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
    int32_t GetNameByPid(uint32_t const pid, ara::core::String &name) noexcept;

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
    int32_t GetPidByName(ara::core::String const &name, uint32_t &pid) noexcept;

    /// @brief Debug, print all process information
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00096
    /// @trace_id_dd=DD_EM_00316
    /// @needwork = dda
    /// @endcode
    void Debug() const noexcept;

private:
    /// @brief Traverse the process list
    /// @param cb Callback function, called once for each process found
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00096
    /// @trace_id_dd=DD_EM_00317
    /// @needwork = dda
    /// @endcode
    void _EachFor(std::function< void(void *proc, uint32_t pid) > const &cb) const noexcept
    {
        if ((nullptr == cb) || (nullptr == mem_)) {
            return;
        }
        Header *const h{reinterpret_cast< Header * >(mem_)};  // PRQA S 3049
        for (uint32_t i{0U}; i < h->procNumber; i++) {
            InfoTable &t{h->tables[i]};  // NOLINT
            cb(t.proc, t.pid);
        }
    };

    /// @brief Find the process node by pid
    /// @param pid Process PID
    /// @return !nullptr process information node; nullptr not found
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00096
    /// @trace_id_dd=DD_EM_00318
    /// @needwork = dda
    /// @endcode
    InfoTable *_FindByPid(uint32_t const pid) noexcept;

    /// @brief Find the process index table entry by process object address
    /// @param proc Process address
    /// @return !nullptr process information node; nullptr not found
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00096
    /// @trace_id_dd=DD_EM_00319
    /// @needwork = dda
    /// @endcode
    InfoTable *_FindByProc(void const *const proc) noexcept;

    /// @brief Find the process index table entry by process object address
    /// @tparam T Process type, must be a type inherited from ara::exec::Process
    /// @param proc Process address
    /// @return !nullptr process information node; nullptr not found
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00096
    /// @trace_id_dd=DD_EM_00320
    /// @needwork = dda
    /// @endcode
    template < typename T >
    InfoTable *_FindByProc(T const *const proc) noexcept
    {
        return _FindByProc(static_cast< const void * >(proc));
    }

    /// @brief Get the node from the index table
    /// @param t Index table address
    /// @return !nullptr process information node; nullptr not found
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00096
    /// @trace_id_dd=DD_EM_00321
    /// @needwork = dda
    /// @endcode
    InfoNode *_Table2Node(InfoTable const *const t) const noexcept
    {
        if ((nullptr == t) || (nullptr == mem_)) {
            return nullptr;
        }
        return reinterpret_cast< InfoNode * >(mem_ + t->offset);  // PRQA S 3049
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
    int32_t _ShareMemoryCreate(uint32_t const size) noexcept;

    /// @brief Destroy shared memory
    /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00096
    /// @trace_id_dd=DD_EM_00323
    /// @needwork = dda
    /// @endcode
    int32_t _SharedMemoryDestroy() noexcept;

    /// @brief Open shared memory
    /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00096
    /// @trace_id_dd=DD_EM_00324
    /// @needwork = dda
    /// @endcode
    int32_t _ShareMemoryOpen() noexcept;

    /// @brief Close shared memory
    /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00096
    /// @trace_id_dd=DD_EM_00325
    /// @needwork = dda
    /// @endcode
    int32_t _SharedMemoryClose() noexcept;

    /// @brief Remap shared memory
    /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00096
    /// @trace_id_dd=DD_EM_00326
    /// @needwork = dda
    /// @endcode
    int32_t _ShareMemoryRemap() noexcept;

private:
    /// @brief Get the shared memory file
    /// @return Shared memory file
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    static constexpr Char8_t const *GetSharedMemoryFile() noexcept { return "ara_exec_internal_find_process_shm"; }

    /// @brief Process list node
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_EM_10008
    /// @trace_id_ad=AD_EM_00096
    /// @trace_id_dd=DD_EM_00327
    /// @needwork = dda
    /// @endcode
    struct ListNode
    {
        /// @brief Process address
        void *proc;
        /// @brief Process FQN
        ara::core::String name;
    };

    /// @brief Process list, used to create shared memory
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00096
    /// @trace_id_dd=DD_EM_00328
    /// @needwork = dda
    /// @endcode
    isoft::core::List< ListNode > procList_;

    /// @brief Shared memory address
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00096
    /// @trace_id_dd=DD_EM_00329
    /// @needwork = dda
    /// @endcode
    uint8_t *mem_;

    /// @brief Shared memory size
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00096
    /// @trace_id_dd=DD_EM_00330
    /// @needwork = dda
    /// @endcode
    uint32_t memSize_;

    /// @brief Read-write lock, prevents data inconsistency caused by multi-threaded operations
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00096
    /// @trace_id_dd=DD_EM_00331
    /// @needwork = dda
    /// @endcode
    std::shared_timed_mutex rwMutex_;
};  ///< class FindProcess

}  // namespace fps
}  // namespace internal
}  // namespace exec
}  // namespace ara
#endif  ///< ARA_EXEC_INTERNAL_FIND_PROCESS_H_
