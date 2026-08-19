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
/// @file       argument.h
/// @brief      Debugger argument class file
/// @details
/// @date       2023-11-22
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/ExecutionManagement/Emd
/// @unit_name=ConfigManager
/// @unit_description=Used to manage all configurations used by the Emd.
/// @interface_level=none
/// @endcode
///
/// ================================================================

#ifndef _ARA_EXEC_INTERNAL_EMD_ARGUMENT_H_
#define _ARA_EXEC_INTERNAL_EMD_ARGUMENT_H_

#include <cstdint>
#include <iostream>

namespace ara {
namespace exec {
namespace internal {
namespace emd {

/// @brief EMD argument class
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
class Argument final
{
public:
    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    Argument() noexcept
    {
        SetVerbose(false);
        DisableResourceGroup(false);
        SetServerOn(false);
    }

    /// @brief Print argument information
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    void Debug() const noexcept
    {
        std::cout << "+++ Debug Argument +++" << std::endl;
        std::cout << "Verbose: " << IsVerboseOn() << std::endl;
        std::cout << "ResourceGroup: " << IsResourceGroupDisabled() << std::endl;
        std::cout << "--- Debug Argument ---" << std::endl;
    }

    /// @brief Whether debug mode is enabled
    /// @return true debug mode enabled; false debug mode not enabled
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    bool IsDebugOn() const noexcept
    {
        if (IsVerboseOn() || IsResourceGroupDisabled()) {
            return true;
        }

        return false;
    }

    /// @brief Set verbose mode
    /// @param isVerbose Whether to enable verbose mode
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    void SetVerbose(bool const isVerbose) noexcept
    {
        if (isVerbose) {
            mode_.verboseOn = 1U;
        } else {
            mode_.verboseOn = 0U;
        }
    }

    /// @brief Get verbose mode status
    /// @return true verbose mode enabled; false verbose mode not enabled
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    bool IsVerboseOn() const noexcept
    {
        if (1U == mode_.verboseOn) {
            return true;
        }

        return false;
    }

    /// @brief Set whether to disable resource groups
    /// @param isDisabled Whether to disable resource groups
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    void DisableResourceGroup(bool const isDisabled) noexcept
    {
        if (isDisabled) {
            mode_.cgroupDisable = 1U;
        } else {
            mode_.cgroupDisable = 0U;
        }
    }

    /// @brief Get resource group disabled status
    /// @return true resource group disabled; false resource group not disabled
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    bool IsResourceGroupDisabled() const noexcept
    {
        if (1U == mode_.cgroupDisable) {
            return true;
        }

        return false;
    }

    /// @brief Set the service port
    /// @param port Port number
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    void SetServerPort(uint16_t const port) noexcept
    {
        serverPort_ = port;
        SetServerOn(true);
    }

    /// @brief Get the service port
    /// @return Service port number
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    uint16_t GetServerPort() const noexcept { return serverPort_; }

    /// @brief Whether the service port is enabled
    /// @return true service enabled; false service not enabled
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    bool IsServerOn() const noexcept
    {
        if (1U == mode_.serverOn) {
            return true;
        }

        return false;
    }

    /// @brief Set whether the service port is enabled
    /// @param isServerOn Whether to turn on the service
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    void SetServerOn(bool const isServerOn) noexcept
    {
        if (isServerOn) {
            mode_.serverOn = 1U;
        } else {
            mode_.serverOn = 0U;
        }
    }

private:
    /// @brief Debug mode information
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    struct Mode
    {
        /// @brief verbose mode
        uint8_t verboseOn : 1;
        /// @brief Resource group disabled
        uint8_t cgroupDisable : 1;
        /// @brief Service port enabled
        uint8_t serverOn : 1;
    };

    /// @brief Debug mode
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    Mode mode_{};

    /// @brief Default debugger service port
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    static constexpr uint16_t const kDefaultServerPort{6000U};

    /// @brief Default debugger service port
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    uint16_t serverPort_{kDefaultServerPort};
};

}  // namespace emd
}  // namespace internal
}  // namespace exec
}  // namespace ara

#endif  ///< _ARA_EXEC_INTERNAL_EMD_ARGUMENT_H_