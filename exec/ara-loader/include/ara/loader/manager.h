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
/// @file       manager.h
/// @brief      ara loader class definition
/// @details
/// @date       2023-01-17
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_LOADER_MANAGER_H_
#define ARA_LOADER_MANAGER_H_

#include <cstdint>
#include <string>
#include <vector>

#include "ara/loader/config.h"
#include "ara/loader/debug_config.h"

namespace ara {
namespace loader {

/// @brief ara loader, responsible for finding emd according to configuration and preparing the startup environment for it
class Manager final
{
public:
    /// @brief char redefinition
    using Char8_t = char;

    /// @brief Create a load manager
    /// @param araSysroot ara root system path
    /// @return Load manager object pointer
    static std::unique_ptr< Manager > CreateManager(std::string const& araSysroot) noexcept  // PRQA S 2024
    {
        std::unique_ptr< Manager > mgr{std::make_unique< Manager >(araSysroot)};
        if (nullptr != mgr) {
            if (0 != mgr->Initialize()) {
                mgr = nullptr;
            }
        }
        return mgr;
    }

    /// @brief Default constructor
    Manager() noexcept
        : araSysroot_{"/"}, araConfig_{}, emdEnvironVars_{}, emdArguments_{}, emdExecPath_{}, debugMode_{}
    {
    }

    /// @brief Constructor with parameters
    /// @param sysroot Specified ara root path
    explicit Manager(std::string sysroot) noexcept
        : araSysroot_{std::move(sysroot)}
        , araConfig_{}
        , emdEnvironVars_{}
        , emdArguments_{}
        , emdExecPath_{}
        , debugMode_{}
    {
    }

    /// @brief Default destructor
    ~Manager() noexcept { std::ignore = Deinitialize(); }

    /// @brief Prohibit move construction
    /// @param other Other object
    Manager(Manager&& other) = delete;

    /// @brief Prohibit copy construction
    /// @param other Other object
    Manager(Manager const& other) = delete;

    /// @brief Prohibit move assignment
    /// @param other Other object
    /// @return Reference to itself
    Manager& operator=(Manager&& other) = delete;

    /// @brief Prohibit copy assignment
    /// @param other Other object
    /// @return Reference to itself
    Manager& operator=(Manager const& other) = delete;

    /// @brief Initialize the loader
    /// @return 0 success; <0 failure
    int32_t Initialize() noexcept;

    /// @brief Execute startup
    /// @param mode Debug mode
    /// @return 0 success; <0 failure
    int32_t Boot(DebugConfig const mode) noexcept;

    /// @brief Deinitialize the loader
    /// @return 0 success; <0 failure
    int32_t Deinitialize() noexcept;

private:
    /// @brief Prepare environment variables for Execution Management
    /// @return 0 success; <0 failure
    int32_t _PrepareEnvironments4Emd() noexcept;

    /// @brief Prepare parameters for Execution Management
    /// @return 0 success; <0 failure
    int32_t _PrepareArguments4Emd() noexcept;

    /// @brief Prepare the executable file for Execution Management
    /// @return 0 success; <0 failure
    int32_t _PrepareExecutablePath4Emd() noexcept;

    /// @brief Execute the EMD program
    /// @return 0 success; <0 failure
    int32_t _ExecuteEmd() const noexcept;

    /// @brief Execute the startup operation
    /// @param recoverMode Whether it is recovery mode
    /// @return 0 success; <0 failure
    int32_t _DoBoot(bool const recoverMode) noexcept;

private:
    /// @brief ara system root path
    std::string araSysroot_;

    /// @brief ara configuration
    std::shared_ptr< Config > araConfig_;  // PRQA S 4656

    /// @brief Environment variables required by Execution Management
    std::vector< std::string > emdEnvironVars_;  // PRQA S 2026

    /// @brief Parameters required by Execution Management
    std::vector< std::string > emdArguments_;

    /// @brief Path to the executable program of Execution Management
    std::string emdExecPath_;

    /// @brief Run in debug mode
    DebugConfig debugMode_;

};  ///< class Manager

}  // namespace loader
}  // namespace ara

#endif  ///< ARA_LOADER_MANAGER_H_