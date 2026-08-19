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
/// @file       resource_group.cpp
/// @brief      Resource group class definition
/// @details
/// @date       2025-09-19
/// @author     xueliang.bao
/// @version    1.2.0
///
/// ================================================================

#include "isoft/osi/resource_group/resource_group.h"

#include <sys/stat.h>
#include <unistd.h>

#include <fstream>

namespace isoft {
namespace osi {
namespace resource_group {

namespace {

/// @brief cpu period default value (100ms)
constexpr uint32_t kDefaultCpuPeriod{100000U};

/// @brief cgroup root directory
std::string const kCgroupRoot{"/sys/fs/cgroup"};  // NOLINT

/// @brief Write data to file
/// @param fileName File name
/// @param content Data
/// @return 0 success; <0 failure
int32_t WriteFile(std::string const &fileName,
                  std::string const &content,
                  std::ios_base::openmode const mode = std::ios_base::out) noexcept
{
    std::ofstream fileHandler(fileName, mode);
    if (!fileHandler) {
        perror(std::string("Failed to open " + fileName).c_str());
        return -1;
    }

    fileHandler << content;
    if (!fileHandler) {
        perror(std::string("Write " + content + " to " + fileName).c_str());
        return -1;
    }
    fileHandler.close();

    return 0;
}

}  // namespace

/// @brief Open resource group
/// @return 0 success; <0 failure
int32_t ResourceGroup::Open() noexcept
{
    if (rgName_.empty()) {
        return -1;
    }

    if (_EnableCpuMemoryController() != 0) {
        return -1;
    }

    if (_CreateCgroup() != 0) {
        return -1;
    }

    if (_LimitCpuMemoryUsage() != 0) {
        _DestroyCgroup();
        return -1;
    }

    return 0;
}

/// @brief Add process to resource group
/// @param pid Process PID
/// @return 0 success; <0 failure
int32_t ResourceGroup::AttachTask(pid_t const pid) noexcept
{
    std::string cgProcPath;

    if (isCgroupV2_) {
        cgProcPath = kCgroupRoot + "/" + rgName_ + "/cgroup.procs";
        if (WriteFile(cgProcPath, std::to_string(pid), std::ios::app) != 0) {
            return -1;
        }

        return 0;
    }

    cgProcPath = kCgroupRoot + "/cpu/" + rgName_ + "/cgroup.procs";
    if (WriteFile(cgProcPath, std::to_string(pid), std::ios::app) != 0) {
        return -1;
    }

    cgProcPath = kCgroupRoot + "/memory/" + rgName_ + "/cgroup.procs";
    if (WriteFile(cgProcPath, std::to_string(pid), std::ios::app) != 0) {
        return -1;
    }

    return 0;
}

void ResourceGroup::_CheckCgroupVersion() noexcept
{
    std::ifstream controllers("/sys/fs/cgroup/cgroup.controllers");
    if (controllers.good()) {
        isCgroupV2_ = true;
        controllers.close();
    } else {
        isCgroupV2_ = false;
    }
}

/// @brief Enable cpu and memory control
/// @return 0 enable success; <0 enable failure
int32_t ResourceGroup::_EnableCpuMemoryController() const noexcept
{
    if (!isCgroupV2_) {
        return 0;
    }

    std::string const subControllerPath{kCgroupRoot + "/cgroup.subtree_control"};
    if (WriteFile(subControllerPath, "+cpu +memory") != 0) {
        return -1;
    }

    return 0;
}

/// @brief Create cgroup
/// @return 0 create success; <0 create failure
int32_t ResourceGroup::_CreateCgroup() const noexcept
{
    if (isCgroupV2_) {
        return _CreateCgroupV2();
    }

    return _CreateCgroupV1();
}

/// @brief Configure cpu and memory limits
/// @return 0 configure success; <0 configure failure
int32_t ResourceGroup::_LimitCpuMemoryUsage() const noexcept
{
    if (isCgroupV2_) {
        if (_LimitCpuUsageV2() != 0 || _LimitMemoryUsageV2() != 0) {
            return -1;
        }
    } else {
        if (_LimitCpuUsageV1() != 0 || _LimitMemoryUsageV1() != 0) {
            return -1;
        }
    }

    return 0;
}

/// @brief Destroy cgroup
/// @return 0 destroy success; <0 destroy failure
int32_t ResourceGroup::_DestroyCgroup() const noexcept
{
    if (isCgroupV2_) {
        return _DestroyCgroupV2();
    }

    return _DestroyCgroupV1();
}

/// @brief Create V1 cgroup
/// @return 0 create success; <0 create failure
int32_t ResourceGroup::_CreateCgroupV1() const noexcept
{
    std::string cgroupPath{kCgroupRoot + "/cpu/" + rgName_};
    if (mkdir(cgroupPath.c_str(), 0755) == -1 && errno != EEXIST) {  // NOLINT
        perror(std::string("Create Cgroup(" + cgroupPath + ") failed").c_str());
        return -1;
    }

    cgroupPath = kCgroupRoot + "/memory/" + rgName_;
    if (mkdir(cgroupPath.c_str(), 0755) == -1 && errno != EEXIST) {  // NOLINT
        perror(std::string("Create Cgroup(" + cgroupPath + ") failed").c_str());
        rmdir(std::string(kCgroupRoot + "/cpu/" + rgName_).c_str());
        return -1;
    }

    return 0;
}

/// @brief Create V2 cgroup
/// @return 0 create success; <0 create failure
int32_t ResourceGroup::_CreateCgroupV2() const noexcept
{
    std::string const cgroupPath{kCgroupRoot + "/" + rgName_};
    if (mkdir(cgroupPath.c_str(), 0755) == -1 && errno != EEXIST) {  // NOLINT
        perror(std::string("Create Cgroup(" + cgroupPath + ") failed").c_str());
        return -1;
    }

    return 0;
}

/// @brief Configure cpu utilization
/// @return 0 configure success; <0 configure failure
int32_t ResourceGroup::_LimitCpuUsageV1() const noexcept
{
    std::string cgCpuPath{kCgroupRoot + "/cpu/" + rgName_ + "/cpu.cfs_period_us"};

    std::ifstream cpuPeriodFile(cgCpuPath);
    if (!cpuPeriodFile) {
        perror(std::string("Failed to open " + cgCpuPath).c_str());
        return -1;
    }

    uint32_t cpuPeriod{0U};
    cpuPeriodFile >> cpuPeriod;
    cpuPeriodFile.close();

    cgCpuPath = kCgroupRoot + "/cpu/" + rgName_ + "/cpu.cfs_quota_us";

 // Calculate CPU quota (cpuQuota = cpuPeriod * cpu_percent)
    int32_t cpuQuota{-1};
    uint32_t const k100Percent{100U};
    if (cpuUsage_ != 0U) {
        cpuQuota = static_cast< int32_t >((cpuPeriod / k100Percent) * cpuUsage_);
    }

    return WriteFile(cgCpuPath, std::to_string(cpuQuota));
}

/// @brief Configure cpu utilization
/// @return 0 configure success; <0 configure failure
int32_t ResourceGroup::_LimitCpuUsageV2() const noexcept
{
    std::string const cgCpuPath{kCgroupRoot + "/" + rgName_ + "/cpu.max"};
    std::string const cpuQuota{(cpuUsage_ == 0U) ? "max" : std::to_string((kDefaultCpuPeriod / 100) * cpuUsage_)};
    std::string const cpuUsageString{cpuQuota + " " + std::to_string(kDefaultCpuPeriod)};

    return WriteFile(cgCpuPath, cpuUsageString);
}

/// @brief Configure physical memory usage in bytes
/// @return 0 configure success; <0 configure failure
int32_t ResourceGroup::_LimitMemoryUsageV1() const noexcept
{
    std::string cgMemoryPath{kCgroupRoot + "/memory/" + rgName_ + "/memory.limit_in_bytes"};

    if (WriteFile(cgMemoryPath, std::to_string(memUsage_)) != 0) {
        return -1;
    }

    cgMemoryPath = kCgroupRoot + "/memory/" + rgName_ + "/memory.swappiness";
    return WriteFile(cgMemoryPath, "0");
}

/// @brief Configure physical memory usage in bytes
/// @return 0 configure success; <0 configure failure
int32_t ResourceGroup::_LimitMemoryUsageV2() const noexcept
{
    std::string cgMemoryPath{kCgroupRoot + "/" + rgName_ + "/memory.max"};

    if (WriteFile(cgMemoryPath, std::to_string(memUsage_)) != 0) {
        return -1;
    }

    cgMemoryPath = kCgroupRoot + "/" + rgName_ + "/memory.swap.max";

    return WriteFile(cgMemoryPath, "0");
}

/// @brief Destroy V1 cgroup
/// @return 0 destroy success; <0 destroy failure
int32_t ResourceGroup::_DestroyCgroupV1() const noexcept
{
    std::string cgroupPath{kCgroupRoot + "/cpu/" + rgName_};
    int32_t ret{rmdir(cgroupPath.c_str())};

    cgroupPath = kCgroupRoot + "/memory/" + rgName_;
    ret += rmdir(cgroupPath.c_str());

    return ret;
}

/// @brief Destroy V2 cgroup
/// @return 0 destroy success; <0 destroy failure
int32_t ResourceGroup::_DestroyCgroupV2() const noexcept
{
    std::string cgroupPath{kCgroupRoot + "/" + rgName_};
    return rmdir(cgroupPath.c_str());
}

}  // namespace resource_group
}  // namespace osi
}  // namespace isoft