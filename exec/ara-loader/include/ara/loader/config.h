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
/// @file       config.h
/// @brief      ara configuration class
/// @details
/// @date       2023-03-06
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_LOADER_CONFIG_H_
#define ARA_LOADER_CONFIG_H_

#include <cstdint>
#include <memory>
#include <string>

namespace ara {
namespace loader {

/// @brief ara configuration class
class Config
{
public:
    /// @brief Create a configuration object
    /// @param araSysroot ara root system path
    /// @return Configuration object pointer
    static std::shared_ptr< Config > CreateConfig(std::string const& araSysroot) noexcept  // PRQA S 2024
    {
        std::shared_ptr< Config > cfg;
        cfg = std::make_shared< Config >(araSysroot);
        if (nullptr != cfg) {
            if (0 != cfg->LoadNormal()) {
                cfg = nullptr;
            }
        }
        return cfg;
    }

    /// @brief Constructor
    /// @param araSysroot - ara root system path
    explicit Config(std::string araSysroot) noexcept
        : araSysroot_{std::move(araSysroot)}
        , araConfigPath_{""}
        , coreVersion_{""}
        , coreDir_{""}
        , frameworkVersion_{""}
        , frameworkDir_{""}
        , varDir_{""}
        , swclsDir_{""}
    {
    }

    /// @brief Disable move construction
    /// @param other the other Config
    Config(Config&& other) noexcept = delete;

    /// @brief Disable copy construction
    /// @param other the other Config
    Config(Config const& other) noexcept = delete;

    /// @brief Disable move assignment
    /// @param other the other Config
    /// @return New configuration
    Config& operator=(Config&& other) noexcept = delete;

    /// @brief Disable copy assignment
    /// @param other the other Config
    /// @return New configuration
    Config& operator=(Config const& other) noexcept = delete;

    /// @brief Default destructor
    ~Config() noexcept = default;

    /// @brief Load normal configuration, read the latest version of the configuration
    /// @return 0 success; <0 failure
    int32_t LoadNormal() noexcept { return _Load(false); }

    /// @brief Load rollback configuration, read the oldest version of the configuration
    /// @return 0 success; <0 failure
    int32_t LoadRollBack() noexcept { return _Load(true); }

    /// @brief Get the configuration file name
    /// @return Configuration file name
    std::string GetConfigFileName() const noexcept
    {
        std::size_t const pos{araConfigPath_.find_last_of("/")};
        return araConfigPath_.substr(pos + 1U);
    }

    /// @brief Get the core version
    /// @return String reference
    std::string const& GetCoreVersion() const noexcept { return coreVersion_; }

    /// @brief Get the core directory path
    /// @return String reference
    std::string const& GetCoreDir() const noexcept { return coreDir_; }

    /// @brief Get the framework version
    /// @return String reference
    std::string const& GetFrameworkVersion() const noexcept { return frameworkVersion_; }

    /// @brief Get the framework directory path
    /// @return String reference
    std::string const& GetFrameworkDir() const noexcept { return frameworkDir_; }

    /// @brief Get the var directory path
    /// @return String reference
    std::string const& GetVarDir() const noexcept { return varDir_; }

    /// @brief Get the user software cluster directory path
    /// @return String reference
    std::string const& GetSwclsDir() const noexcept { return swclsDir_; }

    /// @brief Get the ara directory path
    /// @return Path string
    std::string GetAraDir() const noexcept { return araSysroot_ + "/ara/"; }

    /// @brief Get the platform runtime library directory
    /// @return String
    std::string GetFrameworkLibDir() const noexcept
    {
        return GetFrameworkDir() + "/" + GetFrameworkVersion() + "/lib/";
    }

    /// @brief Get the platform sbin directory
    /// @return String
    std::string GetFrameworkSbinDir() const noexcept
    {
        return GetFrameworkDir() + "/" + GetFrameworkVersion() + "/sbin/";
    }

    /// @brief Print debug information
    void Debug() const noexcept;

private:
    /// @brief Load configuration
    /// @param isRollBack - rollback mode, in this mode the oldest version of configuration will be loaded; normal mode loads the latest version
    /// @return 0 success; <0 failure
    int32_t _Load(bool const isRollBack) noexcept;

private:
    /// @brief ara root system path
    std::string araSysroot_;

    /// @brief ara configuration file path
    std::string araConfigPath_;

    /// @brief core software cluster version
    std::string coreVersion_;

    /// @brief core software cluster directory path
    std::string coreDir_;

    /// @brief framework version
    std::string frameworkVersion_;

    /// @brief framework directory path
    std::string frameworkDir_;

    /// @brief Variable data directory path
    std::string varDir_;

    /// @brief User software cluster directory path
    std::string swclsDir_;
};  ///< class Config

}  // namespace loader
}  // namespace ara

#endif  ///< ARA_LOADER_CONFIG_H_
