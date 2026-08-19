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
/// @brief      Definition of the UCM configuration information class
/// @details
/// @date       2024-08-15
/// @author     cuiyinli
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @export_level=/UCM/Utils
/// @module_path=/UCM/Utils
/// @interface_level=module
/// @trace_id_sr=SR_UCM_00001
/// @unit_name=Config
/// @unit_description=Definition of the UCM configuration information class
/// @endcode
///
/// ================================================================

#ifndef CONFIG_CONFIG_H
#define CONFIG_CONFIG_H

#include <memory>

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief UCM configuration information class
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10197
/// @trace_id_dd=DD_UCM_10270
/// @needwork = ad
/// @endcode
class Config
{
public:
    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10271
    /// @needwork = dda
    /// @endcode
    virtual ~Config() noexcept = default;

    /// @brief Get instance
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10272
    /// @needwork = dda
    /// @endcode
    static Config* GetInstance() noexcept;

    /// @brief Destroy
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10273
    /// @needwork = dda
    /// @endcode
    static void Destroy() noexcept;

    /// @brief Initialize
    /// @param disbaleAuthn Whether to disable package verification
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10274
    /// @needwork = dda
    /// @endcode
    void Init(bool const disbaleAuthn) noexcept;

    /// @brief Returns whether package verification is disabled
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10275
    /// @needwork = dda
    /// @endcode
    bool GetDisbaleAuthn() const noexcept;

private:
    /// @brief Default constructor
    /// @exception nasdasd
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10276
    /// @needwork = dda
    /// @endcode
    Config() noexcept = default;

public:
    /// @brief Copy constructor
    /// @param other
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10277
    /// @needwork = dda
    /// @endcode
    Config(Config const& other) = delete;
    /// @brief Copy assignment operator
    /// @param other
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10278
    /// @needwork = dda
    /// @endcode
    Config& operator=(Config const& other) = delete;
    /// @brief Move constructor
    /// @param other
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10279
    /// @needwork = dda
    /// @endcode
    Config(Config&& other) = delete;
    /// @brief Move assignment operator
    /// @param other
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10280
    /// @needwork = dda
    /// @endcode
    Config& operator=(Config&& other) = delete;

private:
    /// @brief Whether package verification is disabled
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10281
    /// @needwork = dda
    /// @endcode
    bool disbaleAuthn_{false};

    /// @brief s_Instance_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10400
    /// @trace_id_dd=DD_UCM_10282
    /// @needwork = ad
    /// @endcode
    static std::unique_ptr< Config > s_Instance_;
};

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // CONFIG_CONFIG_H
