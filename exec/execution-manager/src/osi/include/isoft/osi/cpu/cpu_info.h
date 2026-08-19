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
/// @file       cpu_info.h
/// @brief      ara configuration series CPU information class
/// @details
/// @date       2023-03-14
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#ifndef ISOFT_OSI_CPU_INFO_H_
#define ISOFT_OSI_CPU_INFO_H_

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace isoft {
namespace osi {
namespace cpu {

/// @brief CPU information management class, used to obtain CPU information of the current machine
class CpuInfo  // PRQA S 5215
{
public:
 /// @brief Default constructor
    CpuInfo() = default;

 /// @brief Default destructor
    ~CpuInfo() = default;

 /// @brief Default move constructor
 /// @param other Other CPU info
    CpuInfo(CpuInfo&& other) = delete;

 /// @brief Default copy constructor
 /// @param other Other CPU info
    CpuInfo(CpuInfo const& other) = delete;

 /// @brief Default move assignment function
 /// @param other Other CPU info
 /// @return New CPU info instance
    CpuInfo& operator=(CpuInfo&& other) = delete;

 /// @brief Default copy assignment function
 /// @param other Other CPU info
 /// @return New CPU info instance
    CpuInfo& operator=(CpuInfo const& other) = delete;

 /// @brief Create CPU info object
 /// @return CPU info handle
    static std::shared_ptr< CpuInfo > CreateCpuInfo() noexcept  // PRQA S 2024
    {
        std::shared_ptr< CpuInfo > info{std::make_shared< CpuInfo >()};
        if (nullptr == info) {
            return info;
        }

        if (0 != info->Load()) {
            info.reset();
        }
        return info;
    }

 /// @brief Load all CPU information of the local machine
 /// @return 0 success; <0 failure
    int32_t Load() noexcept;

 /// @brief Get logical processor numbers based on physical CPU-CORE
 /// @param phyId Physical CPU ID
 /// @param coreId Core ID
 /// @param processors Logical CPU IDs, two logical CPUs obtained in case of hyperthreading
 /// @return <=0 not found; >0 actual number found
    int32_t GetProcessorNumbers(uint16_t const phyId,
                                uint16_t const coreId,
                                std::vector< uint16_t >& processors) const noexcept;

 /// @brief Print CpuInfo information for debugging
    void Debug() noexcept;

private:
 /// @brief Read numbers from file
 /// @param fpath File path
 /// @param numbers Number list
 /// @return 0 success; <0 failure
    static int32_t GetNumbersFromFile(std::string const& fpath, std::vector< int64_t >& numbers) noexcept;

 /// @brief Get list of online logical processors
 /// @param processors Logical processor list
 /// @return 0 success; <0 failure
    static int32_t GetOnlineProcessors(std::vector< uint16_t >& processors) noexcept;

private:
 /// @brief CPU detailed information
    struct Info
    {
    public:
 /// @brief Logical core ID
        uint16_t processor;
        /// @brief CPU ID
        uint16_t physicalId;
 /// @brief Physical core ID
        uint16_t coreId;
    };

 /// @brief CPU information list
    std::vector< Info > cpuInfos_;  // PRQA S 2026
};

}  // namespace cpu
}  // namespace osi
}  // namespace isoft

#endif  ///< #ifndef ISOFT_OSI_CPU_INFO_H_
