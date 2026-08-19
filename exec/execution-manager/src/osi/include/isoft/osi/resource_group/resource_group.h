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
/// @file       resource_group.h
/// @brief      Resource group class definition
/// @details
/// @date       2025-09-19
/// @author     xueliang.bao
/// @version    1.2.0
///
/// ================================================================

#ifndef ISOFT_OSI_RESOURCE_GROUP_H_
#define ISOFT_OSI_RESOURCE_GROUP_H_

#include <cstdint>
#include <string>

namespace isoft {
namespace osi {
namespace resource_group {

/// @brief Resource group management class, can limit CPU utilization and physical memory usage in bytes
/// After CPU utilization is set successfully, the process cannot break the limit
/// MEM limit is the physical memory usage in bytes; once exceeded, the process will be blocked or killed, default is killing behavior
class ResourceGroup
{
public:
 /// @brief Prohibit default constructor
    ResourceGroup() = delete;

 /// @brief Resource group destructor
    ~ResourceGroup() = default;

 /// @brief Disable move construction
    /// @param other the other ResourceGroup
    ResourceGroup(ResourceGroup &&other) noexcept = delete;

 /// @brief Disable copy construction
    /// @param other the other ResourceGroup
    ResourceGroup(ResourceGroup const &other) noexcept = delete;

 /// @brief Disable move assignment
    /// @param other the other ResourceGroup
 /// @return New resource group instance
    ResourceGroup &operator=(ResourceGroup &&other) noexcept = delete;

 /// @brief Disable copy assignment
    /// @param other the other ResourceGroup
 /// @return New resource group instance
    ResourceGroup &operator=(ResourceGroup const &other) noexcept = delete;

 /// @brief Constructor
 /// @param rgName Resource group name
 /// @param cpuUsage Maximum CPU utilization, 0 means no limit
 /// @param memUsage Maximum physical memory usage in bytes, 0 means no limit.
    ResourceGroup(std::string rgName, uint8_t cpuUsage, uint32_t memUsage) noexcept
        : rgName_{std::move(rgName)}, cpuUsage_{cpuUsage}, memUsage_{memUsage}, isCgroupV2_{false}
    {
        _CheckCgroupVersion();
    }

 /// @brief Open resource group
 /// @return 0 success; <0 failure
    int32_t Open() noexcept;

 /// @brief Close resource group
 /// @return 0 success; <0 failure
    int32_t Close() noexcept { return _DestroyCgroup(); }

 /// @brief Add process to resource group
 /// @param pid Process PID
 /// @return 0 success; <0 failure
    int32_t AttachTask(pid_t const pid) noexcept;

 /// @brief Get resource group name
 /// @return Resource group name
    std::string const &GetName() const noexcept { return rgName_; }

private:
 /// @brief Check cgroup version
    void _CheckCgroupVersion() noexcept;

 /// @brief Enable cpu and memory control
 /// @return 0 enable success; <0 enable failure
    int32_t _EnableCpuMemoryController() const noexcept;

 /// @brief Create cgroup
 /// @return 0 create success; <0 create failure
    int32_t _CreateCgroup() const noexcept;

 /// @brief Configure cpu and memory limits
 /// @return 0 configure success; <0 configure failure
    int32_t _LimitCpuMemoryUsage() const noexcept;

 /// @brief Destroy cgroup
 /// @return 0 destroy success; <0 destroy failure
    int32_t _DestroyCgroup() const noexcept;

private:
 /// @brief Create V1 cgroup
 /// @return 0 create success; <0 create failure
    int32_t _CreateCgroupV1() const noexcept;

 /// @brief Create V2 cgroup
 /// @return 0 create success; <0 create failure
    int32_t _CreateCgroupV2() const noexcept;

 /// @brief Configure cpu utilization
 /// @return 0 configure success; <0 configure failure
    int32_t _LimitCpuUsageV1() const noexcept;

 /// @brief Configure cpu utilization
 /// @return 0 configure success; <0 configure failure
    int32_t _LimitCpuUsageV2() const noexcept;

 /// @brief Configure physical memory usage in bytes
 /// @return 0 configure success; <0 configure failure
    int32_t _LimitMemoryUsageV1() const noexcept;

 /// @brief Configure physical memory usage in bytes
 /// @return 0 configure success; <0 configure failure
    int32_t _LimitMemoryUsageV2() const noexcept;

 /// @brief Destroy V1 cgroup
 /// @return 0 destroy success; <0 destroy failure
    int32_t _DestroyCgroupV1() const noexcept;

 /// @brief Destroy V2 cgroup
 /// @return 0 destroy success; <0 destroy failure
    int32_t _DestroyCgroupV2() const noexcept;

private:
 /// @brief Resource group name
    std::string rgName_;

 /// @brief CPU utilization
    uint8_t cpuUsage_;

 /// @brief Physical memory usage in bytes
    uint32_t memUsage_;

 /// @brief Whether it is a V2 cgroup
    bool isCgroupV2_;
};

}  // namespace resource_group
}  // namespace osi
}  // namespace isoft

#endif  ///< ISOFT_OSI_RESOURCE_GROUP_H_
