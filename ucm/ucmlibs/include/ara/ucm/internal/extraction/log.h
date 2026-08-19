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
/// @date       2024-01-01
/// @author     cuiyinli
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/UCM/UCMLib
/// @interface_level=none
/// @trace_id_sr=SR_UCM_00025
/// @unit_name=DefaultLogger
/// @unit_description=log wrapper definition
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_PKGMGR_EXTRACTION_LOG_H_
#define ARA_UCM_PKGMGR_EXTRACTION_LOG_H_

#include <ara/log/logger.h>

#include "alias.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief global default logger
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00007
/// @trace_id_dd=DD_UCM_00047
/// @needwork = ad
/// @endcode
class DefaultLogger
{
public:
    /// @brief constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00007
    /// @trace_id_dd=DD_UCM_00048
    /// @needwork = dda
    /// @endcode
    DefaultLogger() = default;
    /// @brief destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00007
    /// @trace_id_dd=DD_UCM_00049
    /// @needwork = dda
    /// @endcode
    ~DefaultLogger() = default;

    /// @brief delete copy construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00007
    /// @trace_id_dd=DD_UCM_00050
    /// @needwork = dda
    /// @endcode
    DefaultLogger(DefaultLogger const& other) = delete;
    /// @brief delete copy asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00007
    /// @trace_id_dd=DD_UCM_00051
    /// @needwork = dda
    /// @endcode
    DefaultLogger& operator=(DefaultLogger const& other) = delete;
    /// @brief delete move construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00007
    /// @trace_id_dd=DD_UCM_00052
    /// @needwork = dda
    /// @endcode
    DefaultLogger(DefaultLogger&& other) = delete;
    /// @brief delete move asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00007
    /// @trace_id_dd=DD_UCM_00053
    /// @needwork = dda
    /// @endcode
    DefaultLogger& operator=(DefaultLogger&& other) = delete;

    /// @brief get instance
    /// @return instance
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00007
    /// @trace_id_dd=DD_UCM_00054
    /// @needwork = dda
    /// @endcode
    static DefaultLogger& GetInstance();

    /// @brief get inner logger
    /// @return inner logger
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00007
    /// @trace_id_dd=DD_UCM_00055
    /// @needwork = dda
    /// @endcode
    ara::log::Logger const& GetLogger() const noexcept { return log_; }

private:
    /// @brief single instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00007
    /// @trace_id_dd=DD_UCM_00056
    /// @needwork = dd
    /// @endcode
    static std::unique_ptr< DefaultLogger > s_Instance_;
    /// @brief once flag
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00007
    /// @trace_id_dd=DD_UCM_00057
    /// @needwork = dd
    /// @endcode
    static std::once_flag s_Once_;

    /// @brief inner logger
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00007
    /// @trace_id_dd=DD_UCM_00058
    /// @needwork = dda
    /// @endcode
    ara::log::Logger& log_{ara::log::CreateLogger(
        std::move(AraStringView("#UCM")), std::move(AraStringView("UCMCTX")), ara::log::LogLevel::kVerbose)};
};

/// @brief get global default logger
/// @return global default logger
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00007
/// @trace_id_dd=DD_UCM_00059
/// @needwork = dd
/// @endcode
ara::log::Logger const& GetDefaultLogger();

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara

/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00007
/// @trace_id_dd=DD_UCM_00059
/// @needwork = dd
/// @endcode
#define LOGF GetDefaultLogger().LogFatal() << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << " "

/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00007
/// @trace_id_dd=DD_UCM_00059
/// @needwork = dd
/// @endcode
#define LOGE GetDefaultLogger().LogError() << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << " "

/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00007
/// @trace_id_dd=DD_UCM_00059
/// @needwork = dd
/// @endcode
#define LOGW GetDefaultLogger().LogWarn() << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << " "

/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00007
/// @trace_id_dd=DD_UCM_00059
/// @needwork = dd
/// @endcode
#define LOGI GetDefaultLogger().LogInfo() << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << " "

/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00007
/// @trace_id_dd=DD_UCM_00059
/// @needwork = dd
/// @endcode
#define LOGD GetDefaultLogger().LogDebug() << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << " "

/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_00007
/// @trace_id_dd=DD_UCM_00059
/// @needwork = dd
/// @endcode
#define LOGV GetDefaultLogger().LogVerbose() << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << " "

#endif  // ARA_UCM_PKGMGR_EXTRACTION_LOG_H_
