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
/// @file       cpu_info.cpp
/// @brief      ara configuration series CPU information class
/// @details
/// @date       2023-03-14
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#include "isoft/osi/cpu/cpu_info.h"

#include <cstring>
#include <fstream>
#include <sstream>

#include "isoft/osi/log/log.h"
#include "isoft/utils/error.h"

namespace isoft {
namespace osi {
namespace cpu {

/// @brief char type redefinition
using Char8_t = char;

#if 0
/// @brief Load all CPU information of the local machine
/// On ARM architecture, /proc/cpuinfo does not provide physical and core information, so another implementation is needed
int32_t CpuInfo::Load() noexcept {
    static constexpr Char8_t const  *const kSystemCpuInfoPath {"/proc/cpuinfo"};
    static constexpr Char8_t const  *const kDelim{"\t: "};
    static constexpr Char8_t const  *const kProcessor{"processor"};
    static constexpr Char8_t const  *const kCoreId{"core id"};
    static constexpr Char8_t const  *const kPhysicalId{"physical id"};
    const static uint16_t kLineBuffSize = 1024U;

 int32_t ret{0}; ///return value
 Char8_t *lineBuffer{nullptr}; ///Save the read line content
 FILE *fp{nullptr}; ///File stream handle

 /// Substitute for goto
    while (true) {
 /// Open file
        fp = fopen(kSystemCpuInfoPath, "r");
        if (nullptr == fp) {
            LOGE() << "fopen(" << kSystemCpuInfoPath << "): " << isoft::utils::StrError();
            ret = -1;
            break;
        }

        lineBuffer = reinterpret_cast<Char8_t *>(malloc(kLineBuffSize));
        if (nullptr == lineBuffer) {
            ret = -1;
            break;
        }

        size_t len = kLineBuffSize;
        while (true) {
            Info cpu;
            int32_t n{getline(&lineBuffer, &len, fp)};
            if (n <= 0) break;
            std::stringView sv = lineBuffer;
            auto pos = sv.find(kDelim);
            if (std::stringView::npos != pos) {
                std::stringView kn = sv.substr(0, pos);
                std::stringView kv = sv.substr(pos + strlen(kDelim));
                if (0 == strncmp(kProcessor, kn.data(), strlen(kProcessor))) {
                    cpu.processor = atoi(kv.data());
                }
                else if (0 == strncmp(kPhysicalId, kn.data(), strlen(kPhysicalId))) {
                    cpu.physicalId = atoi(kv.data());
                }
                else if (0 == strncmp(kCoreId, kn.data(), strlen(kCoreId))) {
                    cpu.coreId = atoi(kv.data());
                }
            }

 /// Empty line
            if (n == 1) {
                cpuInfos_.emplace_back(cpu);
            }
        }
 /// Simulate GOTO, break out of loop
        break;
 } ///< while(true) substituting goto

    if (nullptr != lineBuffer) free(lineBuffer);

    if (nullptr != fp) {
        if (0 != fclose(fp)) {
            LOGE() << "fclose(): " << isoft::utils::StrError();
            ret = -1;
        }
    }

    return ret;
}
#endif

/// @brief Read numbers from file
/// @param fpath File path
/// @param numbers Number list
/// @return 0 success; <0 failure
int32_t CpuInfo::GetNumbersFromFile(std::string const &fpath, std::vector< int64_t > &numbers) noexcept
{
 /// File characteristics:
 /// 1. All numbers in the file are positive integers, and can span multiple lines
 /// 2. Each line consists of one or more number groups
 /// 3. Number groups are separated by ',': e.g., 0,1,2,3,4
 /// 4. Number groups can also describe a range, separated by '-': e.g., 1-10
    Char8_t const kNumberGroupDelim{','};
    std::string const kNumberCgroupInnerDelim{"-"};
 // 1. Use ifstream to automatically manage file resources (RAII)
    std::ifstream inFile{fpath};
    if (!inFile.is_open()) {
        LOGE() << "fopen(" << fpath << "): " << isoft::utils::StrError();
        return -1;
    }

    numbers.clear();
    std::string line;
 // 2. Use range-based for to read file line by line
    while (true) {
        if (!std::getline(inFile, line)) {
            break;
        }
 // Skip empty lines
        if (line.empty()) {
            continue;
        }

 // 3. Use stringstream to split number groups
        std::istringstream lineStream{line};
        std::string numberGroup;
        while (true) {
            if (!std::getline(lineStream, numberGroup, kNumberGroupDelim)) {
                break;
            }
 // 4. Process number range (e.g., "1-10")
            size_t const dashPos{numberGroup.find(kNumberCgroupInnerDelim)};
            if (dashPos != std::string::npos) {
 // Range format: parse start and end values
                int64_t const start{std::stoll(numberGroup.substr(0U, dashPos))};
                int64_t const end{std::stoll(numberGroup.substr(dashPos + 1U))};

 // Add all numbers within the range
                for (int64_t i{start}; i <= end; ++i) {
                    numbers.push_back(i);
                }
            } else {
 // Single number: direct conversion
                numbers.push_back(std::stoll(numberGroup));
            }
        }
    }
    return 0;
}

/// @brief Get list of online logical processors
/// @param processors Logical processor list
/// @return 0 success; <0 failure
int32_t CpuInfo::GetOnlineProcessors(std::vector< uint16_t > &processors) noexcept
{
    std::string const kSystemCpuOnlinePath{"/sys/devices/system/cpu/online"};
    std::vector< int64_t > processorNum;
    if (0 < GetNumbersFromFile(kSystemCpuOnlinePath, processorNum)) {
        return -1;
    }
    for (int64_t const n : processorNum) {  // PRQA S 2961
        processors.push_back(static_cast< uint16_t >(n));
    }
    return 0;
}

/// @brief Load all CPU information of the local machine
/// @return 0 success; <0 failure
int32_t CpuInfo::Load() noexcept
{
    std::string const kSystemCpuInfoDirPrefix{"/sys/devices/system/cpu/cpu"};
    std::string const kSystemCpuInfoPhysicalIdFile{"/topology/physical_package_id"};
    std::string const kSystemCpuInfoCoreIdFile{"/topology/core_id"};

    std::vector< uint16_t > processors;
    if (0 != this->GetOnlineProcessors(processors)) {
        return -1;
    }

    Info cpuInfo{};
    std::vector< int64_t > physId;
    std::vector< int64_t > coreId;
    for (uint16_t const id : processors) {  // PRQA S 2961
        std::string const idStr{std::to_string(static_cast< int32_t >(id))};
        std::string const physicalIdFile{kSystemCpuInfoDirPrefix + idStr + kSystemCpuInfoPhysicalIdFile};
        std::string const coreIdFile{kSystemCpuInfoDirPrefix + idStr + kSystemCpuInfoCoreIdFile};
        physId.clear();
        coreId.clear();
        std::ignore = this->GetNumbersFromFile(physicalIdFile, physId);
        if (physId.empty()) {
            LOGW() << "Can`t get physical ID from processor " << id;
            continue;
        }
        std::ignore = this->GetNumbersFromFile(coreIdFile, coreId);
        if (coreId.empty()) {
            LOGW() << "Can`t get core ID from processor " << id;
            continue;
        }
        cpuInfo.processor  = id;
        cpuInfo.coreId     = static_cast< uint16_t >(coreId[0U]);
        cpuInfo.physicalId = static_cast< uint16_t >(physId[0U]);
        this->cpuInfos_.emplace_back(cpuInfo);
        // only for QAC
        if (cpuInfo.coreId > 0U) {
        }
    }

    return 0;
}

/// @brief Get logical processor numbers based on physical CPU-CORE
/// @param phyId Physical CPU ID
/// @param coreId Core ID
/// @param processors Logical CPU IDs, two logical CPUs obtained in case of hyperthreading
/// @return <=0 not found; >0 actual number found
int32_t CpuInfo::GetProcessorNumbers(uint16_t const phyId,
                                     uint16_t const coreId,
                                     std::vector< uint16_t > &processors) const noexcept
{
    int32_t n{0};
    for (Info const &cpuInfo : cpuInfos_) {  // PRQA S 2961
        if ((cpuInfo.physicalId == phyId) && (cpuInfo.coreId == coreId)) {
            processors.emplace_back(cpuInfo.processor);
            n++;
        }
    }

    return n;
}

/// @brief Print CpuInfo information for debugging
void CpuInfo::Debug() noexcept
{
    std::cout << "processor | physical id | core id" << std::endl;
    for (Info const &cpuInfo : this->cpuInfos_) {  // PRQA S 2961
        std::cout << cpuInfo.processor << "\t" << cpuInfo.physicalId << "\t" << cpuInfo.coreId << std::endl;
    }
}

}  // namespace cpu
}  // namespace osi
}  // namespace isoft
