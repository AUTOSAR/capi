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
/// @file       debug_config.h
/// @brief      Debugger parameter class file
/// @details
/// @date       2023-11-22
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#ifndef _ARA_LOADER_DEBUG_CONFIG_H_
#define _ARA_LOADER_DEBUG_CONFIG_H_

#include <cstdint>
#include <iostream>

namespace ara {
namespace loader {

/// @brief Debug configuration
class DebugConfig final  // PRQA S 5215
{
public:
    /// @brief Default constructor
    DebugConfig() noexcept
    {
        SetVerbose(false);
        DisableResourceGroup(false);
        SetServerOn(false);
    }

    /// @brief Print debug information
    void Debug() const noexcept
    {
        std::cout << "+++ Debug Argument +++" << std::endl;
        std::cout << "Verbose: " << IsVerboseOn() << std::endl;
        std::cout << "ResourceGroup: " << IsResourceGroupDisabled() << std::endl;
        std::cout << "--- Debug Argument ---" << std::endl;
    }

    /// @brief Determine whether debug mode is enabled
    /// @return Whether debug mode is enabled
    bool IsDebugOn() const noexcept
    {
        if (IsVerboseOn() || IsResourceGroupDisabled()) {
            return true;
        }

        return false;
    }

    /// @brief Set Verbose mode
    /// @param is on/off
    void SetVerbose(bool const is) noexcept
    {
        if (is) {
            mode_.verboseOn = 1U;
        } else {
            mode_.verboseOn = 0U;
        }
    }

    /// @brief Query Verbose mode
    /// @return on / off
    bool IsVerboseOn() const noexcept
    {
        if (1U == mode_.verboseOn) {
            return true;
        }

        return false;
    }

    /// @brief Disable resource group
    /// @param is on / off
    void DisableResourceGroup(bool const is) noexcept
    {
        if (is) {
            mode_.cgroupDisable = 1U;
        } else {
            mode_.cgroupDisable = 0U;
        }
    }

    /// @brief Determine whether a resource group is disabled
    /// @return true / false
    bool IsResourceGroupDisabled() const noexcept
    {
        if (1U == mode_.cgroupDisable) {
            return true;
        }

        return false;
    }

    /// @brief Set the service port
    /// @param port Port number
    void SetServerPort(uint16_t const port) noexcept
    {
        serverPort_ = port;
        SetServerOn(true);
    }

    /// @brief Get the service port number
    /// @return Port number
    uint16_t GetServerPort() const noexcept { return serverPort_; }

    /// @brief Determine whether the service is enabled
    /// @return Whether the service is enabled
    bool IsServerOn() const noexcept
    {
        if (1U == mode_.serverOn) {
            return true;
        }

        return false;
    }

    /// @brief Enable or disable the service
    /// @param is on / off
    void SetServerOn(bool const is) noexcept
    {
        if (is) {
            mode_.serverOn = 1U;
        } else {
            mode_.serverOn = 0U;
        }
    }

private:
    /// @brief Debug mode
    struct Mode
    {
        uint8_t verboseOn : 1;
        uint8_t cgroupDisable : 1;
        uint8_t serverOn : 1;
    };

    /// @brief Mode switch
    Mode mode_{};

    /// @brief Default port number
    static constexpr uint16_t const kDefaultServerPort{6000U};

    /// @brief Default debugger service port
    uint16_t serverPort_{kDefaultServerPort};
};

}  // namespace loader
}  // namespace ara

#endif  ///< _ARA_LOADER_DEBUG_CONFIG_H_