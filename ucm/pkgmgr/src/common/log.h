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
/// @file       log.h
/// @brief      log wrapper
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
/// @trace_id_sr=SR_UCM_00001,SR_UCM_00022
/// @unit_name=DefaultLogger
/// @unit_description=Global default logger
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_PKGMGR_COMMON_LOG_H_
#define ARA_UCM_PKGMGR_COMMON_LOG_H_

#include <ara/log/logger.h>
/// #include <ara/log/log_stream.h>
#include "alias.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief global default logger
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10398
/// @trace_id_dd=DD_UCM_11002
/// @needwork = ad
/// @endcode
class DefaultLogger
{
public:
    /// @brief constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_11003
    /// @needwork = dda
    /// @endcode
    DefaultLogger() = default;
    /// @brief destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_11004
    /// @needwork = dda
    /// @endcode
    ~DefaultLogger() = default;

    /// @brief delete copy construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_11005
    /// @needwork = dda
    /// @endcode
    DefaultLogger(DefaultLogger const& other) = delete;
    /// @brief delete copy asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_11006
    /// @needwork = dda
    /// @endcode
    DefaultLogger& operator=(DefaultLogger const& other) = delete;
    /// @brief delete move construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_11007
    /// @needwork = dda
    /// @endcode
    DefaultLogger(DefaultLogger&& other) = delete;
    /// @brief delete move asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_11008
    /// @needwork = dda
    /// @endcode
    DefaultLogger& operator=(DefaultLogger&& other) = delete;

    /// @brief get instance
    /// @throws no
    /// @return instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_11009
    /// @needwork = dda
    /// @endcode
    static DefaultLogger& GetInstance();

    /// @brief get inner logger
    /// @return inner logger
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_11010
    /// @needwork = dda
    /// @endcode
    ara::log::Logger const& GetLogger() const noexcept { return log_; }

private:
    /// @brief single instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_11011
    /// @needwork = dda
    /// @endcode
    static std::unique_ptr< DefaultLogger > s_Instance_;
    /// @brief once flag
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_11012
    /// @needwork = dda
    /// @endcode
    static std::once_flag s_Once_;

    /// @brief inner logger
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_11013
    /// @needwork = dda
    /// @endcode
    ara::log::Logger& log_{ara::log::CreateLogger(
        std::move(AraStringView("#UCM")), std::move(AraStringView("UCMCTX")), ara::log::LogLevel::kVerbose)};
};

/// @brief get global default logger
/// @throws no
/// @return global default logger
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10419
/// @trace_id_dd=DD_UCM_11014
/// @needwork = ad
/// @endcode
ara::log::Logger const& GetDefaultLogger();

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara

/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define LOGF GetDefaultLogger().LogFatal() << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << " "
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define LOGE GetDefaultLogger().LogError() << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << " "
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define LOGW GetDefaultLogger().LogWarn() << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << " "
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define LOGI GetDefaultLogger().LogInfo() << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << " "
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define LOGD GetDefaultLogger().LogDebug() << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << " "
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define LOGV GetDefaultLogger().LogVerbose() << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << " "

#endif  // ARA_UCM_PKGMGR_COMMON_LOG_H_
