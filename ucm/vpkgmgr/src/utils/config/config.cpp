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
/// @file       config.cpp
/// @brief      Implementation of the ucmm configuration information class
/// @details
/// @date       2024-07-20
/// @author     hanzhibo
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/UCM Master/Utils
/// @interface_level=module
/// @trace_id_sr=SR_UCM_00035
/// @unit_name=Config
/// @unit_description=Implementation of the ucmm configuration information class
/// @endcode
///
/// ================================================================

#include "utils/config/config.h"

#include <mutex>

namespace ara {
namespace ucm {
namespace vpkgmgr {

/// @brief s_Instance_
std::unique_ptr< Config > Config::s_Instance_{nullptr};  // NOLINT

/// @brief Get instance
/// @return
Config* Config::GetInstance() noexcept
{
    if (nullptr == s_Instance_) {
        /// @brief Enable_Config_Public
        struct Enable_Config_Public : public Config
        {
            Enable_Config_Public() : Config() {}
        };

        s_Instance_ = std::make_unique< Enable_Config_Public >();
    }
    return s_Instance_.get();
}

/// @brief Destroy instance
void Config::Destroy() noexcept { s_Instance_.reset(); }

/// @brief Initialize
/// @param disbaleAuthn Whether to disable package verification
/// @return
void Config::Init(bool const disbaleAuthn) noexcept { disbaleAuthn_ = disbaleAuthn; }

/// @brief Returns whether package verification is disabled
/// @return
bool Config::GetDisbaleAuthn() const noexcept { return disbaleAuthn_; }

}  // namespace vpkgmgr
}  // namespace ucm
}  // namespace ara
