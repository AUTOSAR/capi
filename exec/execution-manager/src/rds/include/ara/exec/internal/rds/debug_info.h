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
/// @file       debug_info.h
/// @brief      Debugger information definition file
/// @details
/// @date       2023-11-17
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#ifndef _ARA_EXEC_INTERNAL_DEBUG_INFO_H_
#define _ARA_EXEC_INTERNAL_DEBUG_INFO_H_

#include <ara/core/string.h>
#include <ara/core/string_view.h>
#include <isoft/core/list.h>

#include <cstdint>

namespace ara {
namespace exec {
namespace internal {
namespace rds {

/// @brief Process information
class ProcessInfo  // PRQA S 5215
{
public:
 /// @brief Default constructor
 /// @exception std::bad_alloc Thrown when memory allocation fails
    ProcessInfo() = default;

 /// @brief Default destructor
    ~ProcessInfo() noexcept = default;

 /// @brief Custom constructor
 /// @param name Process name
 /// @param state Process state
 /// @param pid Process ID
 /// @param exitCode Process exit code
    ProcessInfo(ara::core::StringView const& name,
                ara::core::StringView const& state,
                int32_t const pid,
                int32_t const exitCode) noexcept
        : name_{name}, state_{state}, pid_{pid}, exitCode_{exitCode}
    {
    }

 /// @brief Default copy constructor
 /// @param other Process information object
    ProcessInfo(ProcessInfo const& other) = default;

 /// @brief Default move constructor
 /// @param other Process information object
    ProcessInfo(ProcessInfo&& other) = default;

 /// @brief Default copy assignment
 /// @param other Process information object
 /// @return New ProcessInfo object
    ProcessInfo& operator=(ProcessInfo const& other) = default;

 /// @brief Default move assignment
 /// @param other Process information object
 /// @return New ProcessInfo object
    ProcessInfo& operator=(ProcessInfo&& other) = default;

 /// @brief Set process name
 /// @param procName Process name
    void SetName(ara::core::StringView const& procName) noexcept { name_ = procName; }

 /// @brief Get process name
 /// @return Process name
    ara::core::String const& GetName() const noexcept { return name_; }

 /// @brief Set process state
 /// @param state Process state
    void SetState(ara::core::StringView const& state) noexcept { state_ = state; }

 /// @brief Get process state
 /// @return Process state
    ara::core::String const& GetState() const noexcept { return state_; }

 /// @brief Set process ID
 /// @param pid Process ID
    void SetPid(int32_t const pid) noexcept { pid_ = pid; }

 /// @brief Get process ID
 /// @return Process ID
    int32_t GetPid() const noexcept { return pid_; }

 /// @brief Set process exit code
 /// @param exitCode Exit code
    void SetExitCode(int32_t const exitCode) noexcept { exitCode_ = exitCode; }

 /// @brief Get process exit code
 /// @return Process exit code
    int32_t GetExitCode() const noexcept { return exitCode_; }

private:
 /// @brief Process name
    ara::core::String name_;
 /// @brief Process state
    ara::core::String state_;
 /// @brief Process ID
    int32_t pid_{-1};
 /// @brief Process exit code
    int32_t exitCode_{-1};
};

/// @brief Function group information
class FunctionGroupInfo
{
public:
 /// @brief Default constructor
    /// @exception std::bad_alloc
    FunctionGroupInfo() = default;

 /// @brief Default destructor
    ~FunctionGroupInfo() = default;

 /// @brief Custom constructor
 /// @param name Function group name
 /// @param state Function group state
    /// @exception std::bad_alloc
    FunctionGroupInfo(ara::core::StringView const& name, ara::core::StringView const& state)
        : name_{name}, state_{state}, processList_{}
    {
    }

 /// @brief Default copy constructor
 /// @param other Function group information object
    FunctionGroupInfo(FunctionGroupInfo const& other) = default;

 /// @brief Default move constructor
 /// @param other Function group information object
    FunctionGroupInfo(FunctionGroupInfo&& other) = default;

 /// @brief Default copy assignment
 /// @param other Function group information object
 /// @return New FunctionGroupInfo object
    FunctionGroupInfo& operator=(FunctionGroupInfo const& other) = default;

 /// @brief Default move assignment
 /// @param other Function group information object
 /// @return New FunctionGroupInfo object
    FunctionGroupInfo& operator=(FunctionGroupInfo&& other) = default;

 /// @brief Set function group name
 /// @param fgName Function group name
    void SetName(ara::core::StringView const& fgName) noexcept { name_ = fgName; }

 /// @brief Get function group name
 /// @return Function group name
    ara::core::String const& GetName() const noexcept { return name_; }

 /// @brief Set function group state
 /// @param fgState Function group state
    void SetState(ara::core::StringView const& fgState) noexcept { state_ = fgState; }

 /// @brief Get function group state
 /// @return Function group state
    ara::core::String const& GetState() const noexcept { return state_; }

 /// @brief Add process information to function group
 /// @param procName Process name
 /// @param procState Process state
 /// @param pid Process ID
 /// @param exitCode Process exit code
    void AddProcess(ara::core::StringView const& procName,
                    ara::core::StringView const& procState,
                    int32_t const pid,
                    int32_t const exitCode) noexcept
    {
        processList_.emplace_back(ProcessInfo{procName, procState, pid, exitCode});
    }

 /// @brief Add process information to function group
 /// @param procInfo Process information
    void AddProcess(ProcessInfo const& procInfo) noexcept { processList_.emplace_back(procInfo); }

 /// @brief Get process list
 /// @return Process list
    isoft::core::List< ProcessInfo > const& GetProcessList() const& noexcept { return processList_; }

private:
 /// @brief Function group name
    ara::core::String name_;
 /// @brief Function group state
    ara::core::String state_;
 /// @brief Process list
    isoft::core::List< ProcessInfo > processList_;
};

}  // namespace rds
}  // namespace internal
}  // namespace exec
}  // namespace ara

#endif  ///< _ARA_EXEC_INTERNAL_DEBUG_INFO_H_