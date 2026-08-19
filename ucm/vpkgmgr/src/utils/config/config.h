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
/// @brief      Definition of the ucmm configuration information class
/// @details
/// @date       2024-07-20
/// @author     hanzhibo
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// export_level=/UCM Master/Utils
/// @module_path=/UCM Master/Utils
/// @interface_level=module
/// @trace_id_sr=SR_UCM_00035
/// @unit_name=Config
/// @unit_description=Definition of the ucmm configuration information class
/// @endcode
///
/// ================================================================

#ifndef CONFIG_CONFIG_H
#define CONFIG_CONFIG_H

#include <ara/log/logger.h>

#include <memory>

namespace ara {
namespace ucm {
namespace vpkgmgr {

/// @brief ucm configuration information class
/// @code{.isoft}
///
/// export_level=/ucm
///
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00428
/// @trace_id_dd=DD_UCM_Master_00864
/// @needwork = ad
/// @endcode
class Config
{
public:
    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00865
    /// @needwork = dda
    /// @endcode
    virtual ~Config() noexcept = default;

    /// @brief Get instance
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00866
    /// @needwork = dda
    /// @endcode
    static Config* GetInstance() noexcept;

    /// @brief Destroy
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00867
    /// @needwork = dda
    /// @endcode
    static void Destroy() noexcept;

    /// @brief Initialize
    /// @param disbaleAuthn Whether to disable package verification
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00868
    /// @needwork = dda
    /// @endcode
    void Init(bool const disbaleAuthn) noexcept;

    /// @brief Returns whether package verification is disabled
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00869
    /// @needwork = dda
    /// @endcode
    bool GetDisbaleAuthn() const noexcept;

private:
    /// @brief Default constructor
    /// @exception default
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00870
    /// @needwork = dda
    /// @endcode
    Config() noexcept = default;

public:
    /// @brief Copy constructor
    /// @param other
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00871
    /// @needwork = dda
    /// @endcode
    Config(Config const& other) = delete;
    /// @brief Copy assignment operator
    /// @param other
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00872
    /// @needwork = dda
    /// @endcode
    Config& operator=(Config const& other) = delete;
    /// @brief Move constructor
    /// @param other
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00873
    /// @needwork = dda
    /// @endcode
    Config(Config&& other) = delete;
    /// @brief Move assignment operator
    /// @param other
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00874
    /// @needwork = dda
    /// @endcode
    Config& operator=(Config&& other) = delete;

private:
    /// @brief disbaleAuthn_ Whether to disable package verification
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00875
    /// @needwork = dda
    /// @endcode
    bool disbaleAuthn_{false};

    /// @brief instance_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00487
    /// @trace_id_dd=DD_UCM_Master_00876
    /// @needwork = ad
    /// @endcode
    static std::unique_ptr< Config > s_Instance_;
};

}  // namespace vpkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // CONFIG_CONFIG_H
